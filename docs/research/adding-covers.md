# Adding cover support to NSPanelManager

> Research note, not an approved design. Originally checked 2026-10-25 against
> `origin/devel` (`ebe72d56`, the active development branch) and the then-current `main`; the
> implementation is now based on `devel`. Line numbers and generated files move; the paths and
> behaviours below are the durable part.

## Implementation status (2026-10-26)

The manager-side v1 is implemented on this branch: cover entity type, REST endpoint and tests,
React add/edit form, protobuf state/command messages, Home Assistant and OpenHAB cover entities,
entity-page slot serialization and manager tests. Blinds and shutters also have a quick open/close
fallback on the entities page so they remain usable on panels without the cover page; garage
doors, gates and doors are excluded from that quick action. The panel firmware source
(`CoverPage`, `EntityPage` routing, protobufs and Nextion ID lookup) is implemented on a separate
firmware branch. The HMI/TFT `cover` page is implemented in both region HMIs and all 12 shipped
TFTs (EU/US/mirrored, themes 1–4) are rebuilt in this branch; the web form remains marked BETA
until the matching firmware binaries ship. Until the TFT icon font is updated, the entities-page
slot uses the existing switch glyph as a placeholder.

## Scope

I interpreted "covers" as the Home Assistant / OpenHAB **cover domain**: blinds, roller
shutters, curtains, awnings, garage doors, gates and windows. This is the long-standing feature
request in [NSPManager/NSPanelManager#181](https://github.com/NSPManager/NSPanelManager/issues/181),
and the panel UI mockups in that issue show position presets, up/stop/down and a position slider.

Two nearby things are deliberately **not** this feature:

- **Album art / cover images for media players.** That is already implemented on `devel`
  (`#385`, `#410`, `#412`) and has its own image server and protobuf fields.
- **The local relay roller-shutter mode in
  [#114](https://github.com/NSPManager/NSPanelManager/issues/114).** That is about driving two
  NSPanel relays with an interlock and a timed motor stop, without Home Assistant or OpenHAB.
  It is a separate firmware/relay feature. It could share a "cover" UX later, but it does not
  need the Home Assistant cover entity work described here.

If that relay feature is what you meant, the shape is different: add a new button/relay mode to
the NSPanel settings, enforce the "never both relays on" interlock and the motor timeout in
firmware, expose the pair to Home Assistant as a cover via MQTT discovery, and add a relay-pair
configuration UI. It does not need the `Entity`/entity-page/protobuf work below, but it does need
a firmware release and careful safety testing. The two features could later share a cover page,
but they are separately shippable.

## Short answer

A proper cover entity fits the existing architecture, but it is a **vertical feature**, not a
single web-form change:

| Layer | Work | Difficulty |
| --- | --- | --- |
| Django model, REST, React wizard | New `cover` entity type, endpoint, form and tests | Low; closely follows `media_player` |
| Protobuf | A `Cover` state message and a `CoverCommand` command | Low, but must be done before the panel side |
| MQTTManager (C++) | New `CoverEntity` + Home Assistant/OpenHAB implementations, loader, page serialization | Medium; follows `Light`/`Switch`/`MediaPlayer` |
| Panel firmware (separate repo) | New cover page, state handling and command sending | Medium/high; firmware repo PR required |
| HMI/TFT (this repo) | New page, icons and regenerated TFTs for every region/theme | Medium/high; one Nextion Editor compile per region |
| Firmware binaries (this repo) | Rebuilt `firmware.bin`, `data_file.bin`, `merged_flash.bin` for both panel variants | Release engineering, not a normal PR |

The important conclusion: **the web/manager/protobuf half is ordinary work; the panel UX is the
critical path.** A cover that only exists in the web UI and MQTTManager cannot be controlled from
the panel. Shipping a usable feature needs coordinated changes in
`NSPanelManager`, `NSPanelManager-firmware`, and the committed TFT/firmware artifacts.

A realistic order of magnitude for a first version (Home Assistant + OpenHAB, open/close/stop and
position, no tilt) is **2–3 weeks for one experienced contributor**, including panel testing.
The manager/web/protocol part alone is roughly **3–5 focused days**; the firmware and HMI work is
the rest.

**Base the implementation on `devel`, not this worktree's `main`.** `devel` already has the
`EntitySlot.type`/`id` fields, the media player precedent and the Django/C++/React test harnesses.
If the feature must land on `main` first, the `EntitySlot` type/id additions and test harnesses
have to be brought over first, or the cover work will duplicate/conflict with them.

## What exists today

The `media_player` type added on `devel` is the best template. It proves that a non-boolean entity
can be added across the web UI, MQTTManager and protobuf, and it has a draft firmware page
(firmware PR #27) to model the panel work on.

### Web and database

- `docker/web/nspanelmanager/web/models.py` — `Entity.EntityType` currently contains `light`,
  `switch`, `button`, `thermostat` and `media_player`. `Entity.entity_data` is a free-form
  `JSONField`, so cover-specific settings need no new database columns; only the choices list
  needs a migration.
- `docker/web/nspanelmanager/web/rest.py` — one PUT endpoint per entity type. The media player
  endpoint (`entities_media_players` / `put_media_player_entity`) is the current model for
  validation, slot-collision checks and `send_mqttmanager_reload_command()`.
- `docker/web/nspanelmanager/web/urls.py` — one `rest/entities/<type>` route per type.
- `docker/web/nspanelmanager/web/home_assistant_api.py` — already accepts an arbitrary
  `{"type": [...]}` filter, so it needs no cover-specific change for the React list. The older
  `/api/get_all_available_entities` endpoint in `api.py` classifies Home Assistant entities as
  only `light`/`switch` today. The React wizard uses `/rest/home_assistant/entities`, which
  returns arbitrary domains, but the legacy HTMX/select path still exists
  (`modal-select-entity.js`, `htmx.py`); if it is still reachable for adding entities, add cover
  classification there too.
- `docker/web/nspanelmanager/web/tests/test_entities.py` and `tests/harness.py` — REST tests and
  the `MANAGER_CONTROLLERS` contract that checks each saved entity is loadable by the C++ side.

### React UI

- `.../MultiStep_AddEditEntity/step1_select_type.tsx` — type cards.
- `.../step2_select_source.tsx` — per-type controller map (`home_assistant`, `openhab`, `nspm`).
- `.../MultiStep_AddEditEntity.tsx` — step-3 form dispatch.
- `.../entity_types/step3_edit_media_player.tsx` — most recent example; includes tests.
- `.../stores/AvailableEntitiesStore.ts` — fetched Home Assistant/OpenHAB item lists and icon
  mapping. The wizard filters HA options by entity-id prefix in each step-3 form.
- `.../dist/` — committed build output; the media player PR rebuilt it and every hash changed.
  Any React change must rebuild and commit this directory.

### Protobuf

`docker/protobuf/protobuf_nspanel_entity.proto` defines the state pushed to a panel for one
entity. On `devel` it has a `Light`, `Thermostat` and `MediaPlayer` message in a `oneof entity`.

`docker/protobuf/protobuf_nspanel.proto` defines:

- `NSPanelRoomEntitiesPage.EntitySlot`, including `EntityType` (`LIGHT`, `SWITCH`, `BUTTON`,
  `THERMOSTAT`, `SCENE`, `MEDIA_PLAYER`) plus `id` and `mqtt_state_topic`.
- `NSPanelMQTTManagerCommand`, including `LightCommand`, `ThermostatCommand`,
  `MediaPlayerCommand` and the generic `ToggleEntityFromEntitiesPage`.

Generated artifacts live in `docker/MQTTManager/include/protobuf/` (C++),
`docker/web/nspanelmanager/web/protobuf/` (Python, for the protos Django needs),
`protobuf_firmware/` (C for the ESP32), and the firmware repository itself. The regeneration
script is `docker/protobuf/compile_all.sh`.

### MQTTManager

- `docker/MQTTManager/include/entity/entity.hpp` — `MQTT_MANAGER_ENTITY_TYPE` enum and the
  `MqttManagerEntity` interface (`get_id`, `get_type`, `can_toggle`, `toggle`, `get_icon`,
  `get_mqtt_state_topic`, callbacks).
- `entity_manager/entity_manager.cpp` — `load_lights`, `load_buttons`, `load_thermostats`,
  `load_switches`, `load_media_players`, `load_scenes`, then `load_rooms` and `load_nspanels`.
  Entity pages are attached in `RoomEntitiesPage::post_init`, so a new loader must run before
  rooms are loaded.
- `room/room_entities_page.cpp` — converts each entity to an `EntitySlot` and publishes retained
  protobuf state. Media player slots set `ENTITY_TYPE_MEDIA_PLAYER` and the entity id.
- `media_player/media_player.cpp` and `home_assistant_media_player.cpp` — the pattern for a
  dedicated state topic, a `command_callback`, feature flags, initial state and retained updates.
- `CMakeLists.txt` — each entity type is a static library and is linked into
  `MQTTManager_EntityManager`, `MQTTManager_RoomEntitiesPage` and the test executable.

### Panel firmware and HMI

The panel firmware is **not in this repository**; it lives in
[NSPManager/NSPanelManager-firmware](https://github.com/NSPManager/NSPanelManager-firmware).
This repository carries the built binaries in `docker/web/nspanelmanager/firmware/{sonoff,custom}/`.

The firmware currently has:

- `lib/Pages/EntitiesPage.cpp` — draws the generic entity slots. An icon press sends the generic
  toggle command; a label press opens `EntityPage` if the slot has a non-empty state topic.
- `lib/Pages/EntityPage.cpp` — subscribes to the entity state topic and dispatches to
  light/thermostat handling. `devel` has a draft media player extension in the firmware PR
  [#27](https://github.com/NSPManager/NSPanelManager-firmware/pull/27), not merged at the time
  of writing.
- `lib/Pages/GUI_data.hpp` — page and component IDs, matching the HMI.

The HMI projects are `docker/HMI_files/tft_automation/{eu,us}/gui.HMI`, with compiled TFTs under
`output_tft1..4/` and `us_horizontal_mirrored/`. The HMI stores pages, components, fonts and page
code; the TFT is the compiled artifact. Adding a page currently needs the Windows Nextion Editor
for one compile per region, even with the headless tooling.

Two open PRs make that tooling much better:

- [#407](https://github.com/NSPManager/NSPanelManager/pull/407) — headless picture/TFT build,
  HMI reader/editor, CI checks.
- [#408](https://github.com/NSPManager/NSPanelManager/pull/408) — the media player HMI page and
  all theme TFTs, built on #407.

Cover work should reuse #407/#408's approach and should not be started on the HMI until #408 (or
its equivalent) lands, otherwise the page and TFT conflicts will be painful. Even with #407, the
documented workflow still requires one Editor compile per region after an HMI page change.

## Why a cover is not a switch

The existing `SwitchEntity` is deliberately simple: it has an on/off state, no state topic on
`devel` (`get_mqtt_state_topic()` returns `""`), and the panel only sends a blind toggle command.
A cover needs much more:

- **States:** `open`, `closed`, `opening`, `closing`, `unknown`, `unavailable`.
- **Position:** 0–100, and not every cover reports or supports it.
- **Tilt:** a second 0–100 axis for venetian blinds, with its own open/close/stop actions.
- **Capabilities:** Home Assistant exposes a `supported_features` bitmask, so the UI must hide or
  disable actions the device cannot perform.
- **Device class:** `awning`, `blind`, `curtain`, `damper`, `door`, `garage`, `gate`, `shade`,
  `shutter`, `window`; useful for icons and wording.
- **Different controller semantics:** Home Assistant positions are 0 = closed, 100 = open;
  OpenHAB Rollershutter percentages are 0 = up/open, 100 = down/closed. The panel-facing model
  should use one convention (recommended: 0 = closed, 100 = open) and invert for OpenHAB.

Reusing `SwitchEntity` would therefore either lose state/position feedback or produce a confusing
UI. A separate `COVER` type is the right call.

## Cover semantics from the controllers

### Home Assistant

Primary sources: the [Cover integration](https://www.home-assistant.io/integrations/cover/),
[`cover/const.py`](https://github.com/home-assistant/core/blob/dev/homeassistant/components/cover/const.py),
[`cover/entity.py`](https://github.com/home-assistant/core/blob/dev/homeassistant/components/cover/entity.py)
and [`cover/services.yaml`](https://github.com/home-assistant/core/blob/dev/homeassistant/components/cover/services.yaml).

States: `open`, `closed`, `opening`, `closing`, `unavailable`, `unknown`. A panel-facing state
enum should keep `unknown` and `unavailable` distinct: one means "no state received yet", the
other means "the controller says the device is unavailable".

State attributes used by the panel:

| Attribute | Meaning |
| --- | --- |
| `current_position` | 0 = fully closed, 100 = fully open. May be absent. |
| `current_tilt_position` | 0–100. May be absent. |
| `supported_features` | Bitmask of capabilities. |
| `device_class` | One of the cover device classes, or absent. |

`supported_features` bits from `CoverEntityFeature`:

| Bit | Value | Capability | Service |
| --- | ---: | --- | --- |
| `OPEN` | 1 | Open | `cover.open_cover` |
| `CLOSE` | 2 | Close | `cover.close_cover` |
| `SET_POSITION` | 4 | Set position | `cover.set_cover_position` with `position` |
| `STOP` | 8 | Stop | `cover.stop_cover` |
| `OPEN_TILT` | 16 | Open tilt | `cover.open_cover_tilt` |
| `CLOSE_TILT` | 32 | Close tilt | `cover.close_cover_tilt` |
| `STOP_TILT` | 64 | Stop tilt | `cover.stop_cover_tilt` |
| `SET_TILT_POSITION` | 128 | Set tilt position | `cover.set_cover_tilt_position` with `tilt_position` |
| `SPEED` | 256 | Selectable speed | `speed` field on open/close/set position |

The service call shape used elsewhere in this codebase is:

```json
{
  "type": "call_service",
  "domain": "cover",
  "service": "set_cover_position",
  "target": { "entity_id": "cover.living_room" },
  "service_data": { "position": 50 }
}
```

`cover.toggle` is also available and is useful for a single quick action; it chooses open/close
from the current state.

### OpenHAB

Primary sources: [Items](https://www.openhab.org/docs/concepts/items.html) and the
[WebSocket API](https://www.openhab.org/docs/configuration/websocket.html).

A `Rollershutter` item is the natural mapping. It accepts `UpDown` (`UP`/`DOWN`), `StopMove`
(`STOP`/`MOVE`) and `Percent` commands. Its `PercentType` state is 0–100, where **0 is fully
open/up and 100 is fully closed/down** (the docs' conversion table maps `UP` to 0 and `DOWN` to
100).

The panel-facing mapping should therefore be:

| Panel concept | OpenHAB command/state |
| --- | --- |
| Open | `UP`, or `Percent` 0 |
| Close | `DOWN`, or `Percent` 100 |
| Stop | `STOP` |
| Position 0 (closed) … 100 (open) | `Percent` `100 - position` |
| State 0 … 100 (openness) | `100 - item percentage` |

OpenHAB does not expose `opening`/`closing` as a distinct state for a Rollershutter, and has no
tilt concept on that item type. A partially open shutter is simply a percentage between 0 and 100.

The existing OpenHAB bridge (`openhab_manager`) delivers `ItemStateChangedEvent` and an initial
`ItemStateFetched` event; `openhab_switch.cpp` shows the payload shape and the
`openhab/items/<item>/command` topic. A cover implementation can copy that structure and send
the Rollershutter `UP`/`DOWN`, `STOP` and percentage commands. The exact JSON `type` names
(`UpDown`, `StopMove`, `Percent`) should be checked against the target OpenHAB version; the REST
API also accepts the plain-text command form.

## Proposed design

### 1. Entity data and REST contract

Add `Entity.EntityType.COVER = "cover"` and a migration for the choices change (no table column
change; on `devel` this would be the next `0070_alter_entity_entity_type` after `0069`). Store
only what is needed to reach the controller:

```json
{
  "controller": "home_assistant",
  "home_assistant_name": "cover.living_room_blind",
  "openhab_item": ""
}
```

For OpenHAB:

```json
{
  "controller": "openhab",
  "home_assistant_name": "",
  "openhab_item": "LivingRoom_Shutter"
}
```

Capabilities, position and tilt are read from the live controller, not stored as configuration.
This keeps the entity correct when a device is swapped for a different model. If a manual
"invert position" option is ever needed for a non-standard integration, add it explicitly later;
do not make it the default.

Add `PUT /rest/entities/covers` with:

- required fields: `room_id`, `entities_page_id`, `room_view_position`, `controller`,
  `friendly_name`, `home_assistant_name`, `openhab_item` (the wizard also sends
  `type: "entity"`, but the media player endpoint does not require it);
- require the selected controller's field to be non-empty (`home_assistant_name` for Home
  Assistant, `openhab_item` for OpenHAB);
- controller allow-list (`home_assistant`, `openhab`);
- reject a scene page (`entities_page.is_scenes_page`), a page that does not belong to the
  selected room, and a slot outside the page's `page_type`;
- if editing an `id`, reject an entity of another type (409) rather than converting it;
- slot collision check against both `Entity` and `Scene` (the media player endpoint has the
  current pattern);
- `send_mqttmanager_reload_command()` after save;
- a 405 for unsupported methods rather than 403, matching the other entity endpoints.

Add `COVER` to `MANAGER_CONTROLLERS` in the Django test harness so the manager-contract test
covers it.

### 2. React wizard

- Add a **Cover** card to step 1 (`mdi-blinds` or `mdi-window-shutter`).
- Add `cover: ["home_assistant", "openhab"]` to the controller map in step 2.
- Add `entity_types/step3_edit_cover.tsx`:
  - name;
  - Home Assistant cover selector filtered to `cover.` entities, with a short capability hint
    from the selected entity's `supported_features` (read through `home_assistant_entities`);
  - OpenHAB item selector filtered to Rollershutter items (read the item type through
    `openhab_items`; the current store only keeps the generic `"openhab"` type in its option
    objects).
- Dispatch `case "cover"` in `MultiStep_AddEditEntity.tsx`.
- Add the cover icon to `AvailableEntitiesStore`'s `getOptionsFromItems`.
- Add React tests: wizard offers the cover type, HA is offered, the form sends the expected
  request body, missing entity/name is rejected.
- Rebuild `react-components/dist/`.

The form should not try to persist `supported_features`; the controller's live state is the source
of truth. Showing the detected capabilities in the form is enough.

### 3. Protobuf schema

Add to `protobuf_nspanel_entity.proto` (field numbers are new; do not renumber existing fields):

```proto
message NSPanelEntityState {
  message Cover {
    enum CoverState {
      UNKNOWN = 0;
      OPEN = 1;
      CLOSED = 2;
      OPENING = 3;
      CLOSING = 4;
      UNAVAILABLE = 5;
    }

    int32 cover_id = 1;
    string name = 2;
    CoverState state = 3;

    bool has_position = 4;
    int32 position = 5;        // 0 = closed, 100 = open

    bool has_tilt_position = 6;
    int32 tilt_position = 7;   // 0-100

    bool can_open = 8;
    bool can_close = 9;
    bool can_stop = 10;
    bool can_set_position = 11;
    bool can_open_tilt = 12;
    bool can_close_tilt = 13;
    bool can_stop_tilt = 14;
    bool can_set_tilt_position = 15;

    string device_class = 16;  // Optional hint for the panel icon
  }

  oneof entity {
    Light light = 1;
    Thermostat thermostat = 2;
    MediaPlayer media_player = 3; // already on devel
    Cover cover = 4;
  }
}
```

Add to `protobuf_nspanel.proto`:

```proto
message NSPanelMQTTManagerCommand {
  message CoverCommand {
    enum Action {
      NONE = 0;
      OPEN = 1;
      CLOSE = 2;
      STOP = 3;
      OPEN_TILT = 4;
      CLOSE_TILT = 5;
      STOP_TILT = 6;
    }

    int32 cover_id = 1;
    Action action = 2;

    bool has_position = 3;
    int32 position = 4;        // 0 = closed, 100 = open

    bool has_tilt_position = 5;
    int32 tilt_position = 6;   // 0-100
  }

  oneof CommandData {
    // existing fields 1-9 ...
    CoverCommand cover_command = 10;
  }
}

message NSPanelRoomEntitiesPage {
  message EntitySlot {
    enum EntityType {
      // existing values ...
      ENTITY_TYPE_MEDIA_PLAYER = 6;
      ENTITY_TYPE_COVER = 7;
    }
    // existing fields ...
  }
}
```

Regenerate the C++, Python and C artifacts with `docker/protobuf/compile_all.sh` and copy the
firmware C files as the project normally does. The firmware repository also has its own copies of
the generated protobuf C files.

The `has_*` fields follow the existing protobuf-3 convention in this project (see
`MediaPlayerCommand`) because proto3 scalar fields cannot distinguish "unset" from zero.
`has_position` means "the controller currently reports a position"; `can_set_position` means
"the cover accepts a target position". They are not the same: some covers accept a target but do
not report their current position. The panel should show the slider when `can_set_position` is
true, and use `position` only while `has_position` is true (otherwise start from 0 or the last
requested position).

### 4. MQTTManager

Add a `cover/` library alongside `light/`, `switch/`, `thermostat/` and `media_player/`:

- `cover.hpp` / `cover.cpp`
  - `CoverEntity : MqttManagerEntity`
  - common config: id, name, room, page, slot, controller;
  - state: `CoverState`, `position`, `tilt_position`, capability flags, device class;
  - `send_state_update_to_nspanel()` builds `NSPanelEntityState` and publishes retained to
    `nspanel/mqttmanager_<manager>/entities/covers/<id>/state`;
  - only call `_signal_entity_changed()` when something the entities page slot displays changes
    (state/icon color/name), not on every position update; the retained state topic drives the
    cover page, and signalling the page for every percent of movement would flood MQTT;
  - `command_callback(NSPanelMQTTManagerCommand&)` filters by `cover_id` and dispatches to
    `open()`, `close()`, `stop()`, `set_position()`, tilt methods;
  - pure virtual controller methods;
  - `get_icon()` / colors based on state/position;
  - `can_toggle()` decision (see UX below).
- `home_assistant_cover.cpp/.hpp`
  - load `home_assistant_name`, require a `cover.` prefix, attach
    `HomeAssistantManager::attach_event_observer` and detach it in the destructor;
  - parse `state`, `current_position`, `current_tilt_position`, `supported_features`,
    `device_class` from `state_changed`; handle `new_state: null` (entity removed) and
    `unavailable`/`unknown` as unavailable/unknown rather than keeping a stale open/closed state;
  - service-call helper for the cover domain;
  - clamp/round positions, keep the last known position while opening/closing when the attribute
    is absent;
  - send initial state in the constructor (the retained state lets a panel that opens the page
    later get it immediately).
- `openhab_cover.cpp/.hpp`
  - load `openhab_item`, attach `OpenhabManager::attach_event_observer`;
  - handle `ItemStateFetched` and `ItemStateChangedEvent`;
  - parse percentage states defensively (`NULL`, `UNDEF`, decimals);
  - invert position for the panel-facing model;
  - send `ItemCommandEvent` on `openhab/items/<item>/command` with the Rollershutter
    `UP`/`DOWN`, `STOP` or percentage command (verify the exact JSON type names for the target
    OpenHAB version, or use the REST plain-text form).
- `EntityManager::load_covers()`:
  - select entities with `entity_type = "cover"`;
  - remove covers that no longer exist;
  - reload existing covers, or create HA/OpenHAB implementations based on
    `entity_data["controller"]`;
  - when reloading, re-read the controller-specific entity/item id and reattach the observer if
    it changed. The existing switch implementation only reloads name/page/controller in
    `SwitchEntity::reload_config()`; do not copy that limitation for covers;
  - call it before `load_rooms()`.
- `RoomEntitiesPage::_send_mqtt_state_update()`:
  - add a `COVER` branch, set `ENTITY_TYPE_COVER`, `id`, name, icon, colors and state topic;
  - `can_save_scene = false`.
- `EntityManager::_command_callback()`:
  - `CoverCommand` dispatch is handled by each cover object's callback, like media players;
  - keep the generic toggle path working for covers if `can_toggle()` is true.
- `CMakeLists.txt`: add `MQTTManager_Cover` and link it into
  `MQTTManager_EntityManager`, `MQTTManager_RoomEntitiesPage`, and the test executable.

Add unit tests modelled on `tests/media_player_test.cpp`:

- loads config and publishes initial retained state;
- maps every HA state and position;
- clamps positions and handles missing attributes;
- sends the exact HA service calls for open/close/stop/position;
- honors/ignores unsupported features;
- OpenHAB percentage inversion and command payloads;
- panel command dispatch;
- entity manager load/remove/reload behaviour.

### 5. Panel firmware (separate repository)

The firmware needs a new page class, analogous to the draft `MediaPlayerPage` in PR #27:

- `CoverPage.hpp/.cpp` (or an extension of `EntityPage` if the page is simple enough):
  - draw name, state, position and available buttons;
  - handle open/stop/close and the position slider;
  - send `NSPanelMQTTManagerCommand.CoverCommand` on the manager command topic;
  - show a pending position optimistically while the motor moves, then reconcile with the next
    state (the media player volume handling is a useful precedent);
  - hide/disable position or tilt controls the cover does not support.
- `EntityPage.cpp`:
  - recognize `NSPANEL_ENTITY_STATE__ENTITY_COVER`;
  - switch to the cover page, and dispatch touch/string events to it.
- `GUI_data.hpp`: page name and component IDs matching the new HMI page.
- `EntitiesPage.cpp`: no fundamental change needed if slots already carry the state topic and the
  label opens `EntityPage`; if the UX should open the cover page from the icon, add an explicit
  branch based on `EntitySlot.type`.
- Protobuf C files regenerated from the manager protos.
- Firmware tests/QEMU harness where applicable; at minimum a panel smoke test.

UX decision: a single tap on a garage-door icon should probably not open it. Two viable choices:

1. **Media-player pattern (recommended for safety):** `can_toggle()` is false; the icon does
   nothing and the label opens the cover page. The page has explicit open/stop/close controls.
2. **Switch-style quick action:** `can_toggle()` returns true and the icon toggles open/close;
   the label still opens the page for position/tilt. This is convenient for blinds but risky for
   garage doors/gates.

The choice can be per device class later; for v1, prefer explicit controls on a dedicated page.
The implementation in this branch uses a hybrid fallback: blinds/shutters can quick-toggle from
the entities page, while garage doors, gates and doors only use the explicit page once it is
available.

### 6. HMI and TFT

The implemented page is named `cover` and reuses the unused `screensaver2` page slot (page ID 4)
in both region HMIs. Reusing the slot avoids changing resource IDs or appending a page, and the
slot is large enough for the controls. The stock `screensaver2` page was unused by the firmware;
`cleanscreen` is retained because the dropdown page still navigates to it.

The page contains:

- name/state label;
- one `pos` slider (0–100, 0 = closed, 100 = open);
- open, stop and close buttons named `b_open`, `b_stop`, `b_close`;
- a back button named `home`.

The state label also shows the position (`Open 50%`) because the slot is tight; there is no
separate position label. The three button labels are written by the firmware when the page opens,
which keeps the HMI page inside the Nextion page-RAM limit. The page has 12 components to match
the original slot count.

Production steps used:

- `docker/HMI_files/tft_automation/{eu,us}/gui.HMI` were edited and compiled once in the Nextion
  Editor (Windows) to a template TFT per region; no Windows GUI is needed for future theme builds;
- the template TFTs were copied to all four theme slots by swapping picture records from the stock
  theme TFTs (raw RGB565 is generated from the theme BMP when a compressed stock picture has a
  different size);
- the 12 TFTs in `eu`, `us` and `us_horizontal_mirrored` are byte-valid (`checksums_ok`) with the
  expected orientation (270/180/90) and 181 pictures.

The page's Nextion component names are documented in the firmware `CoverPage.md`.

Icon work:

- The NSPanel custom icon font (`design/font/MaterialDesignSmartHome_nspm_custom.sfd`) has no
  blind/cover glyph. Add one (or more) and map it in `entity_icons.hpp`; otherwise use the
  existing switch glyphs as a temporary placeholder.
- The HMI fonts are separate; a cover page can use Material Design Icons or simple text/arrow
  glyphs already present, but the entities-page slot icon comes from the manager's font.
- Device-class-specific icons (blind vs curtain vs garage) are a nice-to-have; one generic cover
  icon is enough for v1.

### 7. Firmware artifacts and release

The manager repository commits the built panel firmware and TFTs. A cover feature therefore
needs:

- a firmware build from `NSPanelManager-firmware` for both `sonoff` and `custom` variants;
- updated `docker/web/nspanelmanager/firmware/{sonoff,custom}/firmware.bin`,
  `data_file.bin`, `merged_flash.bin`;
- the new TFTs in the HMI tree;
- a version bump and release note.

Treat the manager, firmware and TFT as one release unit. If the manager is updated first, a panel
with old firmware will show a cover slot (it is just an icon/name/topic) but cannot control it.

### 8. Compatibility and rollout

- Adding enum values and oneof fields is wire-compatible; old firmware ignores unknown fields.
  Old firmware will, however, not understand `NSPanelEntityState.Cover` and will log an unknown
  entity case if a user opens the state topic. It does not crash, but the page does not work.
- There is no explicit feature-negotiation handshake between manager, firmware and TFT. The panel
  reports firmware/TFT checksums/versions in its status, but the manager does not currently gate
  entity types on them.
- Recommended rollout:
  - mark cover support **BETA** in the web UI while firmware/TFTs are not universal, as the
    media player work did;
  - release manager + firmware + TFTs together;
  - consider gating cover slots on a minimum firmware version if maintainers want a hard guard;
  - keep the protobuf additions additive so older managers/firmware can coexist on a mixed
    installation.

## Suggested implementation phases

1. **Decide the UX and scope.** One generic cover page? Tilt in v1? Quick-toggle icon or
   explicit controls? HA only or HA + OpenHAB?
2. **Protocol + manager + web backend.** Add the `cover` type, REST endpoint, protobuf
   messages, `CoverEntity`/HA implementation, loader, page serialization, tests. This can be
   reviewed without panel changes, but is not yet usable on a panel.
3. **React UI.** Add the wizard/form/tests and rebuild `dist`.
4. **Firmware page.** Add `CoverPage`, state/command handling and firmware tests in the firmware
   repo.
5. **HMI/TFT.** Add the page to both HMIs, add icons, compile the template, build every theme,
   run the build checks.
6. **OpenHAB implementation.** Add `OpenhabCover` and tests; verify percentage inversion against
   a real Rollershutter.
7. **Tilt and polish.** Add tilt controls/device-class icons if desired.
8. **Release artifacts.** Rebuild firmware binaries, commit TFTs, update docs and release notes.
   The user-facing docs need at least the room/entity section in `docs/docs/web-interface.md`
   and the panel page description in `docs/docs/panel-functions.md`.

If the work must be split into mergeable PRs, the natural cut is:

- PR 1: protobuf + manager + web backend + tests (no panel page; feature flagged/beta).
- PR 2: React wizard + tests + dist.
- PR 3: firmware + HMI/TFT + binaries.

Do not merge PR 2 with a panel-facing claim until PR 3 is in the release.

## Test plan

| Area | What to test |
| --- | --- |
| Django REST | Add/edit/delete cover, required fields, controller validation, slot collision, manager reload, migration/choices, `MANAGER_CONTROLLERS` contract |
| React | Wizard type/controller offers, form validation, exact PUT body, edit path, error handling, `tsc -b` |
| Protobuf | Generated code compiles for C++/Python/C; round-trip `Cover`/`CoverCommand`; unknown fields tolerated |
| MQTTManager | HA state mapping, service calls, feature flags, position clamping, retained state; OpenHAB inversion and command payloads; load/reload/remove; page slot serialization |
| Firmware | Page opens from slot, state updates draw correctly, every button/slider sends the right command ID, unsupported controls hidden/disabled, back navigation |
| HMI/TFT | `check_build.py`, `check_build.py --rendered`, all themes/regions/mirrored, page and picture IDs stable, TFT size/orientation |
| Manual | HA `template cover` with position, a position-less cover, a tilt-capable cover if in scope, an OpenHAB Rollershutter, a real panel; verify state after HA restart and after manager reload |

## Risks and open questions

1. **Firmware/TFT coupling is the main risk.** The manager can ship a perfect cover backend, but
   the user cannot control it until the panel firmware and TFT are installed. Coordinate the
   release.
2. **HMI editing is still partly proprietary.** The headless tooling can read/edit and build
   themes, but a new page still needs one Nextion Editor compile per region. Contributors without
   Windows/Editor access cannot finish the HMI alone.
3. **Tilt scope.** Venetian blinds need a second axis, more protocol fields, more HMI controls and
   more testing. It is reasonable to ship open/close/stop/position first and add tilt later, but
   the protocol should reserve the fields now.
4. **OpenHAB inversion is easy to get backwards.** 0 = open, 100 = closed in OpenHAB; the panel
   and HA use 0 = closed, 100 = open. Add explicit tests for both states and commands.
5. **Covers without position.** Garage doors and some awnings only report open/closed. The UI must
   not show a slider that cannot work; use `supported_features`/`has_position`.
6. **Safety.** A single tap that opens a garage door is undesirable. Prefer the dedicated page
   with explicit open/stop/close, at least for `garage`, `gate` and `door` device classes.
7. **Existing switch entities are not a model.** They have no state topic and no position. Do not
   extend `SwitchEntity` in place; make a clean `CoverEntity`.
8. **Scenes.** The current scene system stores only light states (`LightState`). Cover positions
   are not saved by scenes and the scene save button should stay disabled for cover slots. This is
   a separate feature if wanted.
9. **Main page.** Covers do not belong in the room light averages or the all-lights/ceiling/table
   groups; adding a new type naturally keeps them out, but verify no generic "all entities" code
   assumes every entity is a light.
10. **Icons and TFT size.** A new page, fonts and pictures can increase the TFT; check the
    Nextion flash/serial limits and the existing build checks.
11. **Optimistic mode.** Do not copy the light/switch optimistic shortcut into the manager's cover
    position state. Let the controller report the real position; the panel can show a pending
    target locally until the next state arrives. Optimistically skipping to the target in the
    manager would hide the intermediate positions and make stop/position commands racy.

## Recommendation

Build a proper `COVER` entity type on `devel`, following the `media_player` precedent:

- Home Assistant and OpenHAB in the data model/backend from day one, even if OpenHAB ships a
  phase later;
- open/close/stop/position in v1, tilt reserved in the protocol and added later;
- a dedicated cover page with explicit controls and a position slider;
- one generic cover icon initially, device-class icons later;
- BETA labeling and a coordinated manager/firmware/TFT release.

The web/manager/protobuf work is a good first PR and can be reviewed independently. The feature is
not complete for users until the firmware page and TFTs are in the same release.

## Sources

### This repository (checked on `origin/devel` `ebe72d56` unless noted)

- Entity types and JSON config: `docker/web/nspanelmanager/web/models.py`
- Entity REST endpoints and validation: `docker/web/nspanelmanager/web/rest.py`,
  `docker/web/nspanelmanager/web/urls.py`
- Media player precedent: `.../media_player/` in `docker/MQTTManager/include/`, React
  `step3_edit_media_player.tsx`, `media_player_test.cpp`
- Protobuf definitions: `docker/protobuf/protobuf_nspanel_entity.proto`,
  `docker/protobuf/protobuf_nspanel.proto`
- Manager entity loading and page serialization:
  `docker/MQTTManager/include/entity_manager/entity_manager.cpp`,
  `docker/MQTTManager/include/room/room_entities_page.cpp`
- Entity interface: `docker/MQTTManager/include/entity/entity.hpp`
- HA/OpenHAB bridges: `docker/MQTTManager/include/home_assistant_manager/`,
  `docker/MQTTManager/include/openhab_manager/`
- HMI/TFT build notes (open PR #407):
  https://github.com/NSPManager/NSPanelManager/pull/407
- Media player HMI page (open PR #408):
  https://github.com/NSPManager/NSPanelManager/pull/408
- Firmware repository:
  https://github.com/NSPManager/NSPanelManager-firmware
- Firmware media player draft PR:
  https://github.com/NSPManager/NSPanelManager-firmware/pull/27

### External

- Home Assistant Cover integration:
  https://www.home-assistant.io/integrations/cover/
- Home Assistant cover constants (states, features, device classes):
  https://github.com/home-assistant/core/blob/dev/homeassistant/components/cover/const.py
- Home Assistant cover entity (state attributes):
  https://github.com/home-assistant/core/blob/dev/homeassistant/components/cover/entity.py
- Home Assistant cover services:
  https://github.com/home-assistant/core/blob/dev/homeassistant/components/cover/services.yaml
- Home Assistant `cover.set_cover_position` (0 = closed, 100 = open):
  https://www.home-assistant.io/actions/cover.set_cover_position/
- OpenHAB item types and Rollershutter:
  https://www.openhab.org/docs/concepts/items.html
- OpenHAB WebSocket API:
  https://www.openhab.org/docs/configuration/websocket.html
- OpenHAB Rollershutter Javadoc:
  https://www.openhab.org/javadoc/latest/org/openhab/core/library/items/rollershutteritem

### Related issues

- Add support of blinds: https://github.com/NSPManager/NSPanelManager/issues/181
- Roller shutter / roller blind relay mode:
  https://github.com/NSPManager/NSPanelManager/issues/114
