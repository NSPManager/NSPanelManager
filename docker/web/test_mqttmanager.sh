#!/bin/bash

echo "Calling compile_mqttmanager --test $@"
/MQTTManager/compile_mqttmanager.sh --test $@ && echo "Running MQTTMAnager in unit test mode" && /MQTTManager/build/nspm_mqttmanager
