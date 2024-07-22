#ifndef MQTTMANAGER_PIPE_HANDLER
#define MQTTMANAGER_PIPE_HANDLER
#include <thread>
#define MQTTMANAGER_PIPE_HANDLER_PIPE_PATH "/mqttmanager_command_pipe"

/*
 * The PipeHandler manage the named pipe that is used to send commands from the Django web interface to
 * MQTTManager. This includes but are not limited to things like "Delete NSPanel with ID 2" and so on.
 * This is to avoid having to do a full config reload every time something has changed.
 */
class PipeHandler {
public:
  /*
   * Start the pipe handler an begin listening to command over the named pipe
   */
  static void start();

private:
  /*
   * Begin listening for commands over the named pipe
   */
  static void _listen();

  static inline std::thread _instance;
};

#endif // !MQTTMANAGER_PIPE_HANDLER
