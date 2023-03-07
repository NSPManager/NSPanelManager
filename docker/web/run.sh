#!/bin/bash

if [ ! -f "nspanelmanager/mqtt_manager.json" ]; then
    cp nspanelmanager/mqtt_manager.json.example nspanelmanager/mqtt_manager.json
fi

echo "Starting mqtt_manager.py in background"
./mqtt_manager.py &

cd nspanelmanager

echo "Running migrations..."
./manage.py migrate

echo "Starting server"
./manage.py runserver 0.0.0.0:8000

echo "Stopping all python applications"
for app in "$(pgrep python)"; do
    kill $app
done