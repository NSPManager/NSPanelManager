#!/bin/bash
# Starts NSPanelManager in the dev stack (see compose.yaml). Seeds a fresh database, then
# runs the devel web server, which starts MQTTManager.
#
# The build volume is shared by every checkout that runs the stack, so the binary in it may
# have been built from other sources. An existing build is therefore brought up to date
# before the web server starts it; with nothing changed that takes seconds. With no build
# yet, or one the incremental build can't update, the full build (slow the first time, as it
# also builds all dependencies without a shared Conan cache) runs in the background so the
# web UI is up meanwhile, and starts MQTTManager when it finishes.

cd /usr/src/app/nspanelmanager || exit 1
python manage.py migrate --noinput || exit 1
python manage.py shell </dev-stack/seed.py || exit 1

BUILD_DIR=/MQTTManager/build

incremental_build() {
  [ -f "$BUILD_DIR/CMakeCache.txt" ] || return 1
  BUILD_TYPE=$(grep -E "^build_type=" /root/.conan2/profiles/default | cut -d'=' -f 2)
  (cd "$BUILD_DIR" && source "$BUILD_TYPE/generators/conanbuild.sh" && cmake --build . -j "$(nproc)")
}

if [ -x "$BUILD_DIR/nspm_mqttmanager" ]; then
  echo "--> Bringing the MQTTManager build up to date with this checkout."
  if ! incremental_build; then
    echo "--> The incremental build failed. Removing the old binary, so it isn't started, and running a full build."
    rm -f "$BUILD_DIR/nspm_mqttmanager"
  fi
fi

if [ ! -x "$BUILD_DIR/nspm_mqttmanager" ]; then
  echo "--> Building MQTTManager in the background."
  (
    if /MQTTManager/compile_mqttmanager.sh; then
      until curl -fs -o /dev/null http://localhost:8000/api/restart_mqtt_manager; do sleep 2; done
      echo "--> MQTTManager built and started."
    else
      echo "--> MQTTManager build FAILED. Fix the error and run: nspm-dev rebuild"
    fi
  ) &
fi

# The devel nginx config forwards Host without the port, so Django's CSRF origin check only
# passes on the origins CSRF_TRUSTED_ORIGINS lists (localhost:8000). Forward the port too, so
# saving works on any NSPM_DEV_PORT and from any address.
mkdir -p /etc/nginx/sites-templates
sed 's/proxy_set_header Host \$host;/proxy_set_header Host $http_host;/' /nspm-nginx-templates/nspanelmanager_devel.template >/etc/nginx/sites-templates/nspanelmanager_devel.template
grep -q 'proxy_set_header Host \$http_host;' /etc/nginx/sites-templates/nspanelmanager_devel.template || echo "--> WARNING: could not make nginx forward the port, saving may fail the CSRF check."

cd /usr/src/app || exit 1
exec bash ./runserver_devel.sh
