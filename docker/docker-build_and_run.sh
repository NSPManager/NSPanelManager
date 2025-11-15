#!/bin/bash

mkdir -p data
NOSTRIP=""
PORT="8000"
TARGETPLATFORM=""
DATA_VOLUME="$(pwd)/data/"

while true; do
  case "$1" in
  --no-strip)
    NOSTRIP="1"
    shift
    ;;
  --port)
    PORT="$2"
    shift 2
    ;;
  --target-platform)
    TARGETPLATFORM="$2"
    shift 2
    ;;
  --data-volume)
    DATA_VOLUME="$2"
    shift 2
    ;;
  *) break ;;
  esac
done

if [ ! -e "data/nspanelmanager_db.sqlite3" ] && [ -e "$(pwd)/web/nspanelmanager/db.sqlite3" ]; then
	cp "$(pwd)/web/nspanelmanager/db.sqlite3" "data/nspanelmanager_db.sqlite3"
fi

if [ ! -e "data/secret.key" ] && [ -e "$(pwd)/web/nspanelmanager/secret.key" ]; then
	cp "$(pwd)/web/nspanelmanager/secret.key" "data/secret.key"
fi

if [ -z "$TARGETPLATFORM" ]; then
  docker build -t nspanelmanager .
else
  docker buildx build --platform "$TARGETPLATFORM" -t nspanelmanager .
fi

if [ "$?" == 0 ]; then
  docker run --name nspanelmanager -v /etc/timezone:/etc/timezone:ro -v "${DATA_VOLUME}":"/data/" -d -p ${PORT}:8000 -p 8001:8001 nspanelmanager
fi
