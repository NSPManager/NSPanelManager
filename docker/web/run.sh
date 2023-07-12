#!/bin/bash

echo "Starting mqtt_manager.py in background"
/usr/local/bin/python ./mqtt_manager.py &

cd nspanelmanager

echo "Running migrations..."
/usr/local/bin/python ./manage.py migrate

echo "Starting server"
/usr/local/bin/python ./manage.py runserver 0.0.0.0:8000

echo "Stopping all python applications"
for app in "$(pgrep python)"; do
	kill $app
done
