#!/bin/bash

backup_dir="data-$(date '+%Y-%m-%d-%H-%M-%S')"

echo "--> Creating backup of data dir to $backup_dir"
rsync -avhP data/ "$backup_dir"

echo "--> Stopping and deleting old nspanelmanager container"
docker rm -f nspanelmanager

read -p "Pull new beta image? (y/N) " pull
if [ "$pull" == "y" ]; then
	docker rmi nspanelmanager/nspanelmanager-beta-amd64
else
	echo "Will not pull new beta image"
fi

echo "--> Starting new container (beta)"
docker run --name nspanelmanager -v /etc/timezone:/etc/timezone:ro -v "$(pwd)/data/":"/data/" -d -p 8000:8000 nspanelmanager/nspanelmanager-beta-amd64
