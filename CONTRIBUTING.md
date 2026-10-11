# Contributing

## Changing the REST API

The React UI and Django share a typed contract for the `/rest/*` endpoints that have been ported to Django REST Framework (currently `/rest/rooms` and `/rest/rooms/<id>`; the rest are still plain Django views and aren't covered yet). The chain is:

```
web/serializers.py + DRF views in web/rest.py
        │  drf-spectacular
        ▼
docker/web/openapi.yaml                      (committed)
        │  openapi-typescript
        ▼
react-components/src/api/schema.d.ts         (committed)
        │  openapi-fetch
        ▼
api.GET / api.PUT / … in react-components/src/api/client.ts
```

Rename or retype a serializer field and, once the files are regenerated, every UI caller that uses the old name stops compiling.

### After changing a serializer or a DRF view

1. Run `docker/web/generate_api.sh`. It needs the Django requirements (see `docker/web/run_tests.sh`) and Node/npm. It rewrites `openapi.yaml` and `schema.d.ts`.
2. Fix whatever `npx tsc -b` in `react-components/` now reports.
3. Commit both generated files with your change.

CI fails if you skip step 1:

- `web/tests/test_openapi.py` (Django tests) fails if `openapi.yaml` doesn't match the views, or if drf-spectacular warns about a view it can't describe, which would otherwise come out as a loose type.
- The `api-types` job in `.github/workflows/django-tests.yml` fails if `schema.d.ts` doesn't match `openapi.yaml`.

### Conventions for DRF views

- Field names on the wire are snake_case. If a model field is named differently, map it in the serializer (`display_order = serializers.IntegerField(source="displayOrder")`), not in the view.
- Request serializers include `WarnOnUnknownFieldsMixin`. In production it logs a warning for fields the serializer doesn't know and otherwise ignores them, so a UI and server from different versions still work together. Under the test settings (`REST_REJECT_UNKNOWN_FIELDS = True`) it rejects them with a 400, so a test whose request body has drifted from the serializer fails.
- Responses keep the `{"status": "ok", ...}` envelope. Errors are always `ErrorResponseSerializer` (`{"status": "error", "message": ..., "errors"?: {field: [msg]}}`), produced by `web.rest.exception_handler`. List each error status a view can return in `@extend_schema(responses=...)` so it shows up in the UI's types.

### Calling the API from the UI

Use `api` from `src/api/client.ts` rather than `fetch()`. Pass request bodies as typed values (e.g. form data from `useForm<RoomUpdateRequest>()`): openapi-fetch infers `body` generically, so an inline object literal isn't checked for extra keys. In production the server only logs a warning for unknown fields, so a typo there fails quietly.
