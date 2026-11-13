# Contributing

## Changing the REST API

The `/rest/v2/*` endpoints are Django REST Framework views in `docker/web/nspanelmanager/web/rest_v2/`, and the React UI calls them through a typed client. (The older `/rest/*` endpoints in `web/rest.py` are plain Django views and aren't covered.) The chain is:

```
serializers + views in web/rest_v2/
        │  drf-spectacular
        ▼
docker/web/openapi.yaml                      (committed)
        │  openapi-typescript
        ▼
react-components/src/api/schema.d.ts         (committed)
        │  openapi-fetch
        ▼
api.GET / api.PATCH / … in react-components/src/api/client.ts
```

Rename or retype a serializer field and, once the files are regenerated, every UI caller that uses the old name stops compiling.

### After changing a serializer or a view

1. Install `docker/web/requirements-dev.txt` and run `docker/web/generate_api.sh`. It needs Node/npm, and rewrites `openapi.yaml` and `schema.d.ts`.
2. Fix whatever `npx tsc -b` in `docker/web/nspanelmanager/web/react-components/` now reports.
3. Commit both generated files with your change.

CI fails if you skip step 1:

- `web/tests/test_openapi.py` fails if `openapi.yaml` doesn't match the views, or if drf-spectacular warns about a view it can't describe (which would otherwise come out as a loose type).
- The `api-types` job in `.github/workflows/django-tests.yml` fails if `schema.d.ts` doesn't match `openapi.yaml`.

### drf-spectacular is development-only

drf-spectacular is in `requirements-dev.txt`, not `requirements.txt`, so it isn't in the release image. Its `jsonschema` → `rpds-py` dependency is built with Rust and has no wheel for every architecture the image is released for (armhf). So:

- Import `extend_schema` from `web/rest_v2/schema.py`, never from `drf_spectacular` directly. Without drf-spectacular installed, it does nothing.
- drf-spectacular's settings live in `nspanelmanager/test_settings.py`, and only apply when it's installed.
- The `prod-deps` CI job installs only `requirements.txt` and runs the whole suite.

### Conventions for v2 views

- Field names on the wire are snake_case.
- Request fields accept only the matching JSON type (no `"5000"` for an integer, no `"true"` for a boolean); see the `Strict*` fields in `web/rest_v2/global_settings.py`.
- Unknown request fields are a 400. The UI is built with the server, so there's no older or newer UI to tolerate.
- Errors are DRF's own bodies: `{field: [messages]}` for validation (maps onto react-hook-form's `setError`), `{"detail": message}` otherwise. List each status a view can return in `@extend_schema(responses=...)` so it's in the UI's types.
- Unsafe methods need the CSRF token, as the plain views do (`web/rest_v2/auth.py`). `client.ts` sends it.

### Calling the API from the UI

Use `api` from `src/api/client.ts` rather than `fetch()`. Pass request bodies as typed values (e.g. form data from `useForm<SettingsUpdateRequest>()`): openapi-fetch infers `body` generically, so an inline object literal isn't checked for extra keys.

## Settings

`/rest/v2/settings` reads and writes the same `Settings` rows as the settings page, the weather page, `/rest/settings` and MQTTManager, which deletes any key not in its `_setting_key_map` at startup. When adding a global setting:

1. Add it to `_setting_key_map` in `docker/MQTTManager/include/mqtt_manager_config/mqtt_manager_config.hpp`.
2. Add a field to `SettingsUpdateSerializer` in `web/rest_v2/global_settings.py`.
3. Add it to `LEGACY_ROWS` (and `INTEGER_RANGES` / `CHOICES` / `BOOLEANS` as it applies) in `web/tests/test_rest_v2_settings.py`.

`ManagerParityTests` fails if steps 1 and 2 disagree.

## Changing the panel GUI (HMI / TFT)

Each region (`eu`, `us`) has one HMI, `docker/HMI_files/tft_automation/REGION/gui.HMI`, and four themes. The themes are the same HMI with different pictures. You compile one TFT per region in the Nextion Editor, and the scripts build the other themes from it. Committing an HMI on its own builds nothing, and CI fails until the TFTs match it.

```
gimp_scripts/*.scm  --make_pictures.sh-->  pictures_tftN/all_numbered/*.bmp
gui.HMI  --Nextion Editor (once)-->  gui.tft
gui.tft + all_numbered  --make_tfts.sh-->  output_tft1..4/gui.tft (+ us_horizontal_mirrored)
```

### After changing an HMI

All commands run in `docker/HMI_files/tft_automation/`.

1. Save the HMI as `REGION/gui.HMI`. Add new pictures after the existing ones so no picture ID changes.
2. If you changed the pictures or the GIMP scripts, render them: `sudo ./make_pictures.sh [REGION]` (GIMP runs in Docker, no display needed). If you added an image progress bar, list it in `REGION/progress_bars.txt` and its pictures are generated for you.
3. Compile the HMI **once** in the Nextion Editor. The theme whose pictures the HMI holds doesn't matter. The display orientation must be 270 for `eu` and 180 for `us`.
4. Build every theme from that compile: `./make_tfts.sh REGION path/to/gui.tft` (`python3 -m pip install -r requirements.txt` first). It writes `REGION/output_tft1..4/gui.tft`, and building `eu` also writes `us_horizontal_mirrored`. If you only changed pictures, `./make_tfts.sh [REGION]` rebuilds from the committed TFTs and no Editor compile is needed.
5. Check: `./check_build.py`. Then commit the HMI, the pictures and every `output_tft*/gui.tft` together.

The Editor is needed only in step 3 (and not at all for picture-only changes), because the page code in a TFT is encrypted. CI (`.github/workflows/tft-build.yml`) runs the same checks. It fails if the TFTs weren't built from the committed HMI and pictures, but it can't tell whether a TFT was compiled from the latest page or code changes when the pictures didn't change.

See `docker/HMI_files/tft_automation/README.md` for each script, `progress_bars.txt`, and the limits (for example, `make_tfts.sh` asks for a new compile if the Editor compressed a picture that differs between themes).
