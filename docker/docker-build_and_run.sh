#!/bin/bash

if [ ! -e "$(pwd)/web/nspanelmanager/db.sqlite3" ]; then
	touch $(pwd)/web/nspanelmanager/db.sqlite3
fi

if [ ! -e "$(pwd)/web/nspanelmanager/secret.key" ]; then
	touch $(pwd)/web/nspanelmanager/secret.key
fi

docker build -t nspanelmanager . && docker run --name nspanelmanager -v /etc/timezone:/etc/timezone:ro -v "$(pwd)/web/nspanelmanager/secret.key":"/usr/src/app/nspanelmanager/secret.key" -v "$(pwd)/web/nspanelmanager/db.sqlite3":"/usr/src/app/nspanelmanager/db.sqlite3" -d -p 8000:8000 -p 8001:8001 nspanelmanager
