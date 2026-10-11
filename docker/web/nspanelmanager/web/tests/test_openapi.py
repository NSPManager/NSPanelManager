import tempfile
from io import StringIO
from pathlib import Path

from django.core.management import call_command
from django.test import SimpleTestCase

# Committed so the UI can generate its types without running Django, and so API changes show up in review.
SCHEMA_FILE = Path(__file__).resolve().parents[3] / "openapi.yaml"


class OpenAPISchemaTests(SimpleTestCase):
    def test_committed_schema_is_up_to_date(self):
        # --fail-on-warn: a view drf-spectacular can't describe fails here instead of producing a loose type.
        with tempfile.NamedTemporaryFile(suffix=".yaml") as generated:
            call_command("spectacular", "--validate", "--fail-on-warn", "--file", generated.name, stderr=StringIO())
            self.assertEqual(Path(generated.name).read_text(), SCHEMA_FILE.read_text(), "docker/web/openapi.yaml is out of date. Run docker/web/generate_api.sh and commit the result (see CONTRIBUTING.md).")
