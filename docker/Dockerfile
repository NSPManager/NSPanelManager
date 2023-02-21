FROM python:latest

RUN apt-get update \
	&& apt-get install -y --no-install-recommends \
		postgresql-client curl \
	&& rm -rf /var/lib/apt/lists/*

RUN echo "alias ll='ls -lh --color=auto'" >> /etc/bash.bashrc

WORKDIR /usr/src/app
COPY web/ /usr/src/app/
RUN pip install -r requirements.txt

EXPOSE 8000
CMD ["./run.sh"]
