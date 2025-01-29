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

// An NSPanel as presented in the database
// struct NSPanel {
//   int id = 0;
//   std::string mac_address;
//   std::string friendly_name;
// };

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
                                                       // sqlite_orm::make_table("web_nspanel",
                                                       //                        sqlite_orm::make_column("id", &NSPanel::id, sqlite_orm::primary_key()),
                                                       //                        sqlite_orm::make_column("friendly_name", &NSPanel::friendly_name),
                                                       //                        sqlite_orm::make_column("mac_address", &NSPanel::mac_address)),
                                                       sqlite_orm::make_table("web_room",
                                                                              sqlite_orm::make_column("id", &Room::id, sqlite_orm::primary_key()),
                                                                              sqlite_orm::make_column("friendly_name", &Room::friendly_name),
                                                                              sqlite_orm::make_column("displayOrder", &Room::display_order)));

static void init() {
  database_manager::database.open_forever();
}

}; // namespace database_manager
