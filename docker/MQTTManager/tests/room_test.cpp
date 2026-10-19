#include "test_helpers.hpp"
#include <chrono>
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <memory>
#include <room/room.hpp>
#include <string>
#include <thread>

// Each Room runs a thread that sends its status once changes have settled. The thread must stop
// when the Room is destroyed, rather than go on using it.
class RoomStatusThreadTest : public nspm_test::SendCaptureTest {
protected:
  // A Room row that EntityManager has not loaded, so that the test owns the only Room for it.
  class ScopedRoomRow {
  public:
    explicit ScopedRoomRow(const std::string &friendly_name) {
      database_manager::Room room;
      room.friendly_name = friendly_name;
      id = database_manager::database.insert(room);
    }

    ~ScopedRoomRow() {
      database_manager::database.remove<database_manager::Room>(id);
    }

    ScopedRoomRow(const ScopedRoomRow &) = delete;
    ScopedRoomRow &operator=(const ScopedRoomRow &) = delete;

    int id;
  };

  static std::string state_topic(int room_id) {
    return fmt::format("nspanel/mqttmanager_{}/room/{}/state", MqttManagerConfig::get_setting_with_default<std::string>(MQTT_MANAGER_SETTING::MANAGER_ADDRESS), room_id);
  }

  static std::chrono::milliseconds backoff_time() {
    return std::chrono::milliseconds(MqttManagerConfig::get_setting_with_default<uint32_t>(MQTT_MANAGER_SETTING::ROOM_STATUS_BACKOFF_TIME));
  }
};

TEST_F(RoomStatusThreadTest, sends_one_status_after_a_burst_of_changes) {
  ScopedRoomRow row("Room thread test burst");
  auto room = std::make_unique<Room>(row.id);

  room->page_changed_callback(nullptr);
  room->page_changed_callback(nullptr);
  room->page_changed_callback(nullptr);
  std::this_thread::sleep_for(backoff_time() * 4);

  EXPECT_EQ(nspm_test::mqtt_published_to(state_topic(row.id)).size(), 1);
}

TEST_F(RoomStatusThreadTest, destroying_a_room_waiting_to_send_its_status_stops_its_thread) {
  ScopedRoomRow row("Room thread test destroyed");
  auto room = std::make_unique<Room>(row.id);

  // The thread is now waiting for the change to settle, and would send the status once it has.
  room->page_changed_callback(nullptr);
  auto destroy_started = std::chrono::steady_clock::now();
  room.reset();
  auto destroy_time = std::chrono::steady_clock::now() - destroy_started;

  std::this_thread::sleep_for(backoff_time() * 4);
  EXPECT_TRUE(nspm_test::mqtt_published_to(state_topic(row.id)).empty());
  // Stopping wakes the thread rather than waiting out the backoff. Generous, only to catch a hang.
  EXPECT_LT(destroy_time, std::chrono::seconds(1));
}

TEST_F(RoomStatusThreadTest, destroying_a_room_waiting_for_a_change_stops_its_thread) {
  ScopedRoomRow row("Room thread test idle");
  auto room = std::make_unique<Room>(row.id);

  // Send one status, so that the thread is back waiting for the next change.
  room->page_changed_callback(nullptr);
  std::this_thread::sleep_for(backoff_time() * 4);
  ASSERT_EQ(nspm_test::mqtt_published_to(state_topic(row.id)).size(), 1);

  // Nothing marks the room as changed, so only the stop request can end that wait.
  auto destroy_started = std::chrono::steady_clock::now();
  room.reset();
  auto destroy_time = std::chrono::steady_clock::now() - destroy_started;

  EXPECT_LT(destroy_time, std::chrono::seconds(1));
}

TEST_F(RoomStatusThreadTest, a_room_removed_from_the_database_is_unloaded) {
  int room_id = nspm_test::create_room("Room thread test removed");
  ASSERT_TRUE(EntityManager::get_room(room_id).has_value());

  database_manager::database.remove<database_manager::Room>(room_id);
  EntityManager::load_rooms();

  EXPECT_FALSE(EntityManager::get_room(room_id).has_value());
}
