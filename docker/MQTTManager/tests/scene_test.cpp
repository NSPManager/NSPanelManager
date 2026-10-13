#include "test_helpers.hpp"
#include <gtest/gtest.h>
#include <scenes/home_assistant_scene.hpp>

using nspm_test::home_assistant_service_calls;
using nspm_test::ScopedScene;

class HomeAssistantSceneTest : public nspm_test::SendCaptureTest {
protected:
  static void SetUpTestSuite() {
    kitchen_id = nspm_test::create_room("Scene test kitchen");
    lounge_id = nspm_test::create_room("Scene test lounge");
  }

  static inline int kitchen_id;
  static inline int lounge_id;
};

TEST_F(HomeAssistantSceneTest, loads_config_from_database) {
  ScopedScene row("home_assistant", "Movie time", "scene.movie_time", kitchen_id, 6, 3);

  HomeAssistantScene scene(row.id);

  EXPECT_EQ(scene.get_id(), row.id);
  EXPECT_EQ(scene.get_name(), "Movie time");
  EXPECT_EQ(scene.get_entity_page_id(), 6);
  EXPECT_EQ(scene.get_entity_page_slot(), 3);
  EXPECT_FALSE(scene.is_global());
  EXPECT_FALSE(scene.can_save());
  EXPECT_EQ(scene.get_type(), MQTT_MANAGER_ENTITY_TYPE::SCENE);
  EXPECT_EQ(scene.get_controller(), MQTT_MANAGER_ENTITY_CONTROLLER::HOME_ASSISTANT);
}

TEST_F(HomeAssistantSceneTest, scene_without_room_is_global) {
  ScopedScene row("home_assistant", "Good night", "scene.good_night", std::nullopt);

  HomeAssistantScene scene(row.id);

  EXPECT_TRUE(scene.is_global());
}

TEST_F(HomeAssistantSceneTest, activating_a_scene_turns_it_on) {
  ScopedScene row("home_assistant", "Movie time", "scene.movie_time", kitchen_id);
  HomeAssistantScene scene(row.id);

  scene.activate(lounge_id);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0], nlohmann::json({{"type", "call_service"}, {"domain", "scene"}, {"service", "turn_on"}, {"target", {{"entity_id", "scene.movie_time"}}}}));
}

TEST_F(HomeAssistantSceneTest, activating_a_script_passes_the_rooms_to_it) {
  ScopedScene row("home_assistant", "Lights out", "script.lights_out", kitchen_id);
  HomeAssistantScene scene(row.id);

  scene.activate(lounge_id);

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["domain"], "script");
  EXPECT_EQ(calls[0]["service"], "turn_on");
  EXPECT_EQ(calls[0]["target"]["entity_id"], "script.lights_out");
  EXPECT_EQ(calls[0]["service_data"]["variables"]["nspanelmanager"], nlohmann::json({
                                                                         {"scene_name", "Lights out"},
                                                                         {"scene_id", row.id},
                                                                         {"triggering_room_id", lounge_id},
                                                                         {"triggering_room_name", "Scene test lounge"},
                                                                         {"scene_room_id", kitchen_id},
                                                                         {"scene_room_name", "Scene test kitchen"},
                                                                     }));
}

TEST_F(HomeAssistantSceneTest, global_script_without_triggering_room_only_names_the_scene) {
  ScopedScene row("home_assistant", "All off", "script.all_off", std::nullopt);
  HomeAssistantScene scene(row.id);

  scene.activate();

  auto calls = home_assistant_service_calls();
  ASSERT_EQ(calls.size(), 1);
  EXPECT_EQ(calls[0]["service_data"]["variables"]["nspanelmanager"], nlohmann::json({{"scene_name", "All off"}, {"scene_id", row.id}}));
}

TEST_F(HomeAssistantSceneTest, global_script_passes_the_triggering_room) {
  ScopedScene row("home_assistant", "All off", "script.all_off", std::nullopt);
  HomeAssistantScene scene(row.id);

  scene.activate(kitchen_id);

  auto context = home_assistant_service_calls().at(0)["service_data"]["variables"]["nspanelmanager"];
  EXPECT_EQ(context["triggering_room_id"], kitchen_id);
  EXPECT_EQ(context["triggering_room_name"], "Scene test kitchen");
  EXPECT_FALSE(context.contains("scene_room_id"));
}

TEST_F(HomeAssistantSceneTest, unknown_triggering_room_is_left_out) {
  ScopedScene row("home_assistant", "Lights out", "script.lights_out", kitchen_id);
  HomeAssistantScene scene(row.id);

  scene.activate(999999);

  auto context = home_assistant_service_calls().at(0)["service_data"]["variables"]["nspanelmanager"];
  EXPECT_FALSE(context.contains("triggering_room_id"));
  EXPECT_FALSE(context.contains("triggering_room_name"));
  EXPECT_EQ(context["scene_room_id"], kitchen_id);
}

TEST_F(HomeAssistantSceneTest, unsupported_entity_sends_nothing) {
  ScopedScene row("home_assistant", "Porch", "light.porch", kitchen_id);
  HomeAssistantScene scene(row.id);

  scene.activate(kitchen_id);

  EXPECT_TRUE(home_assistant_service_calls().empty());
}

TEST_F(HomeAssistantSceneTest, reload_config_picks_up_database_changes) {
  ScopedScene row("home_assistant", "Movie time", "scene.movie_time", kitchen_id);
  HomeAssistantScene scene(row.id);

  auto db_row = database_manager::database.get<database_manager::Scene>(row.id);
  db_row.friendly_name = "Film night";
  db_row.backend_name = "scene.film_night";
  db_row.room_id = nullptr;
  database_manager::database.update(db_row);
  scene.reload_config();
  scene.activate();

  EXPECT_EQ(scene.get_name(), "Film night");
  EXPECT_TRUE(scene.is_global());
  EXPECT_EQ(home_assistant_service_calls().at(0)["target"]["entity_id"], "scene.film_night");
}
