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
RUN if [ "$IS_DEVEL" != "yes" ]; then apt-get update \
    && apt-get -y install cmake build-essential curl \
    && pip install conan; fi

RUN if [ "$IS_DEVEL" != "yes" ]; then conan profile detect --force && echo 'core.cache:storage_path=/MQTTManager/conan_cache/' > ~/.conan2/global.conf \
    && sed -i "s|cppstd=gnu14|cppstd=gnu17|g" /root/.conan2/profiles/default \
    && sed -i "s|build_type=Release|build_type=Debug|g" /root/.conan2/profiles/default; fi

RUN if [ -z "$no_mqttmanager_build" ]; then /bin/bash /MQTTManager/compile_mqttmanager.sh --target-platform "$TARGETPLATFORM" --strip; else echo "Not building MQTTManager."; fi

FROM python:3.12.5-bookworm
ARG no_mqttmanager_build
ARG IS_DEVEL
WORKDIR /usr/src/app
SHELL ["/bin/bash", "-c"]

COPY web/ /usr/src/app/
COPY nginx/sites-enabled/ /etc/nginx/sites-enabled/
COPY nginx/sites-templates/ /etc/nginx/sites-templates/
COPY --from=build /MQTTManager/build /MQTTManager/build

# Update container
RUN apt-get update && apt-get -y upgrade

# Install software needed to build the manager
RUN if [ "$IS_DEVEL" == "yes" ]; then apt-get install -y --no-install-recommends cmake build-essential gdb curl npm postgresql-client curl inotify-tools net-tools nginx build-essential protobuf-c-compiler \
    && pip install conan && conan profile detect --force \
    && echo 'core.cache:storage_path=/MQTTManager/conan_cache/' > ~/.conan2/global.conf \
    && sed -i "s|cppstd=gnu14|cppstd=gnu17|g" /root/.conan2/profiles/default \
    && sed -i "s|build_type=Release|build_type=Debug|g" /root/.conan2/profiles/default \
    && echo "alias ll='ls -lh --color=auto'" >> /etc/bash.bashrc \
    #&& rm -rf /var/lib/apt/lists/*
    ; fi

RUN pip install --no-cache-dir -r requirements.txt # Install python packages

EXPOSE 8000
CMD ["/bin/bash", "./run_uwsgi.sh"]
