import os
import time

import posthog
from django.conf import settings

from web.settings_helper import (
    get_setting_with_default,
    set_setting_value,
)


def telemetry_enabled() -> bool:
    return get_setting_with_default("telemetry_enabled", "false").lower() == "true"


def enable_telemetry():
    set_setting_value("telemetry_enabled", "true")
    # Send container_started event with unique ID for this install
    posthog.capture(
        distinct_id=settings.UNIQUE_INSTALL_ID,
        event="container_started",
        properties={
            "version": os.getenv("APP_VERSION"),
            "$process_person_profile": False,  # optional: cheaper anonymous events
        },
    )
