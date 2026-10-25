#!/bin/sh
# Regenerate the REST API contract after changing a serializer or a DRF view:
#   1. docker/web/openapi.yaml, from the DRF views (drf-spectacular). web/tests/test_openapi.py fails if this is stale.
#   2. The UI's TypeScript types, src/api/schema.d.ts, from openapi.yaml (openapi-typescript).
# Needs the Django requirements (see run_tests.sh) and Node/npm. Commit both files.
# See "Changing the REST API" in CONTRIBUTING.md. CI fails if either file is stale.
set -e
cd "$(dirname "$0")"

NSPM_DATA_DIR="$(mktemp -d)" python nspanelmanager/manage.py spectacular --validate --fail-on-warn --file openapi.yaml

# generate:api-types runs openapi-typescript with npx rather than as a devDependency: it declares a
# typescript ^5 peer dependency and the UI is on TypeScript 6. CI runs the same script.
cd nspanelmanager/web/react-components
npm run generate:api-types
