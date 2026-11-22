#!/bin/sh
# Regenerate the /rest/v2/ API contract after changing a serializer or a view in web/rest_v2/:
#   1. docker/web/openapi.yaml, from the DRF views (drf-spectacular). web/tests/test_openapi.py fails if it is stale.
#   2. The UI's TypeScript types, src/api/schema.d.ts, from openapi.yaml (openapi-typescript).
# Needs docker/web/requirements-dev.txt installed, and Node/npm. Commit both files.
# See "Changing the REST API" in CONTRIBUTING.md. CI fails if either file is stale.
set -e
cd "$(dirname "$0")"

(cd nspanelmanager && python manage.py spectacular --settings=nspanelmanager.test_settings --validate --fail-on-warn --file ../openapi.yaml)

cd nspanelmanager/web/react-components
npm run generate:api-types
