FROM --platform=$BUILDPLATFORM python:3.11 AS build
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

RUN if [ -z "$no_mqttmanager_build" ]; then /bin/bash /MQTTManager/compile_mqttmanager.sh --target-platform "$TARGETPLATFORM"; else echo "Not building MQTTManager."; fi


FROM python:3.11
ARG no_mqttmanager_build
ARG IS_DEVEL
WORKDIR /usr/src/app
SHELL ["/bin/bash", "-c"]

COPY web/ /usr/src/app/
COPY nginx/sites-enabled/ /etc/nginx/sites-enabled/
COPY nginx/sites-templates/ /etc/nginx/sites-templates/
COPY --from=build /MQTTManager /MQTTManager
# TODO: Make Django use /MQTTManager....-path to access nspm_mqttmanager binary instead of copying it.
COPY --from=build /MQTTManager/build/nspm_mqttmanager /usr/src/app/nspm_mqttmanager

# Install build tools into the container if this is a devel build
RUN if [ "$IS_DEVEL" == "yes" ]; then apt-get update \
    && apt-get -y install cmake build-essential curl \
    && pip install conan; else echo "Not building MQTTManager."; fi

RUN if [ "$IS_DEVEL" == "yes" ]; then apt-get update && apt-get -y install npm; fi

RUN if [ "$IS_DEVEL" == "yes" ]; then conan profile detect --force && echo 'core.cache:storage_path=/MQTTManager/conan_cache/' > ~/.conan2/global.conf \
    && sed -i "s|cppstd=gnu14|cppstd=gnu17|g" /root/.conan2/profiles/default \
    && sed -i "s|build_type=Release|build_type=Debug|g" /root/.conan2/profiles/default \
    ; fi


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
