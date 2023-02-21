#!/bin/bash

docker build -t nspanelmanager . && docker run --rm --name nspanelmanager -it -v ./web:/usr/src/app/ -p 8000:8000 nspanelmanager /bin/bash && docker rmi nspanelmanager
