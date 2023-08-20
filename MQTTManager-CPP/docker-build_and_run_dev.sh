#!/bin/bash

docker build -t nspm_mqttmanager . && docker run --rm --name nspm_mqttmanager --mac-address 02:42:ac:11:ff:fe -it -v /etc/timezone:/etc/timezone:ro -v "$(pwd)":/usr/src/app/ -v "$(pwd)/data":/data/ nspm_mqttmanager /bin/bash && docker rmi nspm_mqttmanager
