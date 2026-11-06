#ifndef MQTT_MANAGER_NSPANEL
#define MQTT_MANAGER_NSPANEL
#include "protobuf_nspanel.pb.h"
#include "websocket_server/websocket_server.hpp"
#include <atomic>
#include <command_manager/command_manager.hpp>
#include <cstdint>
#include <database_manager/database_manager.hpp>
#include <metrics/manager_metrics.hpp>
#include <memory>
#include <metrics/panel_update_tracker.hpp>
#include <mqtt_manager/mqtt_manager.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

class Room; // Forward declare "Room" as to avoid dependancy loops in CMake

enum MQTT_MANAGER_NSPANEL_STATE {
  UNKNOWN,
  WAITING,
  ONLINE,
  OFFLINE,
  UPDATING_FIRMWARE,
  UPDATING_DATA,
  UPDATING_TFT,
  AWAITING_ACCEPT,
  DENIED,
  REBOOTING,
};

enum MQTT_MANAGER_NSPANEL_MODEL {
  SONOFF,
  CUSTOM,
  WEB,
};

/**
 * A copy of what the manager knows about a panel, taken at once for /prometheus/metrics and /prometheus/targets.
 */
struct NSPanelMetricsSnapshot {
  uint32_t id;
  std::string name;
  std::string mac;
  std::string ip_address;
  std::string model; // sonoff, custom or web
  MQTT_MANAGER_NSPANEL_STATE state;
  bool registered;   // Has registered to the manager (has an NSPM id).
  int16_t rssi;
  uint8_t heap_used_pct;
  float temperature;
  std::optional<float> humidity; // Only for panels whose status reports carry it.
  std::optional<float> pressure; // Pa. Only for panels whose status reports carry it.
  uint8_t update_progress;       // Percent.
  std::string md5_firmware;
  std::string md5_littlefs;
  std::string md5_tft;
  bool status_report_received; // Whether rssi, heap, temperature and progress come from a status report.
  uint16_t metrics_port;       // Port the panel serves /metrics on, 0 if it did not advertise one.
};

struct NSPanelWarningWebsocketRepresentation {
  std::string level;
  std::string text;
};

class NSPanel {
public:
  NSPanel(uint32_t panel_id);

  /*
   * Build a new NSPanel object from a discovery request and add it to the database as pending.
   * Returns a shared_ptr to the new NSPanel object if successful, otherwill nullptr.
   */
  static std::shared_ptr<NSPanel> create_from_discovery_request(nlohmann::json request_data);

  /*
   * Reload configuration from database, then send the config to the panel.
   * @param reason: Why, for the reason label of nspm_panel_configs_sent_total (see send_config).
   */
  void reload_config(const char *reason = "reload");

  ~NSPanel();

  /*
  Rebuild all MQTT topics from new settings.
  */
  void reset_mqtt_topics();

  /**
   * Reset MQTT topics used to register entities to HA.
   */
  void reset_ha_mqtt_topics();

  /**
   * Send an updated config to the config topic on MQTT
   * @param reason: What triggered it, a fixed string used as the reason label of nspm_panel_configs_sent_total.
   */
  void send_config(const char *reason = "other");

  /**
   * Get the ID of this NSPanel.
   */
  uint get_id();
  std::string get_mac();
  std::string get_name();
  MQTT_MANAGER_NSPANEL_STATE get_state();
  MQTT_MANAGER_NSPANEL_MODEL get_model();
  void mqtt_callback(std::string topic, std::string payload);

  /**
   * Handle log messages sent over MQTT from panel
   */
  void mqtt_log_callback(std::string topic, std::string payload);

  /**
   * Dump JSON as string and send to NSPanel command topic.
   */
  void send_command(nlohmann::json &command);

  /**
   * Get the JSON message that will be sent over the websocket when a client requests the state of the NSPanel.
   */
  nlohmann::json get_websocket_json_representation();

  /**
   * Copy the panel's state for metrics and Prometheus service discovery.
   */
  NSPanelMetricsSnapshot get_metrics_snapshot();

  /**
   * Sends a reboot command to the panel over MQTT.
   */
  void reboot();

  /**
   * Sends a command to start firmware update.
   */
  void firmware_update(bool force);

  /**
   * Sends a command to start TFT screen update.
   */
  void tft_update();

  /**
   * This panel has been deleted from the manager and we wish to completly remove all details about it every existing.
   */
  void erase();

  /**
   * Returns true if the NSPanel is register in manager, otherwise false.
   */
  bool has_registered_to_manager();

  /**
   * Send an event over the websocket that indicates that the NSPanel status changed.
   */
  void send_websocket_status_update();

  /**
   * Compare stored firmware checksum with current firmware checksum and return true if they are different.
   */
  bool has_firmware_update();

  /**
   * Compare stored LittleFS checksum with current LittleFS checksum and return true if they are different.
   */
  bool has_littlefs_update();

  /**
   * Compare stored TFT checksum with current TFT checksum for the selected panel type and background (ie. TFT-file) and return true if they are different.
   */
  bool has_tft_update();

  /**
   * Register NSPanel to manager.
   */
  bool register_to_manager(const nlohmann::json &register_request_payload);

  /**
   * Register NSPanel to manager.
   */
  void register_to_home_assistant();

  /**
   * Turn a relay on or off
   */
  void set_relay_state(uint8_t relay, bool state);

  /**
   * Handle command from NSPanel
   */
  void command_callback(NSPanelMQTTManagerCommand &command);

  /**
   * When an IPC request for NSPanel status comes in handle it and send the response back
   */
  void handle_stomp_command_callback(StompFrame frame);

  /**
   * Check if NSPanel is locked to default room through settings, and if so, return true, otherwise false.
   */
  bool is_locked_to_default_room();

  /**
   * Get the default room ID from NSPanel settings.
   */
  int32_t get_default_room_id();

private:
  // Labels for this panel's per-panel metrics.
  ManagerMetrics::PanelLabels _metrics_labels();

  std::string _get_nspanel_setting_with_default(std::string key, std::string default_value);
  // A thermostat temperature limit setting, or 0 if it is not a number.
  int _get_nspanel_temperature_limit_setting(std::string key);

  // Vars:
  uint32_t _id;
  database_manager::NSPanel _settings; // Settings loaded from database
  std::mutex _settings_mutex;          // Mutex to only allow access to _settings for one thread at the time
  std::string _mac;                    // MAC address of the NSPanel.
  MQTT_MANAGER_NSPANEL_MODEL _model;   // The model of the NSPanel.
  std::string _name;                   // Friendly name of the NSPanel.
  bool _is_us_panel;
  enum US_PANEL_ORIENTATION {
    LANDSCAPE_LEFT,
    LANDSCAPE_RIGHT,
    PORTRAIT
  };
  US_PANEL_ORIENTATION _us_panel_orientation;
  bool _has_registered_to_manager;
  std::string _ip_address;
  int16_t _rssi;
  float _temperature;
  float _humidity;
  float _pressure;
  uint8_t _heap_used_pct;
  uint8_t _update_progress;
  bool _has_humidity = false;
  bool _has_pressure = false;
  std::atomic<bool> _status_report_received = false;
  // Port the panel serves Prometheus /metrics on, from "metrics_port" in its register_request. 0 if not advertised.
  std::atomic<uint16_t> _metrics_port = 0;
  MQTT_MANAGER_NSPANEL_STATE _state;
  std::vector<NSPanelWarningWebsocketRepresentation> _nspanel_warnings;
  std::string _nspanel_warnings_from_manager;
  std::string _mqtt_register_mac;

  std::string _current_firmware_md5_checksum;
  std::string _current_littlefs_md5_checksum;
  std::string _current_tft_md5_checksum;

  enum ButtonMode {
    DIRECT,
    DETACHED,
    MQTT_PAYLOAD,
    FOLLOW,
    THERMOSTAT_HEATING,
    THERMOSTAT_COOLING,
    OPEN_MEDIA_PLAYER, // Opens the first media player in the panel's room. Stored in the database: only append.
  };

  // MQTT Stuff:
  // Wether or not relay1 should be registered to Home Assistant as a switch or light.
  bool _register_relay1_as_light = false;
  // The topic to send commands to the relay1
  std::string _mqtt_relay1_command_topic;
  // The topic where relay1 state is published
  std::string _mqtt_relay1_state_topic;
  // Wether or not relay1 is on
  bool _relay1_state = false;
  // Wether or not relay2 should be registered to Home Assistant as a switch or light.
  bool _register_relay2_as_light = false;
  // The topic to send commands to the relay2
  std::string _mqtt_relay2_command_topic;
  // The topic where relay2 state is published
  std::string _mqtt_relay2_state_topic;
  // Wether or not relay2 is on
  bool _relay2_state = false;
  // The topic Home Assistant sends the buzzer mute command to
  std::string _mqtt_buzzer_mute_command_topic;
  // The topic where the buzzer mute state is published
  std::string _mqtt_buzzer_mute_state_topic;
  // Wether or not the buzzer is muted. While muted, no buzzer sounds are sent in the config.
  std::atomic<bool> _buzzer_muted = false;
  // The topic to capture logs from MQTT
  std::string _mqtt_log_topic;
  // The topic to capture status (online/offline) from MQTT
  std::string _mqtt_status_topic;
  // The topic to capture status reports from MQTT
  std::string _mqtt_status_report_topic;
  // The topic to capture logs from MQTT, based on MAC-address
  std::string _mqtt_mac_log_topic;
  // The old name based status and status report topics, still subscribed to for older firmware
  std::string _mqtt_legacy_status_topic;
  std::string _mqtt_legacy_status_report_topic;
  // The topic to send out temperature in raw format instead of encoded in protobuf status report
  std::string _mqtt_temperature_topic;
  // The topic to send out humidity in raw format instead of encoded in protobuf status report
  std::string _mqtt_humidity_topic;
  // The topic to send out pressure in raw format instead of encoded in protobuf status report
  std::string _mqtt_pressure_topic;
  // The topic to send commands to panel to via MQTT
  std::string _mqtt_command_topic;
  // Home Assistant MQTT registration topics:
  std::string _mqtt_config_topic;
  std::string _mqtt_sensor_temperature_topic;
  std::string _mqtt_sensor_humidity_topic;
  std::string _mqtt_sensor_pressure_topic;
  std::string _mqtt_switch_relay1_topic;
  std::string _mqtt_light_relay1_topic;
  std::string _mqtt_switch_relay2_topic;
  std::string _mqtt_light_relay2_topic;
  std::string _mqtt_switch_screen_topic;
  std::string _mqtt_number_screen_brightness_topic;
  std::string _mqtt_number_screensaver_brightness_topic;
  std::string _mqtt_select_screensaver_topic;
  std::string _mqtt_switch_buzzer_mute_topic;

  // Topic where to send the NSPanelHomePageStatus protobuf state updates
  std::string _mqtt_topic_home_page_status;

  // Topic where to send the NSPanelHomePageStatus protobuf state updates for "All rooms" mode
  std::string _mqtt_topic_home_page_all_rooms_status;

  // Topic where to send the NSPanelRoomEntitiesPage protobuf state updates
  std::string _mqtt_topic_room_entities_page_status;

  // JSON representation of log messages backlog. Used to buffer historical log messages before sending them to web interface
  nlohmann::json _log_messages_backlog;

  // Mutex to allow only one thread to send config at a time
  std::mutex _send_config_mutex;

  // Follows OTA updates from the reported state, for nspm_panel_updates_total and nspm_panel_update_duration_seconds.
  PanelUpdateTracker _update_tracker;
  // Feed the current state to _update_tracker. Called after the state was set from the panel's status or status report.
  void _track_update_state(bool is_status_report);
};
#endif // !MQTT_MANAGER_NSPANEL
