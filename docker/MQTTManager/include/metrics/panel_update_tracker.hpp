#ifndef MQTT_MANAGER_METRICS_PANEL_UPDATE_TRACKER_HPP
#define MQTT_MANAGER_METRICS_PANEL_UPDATE_TRACKER_HPP

#include <array>
#include <chrono>
#include <cstddef>
#include <optional>
#include <vector>

/**
 * Follows one panel's OTA updates from the states it reports, to tell when an update started,
 * how long it ran and whether it worked.
 *
 * The panel never reports an outcome, so it is inferred. An update that fails returns the panel to
 * ONLINE without rebooting. One that works reboots it and, once back, it reports the md5 of the
 * image the manager has staged. After a firmware update that takes two boots: the first still
 * reports the old firmware md5 (the new one is promoted during that boot, which then reboots again),
 * so success waits for the md5 rather than for the first ONLINE. If the md5 never comes (eg. the
 * firmware rolled back), the update counts as failed after `verdict_timeout`.
 */
class PanelUpdateTracker {
public:
  using clock = std::chrono::steady_clock;

  enum class Type : size_t {
    FIRMWARE = 0,
    LITTLEFS = 1,
    TFT = 2,
  };
  static constexpr size_t type_count = 3;

  // The panel states that matter here.
  enum class Phase {
    ONLINE,
    UPDATING_FIRMWARE,
    UPDATING_LITTLEFS,
    UPDATING_TFT,
    DOWN, // Rebooting, offline or waiting for the panel to come back.
    OTHER,
  };

  struct Event {
    enum class Kind {
      STARTED,
      FINISHED, // Left the updating state; `seconds` is how long it was in it.
      SUCCEEDED,
      FAILED,
    };
    Type type;
    Kind kind;
    double seconds = 0;

    bool operator==(const Event &other) const = default;
  };

  explicit PanelUpdateTracker(clock::duration verdict_timeout = std::chrono::minutes(15))
      : _verdict_timeout(verdict_timeout) {}

  /**
   * The panel is in `phase` at `now`.
   * @param is_status_report: The phase came from a status report, which also carries the md5s in `matches_staged`.
   * @param matches_staged: Per Type, whether the md5 the panel runs equals the one the manager has staged.
   */
  std::vector<Event> observe(Phase phase, bool is_status_report, const std::array<bool, type_count> &matches_staged, clock::time_point now = clock::now()) {
    std::vector<Event> events;
    std::optional<Type> updating = PanelUpdateTracker::_updating_type(phase);

    if (this->_active && (!updating || *updating != this->_active->type)) {
      Type type = this->_active->type;
      events.push_back({.type = type, .kind = Event::Kind::FINISHED, .seconds = std::chrono::duration<double>(now - this->_active->started).count()});
      this->_pending[static_cast<size_t>(type)] = Pending{.rebooted = false, .left_at = now};
      this->_active.reset();
    }

    if (updating && !this->_active) {
      auto &pending = this->_pending[static_cast<size_t>(*updating)];
      if (pending) {
        // Started again before the last attempt was confirmed: that one didn't work.
        events.push_back({.type = *updating, .kind = Event::Kind::FAILED});
        pending.reset();
      }
      this->_active = Active{.type = *updating, .started = now};
      events.push_back({.type = *updating, .kind = Event::Kind::STARTED});
    }

    for (size_t i = 0; i < type_count; i++) {
      auto &pending = this->_pending[i];
      if (!pending) {
        continue;
      }
      if (phase == Phase::DOWN) {
        pending->rebooted = true;
      }
      if (is_status_report) {
        if (matches_staged[i] && pending->rebooted) {
          events.push_back({.type = static_cast<Type>(i), .kind = Event::Kind::SUCCEEDED});
          pending.reset();
          continue;
        } else if (phase == Phase::ONLINE && !pending->rebooted) {
          events.push_back({.type = static_cast<Type>(i), .kind = Event::Kind::FAILED});
          pending.reset();
          continue;
        }
      }
      if (now - pending->left_at > this->_verdict_timeout) {
        events.push_back({.type = static_cast<Type>(i), .kind = Event::Kind::FAILED});
        pending.reset();
      }
    }
    return events;
  }

  // Whether an update is running or waiting for its outcome, ie. whether observe() needs matches_staged.
  bool tracking() const {
    if (this->_active) {
      return true;
    }
    for (const auto &pending : this->_pending) {
      if (pending) {
        return true;
      }
    }
    return false;
  }

  static const char *type_label(Type type) {
    switch (type) {
    case Type::FIRMWARE:
      return "firmware";
    case Type::LITTLEFS:
      return "littlefs";
    case Type::TFT:
      return "tft";
    }
    return "unknown";
  }

  static const char *kind_label(Event::Kind kind) {
    switch (kind) {
    case Event::Kind::STARTED:
      return "started";
    case Event::Kind::FINISHED:
      return "finished";
    case Event::Kind::SUCCEEDED:
      return "succeeded";
    case Event::Kind::FAILED:
      return "failed";
    }
    return "unknown";
  }

private:
  struct Active {
    Type type;
    clock::time_point started;
  };
  struct Pending {
    bool rebooted;
    clock::time_point left_at;
  };

  static std::optional<Type> _updating_type(Phase phase) {
    switch (phase) {
    case Phase::UPDATING_FIRMWARE:
      return Type::FIRMWARE;
    case Phase::UPDATING_LITTLEFS:
      return Type::LITTLEFS;
    case Phase::UPDATING_TFT:
      return Type::TFT;
    default:
      return std::nullopt;
    }
  }

  std::optional<Active> _active;
  std::array<std::optional<Pending>, type_count> _pending;
  clock::duration _verdict_timeout;
};

#endif // !MQTT_MANAGER_METRICS_PANEL_UPDATE_TRACKER_HPP
