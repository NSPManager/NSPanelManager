#!/bin/bash

read -p "Pull new stable image? (y/N) " pull
if [ "$pull" == "y" ]; then
	docker rmi nspanelmanager/nspanelmanager-amd64
else
	echo "Will not pull new image"
fi

docker run --name nspanelmanager -v /etc/timezone:/etc/timezone:ro -v "$(pwd)/data/":"/data/" -d -p 8000:8000 -p 8001:8001 nspanelmanager/nspanelmanager-amd64
