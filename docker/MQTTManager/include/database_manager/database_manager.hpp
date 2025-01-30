#pragma once
#include <csignal>
#include <cstdlib>
#include <spdlog/spdlog.h>
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
  uint64_t button1_detached_mode_light_id = 0;
  int button1_mode = 0;
  uint64_t button2_detached_mode_light_id = 0;
  int button2_mode = 0;
  std::string md5_data_file;
  std::string md5_firmware;
  std::string md5_tft_file;
  bool register_relay1_as_light;
  bool register_relay2_as_light;
  bool denied;
  bool accepted;
};

// A room as presented in the database
struct Room {
  int id = 0;
  std::string friendly_name;
  int display_order = 0;
};

// A Room Entities Page as presented in the database
struct RoomEntitiesPage {
  int id = 0;
  int display_order = 0;
  int page_type = 0;
  bool is_scenes_page = false;

  // Reference to parent room that this page is attached to.
  int room_id = 0;
};

static inline auto database = sqlite_orm::make_storage("/data/nspanelmanager_db.sqlite3",
                                                       sqlite_orm::make_table("web_roomentitiespage",
                                                                              sqlite_orm::make_column("id", &RoomEntitiesPage::id, sqlite_orm::primary_key()),
                                                                              sqlite_orm::make_column("display_order", &RoomEntitiesPage::display_order),
                                                                              sqlite_orm::make_column("page_type", &RoomEntitiesPage::page_type),
                                                                              sqlite_orm::make_column("is_scenes_page", &RoomEntitiesPage::is_scenes_page),
                                                                              sqlite_orm::make_column("room_id", &RoomEntitiesPage::room_id),
                                                                              sqlite_orm::foreign_key(&RoomEntitiesPage::room_id).references(&Room::id)),
                                                       sqlite_orm::make_table("web_settings",
                                                                              sqlite_orm::make_column("id", &SettingHolder::id, sqlite_orm::primary_key()),
                                                                              sqlite_orm::make_column("name", &SettingHolder::name),
                                                                              sqlite_orm::make_column("value", &SettingHolder::value)),
                                                       sqlite_orm::make_table("web_nspanelsettings",
                                                                              sqlite_orm::make_column("id", &NSPanelSettingHolder::id, sqlite_orm::primary_key()),
                                                                              sqlite_orm::make_column("name", &NSPanelSettingHolder::name),
                                                                              sqlite_orm::make_column("value", &NSPanelSettingHolder::value),
                                                                              sqlite_orm::make_column("nspanel_id", &NSPanelSettingHolder::nspanel_id)),
                                                       sqlite_orm::make_table("web_nspanel",
                                                                              sqlite_orm::make_column("id", &NSPanel::id, sqlite_orm::primary_key()),
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
                                                       sqlite_orm::make_table("web_room",
                                                                              sqlite_orm::make_column("id", &Room::id, sqlite_orm::primary_key()),
                                                                              sqlite_orm::make_column("friendly_name", &Room::friendly_name),
                                                                              sqlite_orm::make_column("displayOrder", &Room::display_order)));

static void init() {
  database_manager::database.open_forever();
}

}; // namespace database_manager
