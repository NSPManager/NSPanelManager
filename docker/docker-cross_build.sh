#!/bin/bash

if [ -z "$1 "]; then
  echo "ERROR! You need to set a target architecture to build for."
  exit 1
fi

docker buildx build --platform "$TARGETPLATFORM" -t nspanelmanager . && echo "Build successfull." || echo "Build failed!"
