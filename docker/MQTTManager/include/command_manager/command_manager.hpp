#ifndef MQTTMANAGER_COMMAND_MANAGER
#define MQTTMANAGER_COMMAND_MANAGER

#include "protobuf_nspanel.pb.h"
#include <boost/signals2/signal.hpp>
#include <string>
class CommandManager {
public:
  static void init();
  static void process_command(const std::string &topic, const std::string &payload);

  template <typename CALLBACK_BIND>
  static void attach_callback(CALLBACK_BIND callback) {
    CommandManager::_callbacks.connect(callback);
  }

  template <typename CALLBACK_BIND>
  static void detach_callback(CALLBACK_BIND callback) {
    CommandManager::_callbacks.disconnect(callback);
  }

private:
    static inline boost::signals2::signal<void(NSPanelMQTTManagerCommand &command)> _callbacks;
};

#endif // !MQTTMANAGER_COMMAND_MANAGER
