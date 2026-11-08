#ifndef MQTT_MANAGER_METRICS_DROP_LOG_LIMITER_HPP
#define MQTT_MANAGER_METRICS_DROP_LOG_LIMITER_HPP

#include <chrono>
#include <cstddef>
#include <fmt/format.h>
#include <map>
#include <string>
#include <vector>

/**
 * Decides what to log about MQTT messages dropped because the processing queue is full, so that a
 * burst of drops never logs per message:
 *  - a warning for the first drop after a quiet period,
 *  - while drops continue, one summary at most every `window`,
 *  - one info line once a whole `window` passes with no drops.
 * Not thread safe: used only by the MQTT receive thread, the queue's single producer.
 */
class DropLogLimiter {
public:
  using clock = std::chrono::steady_clock;

  struct Line {
    enum class Level {
      INFO,
      WARNING,
    };
    Level level;
    std::string text;

    bool operator==(const Line &other) const = default;
  };

  explicit DropLogLimiter(size_t queue_capacity, clock::duration window = std::chrono::seconds(10))
      : _queue_capacity(queue_capacity), _window(window) {}

  // A message of this topic class was dropped at `now`.
  std::vector<Line> on_drop(const std::string &topic_class, clock::time_point now = clock::now()) {
    std::vector<Line> lines = this->tick(now);
    this->_episode_total++;
    if (!this->_in_episode) {
      this->_in_episode = true;
      this->_window_start = now;
      this->_window_counts.clear();
      this->_window_total = 0;
      lines.push_back({.level = Line::Level::WARNING, .text = fmt::format("Dropped an MQTT message ({}): processing queue full ({}).", topic_class, this->_queue_capacity)});
    } else {
      this->_window_counts[topic_class]++;
      this->_window_total++;
    }
    return lines;
  }

  // Call regularly (at least about once a second) so the summary and the recovery line are logged on time.
  std::vector<Line> tick(clock::time_point now = clock::now()) {
    std::vector<Line> lines;
    if (!this->_in_episode || now - this->_window_start < this->_window) {
      return lines;
    }
    long window_seconds = std::chrono::duration_cast<std::chrono::seconds>(this->_window).count();
    if (this->_window_total > 0) {
      std::string by_class;
      for (const auto &[topic_class, count] : this->_window_counts) {
        if (!by_class.empty()) {
          by_class.append(", ");
        }
        by_class.append(fmt::format("{}: {}", topic_class, count));
      }
      lines.push_back({.level = Line::Level::WARNING, .text = fmt::format("Dropped {} MQTT messages in the last {} s ({}); processing queue full ({}).", this->_window_total, window_seconds, by_class, this->_queue_capacity)});
      this->_window_start = now;
      this->_window_counts.clear();
      this->_window_total = 0;
    } else {
      lines.push_back({.level = Line::Level::INFO, .text = fmt::format("No MQTT messages dropped in the last {} s; {} dropped in total since the queue filled up.", window_seconds, this->_episode_total)});
      this->_in_episode = false;
      this->_episode_total = 0;
    }
    return lines;
  }

private:
  size_t _queue_capacity;
  clock::duration _window;
  bool _in_episode = false;
  clock::time_point _window_start;
  std::map<std::string, size_t> _window_counts;
  size_t _window_total = 0;
  size_t _episode_total = 0;
};

#endif // !MQTT_MANAGER_METRICS_DROP_LOG_LIMITER_HPP
