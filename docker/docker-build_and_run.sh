#!/bin/bash

docker build -t nspanelmanager . && docker run --name nspanelmanager -v $(pwd)/web/nspanelmanager/db.sqlite3:/usr/src/app/nspanelmanager/db.sqlite3 -d -p 8000:8000 nspanelmanager
