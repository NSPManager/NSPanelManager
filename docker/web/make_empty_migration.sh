#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <migration_name>"
    exit 1
fi

cd nspanelmanager/
./manage.py makemigrations web -n "$1" --empty && chmod a+rw "/usr/src/app/nspanelmanager/web/migrations/*_$1.py"

echo "Killing MQTTManager"
../kill_mqttmanager.sh
