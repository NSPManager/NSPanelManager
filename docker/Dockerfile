FROM --platform=$BUILDPLATFORM node:20-alpine AS ui-builder
ARG BUILDTIME
# Commit of NSPanelManager-Web-UI to build. Pinned so that a given commit of this repo always builds
# the same UI; bump it to ship UI changes. Pass --build-arg WEB_UI_REF=main to build the latest.
ARG WEB_UI_REF=6f3eae63296e1875314641aa9f79eed54ae1408e
WORKDIR /ui-app

# Install git to clone your frontend repository
RUN apk add --no-cache git

# Clone the UI repo
RUN echo "Building UI at $BUILDTIME" && git clone https://github.com/NSPManager/NSPanelManager-Web-UI.git . && git checkout --detach "$WEB_UI_REF" # Use the echo to force a rebuild of this step.

# Install dependencies and build
RUN npm install
RUN npm run build

FROM --platform=$BUILDPLATFORM python:3.12.5-bookworm AS build
ARG TARGETPLATFORM
ARG BUILDPLATFORM
ARG no_mqttmanager_build
ARG IS_DEVEL
WORKDIR /MQTTManager
SHELL ["/bin/bash", "-c"]

# Update container
RUN apt-get update && apt-get -y upgrade

RUN echo "Running on $BUILDPLATFORM, building for $TARGETPLATFORM"
COPY build_scripts/ /build_scripts/
COPY MQTTManager/ /MQTTManager/

# Only build MQTTManager during Docker build if is is not a devel mode.
RUN if [ "$IS_DEVEL" != "yes" ]; then bash /build_scripts/install_build_tools.sh "${IS_DEVEL}"; fi
# The Conan package cache (core.cache:storage_path in install_build_tools.sh) is a BuildKit cache
# mount, so rebuilds reuse the dependencies instead of compiling them from source again, and CI can
# save it between runs. Only /MQTTManager/build is copied out of this stage.
RUN --mount=type=cache,id=nspm-conan,target=/MQTTManager/conan_cache,sharing=locked \
    if [ -z "$no_mqttmanager_build" ]; then /bin/bash /MQTTManager/compile_mqttmanager.sh --target-platform "$TARGETPLATFORM" --strip; else echo "Not building MQTTManager."; fi

FROM python:3.12.5-bookworm
ARG no_mqttmanager_build
ARG IS_DEVEL
WORKDIR /usr/src/app
SHELL ["/bin/bash", "-c"]

COPY build_scripts/ /build_scripts/
COPY web/ /usr/src/app/
COPY nginx/sites-enabled/ /etc/nginx/sites-enabled/
COPY nginx/sites-templates/ /etc/nginx/sites-templates/
COPY "HMI_files/" /usr/src/app/nspanelmanager/HMI_files/
COPY --from=build /MQTTManager/build /MQTTManager/build

# Update container
RUN apt-get update && apt-get -y upgrade

# Install software needed to build the manager
RUN if [ "$IS_DEVEL" == "yes" ]; then bash /build_scripts/install_build_tools.sh "${IS_DEVEL}"; fi

# Install ImageMagick to be able to process images and send them to the NSPanel
RUN apt-get install -y nginx libmagick++-dev
RUN pip install --no-cache-dir -r requirements.txt

EXPOSE 8000
COPY --from=ui-builder /ui-app/dist /var/www/html/webapp
CMD ["/bin/bash", "./run_uwsgi.sh"]
