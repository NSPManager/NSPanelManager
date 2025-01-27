#pragma once
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <memory>
#include <spdlog/spdlog.h>
#include <sqlite_orm/sqlite_orm.h>
#include <string>

class database_manager {
public:
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

  // inline auto get_database(std::string db_path) {
  //   auto db = sqlite_orm::make_storage(db_path,
  //                                      sqlite_orm::make_table("web_roomentitiespage",
  //                                                             sqlite_orm::make_column("id", &RoomEntitiesPage::id, sqlite_orm::primary_key()),
  //                                                             sqlite_orm::make_column("display_order", &RoomEntitiesPage::display_order),
  //                                                             sqlite_orm::make_column("page_type", &RoomEntitiesPage::page_type),
  //                                                             sqlite_orm::make_column("is_scenes_page", &RoomEntitiesPage::is_scenes_page),
  //                                                             sqlite_orm::make_column("room_id", &RoomEntitiesPage::room_id),
  //                                                             sqlite_orm::foreign_key(&RoomEntitiesPage::room_id).references(&Room::id)),
  //                                      sqlite_orm::make_table("web_settings",
  //                                                             sqlite_orm::make_column("id", &SettingHolder::id, sqlite_orm::primary_key()),
  //                                                             sqlite_orm::make_column("name", &SettingHolder::name),
  //                                                             sqlite_orm::make_column("value", &SettingHolder::value)),
  //                                      // sqlite_orm::make_table("web_nspanel",
  //                                      //                        sqlite_orm::make_column("id", &NSPanel::id, sqlite_orm::primary_key()),
  //                                      //                        sqlite_orm::make_column("friendly_name", &NSPanel::friendly_name),
  //                                      //                        sqlite_orm::make_column("mac_address", &NSPanel::mac_address)),
  //                                      sqlite_orm::make_table("web_room",
  //                                                             sqlite_orm::make_column("id", &Room::id, sqlite_orm::primary_key()),
  //                                                             sqlite_orm::make_column("friendly_name", &Room::friendly_name),
  //                                                             sqlite_orm::make_column("displayOrder", &Room::display_order)));
  //   db.open_forever();
  //   return db;
  // }

  // using db_type = decltype(get_database(""));
  // extern db_type database;

  // Define helper functions for accessing database objects:

  static std::string get_setting_with_default(std::string key, std::string default_value) {
    try {
      auto result = database.get_all<SettingHolder>(sqlite_orm::where(sqlite_orm::c(&SettingHolder::name) == key));
      if (result.size() > 0) [[likely]] {
        return result[0].value;
      } else {
        return default_value;
      }
    } catch (std::exception &ex) {
    }
    return nullptr;
  }

  template <class TYPE>
  static std::unique_ptr<TYPE> get_by_id(uint32_t id) {
    try {
      auto result = database.get<TYPE>(id);
      return std::make_unique<TYPE>(std::move(result));
    } catch (std::exception &ex) {
    }
    return nullptr;
  }
};
