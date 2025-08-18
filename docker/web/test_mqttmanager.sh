#!/bin/bash

echo "Calling compile_mqttmanager to compile for unit tests."
/MQTTManager/compile_mqttmanager.sh --test && echo "Running MQTTManager in test mode." && /MQTTManager/build/nspm_mqttmanager
