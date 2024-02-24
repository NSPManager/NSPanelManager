FROM --platform=$BUILDPLATFORM python:3.11 AS build
ARG TARGETPLATFORM
ARG BUILDPLATFORM
ARG no_mqttmanager_build
WORKDIR /MQTTManager

RUN echo "Running on $BUILDPLATFORM, building for $TARGETPLATFORM"
COPY MQTTManager/ /MQTTManager/

RUN apt-get update \
    && apt-get -y install cmake build-essential curl \
    && pip install conan

RUN conan profile detect --force && echo 'core.cache:storage_path=/MQTTManager/conan_cache/' > ~/.conan2/global.conf \
    && sed -i "s|cppstd=gnu14|cppstd=gnu17|g" /root/.conan2/profiles/default \
    && sed -i "s|build_type=Release|build_type=Debug|g" /root/.conan2/profiles/default

#RUN if [ -z "$no_mqttmanager_build" ]; then /bin/bash /usr/src/app/make_mqttmanager.sh --platform "$TARGETPLATFORM" && /bin/bash /MQTTManager/cleanup_build.sh; else echo "Not building MQTTManager."; fi
#RUN if [ -z "$no_mqttmanager_build" ]; then /bin/bash /usr/src/app/make_mqttmanager.sh --platform "$TARGETPLATFORM" && /bin/bash /MQTTManager/cleanup_build.sh; else echo "Not building MQTTManager."; fi
RUN /bin/bash /MQTTManager/compile_mqttmanager.sh --target-platform "$TARGETPLATFORM"


FROM python:3.11
COPY web/ /usr/src/app/
COPY nginx/sites-enabled/ /etc/nginx/sites-enabled/
COPY --from=build /MQTTManager /MQTTManager
# TODO: Make Django use /MQTTManager....-path to access nspm_mqttmanager binary instead of copying it.
COPY --from=build /MQTTManager/build/nspm_mqttmanager /usr/src/app/nspm_mqttmanager

WORKDIR /usr/src/app

# Update container
RUN apt-get update \
	&& apt-get -y upgrade

# Install software needed to build and run the manager
RUN apt-get install -y --no-install-recommends \
		postgresql-client curl inotify-tools net-tools nginx build-essential
	# && rm -rf /var/lib/apt/lists/*

RUN pip install -r requirements.txt # Install python packages
#RUN /bin/bash /MQTTManager/install_cmake.sh # Install cmake from source as it's only available for x86_64 and armv8 in repo

RUN echo "alias ll='ls -lh --color=auto'" >> /etc/bash.bashrc

#RUN if [ -z "$no_mqttmanager_build" ]; then /bin/bash /usr/src/app/make_mqttmanager.sh && /bin/bash /MQTTManager/cleanup_build.sh; else echo "Not building MQTTManager."; fi

EXPOSE 8000
CMD ["/bin/bash", "./run_uwsgi.sh"]
