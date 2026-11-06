#!/usr/bin/env python3
"""Generate the NSPanelManager Grafana dashboards (Prometheus datasource).

Writes nspanel-fleet.json and nspanel-panel.json next to this script. Design rules:
- One unit per panel, never two y-axes.
- Series colours follow the series name (palette-classic-by-name), so a filter never repaints survivors.
- Status colours (green/orange/red thresholds) only where the value means good/bad.
- Headlines are stat tiles; latencies are p50/p95/p99 from histogram buckets.
"""
import json
import pathlib

OUT = pathlib.Path(__file__).parent
DS = {"type": "prometheus", "uid": "${datasource}"}
MGR = 'job="nspanelmanager",role="manager"'
RI = "$__rate_interval"


class Layout:
    def __init__(self):
        self.y = 0
        self.x = 0
        self.row_h = 0
        self.next_id = 1

    def id(self):
        self.next_id += 1
        return self.next_id

    def place(self, w, h):
        if self.x + w > 24:
            self.y += self.row_h
            self.x = 0
            self.row_h = 0
        pos = {"x": self.x, "y": self.y, "w": w, "h": h}
        self.x += w
        self.row_h = max(self.row_h, h)
        return pos

    def newline(self):
        self.y += self.row_h
        self.x = 0
        self.row_h = 0


def target(expr, legend="", ref="A", instant=False, fmt=None):
    t = {"datasource": DS, "expr": expr, "legendFormat": legend, "refId": ref, "range": not instant, "instant": instant}
    if fmt:
        t["format"] = fmt
    return t


def row(L, title, collapsed=False):
    L.newline()
    p = {"type": "row", "title": title, "collapsed": collapsed, "gridPos": {"x": 0, "y": L.y, "w": 24, "h": 1}, "id": L.id(), "panels": []}
    L.y += 1
    return p


def thresholds(steps):
    return {"mode": "absolute", "steps": [{"color": c, "value": v} for c, v in steps]}


def stat(L, title, expr, unit="none", w=4, h=4, steps=None, desc="", decimals=None, mappings=None, legend=""):
    p = {
        "type": "stat", "title": title, "description": desc, "datasource": DS, "id": L.id(), "gridPos": L.place(w, h),
        "targets": [target(expr, legend, instant=True)],
        "options": {"reduceOptions": {"calcs": ["lastNotNull"], "fields": "", "values": False},
                    "colorMode": "value" if steps else "none", "graphMode": "none", "textMode": "auto", "justifyMode": "auto"},
        "fieldConfig": {"defaults": {"unit": unit, "thresholds": thresholds(steps or [("text", None)]),
                                     "color": {"mode": "thresholds"}, "mappings": mappings or []}, "overrides": []},
    }
    if decimals is not None:
        p["fieldConfig"]["defaults"]["decimals"] = decimals
    return p


def ts(L, title, targets, unit="none", w=12, h=8, desc="", stack=False, bars=False, min0=True, steps=None, legend_calcs=("mean", "max", "lastNotNull")):
    custom = {"lineWidth": 2, "fillOpacity": 0 if not (stack or bars) else 60, "showPoints": "never", "spanNulls": True,
              "axisSoftMin": 0 if min0 else None, "drawStyle": "bars" if bars else "line",
              "stacking": {"mode": "normal" if stack else "none", "group": "A"},
              "gradientMode": "none", "axisBorderShow": False, "thresholdsStyle": {"mode": "line" if steps else "off"}}
    return {
        "type": "timeseries", "title": title, "description": desc, "datasource": DS, "id": L.id(), "gridPos": L.place(w, h),
        "targets": targets,
        "options": {"legend": {"displayMode": "table", "placement": "right" if w >= 12 else "bottom", "calcs": list(legend_calcs), "showLegend": True},
                    "tooltip": {"mode": "multi", "sort": "desc"}},
        "fieldConfig": {"defaults": {"unit": unit, "custom": custom, "color": {"mode": "palette-classic-by-name"},
                                     "thresholds": thresholds(steps or [("green", None)])}, "overrides": []},
    }


def quantiles(metric, by, sel, legend_prefix, qs=(0.5, 0.95, 0.99)):
    out = []
    for i, q in enumerate(qs):
        label = f"p{int(q * 100)}"
        out.append(target(f'histogram_quantile({q}, sum by (le{", " + by if by else ""}) (rate({metric}_bucket{{{sel}}}[{RI}])))',
                          f"{legend_prefix}{label}", chr(65 + i)))
    return out


def by_quantile(metric, by, sel, q=0.95):
    return [target(f'histogram_quantile({q}, sum by (le, {by}) (rate({metric}_bucket{{{sel}}}[{RI}])))', "{{" + by + "}}")]


def table(L, title, targets, w=24, h=10, desc="", overrides=None, transformations=None):
    return {
        "type": "table", "title": title, "description": desc, "datasource": DS, "id": L.id(), "gridPos": L.place(w, h),
        "targets": targets,
        "options": {"showHeader": True, "cellHeight": "sm", "sortBy": [{"displayName": "Panel", "desc": False}]},
        "fieldConfig": {"defaults": {"custom": {"align": "auto", "cellOptions": {"type": "auto"}}}, "overrides": overrides or []},
        "transformations": transformations or [],
    }


def datasource_var():
    return {"name": "datasource", "label": "Prometheus", "type": "datasource", "query": "prometheus", "current": {}, "hide": 0, "refresh": 1}


def dashboard(uid, title, panels, variables, desc, links):
    return {
        "uid": uid, "title": title, "description": desc, "tags": ["nspanel", "nspanelmanager"], "timezone": "browser",
        "editable": True, "graphTooltip": 1, "refresh": "30s", "time": {"from": "now-6h", "to": "now"},
        "schemaVersion": 41, "templating": {"list": variables}, "panels": panels, "links": links,
        "annotations": {"list": [{
            "name": "Panel reboots", "datasource": DS, "enable": True, "iconColor": "rgba(150, 150, 150, 0.6)",
            "expr": 'changes(nspanel_uptime_seconds{panel=~"${panel:regex}"}[1m]) > 0 and nspanel_uptime_seconds{panel=~"${panel:regex}"} < 120',
            "titleFormat": "{{panel}} rebooted", "step": "60s", "hide": False}]} if uid == "nspanel-panel" else {"list": []},
    }


# --------------------------------------------------------------------------------------------- fleet
def fleet():
    L = Layout()
    P = []
    up_mapping = [{"type": "value", "options": {"0": {"text": "down", "color": "red"}, "1": {"text": "up", "color": "green"}}}]

    P.append(row(L, "Fleet"))
    P.append(stat(L, "Panels online", 'sum(nspm_panels{state="online"})', steps=[("text", None)], w=4,
                  desc="Accepted panels the manager sees as ONLINE."))
    P.append(stat(L, "Panels offline", 'sum(nspm_panels{state="offline"})', steps=[("green", None), ("orange", 1)], w=4))
    P.append(stat(L, "Updating", 'sum(nspm_panels{state=~"updating_.*"})', steps=[("text", None), ("blue", 1)], w=4))
    P.append(stat(L, "Behind staged firmware",
                  'count((max by (panel, md5_firmware) (nspm_panel_info{model="sonoff"}) unless on(md5_firmware) label_replace(nspm_staged_image_info{type="firmware",variant="sonoff"}, "md5_firmware", "$1", "md5", "(.*)")) and on(panel) (max by (panel) (nspm_panel_up) == 1)) or vector(0)',
                  steps=[("green", None), ("orange", 1)], w=4,
                  desc="Panels that are up and whose running sonoff firmware md5 differs from the manager's staged image."))
    P.append(stat(L, "Panels with updates pending", 'count(max by (panel) (nspm_panel_update_pending) == 1) or vector(0)', steps=[("green", None), ("orange", 1)], w=4,
                  desc="Panels with at least one image (firmware, LittleFS or TFT) the manager says is newer than what the panel runs."))
    P.append(stat(L, "Manager warnings", 'sum(nspm_warnings_active) or vector(0)', steps=[("green", None), ("orange", 1)], w=4))

    P.append({
        "type": "state-timeline", "title": "Panel up", "datasource": DS, "id": L.id(), "gridPos": L.place(24, 9),
        "description": "nspm_panel_up from the manager: 1 while online or updating, 0 while waiting, rebooting or offline.",
        "targets": [target("max by (panel) (nspm_panel_up)", "{{panel}}")],
        "options": {"showValue": "never", "rowHeight": 0.8, "mergeValues": True, "alignValue": "left", "legend": {"showLegend": False}},
        "fieldConfig": {"defaults": {"mappings": up_mapping, "color": {"mode": "thresholds"},
                                     "thresholds": thresholds([("red", None), ("green", 1)])}, "overrides": []},
    })

    P.append(table(L, "Panels", [
        target('max by (panel, ip, md5_firmware, md5_littlefs, md5_tft) (nspm_panel_info)', "", "A", True, "table"),
        target("max by (panel) (nspm_panel_up)", "", "B", True, "table"),
        target("max by (panel) (nspm_panel_wifi_rssi_dbm)", "", "C", True, "table"),
        target("max by (panel) (nspm_panel_temperature_celsius)", "", "D", True, "table"),
        target("max by (panel) (nspm_panel_heap_used_ratio)", "", "E", True, "table"),
        target("max by (panel) (time() - nspm_panel_last_status_report_timestamp_seconds)", "", "F", True, "table"),
        target('count by (panel) (up{job="nspanelmanager",role="panel"} == 1)', "", "G", True, "table"),
    ], h=17, desc="One row per panel. 'Metrics' = the panel's own /metrics is being scraped (metrics firmware).",
        transformations=[
            {"id": "merge", "options": {}},
            {"id": "organize", "options": {
                "excludeByName": {"Time": True},
                "renameByName": {"panel": "Panel", "ip": "IP", "md5_firmware": "Firmware", "md5_littlefs": "LittleFS", "md5_tft": "TFT",
                                 "Value #B": "Up", "Value #C": "RSSI", "Value #D": "Temp", "Value #E": "Heap used",
                                 "Value #F": "Report age", "Value #G": "Metrics", "Value #A": "info"},
                "indexByName": {"panel": 0, "Value #B": 1, "ip": 2, "Value #C": 3, "Value #D": 4, "Value #E": 5, "Value #F": 6,
                                "Value #G": 7, "md5_firmware": 8, "md5_littlefs": 9, "md5_tft": 10}}},
            {"id": "organize", "options": {"excludeByName": {"info": True}}},
        ],
        overrides=[
            {"matcher": {"id": "byName", "options": "Up"}, "properties": [{"id": "mappings", "value": up_mapping},
                                                                          {"id": "custom.cellOptions", "value": {"type": "color-text"}}]},
            {"matcher": {"id": "byName", "options": "RSSI"}, "properties": [
                {"id": "unit", "value": "dBm"}, {"id": "custom.cellOptions", "value": {"type": "color-text"}},
                {"id": "thresholds", "value": thresholds([("red", None), ("orange", -80), ("green", -70)])}]},
            {"matcher": {"id": "byName", "options": "Temp"}, "properties": [{"id": "unit", "value": "celsius"}, {"id": "decimals", "value": 1}]},
            {"matcher": {"id": "byName", "options": "Heap used"}, "properties": [{"id": "unit", "value": "percentunit"}, {"id": "decimals", "value": 0}]},
            {"matcher": {"id": "byName", "options": "Report age"}, "properties": [
                {"id": "unit", "value": "s"}, {"id": "decimals", "value": 0}, {"id": "custom.cellOptions", "value": {"type": "color-text"}},
                {"id": "thresholds", "value": thresholds([("green", None), ("orange", 120), ("red", 300)])}]},
            {"matcher": {"id": "byName", "options": "Metrics"}, "properties": [
                {"id": "mappings", "value": [{"type": "value", "options": {"1": {"text": "yes"}}}, {"type": "special", "options": {"match": "null", "result": {"text": "–"}}}]}]},
            {"matcher": {"id": "byRegexp", "options": "Firmware|LittleFS|TFT"}, "properties": [
                {"id": "custom.width", "value": 110},
                {"id": "mappings", "value": [{"type": "regex", "options": {"pattern": "^(.{8}).*$", "result": {"text": "$1"}}}]}]},
        ]))

    P.append(ts(L, "Wi-Fi signal (RSSI)", [target("max by (panel) (nspm_panel_wifi_rssi_dbm)", "{{panel}}")], unit="dBm", min0=False,
                desc="From each panel's status report (works on any firmware). Below −80 dBm is marginal.", legend_calcs=("min", "mean", "lastNotNull")))
    P.append(ts(L, "Temperature", [target("max by (panel) (nspm_panel_temperature_celsius)", "{{panel}}")], unit="celsius", min0=False,
                desc="Panel temperature sensor, converted to °C by the manager.", legend_calcs=("min", "max", "lastNotNull")))
    P.append(ts(L, "Heap used", [target("max by (panel) (nspm_panel_heap_used_ratio)", "{{panel}}")], unit="percentunit",
                legend_calcs=("max", "lastNotNull")))
    P.append(ts(L, "Status report age", [target(f"max by (panel) (max_over_time((time() - nspm_panel_last_status_report_timestamp_seconds)[5m:30s]))", "{{panel}}")],
                unit="s", desc="Worst seconds-since-last-status-report in each 5 min window. A panel that is 'up' but stops reporting shows here first.",
                steps=[("green", None), ("orange", 120)], legend_calcs=("max", "lastNotNull")))
    P.append(ts(L, "Panel warnings and errors (log lines/min)",
                [target('sum by (panel) (rate(nspm_panel_log_lines_total{level=~"warning|error"}[' + RI + '])) * 60', "{{panel}}")],
                unit="none", desc="Warning + error log lines each panel sends to the manager, per minute.", legend_calcs=("mean", "max")))
    P.append(ts(L, "Configs sent (per hour, by reason)",
                [target('sum by (reason) (increase(nspm_panel_configs_sent_total[1h]))', "{{reason}}")], bars=True, stack=True,
                desc="reload_resend is the duplicate send after every reload (see FIXES_PENDING.md).", legend_calcs=("sum",)))

    P.append(row(L, "Manager: MQTT"))
    P.append(stat(L, "Broker connected", f"max(nspm_mqtt_connected{{{MGR}}})", w=4,
                  mappings=[{"type": "value", "options": {"0": {"text": "disconnected", "color": "red"}, "1": {"text": "connected", "color": "green"}}}],
                  steps=[("red", None), ("green", 1)]))
    P.append(stat(L, "Reconnects (24h)", f"sum(increase(nspm_mqtt_connects_total{{{MGR}}}[24h]))", w=4, steps=[("green", None), ("orange", 3)], decimals=0))
    P.append(stat(L, "Dropped messages (24h)", f"sum(increase(nspm_mqtt_messages_dropped_total{{{MGR}}}[24h])) or vector(0)", w=4,
                  steps=[("green", None), ("red", 1)], decimals=0, desc="Messages lost because the processing queue was full."))
    P.append(stat(L, "Queue depth (max)", f"max(nspm_mqtt_queue_depth_max{{{MGR}}})", w=4, steps=[("green", None), ("orange", 128), ("red", 230)],
                  desc="Highest processing-queue depth since the manager started (capacity 256)."))
    P.append(stat(L, "Buffered while disconnected", f"max(nspm_mqtt_buffered_messages{{{MGR}}})", w=4, steps=[("green", None), ("orange", 1)]))
    P.append(stat(L, "Retained topics", f"max(nspm_mqtt_retained_topics{{{MGR}}})", w=4))
    P.append(ts(L, "Messages received/s by topic class",
                [target(f"sum by (topic_class) (rate(nspm_mqtt_messages_received_total{{{MGR}}}[{RI}]))", "{{topic_class}}")], unit="reqps", stack=True))
    P.append(ts(L, "Messages published/s",
                [target(f"sum by (retained) (rate(nspm_mqtt_messages_published_total{{{MGR}}}[{RI}]))", "retained={{retained}}")], unit="reqps", stack=True))
    P.append(ts(L, "Queue wait p95 by topic class", by_quantile("nspm_mqtt_queue_wait_seconds", "topic_class", MGR), unit="s",
                desc="Time a received message waits before it's processed. Includes the processing thread's 100 ms idle poll."))
    P.append(ts(L, "Handler time p95 by topic class", by_quantile("nspm_mqtt_handler_duration_seconds", "topic_class", MGR), unit="s"))
    P.append(ts(L, "Queue depth", [target(f"max(nspm_mqtt_queue_depth{{{MGR}}})", "depth"), target(f"max(nspm_mqtt_queue_depth_max{{{MGR}}})", "max since start", "B")],
                unit="none", legend_calcs=("max", "lastNotNull")))
    P.append(ts(L, "Queue full: subscribe-path waits & drops (per minute)", [
        target(f"sum(rate(nspm_mqtt_queue_full_waits_total{{{MGR}}}[{RI}])) * 60", "subscribe waits"),
        target(f"sum by (topic_class) (rate(nspm_mqtt_messages_dropped_total{{{MGR}}}[{RI}])) * 60", "dropped {{topic_class}}", "B"),
        target(f"sum(rate(nspm_mqtt_handler_exceptions_total{{{MGR}}}[{RI}])) * 60", "handler exceptions", "C"),
    ], unit="none", legend_calcs=("max", "sum")))

    P.append(row(L, "Manager: integrations"))
    P.append(stat(L, "Home Assistant", f"min(nspm_home_assistant_authenticated{{{MGR}}})", w=6,
                  mappings=[{"type": "value", "options": {"0": {"text": "not authenticated", "color": "red"}, "1": {"text": "authenticated", "color": "green"}}}],
                  steps=[("red", None), ("green", 1)]))
    P.append(stat(L, "OpenHAB", f"max(nspm_openhab_connected{{{MGR}}})", w=6,
                  mappings=[{"type": "value", "options": {"0": {"text": "not connected"}, "1": {"text": "connected", "color": "green"}}}],
                  desc="Only meaningful if OpenHAB is configured."))
    P.append(stat(L, "Web UI clients", f"max(nspm_websocket_clients{{{MGR}}})", w=6))
    P.append(stat(L, "HA reconnects (24h)", f"sum(increase(nspm_home_assistant_reconnects_total{{{MGR}}}[24h]))", w=6, decimals=0,
                  steps=[("green", None), ("orange", 2)]))
    P.append(ts(L, "Home Assistant call latency p95 by domain", by_quantile("nspm_home_assistant_call_duration_seconds", "domain", MGR), unit="s",
                desc="call_service sent → result received."))
    P.append(ts(L, "Home Assistant call failures/min by domain",
                [target(f"sum by (domain) (rate(nspm_home_assistant_call_failures_total{{{MGR}}}[{RI}])) * 60", "{{domain}}")], legend_calcs=("max", "sum")))
    P.append(ts(L, "Home Assistant state changes/s by domain",
                [target(f"sum by (domain) (rate(nspm_home_assistant_state_changes_total{{{MGR}}}[{RI}]))", "{{domain}}")], unit="reqps", stack=True))
    P.append(ts(L, "Panel command → integration call, p95", by_quantile("nspm_panel_command_duration_seconds", "type", MGR), unit="s",
                desc="From a panel command arriving to the first HA/OpenHAB call it makes."))

    P.append(row(L, "Manager: config, updates, services"))
    P.append(ts(L, "Config build time", quantiles("nspm_panel_config_build_seconds", "", MGR, ""), unit="s"))
    P.append(ts(L, "Panel updates by outcome (per hour)",
                [target(f"sum by (type, outcome) (increase(nspm_panel_updates_total{{{MGR}}}[1h]))", "{{type}} {{outcome}}")], bars=True,
                legend_calcs=("sum",)))
    P.append(ts(L, "Update duration p95 by type", by_quantile("nspm_panel_update_duration_seconds", "type", MGR), unit="s"))
    P.append(ts(L, "Album art requests/s (cache)",
                [target(f"sum by (cache) (rate(nspm_image_requests_total{{{MGR}}}[{RI}]))", "{{cache}}")], unit="reqps", stack=True))
    P.append(ts(L, "Weather fetches (per hour)", [target(f"sum by (outcome) (increase(nspm_weather_fetches_total{{{MGR}}}[1h]))", "{{outcome}}")],
                bars=True, legend_calcs=("sum",)))
    P.append(ts(L, "HTTP requests/s by route", [target(f"sum by (route, code) (rate(nspm_http_requests_total{{{MGR}}}[{RI}]))", "{{route}} {{code}}")],
                unit="reqps"))

    P.append(row(L, "Manager: process"))
    P.append(ts(L, "CPU", [target(f"rate(process_cpu_seconds_total{{{MGR}}}[{RI}])", "cpu")], unit="percentunit", w=8))
    P.append(ts(L, "Resident memory", [target(f"process_resident_memory_bytes{{{MGR}}}", "rss")], unit="bytes", w=8))
    P.append(ts(L, "Threads and open files", [target(f"process_threads{{{MGR}}}", "threads"), target(f"process_open_fds{{{MGR}}}", "open fds", "B")], w=8))

    return dashboard("nspanel-fleet", "NSPanel: Fleet & Manager", P, [datasource_var()],
                     "Fleet health from NSPanelManager's /prometheus/metrics, plus the manager's own MQTT, integrations and process metrics.",
                     [{"title": "Panel detail", "type": "link", "url": "/d/nspanel-panel", "keepTime": True, "includeVars": False, "icon": "dashboard"}])


# --------------------------------------------------------------------------------------------- panel detail
def panel_detail():
    L = Layout()
    P = []
    S = 'panel=~"${panel:regex}"'
    SP = 'job="nspanelmanager",role="panel",' + S
    var = {"name": "panel", "label": "Panel", "type": "query", "datasource": DS, "refresh": 2, "sort": 1, "multi": True, "includeAll": False,
           "query": {"query": 'label_values(nspanel_uptime_seconds, panel)', "refId": "panels"},
           "definition": "label_values(nspanel_uptime_seconds, panel)", "current": {}}

    P.append(row(L, "Overview"))
    P.append(stat(L, "Uptime", f"max by (panel) (nspanel_uptime_seconds{{{SP}}})", unit="s", w=4, legend="{{panel}}"))
    P.append(stat(L, "RSSI", f"max by (panel) (nspm_panel_wifi_rssi_dbm{{{S}}})", unit="dBm", w=4, legend="{{panel}}",
                  steps=[("red", None), ("orange", -80), ("green", -70)]))
    P.append(stat(L, "Temperature", f"max by (panel) (nspm_panel_temperature_celsius{{{S}}})", unit="celsius", w=4, decimals=1, legend="{{panel}}"))
    P.append(stat(L, "Heap free (min since boot)", f"min by (panel) (nspanel_heap_min_free_bytes{{{SP}}})", unit="bytes", w=4, legend="{{panel}}"))
    P.append(stat(L, "Wi-Fi power save", f"max by (panel) (nspanel_wifi_power_save_mode{{{SP}}})", w=4, legend="{{panel}}",
                  mappings=[{"type": "value", "options": {"0": {"text": "awake (none)"}, "1": {"text": "modem sleep"}}}]))
    P.append(stat(L, "Log lines dropped (24h)", f"sum by (panel) (increase(nspanel_log_lines_dropped_total{{{SP}}}[24h]))", w=4, decimals=0,
                  legend="{{panel}}", steps=[("green", None), ("orange", 1)]))

    P.append(row(L, "MQTT task"))
    P.append(ts(L, "Handler time p95 by handler", by_quantile("nspanel_mqtt_handler_duration_seconds", "handler", SP), unit="s",
                desc="All handlers run one after another on the esp-mqtt task; a slow one delays everything behind it."))
    P.append(ts(L, "Share of MQTT task time by handler",
                [target(f"sum by (handler) (rate(nspanel_mqtt_handler_duration_seconds_sum{{{SP}}}[{RI}]))", "{{handler}}")],
                unit="percentunit", stack=True, desc="Seconds of handler time per second of wall time."))
    P.append(ts(L, "Subscribe latency p95 by stage (new subscriptions)",
                by_quantile("nspanel_mqtt_subscribe_latency_seconds", "stage", SP + ',cause="subscribe"'), unit="s",
                desc="queued → subscribe sent, suback → SUBACK received, data → retained value received, total → end to end."))
    P.append(ts(L, "Subscribe latency p95 by stage (after reconnect)",
                by_quantile("nspanel_mqtt_subscribe_latency_seconds", "stage", SP + ',cause="reconnect"'), unit="s"))
    P.append(ts(L, "publish() duration", quantiles("nspanel_mqtt_publish_duration_seconds", "", SP, ""), unit="s"))
    P.append(ts(L, "MQTT connects, disconnects, publish failures (per hour)", [
        target(f"sum by (panel) (increase(nspanel_mqtt_connects_total{{{SP}}}[1h]))", "{{panel}} connects"),
        target(f"sum by (panel) (increase(nspanel_mqtt_disconnects_total{{{SP}}}[1h]))", "{{panel}} disconnects", "B"),
        target(f"sum by (panel, reason) (increase(nspanel_mqtt_publish_failures_total{{{SP}}}[1h]))", "{{panel}} publish failed ({{reason}})", "C"),
    ], bars=True, legend_calcs=("sum",)))

    P.append(row(L, "Timers (esp_timer task)"))
    P.append(ts(L, "esp_timer callback p95 by callback", by_quantile("nspanel_esp_timer_callback_duration_seconds", "callback", SP), unit="s",
                desc="All esp_timer callbacks share one task with the Wi-Fi driver's timers. status_update publishes over MQTT from here (FIXES_PENDING.md)."))
    P.append(ts(L, "esp_timer callback p99 by callback",
                by_quantile("nspanel_esp_timer_callback_duration_seconds", "callback", SP, 0.99), unit="s"))

    P.append(row(L, "Wi-Fi"))
    P.append(ts(L, "RSSI", [target(f"max by (panel) (nspm_panel_wifi_rssi_dbm{{{S}}})", "{{panel}}")], unit="dBm", min0=False,
                legend_calcs=("min", "mean", "lastNotNull")))
    P.append(ts(L, "Disconnects by reason (per hour)",
                [target(f"sum by (reason) (increase(nspanel_wifi_disconnects_total{{{SP}}}[1h]))", "{{reason}}")], bars=True, stack=True,
                legend_calcs=("sum",)))
    P.append(ts(L, "Time in modem sleep", [target(
        f'sum by (panel) (rate(nspanel_wifi_power_save_seconds_total{{{SP},mode="modem_sleep"}}[{RI}])) / sum by (panel) (rate(nspanel_wifi_power_save_seconds_total{{{SP}}}[{RI}]))',
        "{{panel}}")], unit="percentunit", desc="Fraction of time the radio sleeps between beacons (screensaver). Compare with temperature and latency for PR #58.",
        legend_calcs=("mean", "lastNotNull")))
    P.append(ts(L, "Temperature", [target(f"max by (panel) (nspm_panel_temperature_celsius{{{S}}})", "{{panel}}")], unit="celsius", min0=False,
                legend_calcs=("min", "max", "lastNotNull")))

    P.append(row(L, "Logging and memory"))
    P.append(ts(L, "Log call duration", quantiles("nspanel_log_call_duration_seconds", "", SP, ""), unit="s", w=8))
    P.append(ts(L, "Serial write per log line", quantiles("nspanel_log_serial_write_duration_seconds", "", SP, ""), unit="s", w=8,
                desc="Only non-zero while serial echo is on (MQTT down, per PR #60)."))
    P.append(ts(L, "Log lines dropped/min", [
        target(f"sum by (panel) (rate(nspanel_log_lines_dropped_total{{{SP}}}[{RI}])) * 60", "{{panel}} dropped/min"),
    ], w=8, legend_calcs=("max", "mean")))
    P.append(ts(L, "Heap free", [target(f"min by (panel) (nspanel_heap_free_bytes{{{SP}}})", "{{panel}} free"),
                                 target(f"min by (panel) (nspanel_heap_min_free_bytes{{{SP}}})", "{{panel}} min since boot", "B")],
                unit="bytes", min0=False, legend_calcs=("min", "lastNotNull")))
    P.append(ts(L, "Log queue high-water", [target(f"max by (panel) (nspanel_log_queue_high_water{{{SP}}})", "{{panel}}")],
                legend_calcs=("max", "lastNotNull")))

    return dashboard("nspanel-panel", "NSPanel: Panel detail", P, [datasource_var(), var],
                     "Per-panel internals from the panel's own /metrics (metrics firmware only), plus manager-side status-report values.",
                     [{"title": "Fleet & Manager", "type": "link", "url": "/d/nspanel-fleet", "keepTime": True, "includeVars": False, "icon": "dashboard"}])


if __name__ == "__main__":
    for d in (fleet(), panel_detail()):
        (OUT / f"{d['uid']}.json").write_text(json.dumps(d, indent=2) + "\n")
        print("wrote", OUT / f"{d['uid']}.json", len(d["panels"]), "panels")
