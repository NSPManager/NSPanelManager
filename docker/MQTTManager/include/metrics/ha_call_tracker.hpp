#ifndef MQTT_MANAGER_METRICS_HA_CALL_TRACKER_HPP
#define MQTT_MANAGER_METRICS_HA_CALL_TRACKER_HPP

#include <chrono>
#include <cstddef>
#include <map>
#include <mutex>
#include <optional>
#include <string>

/**
 * Matches Home Assistant websocket requests to their results by message id, to time them.
 * Results for requests that are never answered (the connection dropped, Home Assistant restarted)
 * never come, so entries are pruned by age and the number kept is capped.
 * Thread-safe: requests are sent from any thread, results arrive on the websocket thread.
 */
class HaCallTracker {
public:
  using clock = std::chrono::steady_clock;

  struct Result {
    std::string domain;
    double seconds;
  };

  explicit HaCallTracker(clock::duration max_age = std::chrono::seconds(60), size_t max_entries = 1024)
      : _max_age(max_age), _max_entries(max_entries) {}

  // A request with this id was sent at `now`.
  void start(long id, const std::string &domain, clock::time_point now = clock::now()) {
    std::lock_guard<std::mutex> lock_guard(this->_mutex);
    // Ids only ever increase, so the map is ordered oldest first.
    while (!this->_pending.empty() && (now - this->_pending.begin()->second.started > this->_max_age || this->_pending.size() >= this->_max_entries)) {
      this->_pending.erase(this->_pending.begin());
    }
    this->_pending[id] = {.domain = domain, .started = now};
  }

  // The result for this id arrived at `now`. Empty if the id wasn't tracked (not a timed request, or pruned).
  std::optional<Result> finish(long id, clock::time_point now = clock::now()) {
    std::lock_guard<std::mutex> lock_guard(this->_mutex);
    auto it = this->_pending.find(id);
    if (it == this->_pending.end()) {
      return std::nullopt;
    }
    Result result = {.domain = std::move(it->second.domain), .seconds = std::chrono::duration<double>(now - it->second.started).count()};
    this->_pending.erase(it);
    return result;
  }

  // Forget every pending request, eg. when the connection is lost.
  void clear() {
    std::lock_guard<std::mutex> lock_guard(this->_mutex);
    this->_pending.clear();
  }

  size_t pending() {
    std::lock_guard<std::mutex> lock_guard(this->_mutex);
    return this->_pending.size();
  }

private:
  struct Pending {
    std::string domain;
    clock::time_point started;
  };

  std::mutex _mutex;
  std::map<long, Pending> _pending;
  clock::duration _max_age;
  size_t _max_entries;
};

#endif // !MQTT_MANAGER_METRICS_HA_CALL_TRACKER_HPP
