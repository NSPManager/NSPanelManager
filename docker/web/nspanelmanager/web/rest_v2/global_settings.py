"""
/rest/v2/settings: the global settings (web.models.Settings), typed.

Storage is unchanged from the older /rest/settings and the settings page: one Settings row per key, its
value a string. MQTTManager reads the same rows (MqttManagerConfig::get_setting_with_default<T>) and, at
startup, deletes any key missing from its _setting_key_map and adds missing keys with their defaults. So:

  * Values are written exactly as the legacy views write them: booleans as "True"/"False", integers as
    str(int), everything else as given.
  * Values are read the way MQTTManager reads them, e.g. a boolean is true only if it lower-cases to "true".
  * A key is written only when its value changes, so a stored "true" (rather than "True") is left as it is
    when a client sends back the value it was given.
  * SettingsUpdateSerializer's fields are the keys this API knows. web/tests/test_rest_v2_settings.py checks
    they match _setting_key_map in docker/MQTTManager/include/mqtt_manager_config/mqtt_manager_config.hpp.

Per-panel settings (web.models.NSPanelSettings) are not part of this endpoint.
"""

import logging
import re
from decimal import Decimal, InvalidOperation

import environ
from django.db import transaction
from rest_framework import serializers
from rest_framework.response import Response
from rest_framework.views import APIView

from web.apps import send_mqttmanager_reload_command
from web.models import Settings

from .schema import extend_schema

logger = logging.getLogger(__name__)

# MQTTManager parses integers with std::stoi.
INT32_MAX = 2**31 - 1

# Never returned. GET reports "<name>_set" instead; PATCH "" clears one.
SECRETS = ("home_assistant_token", "openhab_token", "mqtt_password")

# Set by the add-on (web/apps.py) when running as a Home Assistant add-on, so not editable then.
ADDON_MANAGED = ("home_assistant_address", "home_assistant_token")

ADDRESSES = ("home_assistant_address", "openhab_address")


def is_home_assistant_addon():
    # The same test views.get_base_data uses. The is_home_assistant_addon DB row is written by MQTTManager
    # at startup, so it can lag behind the environment.
    environment = environ.Env()
    return "IS_HOME_ASSISTANT_ADDON" in environment and environment("IS_HOME_ASSISTANT_ADDON") == "true"


# The Strict* fields accept only the matching JSON type: DRF's own fields also take "5000" for an
# integer, "yes" or 1 for a boolean and 5 for a string, which would hide a client sending the wrong type.


class StrictBooleanField(serializers.BooleanField):
    def to_internal_value(self, data):
        if not isinstance(data, bool):
            self.fail("invalid", input=data)
        return data


class StrictIntegerField(serializers.IntegerField):
    def to_internal_value(self, data):
        if isinstance(data, bool) or not isinstance(data, int):
            self.fail("invalid")
        return super().to_internal_value(data)


class StrictCharField(serializers.CharField):
    def __init__(self, **kwargs):
        # Stored exactly as sent, as the legacy views do (a date format may end in a space).
        kwargs.setdefault("trim_whitespace", False)
        kwargs.setdefault("max_length", Settings._meta.get_field("value").max_length)
        super().__init__(**kwargs)

    def to_internal_value(self, data):
        if not isinstance(data, str):
            self.fail("invalid")
        return super().to_internal_value(data)


class TrimmedCharField(StrictCharField):
    """Leading and trailing whitespace is removed, for values that are usually pasted (addresses, tokens): a
    stray space would otherwise be stored and break the connection. Only whitespace is an error, not blank,
    since blank clears a token."""

    default_error_messages = {"whitespace": "Only whitespace was entered."}

    def to_internal_value(self, data):
        value = super().to_internal_value(data)
        trimmed = value.strip()
        if value and not trimmed:
            self.fail("whitespace")
        return trimmed


class StrictChoiceField(serializers.ChoiceField):
    def to_internal_value(self, data):
        if not isinstance(data, str):
            self.fail("invalid_choice", input=data)
        return super().to_internal_value(data)


def _int(min_value=0, max_value=INT32_MAX, **kwargs):
    return StrictIntegerField(min_value=min_value, max_value=max_value, **kwargs)


def _text(**kwargs):
    kwargs.setdefault("allow_blank", True)
    return StrictCharField(**kwargs)


_TRIMMED = "Leading and trailing whitespace is removed."


def _trimmed(help_text=None, **kwargs):
    kwargs.setdefault("allow_blank", True)
    return TrimmedCharField(help_text=f"{_TRIMMED} {help_text}" if help_text else _TRIMMED, **kwargs)


def _coordinate(limit):
    def validate(value):
        if value == "":
            return
        try:
            number = Decimal(value)
        except InvalidOperation:
            raise serializers.ValidationError("Must be a decimal number.") from None
        if not number.is_finite() or not -limit <= number <= limit:
            raise serializers.ValidationError(f"Must be between -{limit} and {limit}.")

    return _trimmed(validators=[validate])


# Whether a stored integer is one this API reports. MQTTManager's std::stoi would also accept " 50" or
# "50abc"; those are reported as null so the UI asks for a proper value instead of showing a guess.
# Used with fullmatch(), so the whole string must match (unlike ^...$, which also accepts "50\n").
_INTEGER = re.compile(r"-?[0-9]+")


class SettingsUpdateSerializer(serializers.Serializer):
    """A PATCH /rest/v2/settings body. Send only the settings that change."""

    # Manager
    manager_address = _trimmed()
    manager_port = _int(1, 65535, allow_null=True, help_text="null (stored blank) until set.")

    # Logging
    mqttmanager_log_level = StrictChoiceField(choices=["error", "warning", "info", "debug", "trace"])
    max_live_log_messages = _int()
    max_log_buffer_size = _int()

    # Panels and lights
    default_nspanel_type = StrictChoiceField(choices=["eu", "us"])
    color_temp_min = _int(1)
    color_temp_max = _int(1)
    reverse_color_temp = StrictBooleanField()
    turn_on_behavior = StrictChoiceField(choices=["color_temp", "restore"])
    raise_to_100_light_level = _int(0, 100)
    light_turn_on_brightness = _int(1, 100)
    min_button_push_time = _int()
    button_long_press_time = _int()
    special_mode_trigger_time = _int()
    special_mode_release_time = _int()
    optimistic_mode = StrictBooleanField()
    room_status_backoff_time = _int()
    all_rooms_status_backoff_time = _int()
    mqtt_wait_time = _int()

    # Screen and screensaver
    screen_dim_level = _int(1, 100)
    screensaver_dim_level = _int(0, 100)
    screensaver_activation_timeout = _int()
    screensaver_mode = StrictChoiceField(choices=["with_background", "without_background", "datetime_with_background", "datetime_without_background", "no_screensaver"])
    show_screensaver_inside_temperature = StrictBooleanField()
    show_screensaver_outside_temperature = StrictBooleanField()

    # Weather and time
    location_latitude = _coordinate(90)
    location_longitude = _coordinate(180)
    weather_update_interval = _int(1, help_text="Minutes between weather updates.")
    weather_wind_speed_format = StrictChoiceField(choices=["kmh", "ms", "mph", "knots"])
    weather_precipitation_format = StrictChoiceField(choices=["mm", "inch"])
    outside_temp_sensor_provider = StrictChoiceField(choices=["home_assistant", "openhab"], allow_blank=True, help_text="Blank for no sensor.")
    outside_temp_sensor_entity_id = _trimmed()
    date_format = _text(allow_blank=False, help_text="strftime() format.")
    clock_us_style = StrictBooleanField()
    use_fahrenheit = StrictBooleanField()

    # Web interface
    theme = _text(allow_blank=False)

    # Buzzer (RTTTL; blank for no sound)
    buzzer_sound_touch = _text()
    buzzer_sound_scene_activated = _text()
    buzzer_sound_scene_saved = _text()

    # MQTT
    mqtt_server = _trimmed()
    mqtt_port = _int(1, 65535)
    mqtt_username = _text()
    mqtt_password = _text(write_only=True, help_text="Blank clears the password.")

    # Home Assistant (read-only when running as the Home Assistant add-on)
    home_assistant_address = _trimmed(help_text="A trailing / is removed.")
    home_assistant_token = _trimmed(write_only=True, help_text="Blank clears the token.")

    # OpenHAB
    openhab_address = _trimmed(help_text="A trailing / is removed.")
    openhab_token = _trimmed(write_only=True, help_text="Blank clears the token.")

    def to_internal_value(self, data):
        # Unknown keys are an error rather than ignored: the UI is built with the server, so one it
        # doesn't know is a bug (or a typo) that a 200 would hide.
        if isinstance(data, dict):
            errors = {key: ["This setting is read-only."] if key in READ_ONLY else ["Unknown setting."] for key in data if key not in self.fields}
            if errors:
                raise serializers.ValidationError(errors)
        return super().to_internal_value(data)

    def validate_home_assistant_address(self, value):
        return value[:-1] if value.endswith("/") else value

    def validate_openhab_address(self, value):
        return value[:-1] if value.endswith("/") else value

    def validate(self, attrs):
        errors = {}
        if is_home_assistant_addon():
            for key in ADDON_MANAGED:
                if key in attrs:
                    errors[key] = ["Set by the Home Assistant add-on."]

        # Pairs are checked against the stored value of whichever half isn't being changed.
        merged = {**read_settings(), **attrs}
        for low, high in (("color_temp_min", "color_temp_max"),):
            if (low in attrs or high in attrs) and merged.get(low) is not None and merged.get(high) is not None and merged[low] >= merged[high]:
                errors[low if low in attrs else high] = [f"{low} must be less than {high}."]

        if errors:
            raise serializers.ValidationError(errors)
        return attrs


def _response_field(field):
    # Every value can be null: a key MQTTManager hasn't written yet, or one stored in a form this API
    # doesn't recognise (e.g. "abc" for an integer).
    if isinstance(field, serializers.BooleanField):
        return serializers.BooleanField(allow_null=True)
    if isinstance(field, serializers.IntegerField):
        return serializers.IntegerField(allow_null=True)
    if isinstance(field, serializers.ChoiceField):
        return serializers.ChoiceField(choices=list(field.choices), allow_blank=field.allow_blank, allow_null=True)
    return serializers.CharField(allow_null=True)


def _build_settings_serializer():
    fields = {"__module__": __name__, "__doc__": "The global settings. Secrets are never returned, only whether each is set."}
    for name, field in SettingsUpdateSerializer().get_fields().items():
        if name in SECRETS:
            fields[f"{name}_set"] = serializers.BooleanField(help_text=f"Whether {name} is set.")
        else:
            fields[name] = _response_field(field)
    fields["is_home_assistant_addon"] = serializers.BooleanField(help_text="Running as the Home Assistant add-on; home_assistant_* are read-only.")
    return type("SettingsSerializer", (serializers.Serializer,), fields)


# GET and PATCH response: SettingsUpdateSerializer's fields, typed for reading.
SettingsSerializer = _build_settings_serializer()

_FIELDS = SettingsUpdateSerializer().get_fields()

# In GET responses but not writable: the secrets' *_set flags and is_home_assistant_addon.
READ_ONLY = frozenset(SettingsSerializer().fields) - frozenset(_FIELDS)


def _decode(name, raw):
    """A stored string as the value this API reports, or None."""
    if raw is None:
        return None
    field = _FIELDS[name]
    if isinstance(field, serializers.BooleanField):
        # MqttManagerConfig::get_setting_with_default<bool>: boost::to_lower, then == "true".
        return raw.lower() == "true"
    if isinstance(field, serializers.IntegerField):
        if field.allow_null and raw == "":
            return None
        return int(raw) if _INTEGER.fullmatch(raw) else None
    if isinstance(field, serializers.ChoiceField):
        return raw if raw in field.choices or (field.allow_blank and raw == "") else None
    return raw


def _encode(name, value):
    """A validated value as the string the legacy views store."""
    if value is None:
        return ""
    if isinstance(value, bool):
        return "True" if value else "False"
    return str(value)


def _stored_rows():
    # Settings.name isn't unique. Like get_setting_with_default (and MQTTManager) use the first row.
    rows = {}
    for name, value in Settings.objects.order_by("-id").values_list("name", "value"):
        rows[name] = value
    return rows


def read_settings():
    """Every setting this API knows, decoded. Secrets are included (callers must not return them)."""
    rows = _stored_rows()
    return {name: (rows.get(name) if name in SECRETS else _decode(name, rows.get(name))) for name in _FIELDS}


def representation():
    values = read_settings()
    for secret in SECRETS:
        values[f"{secret}_set"] = (values.pop(secret) or "") != ""
    values["is_home_assistant_addon"] = is_home_assistant_addon()
    return SettingsSerializer(values).data


def write_settings(validated):
    """Store the validated settings that differ from what is stored. Returns the names written."""
    changed = []
    with transaction.atomic():
        rows = _stored_rows()
        for name, value in validated.items():
            raw = rows.get(name)
            current = raw if name in SECRETS else _decode(name, raw)
            if raw is not None and current == value:
                continue
            encoded = _encode(name, value)
            if raw == encoded:
                continue
            if not Settings.objects.filter(name=name).update(value=encoded):
                Settings.objects.create(name=name, value=encoded)
            changed.append(name)
    return changed


def _build_errors_serializer():
    messages = {"__module__": __name__, "__doc__": "A 400 from PATCH: error messages by setting name. Maps onto react-hook-form's setError()."}
    for name in [*SettingsSerializer().fields, *_FIELDS, "non_field_errors"]:
        messages[name] = serializers.ListField(child=serializers.CharField(), required=False)
    # Instead of the above, for a body that isn't valid JSON.
    messages["detail"] = serializers.CharField(required=False)
    return type("SettingsErrorsSerializer", (serializers.Serializer,), messages)


SettingsErrorsSerializer = _build_errors_serializer()


class DetailSerializer(serializers.Serializer):
    """DRF's error body for a request it rejects before validation, e.g. invalid JSON or a failed CSRF check."""

    detail = serializers.CharField()


class SettingsView(APIView):
    http_method_names = ["get", "patch", "head"]

    @extend_schema(responses=SettingsSerializer)
    def get(self, request):
        return Response(representation())

    @extend_schema(request=SettingsUpdateSerializer, responses={200: SettingsSerializer, 400: SettingsErrorsSerializer, 403: DetailSerializer, 415: DetailSerializer})
    def patch(self, request):
        serializer = SettingsUpdateSerializer(data=request.data, partial=True)
        serializer.is_valid(raise_exception=True)
        changed = write_settings(serializer.validated_data)
        if changed:
            logger.info("Settings changed: %s", ", ".join(changed))
            send_mqttmanager_reload_command()
        return Response(representation())
