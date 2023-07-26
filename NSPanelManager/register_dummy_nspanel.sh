#!/bin/bash

which curl &>/dev/null
if [ "$?" -gt 0 ]; then
    echo "This script requieres curl. Will exit."
    exit 1
fi

read -p "Enter NSPanel friendly name: " friendly_name
mac_address="$(printf '00:60:2F:%02X:%02X:%02X\n' $[RANDOM%256] $[RANDOM%256] $[RANDOM%256])"
echo "Registering '$friendly_name' with MAC: $mac_address"

curl --location --request POST '127.0.0.1:8000/api/register_nspanel' --form 'mac_address="'$mac_address'"' --form 'friendly_name="'$friendly_name'"'