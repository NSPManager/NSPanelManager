#!/bin/bash
# Runs inside the dev stack's nspm container: rebuilds MQTTManager and restarts it.
#
#   rebuild-manager.sh          incremental build (CMake reconfigures itself if CMakeLists.txt changed)
#   rebuild-manager.sh --full   full compile_mqttmanager.sh run; needed after conanfile.py changes
#   rebuild-manager.sh --watch  rebuild on every change under include/, src/ or CMakeLists.txt

BUILD_DIR=/MQTTManager/build

build() {
  if [ "$1" == "--full" ] || [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
    /MQTTManager/compile_mqttmanager.sh || return 1
  else
    BUILD_TYPE=$(grep -E "^build_type=" /root/.conan2/profiles/default | cut -d'=' -f 2)
    (cd "$BUILD_DIR" && source "$BUILD_TYPE/generators/conanbuild.sh" && cmake --build . -j "$(nproc)") || return 1
  fi
}

rebuild_and_restart() {
  local started=$SECONDS
  if build "$1"; then
    curl -fsS -o /dev/null http://localhost:8000/api/restart_mqtt_manager &&
      echo "--> MQTTManager rebuilt and restarted in $((SECONDS - started))s."
  else
    echo "--> MQTTManager build FAILED; the previous build is still running."
    return 1
  fi
}

if [ "$1" == "--watch" ]; then
  echo "--> Watching MQTTManager sources. Ctrl-C to stop."
  rebuild_and_restart
  while inotifywait -qq -r -e close_write,create,delete,move /MQTTManager/include /MQTTManager/src /MQTTManager/CMakeLists.txt; do
    # Editors often write several files at once; let them finish before building.
    sleep 1
    rebuild_and_restart
  done
else
  rebuild_and_restart "$1"
fi
