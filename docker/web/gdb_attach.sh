#!/bin/bash

gdb /MQTTManager/build/nspm_mqttmanager $(pgrep -f "/nspm_mqttmanager") -ex "handle SIGUSR1 nostop pass" -ex "handle SIGPIPE nostop pass" -ex "catch throw" -ex "continue"
