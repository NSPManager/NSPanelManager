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
2. Fix whatever `npx tsc -b` in `react-components/` now reports.
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
