#!/bin/bash

echo "Running migrations..."
/usr/local/bin/python nspanelmanager/manage.py migrate

echo "Starting Nginx"
/etc/init.d/nginx start

echo "Starting NSPanelManager web interface server"
uwsgi --chdir=/usr/src/app/nspanelmanager/ \
	--module=nspanelmanager.wsgi:application \
	--env DJANGO_SETTINGS_MODULE=nspanelmanager.settings \
	--master --pidfile=/tmp/project-master.pid \
	--socket=127.0.0.1:8001 --processes=5 --harakiri=20 --max-requests=5000 --vacuum
