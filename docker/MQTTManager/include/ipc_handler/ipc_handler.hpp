#ifndef MQTTMANAGER_PIPE_HANDLER
#define MQTTMANAGER_PIPE_HANDLER
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/signals2.hpp>
#include <thread>

/*
 * The IPCHandler manages the ZeroMQ communication channel that is used to send commands from the Django web interface to
 * MQTTManager. This includes but are not limited to things like "Delete NSPanel with ID 2" and so on.
 * This is to avoid having to do a full config reload every time something has changed.
 */
class IPCHandler {
public:
  /*
   * Start the IPC handler an begin listening to command over the named pipe
   */
  static void start();

  template <typename CALLBACK_BIND>
  static void attach_callback(std::string topic, CALLBACK_BIND callback) {
    IPCHandler::_registered_handlers[topic].connect(callback);
  }

  template <typename CALLBACK_BIND>
  static void detach_callback(std::string topic, CALLBACK_BIND callback) {
    IPCHandler::_registered_handlers[topic].disconnect(callback);
  }

private:
  /*
   * Begin listening for commands over ZeroMQ
   */
  static void _listen();

  static inline boost::ptr_map<std::string, boost::signals2::signal<bool(std::string message, std::string *response_buffer)>> _registered_handlers;
  static inline std::thread _instance; // The thread object of the running _listen() function.
};

#endif // !MQTTMANAGER_PIPE_HANDLER
