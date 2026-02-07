#ifndef MQTTMANAGER_NEXTION_IMAGE_SERVER_HPP
#define MQTTMANAGER_NEXTION_IMAGE_SERVER_HPP

#include <ixwebsocket/IXHttpServer.h>


class NextionImageServer
{
public:
    /**
     * Start the websocket server and bind to 0.0.0.0:8003.
     */
    static void start();
private:
    static inline ix::HttpServer *_server;
};

#endif