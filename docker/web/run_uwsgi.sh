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

cp /etc/nginx/sites-templates/nspanelmanager_prod.template /etc/nginx/sites-enabled/nspanelmanager.conf
echo "Starting Nginx"
/etc/init.d/nginx restart

echo "Starting NSPanelManager web interface server"
uwsgi --chdir=/usr/src/app/nspanelmanager/ \
	--module=nspanelmanager.wsgi:application \
	--env DJANGO_SETTINGS_MODULE=nspanelmanager.settings \
	--master --pidfile=/tmp/project-master.pid \
	--buffer-size 16384 \
	--socket=127.0.0.1:8001 --processes=5 --harakiri=20 --max-requests=5000 --vacuum \
	--touch-reload ./web/templates/index.html
