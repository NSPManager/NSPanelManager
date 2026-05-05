FROM --platform=$BUILDPLATFORM python:3.12.5-bookworm AS build
ARG TARGETPLATFORM
ARG BUILDPLATFORM
ARG no_mqttmanager_build
ARG IS_DEVEL
WORKDIR /MQTTManager
SHELL ["/bin/bash", "-c"]

RUN echo "Running on $BUILDPLATFORM, building for $TARGETPLATFORM"
COPY MQTTManager/ /MQTTManager/

# Only build MQTTManager during Docker build if is is not a devel mode.
RUN if [ "$IS_DEVEL" != "yes" ]; then bash /build_scripts/install_build_tools.sh "${IS_DEVEL}"; fi
RUN if [ -z "$no_mqttmanager_build" ]; then /bin/bash /MQTTManager/compile_mqttmanager.sh --target-platform "$TARGETPLATFORM" --strip; else echo "Not building MQTTManager."; fi

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
CMD ["/bin/bash", "./run_uwsgi.sh"]
