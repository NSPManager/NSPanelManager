#ifndef MQTTMANAGER_COMMAND_MANAGER
#define MQTTMANAGER_COMMAND_MANAGER

#include <string>
class CommandManager {
public:
  static void init();
  static void process_command(const std::string &topic, const std::string &payload);

private:
};

#endif // !MQTTMANAGER_COMMAND_MANAGER
