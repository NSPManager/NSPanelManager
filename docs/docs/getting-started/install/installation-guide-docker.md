---
title: Docker standalone
sidebar_position: 2
---
import CenteredImage from '@site/src/components/CenteredImage';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

# Docker (standalone)

:::info
NSPanelManager docker images are hosted on Docker HUB: https://hub.docker.com/u/nspanelmanager
:::

## Run docker image

Standard scenario for running the docker image:

```
docker run --name nspanelmanager \
  -e TZ=Europe/Stockholm \
  -v "/nspmdata/":"/data/" \
  -p 8000:8000 \
  -d nspanelmanager/nspanelmanager-amd64:latest
```

:::note

This example is for an `x86_64` machine placed in the `Europe/Stockholm` timezone. Replace image name and timezone as needed.

Following CPU architectures are currently supported:
* armhf
* armv7
* aarch64
* i386
* amd64
:::


After you executed the command, you can access the NSPanelManager management UI at the IP of the docker host followed by `:8000`. (i.e: http://localhost:8000)


You can now [configure NSPanelManager](./../configuration).

## Backup

All NSPanelManager's data are stored within `/data` directory in the docker image. So if you are using the example presented earlier,
simply copy `/nspmdata` and you have a backup.

## Update

As always, it is recommended to perform a backup of the data before upgrading the docker image.