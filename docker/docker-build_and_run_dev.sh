#!/bin/bash

set -x

export DOCKER_BUILDKIT=1

TARGETPLATFORM=""
CONFIG_SEED_PATH="$(pwd)"
CONFIG_SEED_PATH_SET=0
CONTAINER_NAME="nspanelmanager"
EXTERNAL_PORT=8000

while true; do
  case "$1" in
  --target-platform)
    TARGETPLATFORM="$2"
    shift
    shift
    ;;
  --external-port)
    EXTERNAL_PORT="$2"
    shift
    shift
    ;;
  --config-seed-path)
    CONFIG_SEED_PATH="$2"
    CONFIG_SEED_PATH_SET=1
    shift
    shift
    ;;
  --container-name)
    CONTAINER_NAME="$2"
    shift
    shift
    ;;
  *) break ;;
  esac
done

if [ "$CONFIG_SEED_PATH_SET" == 1 ]; then
  for f in db.sqlite3 secret.key; do
    if [ ! -e "${CONFIG_SEED_PATH}/web/nspanelmanager/${f}" ]; then
      echo "ERROR: --config-seed-path given but ${CONFIG_SEED_PATH}/web/nspanelmanager/${f} does not exist." >&2
      exit 1
    fi
  done
  if [ -e "data/nspanelmanager_db.sqlite3" ] || [ -e "data/secret.key" ]; then
    echo "WARNING: data/ already contains a database or secret key; existing files will not be overwritten from --config-seed-path." >&2
  fi
fi

mkdir -p data

if [ ! -e "data/nspanelmanager_db.sqlite3" ] && [ -e "${CONFIG_SEED_PATH}/web/nspanelmanager/db.sqlite3" ]; then
  cp "${CONFIG_SEED_PATH}/web/nspanelmanager/db.sqlite3" "data/nspanelmanager_db.sqlite3"
fi

if [ ! -e "data/secret.key" ] && [ -e "${CONFIG_SEED_PATH}/web/nspanelmanager/secret.key" ]; then
  cp "${CONFIG_SEED_PATH}/web/nspanelmanager/secret.key" "data/secret.key"
fi

mkdir -p MQTTManager/build/

echo "--> Running as dev container. Will compile for platform '${TARGETPLATFORM}'. Will not compile MQTTManager during docker build."

if [ -z "$TARGETPLATFORM" ]; then
  docker build --build-arg no_mqttmanager_build=yes --build-arg IS_DEVEL=yes --build-arg BUILDTIME="$(date)" -t nspanelmanager .
else
  docker buildx build --platform "$TARGETPLATFORM" --build-arg no_mqttmanager_build=yes --build-arg IS_DEVEL=yes --build-arg BUILDTIME="$(date)" -t nspanelmanager .
fi
if [ "$?" == 0 ]; then
  #docker run --rm --name ${CONTAINER_NAME} --cap-add=SYS_PTRACE --security-opt seccomp=unconfined --mac-address 02:42:ac:11:ff:ff -it -v /etc/timezone:/etc/timezone:ro -v "$(pwd)/web":/usr/src/app/ -v "$(pwd)/data":/data/ -v "$(pwd)/MQTTManager/":/MQTTManager/ -v "$(pwd)/nginx/sites-templates/":/etc/nginx/sites-templates/ -v "$(pwd)/nginx/sites-enabled/":/etc/nginx/sites-enabled/ -v "$(pwd)/HMI_files/":/usr/src/app/nspanelmanager/HMI_files/ -v "$(pwd)/../":/full_git/ -p 8000:8000 nspanelmanager /bin/bash
  docker run --rm --name ${CONTAINER_NAME} --cap-add=SYS_PTRACE --security-opt seccomp=unconfined --mac-address 02:42:ac:11:ff:ff -it -v /etc/localtime:/etc/localtime:ro -v "$(pwd)/web":/usr/src/app/ -v "$(pwd)/data":/data/ -v "$(pwd)/MQTTManager/":/MQTTManager/ -v "$(pwd)/nginx/sites-templates/":/etc/nginx/sites-templates/ -v "$(pwd)/nginx/sites-enabled/":/etc/nginx/sites-enabled/ -v "$(pwd)/HMI_files/":/usr/src/app/nspanelmanager/HMI_files/ -v "$(pwd)/../":/full_git/ -p ${EXTERNAL_PORT}:8000 nspanelmanager /bin/bash
  docker rmi nspanelmanager
fi
