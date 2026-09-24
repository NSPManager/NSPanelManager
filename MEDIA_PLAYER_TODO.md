# Media player — remaining work

Tracking what is still missing from Home Assistant media player support (PR #385).

## Web UI for adding and editing media players

Media players can only be created through `PUT /rest/entities/media_players`. The React
add/edit entity wizard (`docker/web/nspanelmanager/web/react-components/src/components/MultiStep_AddEditEntity/`)
does not offer them yet.

Needed:

- A "Media player" tile in `step1_select_type.tsx`, alongside light/switch/button/thermostat.
- A `entity_types/step3_edit_media_player.tsx`, modelled on `step3_edit_thermostat.tsx`. It PUTs a
  flat JSON body to `/rest/entities/media_players` (no `values` wrapper — that is the light endpoint's
  legacy shape) and only needs `room_id`, `entities_page_id`, `room_view_position`, `controller`,
  `friendly_name` and `home_assistant_name`.
- A "Source volume" section in that step for the three optional fields:
  - `source_volume_strategy` — a select of `none`, `player_attributes`, `source_entity`.
  - `source_entity_attribute` — shown for `player_attributes` and `source_entity`.
  - `source_volume_attribute` — shown for `player_attributes` only.

  The endpoint rejects the invalid combinations, so the form should mirror those rules rather than
  rely on the error response. Editing round-trips for free: `get_rest_entitiy_representation` splats
  `**entity.entity_data`, so `GET /rest/entities/<id>` already returns these keys at the top level.

  Both attribute fields should be a **dropdown, not free text**. `rest/home_assistant/entities`
  returns each entity's full Home Assistant state object under `item`, so the wizard already holds the
  selected player's live attributes — offer `Object.keys(item.attributes)` instead of asking the user
  to guess an attribute name they cannot see.
- A "Volume step" number input for the optional `volume_step` (whole %, 1-100, default 5), with
  +/- buttons like the thermostat's step size. It applies to both the volume and the source volume.

## Deleted entities leave a retained state topic behind (pre-existing)

Not introduced by this PR, but easy to hit while testing media players.

`MediaPlayerEntity::send_state_update_to_nspanel` publishes to
`nspanel/mqttmanager_<manager_address>/entities/media_players/<id>/state` with retain set. Nothing
ever clears it: `~MediaPlayerEntity` only detaches its command callback, and `EntityManager`'s
removal path just erases the entity from `_entities`. The broker therefore keeps serving the last
state of a deleted entity to every new subscriber, indefinitely.

Lights (`light.cpp:424`) and thermostats (`thermostat.cpp:567`) build and publish their topics the
same way, so this affects every entity type. Media players only make it obvious because a deleted
one shows up as a second, frozen player next to the live one.

The fix belongs in the base class rather than here, but note that `~MqttManagerEntity`
(`entity/entity.hpp:121`) cannot simply call `get_mqtt_state_topic()`: it is pure virtual
(`entity/entity.hpp:80`), and by the time the base destructor runs the derived object is gone and
the vtable points at the pure slot, which aborts with "pure virtual method called".

Two workable shapes:

- Cache the topic in a base member when it is first built and have `~MqttManagerEntity` publish the
  empty retained payload using the cached string. `Room` already does exactly this caching with
  `_mqtt_state_topic` (`room/room.cpp:59`).
- Or publish the tombstone from each derived destructor, where calling the derived
  `get_mqtt_state_topic()` is still safe. Simpler per class, but has to be repeated for every
  entity type and is easy to forget on the next one.

Clearing an existing ghost by hand is a retained publish of an empty payload to the topic.

## Volume step (review follow-up)

Per media player `volume_step` (whole %, 1-100, default 5) is stored in `entity_data`, accepted by
`PUT /rest/entities/media_players` and sent to the panel as `NSPanelEntityState.MediaPlayer.volume_step`.
The same step applies to the volume and the source volume. Still missing:

- Firmware: volume up/down buttons that send `clamp(volume ± volume_step, 0, 100)` as an absolute
  volume. Step from the last volume the panel sent rather than the last state received, so quick
  presses are not lost while Home Assistant catches up.
- The field in the web UI (see above).

## Other

- [ ] Build the Docker image, including the static release build
- [ ] End-to-end test: Home Assistant media player → panel state → `curl .../album_art?format=png`
- [ ] Firmware: fetch and draw the album art on the panel
- [ ] Remove the now-unused `libmagick++-dev` from the Dockerfile
