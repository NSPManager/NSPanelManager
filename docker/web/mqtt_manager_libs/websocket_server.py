import asyncio
import datetime
import random
import websockets
import logging
from threading import Thread

CONNECTIONS = set()

def send_message(raw_data):
    websockets.broadcast(CONNECTIONS, raw_data)

async def _register_connection(websocket):
    CONNECTIONS.add(websocket)
    try:
        await websocket.wait_closed()
    finally:
        CONNECTIONS.remove(websocket)

async def _run_server():
    async with websockets.serve(_register_connection, host=None, port=8001):
        await asyncio.Future()  # run forever

def _main():
    logging.info("Starting websocket server")
    asyncio.run(_run_server())
    logging.info("Stopping websocket server")

def start_server():
    Thread(target=_main, daemon=True).start()
