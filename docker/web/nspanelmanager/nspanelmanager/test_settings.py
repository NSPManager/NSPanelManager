"""
Settings used when running the Django test suite (see docker/web/run_tests.sh).

Identical to the production settings except that persistent data (the secret key)
is written to a throw-away directory instead of /data. The Django test runner
automatically uses an in-memory SQLite database, so the real database is never touched.
"""

import os
import tempfile

os.environ.setdefault("NSPM_DATA_DIR", tempfile.mkdtemp(prefix="nspm-test-"))

from .settings import *

MEDIA_ROOT = os.path.join(DATA_DIR, "media")

# Hashing speed is irrelevant for tests.
PASSWORD_HASHERS = ["django.contrib.auth.hashers.MD5PasswordHasher"]

# Many views log full tracebacks when rejecting bad input, which the tests do on purpose.
# Set NSPM_TEST_LOG_LEVEL=INFO (or DEBUG) to see them when debugging a failing test.
LOGGING["root"]["level"] = os.environ.get("NSPM_TEST_LOG_LEVEL", "CRITICAL")
LOGGING["loggers"] = {"django.request": {"level": os.environ.get("NSPM_TEST_LOG_LEVEL", "CRITICAL")}}

# CI sets NSPM_TEST_JUNIT_DIR to also write JUnit XML results (needs unittest-xml-reporting).
if os.environ.get("NSPM_TEST_JUNIT_DIR"):
    TEST_RUNNER = "xmlrunner.extra.djangotestrunner.XMLTestRunner"
    TEST_OUTPUT_DIR = os.environ["NSPM_TEST_JUNIT_DIR"]
    TEST_OUTPUT_FILE_NAME = "django-tests.xml"
