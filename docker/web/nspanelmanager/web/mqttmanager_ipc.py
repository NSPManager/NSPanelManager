import zmq
import logging
import json

"""
    Send data to IPC socket for ZeroMQ that MQTTManager is listening on.
    @param topic: The handler topic to send the data to.
    @param data: A dict of data to send along with the request.
"""
def send_ipc_request(topic, data):
    data["topic"] = topic; # This is simply passed in as a paramter to make it mandatory

    context = zmq.Context()
    logging.info("Trying to connect to ZMQ socket for MQTTManager...")
    socket = context.socket(zmq.REQ)
    socket.connect("ipc:///tmp/mqttmanager_ipc")

    json_data = json.dumps(data).encode()
    socket.send(json_data)
    message = socket.recv()
    logging.debug(F"Received IPC data: {message}")
    return message
