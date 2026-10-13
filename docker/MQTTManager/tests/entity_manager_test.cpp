#include "test_helpers.hpp"
#include <button/button.hpp>
#include <entity_manager/entity_manager.hpp>
#include <gtest/gtest.h>
#include <media_player/media_player.hpp>
#include <nspanel/nspanel.hpp>
#include <room/room.hpp>
#include <scenes/scene.hpp>
#include <switch/switch.hpp>
#include <thermostat/thermostat.hpp>

using nspm_test::django_button_data;
using nspm_test::django_media_player_data;
using nspm_test::django_switch_data;
using nspm_test::django_thermostat_data;
using nspm_test::home_assistant_service_calls;
using nspm_test::ScopedEntity;
using nspm_test::ScopedNSPanel;
using nspm_test::ScopedScene;

// EntityManager keeps its entities in static lists, so every test reloads once its rows are
// gone (the rows are removed when the test body ends, before TearDown).
class EntityManagerTest : public nspm_test::SendCaptureTest {
protected:
  void TearDown() override {
    reload_all();
  }

  static void reload_all() {
    EntityManager::load_switches();
    EntityManager::load_buttons();
    EntityManager::load_thermostats();
    EntityManager::load_media_players();
    EntityManager::load_scenes();
  }

  template <class EntityClass>
  static size_t count(MQTT_MANAGER_ENTITY_TYPE type) {
    auto entities = EntityManager::get_all_entities_by_type<EntityClass>(type);
    return entities ? entities->size() : 0;
  }
};

TEST_F(EntityManagerTest, loads_new_switches_from_the_database) {
  ScopedEntity fan("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));

  EntityManager::load_switches();

  auto loaded = EntityManager::get_entity_by_id<SwitchEntity>(MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY, fan.id);
  ASSERT_TRUE(loaded.has_value());
  EXPECT_EQ((*loaded)->get_name(), "Fan");
  EXPECT_EQ((*loaded)->get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT);
}

TEST_F(EntityManagerTest, reload_updates_existing_entities_in_place) {
  ScopedEntity fan("switch", "Fan", django_switch_data("home_assistant", "switch.fan"), 1, 1, 0);
  EntityManager::load_switches();
  auto before = EntityManager::get_entity_by_id<SwitchEntity>(MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY, fan.id);
  ASSERT_TRUE(before.has_value());

  auto row = database_manager::database.get<database_manager::Entity>(fan.id);
  row.friendly_name = "Ceiling fan";
  row.room_view_position = 3;
  database_manager::database.update(row);
  EntityManager::load_switches();

  auto after = EntityManager::get_entity_by_id<SwitchEntity>(MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY, fan.id);
  ASSERT_TRUE(after.has_value());
  EXPECT_EQ(before->get(), after->get()); // Same object, so observers stay attached.
  EXPECT_EQ((*after)->get_name(), "Ceiling fan");
  EXPECT_EQ((*after)->get_entity_page_slot(), 3);
  EXPECT_EQ(count<SwitchEntity>(MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY), 1);
}

TEST_F(EntityManagerTest, removes_entities_deleted_from_the_database) {
  int fan_id;
  {
    ScopedEntity fan("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));
    fan_id = fan.id;
    EntityManager::load_switches();
    ASSERT_TRUE(EntityManager::get_entity_by_id<SwitchEntity>(MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY, fan_id).has_value());
  }

  EntityManager::load_switches();

  auto loaded = EntityManager::get_entity_by_id<SwitchEntity>(MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY, fan_id);
  ASSERT_FALSE(loaded.has_value());
  EXPECT_EQ(loaded.error(), EntityManager::EntityError::NOT_FOUND);
}

TEST_F(EntityManagerTest, loading_one_type_leaves_other_types_alone) {
  ScopedEntity fan("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));
  ScopedEntity doorbell("button", "Doorbell", django_button_data("home_assistant", "button.doorbell"));
  EntityManager::load_switches();
  EntityManager::load_buttons();

  EntityManager::load_switches();

  EXPECT_TRUE(EntityManager::get_entity_by_id<ButtonEntity>(MQTT_MANAGER_ENTITY_TYPE::BUTTON, doorbell.id).has_value());
  EXPECT_EQ(count<SwitchEntity>(MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY), 1);
  EXPECT_EQ(count<ButtonEntity>(MQTT_MANAGER_ENTITY_TYPE::BUTTON), 1);
}

TEST_F(EntityManagerTest, lookup_by_id_checks_the_type) {
  ScopedEntity fan("switch", "Fan", django_switch_data("home_assistant", "switch.fan"));
  EntityManager::load_switches();

  EXPECT_FALSE(EntityManager::get_entity_by_id<ButtonEntity>(MQTT_MANAGER_ENTITY_TYPE::BUTTON, fan.id).has_value());
  EXPECT_TRUE(EntityManager::get_entity_by_id<MqttManagerEntity>(MQTT_MANAGER_ENTITY_TYPE::ANY, fan.id).has_value());
}

TEST_F(EntityManagerTest, buttons_are_created_for_their_controller) {
  ScopedEntity ha("button", "Doorbell", django_button_data("home_assistant", "button.doorbell"));
  ScopedEntity nspm("button", "Garage", django_button_data("nspm", "", "garage/set", "toggle"));
  ScopedEntity unknown("button", "Zigbee", django_button_data("zigbee", ""));

  EntityManager::load_buttons();

  auto ha_button = EntityManager::get_entity_by_id<ButtonEntity>(MQTT_MANAGER_ENTITY_TYPE::BUTTON, ha.id);
  auto nspm_button = EntityManager::get_entity_by_id<ButtonEntity>(MQTT_MANAGER_ENTITY_TYPE::BUTTON, nspm.id);
  ASSERT_TRUE(ha_button.has_value());
  ASSERT_TRUE(nspm_button.has_value());
  EXPECT_EQ((*ha_button)->get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT);
  EXPECT_EQ((*nspm_button)->get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::NSPM);
  EXPECT_FALSE(EntityManager::get_entity_by_id<ButtonEntity>(MQTT_MANAGER_ENTITY_TYPE::BUTTON, unknown.id).has_value());

  (*nspm_button)->toggle();
  EXPECT_EQ(nspm_test::mqtt_published_to("garage/set").size(), 1);
}

TEST_F(EntityManagerTest, loads_thermostats_and_media_players) {
  ScopedEntity thermostat("thermostat", "Heat pump", django_thermostat_data("climate.hallway"));
  ScopedEntity speaker("media_player", "Speaker", django_media_player_data("media_player.lounge"));
  nlohmann::json openhab_speaker_data = django_media_player_data("");
  openhab_speaker_data["controller"] = "openhab";
  ScopedEntity openhab_speaker("media_player", "OpenHAB speaker", openhab_speaker_data);

  EntityManager::load_thermostats();
  EntityManager::load_media_players();

  EXPECT_TRUE(EntityManager::get_entity_by_id<ThermostatEntity>(MQTT_MANAGER_ENTITY_TYPE::THERMOSTAT, thermostat.id).has_value());
  EXPECT_TRUE(EntityManager::get_entity_by_id<MediaPlayerEntity>(MQTT_MANAGER_ENTITY_TYPE::MEDIA_PLAYER, speaker.id).has_value());
  // OpenHAB media players are not implemented, so none is created.
  EXPECT_FALSE(EntityManager::get_entity_by_id<MediaPlayerEntity>(MQTT_MANAGER_ENTITY_TYPE::MEDIA_PLAYER, openhab_speaker.id).has_value());
}

TEST_F(EntityManagerTest, scenes_are_created_for_their_type) {
  ScopedScene ha("home_assistant", "Movie time", "scene.movie_time", std::nullopt);
  ScopedScene unknown("hue", "Hue scene", "hue.scene", std::nullopt);

  EntityManager::load_scenes();

  auto scene = EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, ha.id);
  ASSERT_TRUE(scene.has_value());
  EXPECT_EQ((*scene)->get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT);
  EXPECT_FALSE(EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, unknown.id).has_value());
}

TEST_F(EntityManagerTest, finds_the_entity_in_a_page_slot) {
  ScopedEntity fan("switch", "Fan", django_switch_data("home_assistant", "switch.fan"), 1, 4242, 2);
  ScopedScene scene("home_assistant", "Movie time", "scene.movie_time", std::nullopt, 4242, 5);
  EntityManager::load_switches();
  EntityManager::load_scenes();

  auto in_slot_2 = EntityManager::get_entity_by_page_id_and_slot(4242, 2);
  auto in_slot_5 = EntityManager::get_entity_by_page_id_and_slot(4242, 5);
  ASSERT_TRUE(in_slot_2.has_value());
  ASSERT_TRUE(in_slot_5.has_value());
  EXPECT_EQ((*in_slot_2)->get_id(), fan.id);
  EXPECT_EQ((*in_slot_2)->get_type(), MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY);
  EXPECT_EQ((*in_slot_5)->get_id(), scene.id);
  EXPECT_FALSE(EntityManager::get_entity_by_page_id_and_slot(4242, 3).has_value());
}

TEST_F(EntityManagerTest, get_all_entities_by_type_reports_none_loaded) {
  auto switches = EntityManager::get_all_entities_by_type<SwitchEntity>(MQTT_MANAGER_ENTITY_TYPE::SWITCH_ENTITY);

  ASSERT_FALSE(switches.has_value());
  EXPECT_EQ(switches.error(), EntityManager::EntityError::NONE_LOADED);
}

TEST_F(EntityManagerTest, loaded_switch_is_controlled_through_the_manager) {
  ScopedEntity fan("switch", "Fan", django_switch_data("home_assistant", "switch.fan"), 1, 4343, 0);
  EntityManager::load_switches();

  (*EntityManager::get_entity_by_page_id_and_slot(4343, 0))->toggle();

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["target"]["entity_id"], "switch.fan");
}

class EntityManagerRoomTest : public nspm_test::SendCaptureTest {
protected:
  static void SetUpTestSuite() {
    study_id = nspm_test::create_room("Room test study");
  }

  static inline int study_id;
};

TEST_F(EntityManagerRoomTest, loads_rooms_from_the_database) {
  auto room = EntityManager::get_room(study_id);

  ASSERT_TRUE(room.has_value());
  EXPECT_EQ((*room)->get_id(), study_id);
  EXPECT_EQ((*room)->get_name(), "Room test study");
}

TEST_F(EntityManagerRoomTest, reload_renames_rooms) {
  auto row = database_manager::database.get<database_manager::Room>(study_id);
  row.friendly_name = "Room test library";
  database_manager::database.update(row);

  EntityManager::load_rooms();

  EXPECT_EQ((*EntityManager::get_room(study_id))->get_name(), "Room test library");
}

TEST_F(EntityManagerRoomTest, room_without_temperature_sensor_loads_without_errors) {
  nspm_test::ScopedErrorLog log;

  nspm_test::create_room("Room test hallway");

  for (auto &error : log.errors()) {
    EXPECT_EQ(error.find("temperature provider"), std::string::npos) << error;
  }
}

TEST_F(EntityManagerRoomTest, unknown_room_is_not_found) {
  auto room = EntityManager::get_room(999999);

  ASSERT_FALSE(room.has_value());
  EXPECT_EQ(room.error(), EntityManager::EntityError::NOT_FOUND);
}

class EntityManagerNSPanelTest : public EntityManagerRoomTest {
protected:
  void SetUp() override {
    EntityManagerRoomTest::SetUp();
    panel = std::make_unique<ScopedNSPanel>("Study panel", "C0:49:EF:00:00:99", study_id);
    EntityManager::load_nspanels();
  }

  void TearDown() override {
    panel.reset();
    EntityManager::load_nspanels();
    EntityManager::load_scenes();
  }

  std::unique_ptr<ScopedNSPanel> panel;
};

TEST_F(EntityManagerNSPanelTest, loads_panels_from_the_database) {
  auto loaded = EntityManager::get_nspanel_by_id(panel->id);

  ASSERT_TRUE(loaded.has_value());
  EXPECT_EQ((*loaded)->get_name(), "Study panel");
  EXPECT_EQ((*loaded)->get_mac(), "C0:49:EF:00:00:99");
  EXPECT_TRUE(EntityManager::get_nspanel_by_mac("C0:49:EF:00:00:99").has_value());
}

TEST_F(EntityManagerNSPanelTest, room_id_for_panel_is_the_panels_room) {
  auto room_id = EntityManager::get_room_id_for_panel_id(panel->id);

  ASSERT_TRUE(room_id.has_value());
  EXPECT_EQ(*room_id, study_id);
}

TEST_F(EntityManagerNSPanelTest, room_id_for_unknown_panel_is_not_found) {
  auto room_id = EntityManager::get_room_id_for_panel_id(999999);

  ASSERT_FALSE(room_id.has_value());
  EXPECT_EQ(room_id.error(), EntityManager::EntityError::NOT_FOUND);
}

TEST_F(EntityManagerNSPanelTest, removed_panel_is_unloaded) {
  int panel_id = panel->id;

  panel.reset();
  EntityManager::load_nspanels();

  EXPECT_FALSE(EntityManager::get_nspanel_by_id(panel_id).has_value());
  EXPECT_FALSE(EntityManager::get_nspanel_by_mac("C0:49:EF:00:00:99").has_value());
}

TEST_F(EntityManagerNSPanelTest, script_activated_from_a_panel_gets_the_panels_room) {
  ScopedScene script("home_assistant", "Lights out", "script.lights_out", std::nullopt);
  EntityManager::load_scenes();
  auto scene = EntityManager::get_entity_by_id<Scene>(MQTT_MANAGER_ENTITY_TYPE::SCENE, script.id);
  ASSERT_TRUE(scene.has_value());

  (*scene)->activate(EntityManager::get_room_id_for_panel_id(panel->id));

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service_data"]["variables"]["nspanelmanager"]["triggering_room_id"], study_id);
}

// A scene or script tapped on a panel's scenes or entities page arrives as a
// ToggleEntityFromEntitiesPage command carrying the panel's id.
class EntityManagerSceneCommandTest : public EntityManagerNSPanelTest {
protected:
  static void SetUpTestSuite() {
    EntityManagerNSPanelTest::SetUpTestSuite();
    kitchen_id = nspm_test::create_room("Scene command kitchen");
  }

  static void tap(int nspanel_id, int entity_page_id, int entity_slot) {
    NSPanelMQTTManagerCommand command;
    command.set_nspanel_id(nspanel_id);
    command.mutable_toggle_entity_from_entities_page()->set_entity_page_id(entity_page_id);
    command.mutable_toggle_entity_from_entities_page()->set_entity_slot(entity_slot);
    EntityManager::test_process_command(command);
  }

  // Renames a room until it goes out of scope.
  class ScopedRoomName {
  public:
    ScopedRoomName(int room_id, const std::string &name) : _room_id(room_id) {
      _old_name = database_manager::database.get<database_manager::Room>(room_id).friendly_name;
      rename(name);
    }

    ~ScopedRoomName() {
      rename(_old_name);
    }

  private:
    void rename(const std::string &name) {
      auto row = database_manager::database.get<database_manager::Room>(_room_id);
      row.friendly_name = name;
      database_manager::database.update(row);
      EntityManager::load_rooms();
    }

    int _room_id;
    std::string _old_name;
  };

  static inline int kitchen_id;
};

TEST_F(EntityManagerSceneCommandTest, tapping_a_script_sends_the_panels_room_and_the_scripts_room) {
  ScopedScene script("home_assistant", "Lights out", "script.lights_out", kitchen_id, 5151, 3);
  EntityManager::load_scenes();

  tap(panel->id, 5151, 3);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["domain"], "script");
  EXPECT_EQ(calls[0]["service"], "turn_on");
  EXPECT_EQ(calls[0]["target"]["entity_id"], "script.lights_out");
  EXPECT_EQ(calls[0]["service_data"]["variables"]["nspanelmanager"], nlohmann::json({
                                                                         {"scene_name", "Lights out"},
                                                                         {"scene_id", script.id},
                                                                         {"triggering_room_id", study_id},
                                                                         {"triggering_room_name", "Room test study"},
                                                                         {"scene_room_id", kitchen_id},
                                                                         {"scene_room_name", "Scene command kitchen"},
                                                                     }));
}

TEST_F(EntityManagerSceneCommandTest, tapping_a_global_script_sends_the_panels_room) {
  ScopedScene script("home_assistant", "All off", "script.all_off", std::nullopt, 5151, 4);
  EntityManager::load_scenes();

  tap(panel->id, 5151, 4);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service_data"]["variables"]["nspanelmanager"], nlohmann::json({
                                                                         {"scene_name", "All off"},
                                                                         {"scene_id", script.id},
                                                                         {"triggering_room_id", study_id},
                                                                         {"triggering_room_name", "Room test study"},
                                                                     }));
}

TEST_F(EntityManagerSceneCommandTest, tapping_a_scene_turns_it_on) {
  ScopedScene scene("home_assistant", "Movie time", "scene.movie_time", kitchen_id, 5151, 5);
  EntityManager::load_scenes();

  tap(panel->id, 5151, 5);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0], nlohmann::json({{"type", "call_service"}, {"domain", "scene"}, {"service", "turn_on"}, {"target", {{"entity_id", "scene.movie_time"}}}}));
}

// The script still runs, it just can't be told which room it was started from.
TEST_F(EntityManagerSceneCommandTest, script_tapped_on_an_unknown_panel_runs_without_a_triggering_room) {
  ScopedScene script("home_assistant", "Lights out", "script.lights_out", kitchen_id, 5151, 6);
  EntityManager::load_scenes();

  tap(999999, 5151, 6);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  auto context = calls[0]["service_data"]["variables"]["nspanelmanager"];
  EXPECT_FALSE(context.contains("triggering_room_id"));
  EXPECT_FALSE(context.contains("triggering_room_name"));
  EXPECT_EQ(context["scene_room_id"], kitchen_id);
}

TEST_F(EntityManagerSceneCommandTest, script_follows_the_panel_when_it_moves_room) {
  ScopedScene script("home_assistant", "All off", "script.all_off", std::nullopt, 5151, 7);
  EntityManager::load_scenes();
  auto row = database_manager::database.get<database_manager::NSPanel>(panel->id);
  row.room_id = kitchen_id;
  database_manager::database.update(row);
  EntityManager::load_nspanels();

  tap(panel->id, 5151, 7);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service_data"]["variables"]["nspanelmanager"]["triggering_room_id"], kitchen_id);
  EXPECT_EQ(calls[0]["service_data"]["variables"]["nspanelmanager"]["triggering_room_name"], "Scene command kitchen");
}

TEST_F(EntityManagerSceneCommandTest, script_follows_a_renamed_room) {
  ScopedScene script("home_assistant", "All off", "script.all_off", std::nullopt, 5151, 8);
  EntityManager::load_scenes();
  ScopedRoomName renamed(study_id, "Renamed study");

  tap(panel->id, 5151, 8);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service_data"]["variables"]["nspanelmanager"]["triggering_room_name"], "Renamed study");
}
