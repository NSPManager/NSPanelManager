#!/bin/bash

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