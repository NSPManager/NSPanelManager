FROM python:3.11

ARG no_mqttmanager_build
ARG BUILD_ARCH

COPY web/ /usr/src/app/
COPY nginx/sites-enabled/ /etc/nginx/sites-enabled/
COPY MQTTManager/ /MQTTManager/

# Update container
RUN apt-get update \
	&& apt-get -y upgrade

RUN apt-get install -y --no-install-recommends \
		postgresql-client curl inotify-tools net-tools nginx build-essential cmake
	# && rm -rf /var/lib/apt/lists/*

RUN echo "alias ll='ls -lh --color=auto'" >> /etc/bash.bashrc

WORKDIR /usr/src/app
RUN pip install -r requirements.txt
RUN conan profile detect --force && echo 'core.cache:storage_path=/MQTTManager/conan_cache/' > ~/.conan2/global.conf
RUN sed -i "s|cppstd=gnu14|cppstd=gnu17|g" /root/.conan2/profiles/default \
    && sed -i "s|build_type=Release|build_type=Debug|g" /root/.conan2/profiles/default \
    && echo "arch=$BUILD_ARCH" >> /buildinfo

RUN if [ -z "$no_mqttmanager_build" ]; then /bin/bash /usr/src/app/make_mqttmanager.sh; else echo "Not building MQTTManager."; fi

EXPOSE 8000
CMD ["/bin/bash", "./run_uwsgi.sh"]
