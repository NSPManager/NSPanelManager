# Build environment for the MQTTManager unit tests: the same base image and compiler as the
# production build in docker/Dockerfile, without the web app and Node tooling.
FROM python:3.12.5-bookworm

RUN apt-get update \
    && apt-get install -y --no-install-recommends cmake build-essential libssl-dev libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

RUN pip install --no-cache-dir conan \
    && conan profile detect --force \
    && sed -i "s|cppstd=.*|cppstd=gnu23|g; s|build_type=.*|build_type=Debug|g" /root/.conan2/profiles/default
