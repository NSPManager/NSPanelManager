#!/bin/bash

docker build -t nspanelmanager . && docker run --rm --name nspanelmanager -it -v "$(pwd)/web":/usr/src/app/ -p 8000:8000 -p 8001:8001 nspanelmanager /bin/bash && docker rmi nspanelmanager
