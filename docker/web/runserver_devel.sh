#!/bin/bash

if [ "$TZ" != "" ]; then
	echo "Setting /etc/timezone to $TZ"
	echo "$TZ" >/etc/timezone
fi
export TZ="$(cat /etc/timezone)"
echo "Will use timezone: $TZ"

echo "Running migrations..."
cd nspanelmanager
/usr/local/bin/python manage.py migrate
cd ..

cp /etc/nginx/sites-templates/nspanelmanager_devel.template /etc/nginx/sites-enabled/nspanelmanager.conf
echo "Starting Nginx"
/etc/init.d/nginx restart

echo "Starting NSPanelManager web interface server"
cd nspanelmanager
while true; do
    ./manage.py runserver 0.0.0.0:8001
    sleep 0.5
done
