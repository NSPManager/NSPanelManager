#!/bin/sh
# Run the Django test suite. Extra arguments are passed to "manage.py test", e.g.
#   ./run_tests.sh web.tests.test_entities
#   ./run_tests.sh -v 2
cd "$(dirname "$0")/nspanelmanager" || exit 1
exec python manage.py test --settings=nspanelmanager.test_settings "$@"
