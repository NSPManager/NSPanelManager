#!/bin/bash
# Build and run the MQTTManager unit tests in Docker.
#
#   ./run_tests.sh                                   # everything
#   ./run_tests.sh --gtest_filter='LightConfig.*'    # extra arguments go to the test binary
#
# Output is also written to tests/last_run.log. The first run builds all Conan dependencies
# and takes a while; later runs reuse the nspm-mqttmanager-conan and -build Docker volumes.
# To start from scratch: docker volume rm nspm-mqttmanager-conan nspm-mqttmanager-build
cd "$(dirname "$0")" || exit 1

IMAGE=nspm-mqttmanager-tests
LOG=tests/last_run.log

{
  docker build -q -t "$IMAGE" tests/docker &&
    docker run --rm \
      -v "$PWD":/src:ro \
      -v nspm-mqttmanager-conan:/root/.conan2/p \
      -v nspm-mqttmanager-build:/MQTTManager/build \
      "$IMAGE" bash /src/tests/docker/run_in_container.sh "$@"
} 2>&1 | tee "$LOG"
status=${PIPESTATUS[0]}

# When run with sudo, hand the log back to the invoking user.
if [ -n "${SUDO_UID:-}" ]; then
  chown "$SUDO_UID:$SUDO_GID" "$LOG"
fi
exit "$status"
