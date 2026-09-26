#!/bin/bash
# Build and run the MQTTManager unit tests in Docker.
#
#   ./run_tests.sh                                   # everything
#   ./run_tests.sh --gtest_filter='LightConfig.*'    # extra arguments go to the test binary
#
# Output is also written to tests/last_run.log. The first run builds all Conan dependencies
# and takes a while; later runs reuse the nspm-mqttmanager-conan and -build Docker volumes.
# To start from scratch: docker volume rm nspm-mqttmanager-conan nspm-mqttmanager-build
#
# NSPM_TEST_CONAN_CACHE and NSPM_TEST_BUILD_DIR override those volumes with another volume name
# or an absolute host path (CI uses a host directory so it can cache it between runs).
# NSPM_TEST_RESULTS_DIR, an absolute host path, also writes JUnit XML results there.
cd "$(dirname "$0")" || exit 1

IMAGE=nspm-mqttmanager-tests
LOG=tests/last_run.log
CONAN_CACHE=${NSPM_TEST_CONAN_CACHE:-nspm-mqttmanager-conan}
BUILD_DIR=${NSPM_TEST_BUILD_DIR:-nspm-mqttmanager-build}

RESULTS_MOUNT=()
if [ -n "${NSPM_TEST_RESULTS_DIR:-}" ]; then
  mkdir -p "$NSPM_TEST_RESULTS_DIR"
  RESULTS_MOUNT=(-v "$NSPM_TEST_RESULTS_DIR":/results)
  set -- --gtest_output=xml:/results/mqttmanager-tests.xml "$@"
fi

{
  docker build -q -t "$IMAGE" tests/docker &&
    docker run --rm \
      -v "$PWD":/src:ro \
      -v "$CONAN_CACHE":/root/.conan2/p \
      -v "$BUILD_DIR":/MQTTManager/build \
      "${RESULTS_MOUNT[@]}" \
      "$IMAGE" bash /src/tests/docker/run_in_container.sh "$@"
} 2>&1 | tee "$LOG"
status=${PIPESTATUS[0]}

# When run with sudo, hand the log back to the invoking user.
if [ -n "${SUDO_UID:-}" ]; then
  chown "$SUDO_UID:$SUDO_GID" "$LOG"
fi
exit "$status"
