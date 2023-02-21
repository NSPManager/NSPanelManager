FROM django:latest

RUN echo "alias ll='ls -lh --color=auto'" >> /etc/bash.bashrc

WORKDIR /usr/src/app
COPY requirements.txt ./
RUN pip install -r requirements.txt
COPY web/ /usr/src/app/

EXPOSE 8000
CMD ["./run.sh"]
