#!/bin/bash

echo "Calling compile_mqttmanager --test $@"
/MQTTManager/compile_mqttmanager.sh --test $@ && /MQTTManager/build/nspm_mqttmanager
