FROM python:latest

RUN apt-get update \
	&& apt-get install -y --no-install-recommends \
		build-essential libssl-dev

# Install cmake
RUN cd /tmp \
	&& wget https://github.com/Kitware/CMake/releases/download/v3.27.2/cmake-3.27.2-linux-x86_64.tar.gz && \
  tar -zxvf cmake-*.tar.gz && rm -f cmake-*.tar.gz && mv cmake-* /cmake/ && for f in /cmake/bin/*; do ln -s $f /bin/${f##*/}; done

RUN echo "alias ll='ls -lh --color=auto'" >> /etc/bash.bashrc

WORKDIR /usr/src/app
COPY * /usr/src/app/
RUN pip install -r requirements.txt
RUN conan profile detect --force && echo 'core.cache:storage_path=/usr/src/app/conan_cache/' > ~/.conan2/global.conf && sed -i "s|cppstd=gnu14|cppstd=gnu17|g" /root/.conan2/profiles/default


CMD ["/bin/bash", "./run.sh"]
