# Grafana dashboards

Two dashboards for the Prometheus metrics that MQTTManager and the panels expose:

- `nspanel-fleet.json`: the manager, and every panel side by side.
- `nspanel-panel.json`: one panel in detail (pick it with the `panel` variable).

## Prometheus

MQTTManager lists itself and every online panel at `/prometheus/targets` (Prometheus HTTP service discovery), so one
scrape job covers the whole fleet. The dashboards expect the job to be called `nspanelmanager`:

```yaml
scrape_configs:
  - job_name: nspanelmanager
    http_sd_configs:
      - url: http://<nspanelmanager-host>/prometheus/targets
        refresh_interval: 60s
```

Each target carries a `role` label (`manager` or `panel`), and panels also carry `panel`, `mac` and `nspm_id`.

## Importing

In Grafana, go to Dashboards → New → Import and upload a JSON file. Both dashboards have a `datasource` variable, so
choose your Prometheus data source there; nothing in them names a specific data source or host.

## Changing them

Edit `gen_dashboards.py` and run it. It rewrites both JSON files next to itself:

```bash
python3 docker/monitoring/grafana/gen_dashboards.py
```

Commit the script and the regenerated JSON together, rather than editing the JSON by hand.
