#pragma once
#include <csignal>
#include <cstdlib>
#include <memory>
#include <optional>
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <sqlite_orm/sqlite_orm.h>
#include <string>

namespace database_manager {
// A setting as represented in the database
struct SettingHolder {
  int id = 0;
  std::string name;
  std::string value;
};

// A setting as represented in the database
struct NSPanelSettingHolder {
  int id = 0;
  std::string name;
  std::string value;
  int nspanel_id = 0;
};

// An NSPanel as presented in the database
struct NSPanel {
  int id = 0;
  std::string mac_address;
  std::string friendly_name;
  int room_id = 0;
  std::string version;
  std::optional<uint64_t> button1_detached_mode_light_id = 0;
  int button1_mode = 0;
  std::optional<uint64_t> button2_detached_mode_light_id = 0;
  int button2_mode = 0;
  std::string md5_data_file;
  std::string md5_firmware;
  std::string md5_tft_file;
  bool register_relay1_as_light;
  bool register_relay2_as_light;
  bool denied;
  bool accepted;
};

struct NSPanelRelayGroupBinding {
  int id = 0;
  int relay_num = 0;
  int nspanel_id = 0;
  int relay_group_id = 0;
};

// A room as presented in the database
struct Room {
  int id = 0;
  std::string friendly_name;
  int display_order = 0;
};

struct Scene {
  int id = 0;
  std::unique_ptr<int> room_id = 0;
  std::string friendly_name;
  std::string scene_type;
  std::string backend_name;
  int room_view_position;
  int entities_page_id;
};

struct SceneLightState {
  int id = 0;
  int light_level;
  std::string color_mode;
  int color_temperature;
  int hue;
  int saturation;
  int light_id;
  int scene_id;
};

// A Room Entities Page as presented in the database
struct RoomEntitiesPage {
  int id = 0;
  int display_order = 0;
  int page_type = 0;
  bool is_scenes_page = false;

  // Reference to parent room that this page is attached to. Not set in case of global page.
  std::optional<int> room_id = 0;
};

struct Light {
  int id = 0;
  bool can_dim;
  bool can_rgb;
  bool can_color_temperature;
  int room_id;
  std::string friendly_name;
  bool is_ceiling_light;
  std::string type;
  std::string openhab_item_color_temperature;
  std::string openhab_item_dimmer;
  std::string openhab_item_switch;
  std::string openhab_item_rgb;
  std::string openhab_control_mode;
  std::string openhab_name;
  int room_view_position;
  int entities_page_id;
  std::string home_assistant_name;
  bool controlled_by_nspanel_main_page;
};

struct Switch {
  int id = 0;
  std::string friendly_name;
  std::string type;
  std::string home_assistant_name;
  std::string openhab_name;
  std::string openhab_item_switch;
  int entities_page_id;
  int room_view_position;
  int room_id;
};

static inline auto database = sqlite_orm::make_storage("/data/nspanelmanager_db.sqlite3",
                                                       sqlite_orm::make_table("web_roomentitiespage",
                                                                              sqlite_orm::make_column("id", &RoomEntitiesPage::id, sqlite_orm::primary_key().autoincrement()),
                                                                              sqlite_orm::make_column("display_order", &RoomEntitiesPage::display_order),
                                                                              sqlite_orm::make_column("page_type", &RoomEntitiesPage::page_type),
                                                                              sqlite_orm::make_column("is_scenes_page", &RoomEntitiesPage::is_scenes_page),
                                                                              sqlite_orm::make_column("room_id", &RoomEntitiesPage::room_id)),
                                                       sqlite_orm::make_table("web_settings",
                                                                              sqlite_orm::make_column("id", &SettingHolder::id, sqlite_orm::primary_key().autoincrement()),
                                                                              sqlite_orm::make_column("name", &SettingHolder::name),
                                                                              sqlite_orm::make_column("value", &SettingHolder::value)),
                                                       sqlite_orm::make_table("web_nspanelsettings",
                                                                              sqlite_orm::make_column("id", &NSPanelSettingHolder::id, sqlite_orm::primary_key().autoincrement()),
                                                                              sqlite_orm::make_column("name", &NSPanelSettingHolder::name),
                                                                              sqlite_orm::make_column("value", &NSPanelSettingHolder::value),
                                                                              sqlite_orm::make_column("nspanel_id", &NSPanelSettingHolder::nspanel_id)),
                                                       sqlite_orm::make_table("web_nspanel",
                                                                              sqlite_orm::make_column("id", &NSPanel::id, sqlite_orm::primary_key().autoincrement()),
                                                                              sqlite_orm::make_column("friendly_name", &NSPanel::friendly_name),
                                                                              sqlite_orm::make_column("mac_address", &NSPanel::mac_address),
                                                                              sqlite_orm::make_column("room_id", &NSPanel::room_id),
                                                                              sqlite_orm::make_column("version", &NSPanel::version),
                                                                              sqlite_orm::make_column("button1_detached_mode_light_id", &NSPanel::button1_detached_mode_light_id),
                                                                              sqlite_orm::make_column("button1_mode", &NSPanel::button1_mode),
                                                                              sqlite_orm::make_column("button2_detached_mode_light_id", &NSPanel::button2_detached_mode_light_id),
                                                                              sqlite_orm::make_column("button2_mode", &NSPanel::button2_mode),
                                                                              sqlite_orm::make_column("md5_data_file", &NSPanel::md5_data_file),
                                                                              sqlite_orm::make_column("md5_firmware", &NSPanel::md5_firmware),
                                                                              sqlite_orm::make_column("md5_tft_file", &NSPanel::md5_tft_file),
                                                                              sqlite_orm::make_column("register_relay1_as_light", &NSPanel::register_relay1_as_light),
                                                                              sqlite_orm::make_column("register_relay2_as_light", &NSPanel::register_relay2_as_light),
                                                                              sqlite_orm::make_column("denied", &NSPanel::denied),
                                                                              sqlite_orm::make_column("accepted", &NSPanel::accepted)),
                                                       sqlite_orm::make_table("web_relaygroupbinding",
                                                                              sqlite_orm::make_column("id", &NSPanelRelayGroupBinding::id, sqlite_orm::primary_key().autoincrement()),
                                                                              sqlite_orm::make_column("relay_num", &NSPanelRelayGroupBinding::relay_num),
                                                                              sqlite_orm::make_column("nspanel_id", &NSPanelRelayGroupBinding::nspanel_id),
                                                                              sqlite_orm::make_column("relay_group_id", &NSPanelRelayGroupBinding::relay_group_id)),
                                                       sqlite_orm::make_table("web_room",
                                                                              sqlite_orm::make_column("id", &Room::id, sqlite_orm::primary_key().autoincrement()),
                                                                              sqlite_orm::make_column("friendly_name", &Room::friendly_name),
                                                                              sqlite_orm::make_column("displayOrder", &Room::display_order)),
                                                       sqlite_orm::make_table("web_scene",
                                                                              sqlite_orm::make_column("id", &Scene::id, sqlite_orm::primary_key().autoincrement()),
                                                                              sqlite_orm::make_column("room_id", &Scene::room_id),
                                                                              sqlite_orm::make_column("friendly_name", &Scene::friendly_name),
                                                                              sqlite_orm::make_column("scene_type", &Scene::scene_type),
                                                                              sqlite_orm::make_column("backend_name", &Scene::backend_name),
                                                                              sqlite_orm::make_column("entities_page_id", &Scene::entities_page_id),
                                                                              sqlite_orm::make_column("room_view_position", &Scene::room_view_position)),
                                                       sqlite_orm::make_table("web_lightstate",
                                                                              sqlite_orm::make_column("id", &SceneLightState::id, sqlite_orm::primary_key().autoincrement()),
                                                                              sqlite_orm::make_column("scene_id", &SceneLightState::scene_id),
                                                                              sqlite_orm::make_column("light_id", &SceneLightState::light_id),
                                                                              sqlite_orm::make_column("light_level", &SceneLightState::light_level),
                                                                              sqlite_orm::make_column("color_mode", &SceneLightState::color_mode),
                                                                              sqlite_orm::make_column("color_temperature", &SceneLightState::color_temperature),
                                                                              sqlite_orm::make_column("hue", &SceneLightState::hue),
                                                                              sqlite_orm::make_column("saturation", &SceneLightState::saturation)),
                                                       sqlite_orm::make_table("web_light",
                                                                              sqlite_orm::make_column("id", &Light::id, sqlite_orm::primary_key().autoincrement()),
                                                                              sqlite_orm::make_column("can_dim", &Light::can_dim),
                                                                              sqlite_orm::make_column("can_rgb", &Light::can_rgb),
                                                                              sqlite_orm::make_column("can_color_temperature", &Light::can_color_temperature),
                                                                              sqlite_orm::make_column("room_id", &Light::room_id),
                                                                              sqlite_orm::make_column("friendly_name", &Light::friendly_name),
                                                                              sqlite_orm::make_column("is_ceiling_light", &Light::is_ceiling_light),
                                                                              sqlite_orm::make_column("type", &Light::type),
                                                                              sqlite_orm::make_column("openhab_item_color_temp", &Light::openhab_item_color_temperature),
                                                                              sqlite_orm::make_column("openhab_item_dimmer", &Light::openhab_item_dimmer),
                                                                              sqlite_orm::make_column("openhab_item_switch", &Light::openhab_item_switch),
                                                                              sqlite_orm::make_column("openhab_item_rgb", &Light::openhab_item_rgb),
                                                                              sqlite_orm::make_column("openhab_control_mode", &Light::openhab_control_mode),
                                                                              sqlite_orm::make_column("openhab_name", &Light::openhab_name),
                                                                              sqlite_orm::make_column("home_assistant_name", &Light::home_assistant_name),
                                                                              sqlite_orm::make_column("room_view_position", &Light::room_view_position),
                                                                              sqlite_orm::make_column("entities_page_id", &Light::entities_page_id),
                                                                              sqlite_orm::foreign_key(&Light::entities_page_id).references(&RoomEntitiesPage::id),
                                                                              sqlite_orm::make_column("controlled_by_nspanel_main_page", &Light::controlled_by_nspanel_main_page)),
                                                       sqlite_orm::make_table("web_switch",
                                                                              sqlite_orm::make_column("id", &Switch::id, sqlite_orm::primary_key().autoincrement()),
                                                                              sqlite_orm::make_column("room_view_position", &Switch::room_view_position),
                                                                              sqlite_orm::make_column("friendly_name", &Switch::friendly_name),
                                                                              sqlite_orm::make_column("type", &Switch::type),
                                                                              sqlite_orm::make_column("home_assistant_name", &Switch::home_assistant_name),
                                                                              sqlite_orm::make_column("openhab_item_switch", &Switch::openhab_item_switch),
                                                                              sqlite_orm::make_column("entities_page_id", &Switch::entities_page_id),
                                                                              sqlite_orm::make_column("room_id", &Switch::room_id)));

static void init() {
  database_manager::database.open_forever();
}

}; // namespace database_manager
