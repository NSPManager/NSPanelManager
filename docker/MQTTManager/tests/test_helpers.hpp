#pragma once

#include <algorithm>
#include <database_manager/database_manager.hpp>
#include <entity_manager/entity_manager.hpp>
#include <gtest/gtest.h>
#include <home_assistant_manager/home_assistant_manager.hpp>
#include <memory>
#include <mqtt_manager/mqtt_manager.hpp>
#include <mqtt_manager_config/mqtt_manager_config.hpp>
#include <nlohmann/json.hpp>
#include <openhab_manager/openhab_manager.hpp>
#include <optional>
#include <string>
#include <vector>

namespace nspm_test {

// Inserts an Entity row the way the Django web app writes it and removes it again when
// the fixture goes out of scope, so each test starts from the rows it creates itself.
class ScopedEntity {
public:
  ScopedEntity(const std::string &entity_type, const std::string &friendly_name, const nlohmann::json &entity_data, int room_id = 1, int entities_page_id = 1, int room_view_position = 0) {
    database_manager::Entity entity;
    entity.entity_type = entity_type;
    entity.friendly_name = friendly_name;
    entity.room_id = room_id;
    entity.entities_page_id = entities_page_id;
    entity.room_view_position = room_view_position;
    entity.entity_data = entity_data.dump();
    id = database_manager::database.insert(entity);
  }

  ~ScopedEntity() {
    database_manager::database.remove<database_manager::Entity>(id);
  }

  ScopedEntity(const ScopedEntity &) = delete;
  ScopedEntity &operator=(const ScopedEntity &) = delete;

  int id;
};

// entity_data for a light exactly as web/rest.py put_light_entity() stores it.
inline nlohmann::json django_light_data(const std::string &controller = "home_assistant") {
  return {
      {"controller", controller},
      {"home_assistant_name", controller == "home_assistant" ? "light.kitchen_ceiling" : ""},
      {"openhab_control_mode", "dimmer"},
      {"openhab_item_dimmer", controller == "openhab" ? "Kitchen_Dimmer" : ""},
      {"openhab_item_color_temp", ""},
      {"openhab_item_rgb", ""},
      {"can_dim", true},
      {"can_color_temperature", true},
      {"can_rgb", false},
      {"is_ceiling_light", true},
      {"controlled_by_nspanel_main_page", true},
  };
}

// A Scene row. room_id is empty for a global scene.
class ScopedScene {
public:
  ScopedScene(const std::string &scene_type, const std::string &friendly_name, const std::string &backend_name, std::optional<int> room_id, int entities_page_id = 1, int room_view_position = 0) {
    database_manager::Scene scene;
    scene.scene_type = scene_type;
    scene.friendly_name = friendly_name;
    scene.backend_name = backend_name;
    scene.room_id = room_id ? std::make_unique<int>(*room_id) : nullptr;
    scene.entities_page_id = entities_page_id;
    scene.room_view_position = room_view_position;
    id = database_manager::database.insert(scene);
  }

  ~ScopedScene() {
    database_manager::database.remove<database_manager::Scene>(id);
  }

  ScopedScene(const ScopedScene &) = delete;
  ScopedScene &operator=(const ScopedScene &) = delete;

  int id;
};

// An NSPanel row, accepted into the given room.
class ScopedNSPanel {
public:
  ScopedNSPanel(const std::string &friendly_name, const std::string &mac_address, int room_id) {
    database_manager::NSPanel panel;
    panel.friendly_name = friendly_name;
    panel.mac_address = mac_address;
    panel.model = "sonoff";
    panel.room_id = room_id;
    panel.button1_detached_mode_entity_id = std::nullopt;
    panel.button2_detached_mode_entity_id = std::nullopt;
    panel.denied = false;
    panel.accepted = true;
    id = database_manager::database.insert(panel);
  }

  ~ScopedNSPanel() {
    database_manager::database.remove<database_manager::NSPanel>(id);
  }

  ScopedNSPanel(const ScopedNSPanel &) = delete;
  ScopedNSPanel &operator=(const ScopedNSPanel &) = delete;

  int id;
};

// Adds a Room row and loads it into the EntityManager. Rooms are never removed again: each
// Room starts a detached status thread that keeps using the Room, so destroying one is not
// safe. Call it with a name no other test uses.
inline int create_room(const std::string &friendly_name) {
  database_manager::Room room;
  room.friendly_name = friendly_name;
  int id = database_manager::database.insert(room);
  EntityManager::load_rooms();
  return id;
}

// entity_data for a switch exactly as web/rest.py put_switch_entity() stores it.
inline nlohmann::json django_switch_data(const std::string &controller, const std::string &home_assistant_name, const std::string &openhab_item_switch = "") {
  return {
      {"openhab_item_switch", openhab_item_switch},
      {"home_assistant_name", home_assistant_name},
      {"controller", controller},
  };
}

// entity_data for a button exactly as web/rest.py put_button_entity() stores it.
inline nlohmann::json django_button_data(const std::string &controller, const std::string &home_assistant_name, const std::string &mqtt_topic = "", const std::string &mqtt_payload = "") {
  return {
      {"mqtt_topic", mqtt_topic},
      {"mqtt_payload", mqtt_payload},
      {"home_assistant_name", home_assistant_name},
      {"controller", controller},
  };
}

// One entry of a thermostat's ..._modes lists.
inline nlohmann::json thermostat_mode(const std::string &value, const std::string &label) {
  return {{"value", value}, {"label", label}, {"icon", "icon"}};
}

// entity_data for a Home Assistant thermostat exactly as web/rest.py put_thermostat_entity() stores it.
inline nlohmann::json django_thermostat_data(const std::string &home_assistant_name) {
  return {
      {"controller", "home_assistant"},
      {"fan_modes", nlohmann::json::array({thermostat_mode("auto", "Auto"), thermostat_mode("high", "High")})},
      {"hvac_modes", nlohmann::json::array({thermostat_mode("off", "Off"), thermostat_mode("heat", "Heat"), thermostat_mode("cool", "Cool")})},
      {"preset_modes", nlohmann::json::array()},
      {"swing_modes", nlohmann::json::array()},
      {"swingh_modes", nlohmann::json::array()},
      {"use_current_temperature", true},
      {"home_assistant_name", home_assistant_name},
      {"openhab_fan_mode_item", ""},
      {"openhab_hvac_mode_item", ""},
      {"openhab_preset_mode_item", ""},
      {"openhab_swing_mode_item", ""},
      {"openhab_swingh_mode_item", ""},
      {"openhab_temperature_item", ""},
      {"openhab_current_temperature_item", ""},
      {"step_size", 0.5},
  };
}

// entity_data for a media player exactly as web/rest.py put_media_player_entity() stores it.
inline nlohmann::json django_media_player_data(const std::string &home_assistant_name, int volume_step = 5) {
  return {
      {"controller", "home_assistant"},
      {"home_assistant_name", home_assistant_name},
      {"source_volume_strategy", "none"},
      {"source_entity_attribute", ""},
      {"source_volume_attribute", ""},
      {"volume_step", volume_step},
  };
}

// A Home Assistant state_changed event as it arrives over the websocket.
inline nlohmann::json home_assistant_state_changed(const std::string &entity_id, const std::string &state, const nlohmann::json &attributes = nlohmann::json::object()) {
  return {
      {"type", "event"},
      {"event", {{"event_type", "state_changed"}, {"data", {{"entity_id", entity_id}, {"new_state", {{"entity_id", entity_id}, {"state", state}, {"attributes", attributes}}}}}}},
  };
}

// The call_service messages sent to Home Assistant since the last call. Attaching an event
// observer also sends a get_states request, which is left out.
inline std::vector<nlohmann::json> home_assistant_service_calls() {
  std::vector<nlohmann::json> calls;
  for (auto &message : HomeAssistantManager::test_take_sent_messages()) {
    if (message.value("type", "") == "call_service") {
      message.erase("id"); // A running counter, not part of what is being tested.
      calls.push_back(message);
    }
  }
  return calls;
}

// The messages published on a topic since the last call (messages on other topics are dropped).
inline std::vector<MQTT_Manager::TestPublishedMessage> mqtt_published_to(const std::string &topic) {
  std::vector<MQTT_Manager::TestPublishedMessage> messages;
  for (auto &message : MQTT_Manager::test_take_published_messages()) {
    if (message.topic == topic) {
      messages.push_back(message);
    }
  }
  return messages;
}

// Starts each test with nothing recorded as sent, so a test only sees what it caused.
class SendCaptureTest : public ::testing::Test {
protected:
  void SetUp() override {
    HomeAssistantManager::test_take_sent_messages();
    OpenhabManager::test_take_sent_messages();
    MQTT_Manager::test_take_published_messages();
  }

  static void set_optimistic_mode(bool enabled) {
    MqttManagerConfig::set_setting_value(MQTT_MANAGER_SETTING::OPTIMISTIC_MODE, enabled ? "true" : "false");
  }
};

} // namespace nspm_test
