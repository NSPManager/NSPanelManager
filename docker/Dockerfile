FROM --platform=$BUILDPLATFORM node:20-alpine AS ui-builder
# Branch, tag or full commit hash of NSPanelManager-Web-UI to build, e.g. --build-arg WEB_UI_REF=devel.
ARG WEB_UI_REF=main
WORKDIR /ui-app

# Install git to clone your frontend repository
RUN apk add --no-cache git

# Clone the UI repo. The ADD fetches the commit WEB_UI_REF points at, so the clone and the UI
# build below rerun when that changes and are reused from the cache otherwise.
ADD https://api.github.com/repos/NSPManager/NSPanelManager-Web-UI/commits/${WEB_UI_REF} /ui-version.json
RUN git clone https://github.com/NSPManager/NSPanelManager-Web-UI.git . && git fetch origin "$WEB_UI_REF" && git checkout --detach FETCH_HEAD

# Install dependencies and build
RUN npm install
RUN npm run build

# Builds MQTTManager for the release image. The devel target does not use this stage, so
# docker build --target devel skips it.
FROM --platform=$BUILDPLATFORM python:3.12.5-bookworm AS build
ARG TARGETPLATFORM
ARG BUILDPLATFORM
WORKDIR /MQTTManager
SHELL ["/bin/bash", "-c"]

# Update container
RUN apt-get update && apt-get -y upgrade

RUN echo "Running on $BUILDPLATFORM, building for $TARGETPLATFORM"
COPY build_scripts/ /build_scripts/
RUN bash /build_scripts/install_build_tools.sh no
COPY MQTTManager/ /MQTTManager/
# The Conan package cache (core.cache:storage_path in install_build_tools.sh) is a BuildKit cache
# mount, so rebuilds reuse the dependencies instead of compiling them from source again, and CI can
# save it between runs. Only /MQTTManager/build is copied out of this stage.
RUN --mount=type=cache,id=nspm-conan,target=/MQTTManager/conan_cache,sharing=locked \
    /bin/bash /MQTTManager/compile_mqttmanager.sh --target-platform "$TARGETPLATFORM" --strip

# What the devel and release images share. It contains no source, so editing the source
# does not invalidate it.
FROM python:3.12.5-bookworm AS runtime-base
WORKDIR /usr/src/app
SHELL ["/bin/bash", "-c"]

# Update container
RUN apt-get update && apt-get -y upgrade

# Install ImageMagick to be able to process images and send them to the NSPanel
RUN apt-get install -y nginx libmagick++-dev
COPY web/requirements.txt /tmp/requirements.txt
RUN pip install --no-cache-dir -r /tmp/requirements.txt

EXPOSE 8000
COPY --from=ui-builder /ui-app/dist /var/www/html/webapp

# Development image, built with --target devel (see docker-build_and_run_dev.sh). It has the
# tools to build MQTTManager but no source: web/, MQTTManager/, HMI_files/ and the nginx
# files are mounted in at runtime.
FROM runtime-base AS devel
# Install software needed to build the manager. This also runs npm install for the react
# components, which only needs their package files.
COPY build_scripts/ /build_scripts/
COPY web/nspanelmanager/web/react-components/package.json web/nspanelmanager/web/react-components/package-lock.json /usr/src/app/nspanelmanager/web/react-components/
RUN bash /build_scripts/install_build_tools.sh yes
CMD ["/bin/bash", "./runserver_devel.sh"]

# Release image. This must stay the last stage: it is what a plain docker build, and the
# Home Assistant builder in release.yml, build.
FROM runtime-base AS release
COPY web/ /usr/src/app/
COPY nginx/sites-enabled/ /etc/nginx/sites-enabled/
COPY nginx/sites-templates/ /etc/nginx/sites-templates/
COPY "HMI_files/" /usr/src/app/nspanelmanager/HMI_files/
COPY --from=build /MQTTManager/build /MQTTManager/build
CMD ["/bin/bash", "./run_uwsgi.sh"]
