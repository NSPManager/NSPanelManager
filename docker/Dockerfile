# syntax=docker/dockerfile:1
# check=error=true

FROM python:3.11 AS build
ARG TARGETPLATFORM
ARG BUILDPLATFORM
ARG no_mqttmanager_build
ARG IS_DEVEL
WORKDIR /MQTTManager
SHELL ["/bin/bash", "-c"]

RUN echo "Running on $BUILDPLATFORM, building for $TARGETPLATFORM"
COPY docker/MQTTManager/ /MQTTManager/

# Only build MQTTManager during Docker build if is is not a devel mode.
RUN if [ "$IS_DEVEL" != "yes" ]; then apt-get update \
  && apt-get -y install cmake build-essential curl \
  && pip install conan; fi

RUN if [ "$IS_DEVEL" != "yes" ]; then conan profile detect --force && echo 'core.cache:storage_path=/MQTTManager/conan_cache/' > ~/.conan2/global.conf \
  && sed -i "s|cppstd=gnu14|cppstd=gnu17|g" /root/.conan2/profiles/default \
  && sed -i "s|build_type=Release|build_type=Debug|g" /root/.conan2/profiles/default; fi

RUN if [ -z "$no_mqttmanager_build" ]; then /bin/bash /MQTTManager/compile_mqttmanager.sh --target-platform "$TARGETPLATFORM" --strip; else echo "Not building MQTTManager."; fi


FROM python:3.11
ARG no_mqttmanager_build
ARG IS_DEVEL
ARG GITHUB_REF_NAME
ENV VERSION=$GITHUB_REF_NAME
WORKDIR /usr/src/app
SHELL ["/bin/bash", "-c"]

COPY docker/web/ /usr/src/app/
COPY docker/nginx/sites-enabled/ /etc/nginx/sites-enabled/
COPY docker/nginx/sites-templates/ /etc/nginx/sites-templates/
COPY --from=build /MQTTManager/build /MQTTManager/build

COPY docker/web/nspanelmanager/web/templates/footer_template.html docker/web/nspanelmanager/web/templates/footer.html
COPY docs/tex/manual.pdf docker/web/nspanelmanager/manual.pdf

RUN  sed -i "s/%version%/$VERSION/g" docker/web/nspanelmanager/web/templates/footer.html


# Update container
RUN apt-get update \
  && apt-get -y upgrade

# Install build tools into the container if this is a devel build
RUN if [ "$IS_DEVEL" == "yes" ]; then apt-get -y install cmake build-essential curl npm gdb \
  && pip install conan pipx; else echo "Not building MQTTManager."; fi

RUN if [ "$IS_DEVEL" == "yes" ]; then conan profile detect --force && echo 'core.cache:storage_path=/MQTTManager/conan_cache/' > ~/.conan2/global.conf \
  && sed -i "s|cppstd=gnu14|cppstd=gnu17|g" /root/.conan2/profiles/default \
  && sed -i "s|build_type=Release|build_type=Debug|g" /root/.conan2/profiles/default \
  && pip install pipx \
  && python3 -m userpath append ~/.local/bin \
  && source ~/.bashrc \
  && pipx install gdbgui; \
  fi

# Install software needed to build and run the manager
RUN apt-get install -y --no-install-recommends \
  postgresql-client curl inotify-tools net-tools nginx build-essential \
  && rm -rf /var/lib/apt/lists/*

RUN pip install -r requirements.txt # Install python packages

RUN echo "alias ll='ls -lh --color=auto'" >> /etc/bash.bashrc


EXPOSE 8000
CMD ["/bin/bash", "./run_uwsgi.sh"]
