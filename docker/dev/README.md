# NSPanelManager dev stack

A self-contained stack for working on NSPanelManager without a separate manager or a copy of
a production database:

| Service | What it is |
|---|---|
| `mosquitto` | The stack's own MQTT broker, published on a free port (`nspm-dev up` prints it) |
| `nspm` | NSPanelManager built from this checkout, on `http://localhost:8010` |

The stack has its own broker, so it never touches real panels and can run next to a
production install. Its image is the `devel` target of [`../Dockerfile`](../Dockerfile), the
same image `docker-build_and_run_dev.sh` builds: it holds only tools and packages, and the
source is mounted in, so editing the source never rebuilds it. A fresh stack seeds its
database ([`seed.py`](seed.py)) with MQTT settings pointing at `mosquitto` and a room.

The stack has no panels of its own. Point a panel, or the firmware running under QEMU, at
the broker port `nspm-dev up` prints, and accept it in the web UI as usual.

## Usage

```bash
docker/dev/nspm-dev up          # first start builds the image and MQTTManager; slow once
                                # (the web UI is up meanwhile; the manager starts when its build is done)
docker/dev/nspm-dev logs nspm   # follow it
```

Change MQTTManager code, then:

```bash
docker/dev/nspm-dev rebuild         # incremental build + restart of MQTTManager
docker/dev/nspm-dev rebuild --full  # after conanfile.py changes
docker/dev/nspm-dev watch           # rebuild on every save
```

The web app (`docker/web`) is mounted in and served by Django's dev server, so Python and
template changes apply without a rebuild.

Other commands:

```bash
docker/dev/nspm-dev shell    # bash in the nspm container
docker/dev/nspm-dev reset    # delete the database; the next `up` seeds a fresh one
docker/dev/nspm-dev down
```

## Settings

Environment variables, read by `nspm-dev up`:

| Variable | Default | |
|---|---|---|
| `NSPM_DEV_PORT` | `8010` | Web UI port on the host |
| `NSPM_DEV_MQTT_PORT` | picked by Docker | Broker port on the host |
| `NSPM_DEV_MQTT_BIND` | `0.0.0.0` | Broker address on the host; `127.0.0.1` keeps the anonymous broker off the network |
| `NSPM_DEV_TZ` | the image's | The manager's timezone, e.g. `Europe/Stockholm` |
| `NSPM_DEV_WEB_UI_REF` | `main` | Branch, tag or commit of NSPanelManager-Web-UI to build |
| `NSPM_CONAN_CACHE` | a volume | Host directory for Conan packages, to share with other checkouts |

With `sudo`, keep them with `sudo --preserve-env=NSPM_CONAN_CACHE,...`.
