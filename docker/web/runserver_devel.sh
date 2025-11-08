#!/bin/bash

if [ "$TZ" != "" ]; then
	echo "Setting /etc/timezone to $TZ"
	echo "$TZ" >/etc/timezone
fi
export TZ="$(cat /etc/timezone)"
echo "Will use timezone: $TZ"

echo "Creating backup of database"
cp /data/nspanelmanager.sqlite3 /data/nspanelmanager.sqlite3.backup_"$(date +%Y-%m-%d_%H:%M:%S)"
echo "Backup complete, checking and removing backup files if more than 5 exists"
ls -t /data/nspanelmanager_db.sqlite3.backup_* | sed -e '1,5d' | xargs -d '\n' rm

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
