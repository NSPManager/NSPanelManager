#!/bin/bash
# Kill MQTTManager

kill -9 $(pgrep -f nspm_mqttmanager)
