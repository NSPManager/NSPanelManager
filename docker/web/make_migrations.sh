#!/bin/bash

cd nspanelmanager/
./manage.py makemigrations

echo "Killing MQTTManager"
../kill_mqttmanager.sh
