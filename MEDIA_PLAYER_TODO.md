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

## Other

- [ ] Build the Docker image, including the static release build
- [ ] End-to-end test: Home Assistant media player → panel state → `curl .../album_art?format=png`
- [ ] Firmware: fetch and draw the album art on the panel
- [ ] Remove the now-unused `libmagick++-dev` from the Dockerfile
