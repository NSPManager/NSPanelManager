import zmq
import logging
import json
import os
import psutil

"""
    Send data to IPC socket for ZeroMQ that MQTTManager is listening on.
    @param topic: The handler topic to send the data to.
    @param data: A dict of data to send along with the request.
    @return: Dict with decoded JSON response or a dict as follows: {'result': 'error'}.
"""
def send_ipc_request(topic: str, data: dict) -> dict:
    data["topic"] = topic; # This is simply passed in as a paramter to make it mandatory

    context = zmq.Context()
    logging.debug("Trying to connect to ZMQ socket for MQTTManager...")
    socket = context.socket(zmq.REQ)
    socket.setsockopt(zmq.RCVTIMEO, 100) # Allow 100ms before timeout
    socket.setsockopt(zmq.LINGER, 0) # Allow 50ms before timeout
    socket.connect("tcp://127.0.0.1:7999")

    try:
        json_data = json.dumps(data).encode()
        # print(F"Sending IPC request data: {json_data}")
        socket.send(json_data)
        message = socket.recv().decode()
        logging.debug(F"Received IPC data: {message}")
        return json.loads(message)
    except Exception as ex:
        logging.error("Received exception while trying to communicate with MQTTManager. Will return Error dict: {'result': 'error'}. Exception:")
        logging.exception(ex)
        return {"status": "error"}
