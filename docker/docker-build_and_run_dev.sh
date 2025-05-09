#!/bin/bash

TARGETPLATFORM=""

while true; do
  case "$1" in
  --target-platform)
    TARGETPLATFORM="$2"
    shift
    ;;
  *) break ;;
  esac
done

mkdir -p data

if [ ! -e "data/nspanelmanager_db.sqlite3" ] && [ -e "$(pwd)/web/nspanelmanager/db.sqlite3" ]; then
  cp "$(pwd)/web/nspanelmanager/db.sqlite3" "data/nspanelmanager_db.sqlite3"
fi

if [ ! -e "data/secret.key" ] && [ -e "$(pwd)/web/nspanelmanager/secret.key" ]; then
  cp "$(pwd)/web/nspanelmanager/secret.key" "data/secret.key"
fi

mkdir -p MQTTManager/build/

echo "--> Running as dev container. Will compile for platform '${TARGETPLATFORM}'. Will not compile MQTTManager during docker build."

if [ -z "$TARGETPLATFORM" ]; then
  docker build --build-arg no_mqttmanager_build=yes --build-arg IS_DEVEL=yes -t nspanelmanager .
else
  docker buildx build --platform "$TARGETPLATFORM" --build-arg no_mqttmanager_build=yes --build-arg IS_DEVEL=yes -t nspanelmanager .
fi
if [ "$?" == 0 ]; then
  docker run --rm --name nspanelmanager --cap-add=SYS_PTRACE --security-opt seccomp=unconfined --mac-address 02:42:ac:11:ff:ff -it -v /etc/timezone:/etc/timezone:ro -v "$(pwd)/web":/usr/src/app/ -v "$(pwd)/data":/data/ -v "$(pwd)/MQTTManager/":/MQTTManager/ -v "$(pwd)/nginx/sites-templates/":/etc/nginx/sites-templates/ -v "$(pwd)/nginx/sites-enabled/":/etc/nginx/sites-enabled/ -v "$(pwd)/../":/full_git/ -p 8000:8000 nspanelmanager /bin/bash
  docker rmi nspanelmanager
fi
