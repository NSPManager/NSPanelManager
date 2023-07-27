#!/bin/bash

mkdir -p data

if [ ! -e "data/nspanelmanager_db.sqlite3" ] && [ -e "$(pwd)/web/nspanelmanager/db.sqlite3" ]; then
	cp "$(pwd)/web/nspanelmanager/db.sqlite3" "data/nspanelmanager_db.sqlite3"
fi

if [ ! -e "data/secret.key" ] && [ -e "$(pwd)/web/nspanelmanager/secret.key" ]; then
	cp "$(pwd)/web/nspanelmanager/secret.key" "data/secret.key"
fi

docker build -t nspanelmanager . && docker run --name nspanelmanager -v /etc/timezone:/etc/timezone:ro -v "$(pwd)/data/":"/data/" -d -p 8000:8000 -p 8001:8001 nspanelmanager
