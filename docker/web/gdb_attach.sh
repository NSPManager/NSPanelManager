#!/bin/bash

gdb /MQTTManager/build/nspm_mqttmanager $(pgrep -f "/nspm_mqttmanager") -ex "handle SIGUSR1 nostop" -ex "handle SIGPIPE nostop" -ex "continue"
