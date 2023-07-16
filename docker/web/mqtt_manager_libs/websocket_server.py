import asyncio
import datetime
import random
import websockets
import logging
from threading import Thread
import json

CONNECTIONS = set()
MESSAGE_HANDLER = None

def send_message(raw_data):
    websockets.broadcast(CONNECTIONS, raw_data)

def register_message_handler(handler):
    global MESSAGE_HANDLER
    MESSAGE_HANDLER = handler

async def _connection_handler(websocket):
    CONNECTIONS.add(websocket)
    try:
        async for message in websocket:
            if MESSAGE_HANDLER is not None:
                try:
                    json_message = json.loads(message)
                    await MESSAGE_HANDLER(websocket, json_message)
                except:
                    logging.exception("Exception occured while trying to decode message from websocket.")
        await websocket.wait_closed()
    finally:
        CONNECTIONS.remove(websocket)

async def _run_server():
    async with websockets.serve(_connection_handler, host=None, port=8001):
        await asyncio.Future()  # run forever

def _main():
    #logging.info("Starting websocket server")
    asyncio.run(_run_server())
    #logging.info("Stopping websocket server")

def start_server():
    Thread(target=_main, daemon=True).start()
