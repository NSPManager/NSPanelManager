"""
Django REST Framework serializers for the /rest/* API.

These are the contract between the React UI and Django: drf-spectacular generates
docker/web/openapi.yaml from them, and the UI's TypeScript types are generated from that
(see docker/web/generate_api.sh). Rename a field here and the UI stops compiling.
"""

import logging
from typing import ClassVar

from django.conf import settings
from rest_framework import serializers

from .models import Room

logger = logging.getLogger(__name__)


# DRF silently drops keys it doesn't know. Log them instead, so a misspelled field
# (e.g. "displayOrder" for "display_order") leaves a trace rather than a save that quietly does nothing.
# We COULD return a 400 in production too, which catches such bugs immediately, but then an older or
# newer UI that sends a field this version doesn't know (one that was added or removed) would have
# every save rejected. So only the test suite rejects them (REST_REJECT_UNKNOWN_FIELDS in
# test_settings.py), where a request body that has drifted from the serializer should fail loudly.
# (A comment rather than a docstring: drf-spectacular would copy a docstring into every schema.)
class WarnOnUnknownFieldsMixin:
    def to_internal_value(self, data):
        if isinstance(data, dict):
            unknown = set(data) - set(self.fields)
            if unknown:
                if settings.REST_REJECT_UNKNOWN_FIELDS:
                    raise serializers.ValidationError({key: ["Unknown field."] for key in sorted(unknown)})
                logger.warning("%s: ignoring unknown field(s): %s", type(self).__name__, ", ".join(sorted(unknown)))
        return super().to_internal_value(data)


class OkResponseSerializer(serializers.Serializer):
    status = serializers.ChoiceField(choices=["ok"])


class ErrorResponseSerializer(serializers.Serializer):
    status = serializers.ChoiceField(choices=["error"])
    message = serializers.CharField()
    errors = serializers.DictField(child=serializers.ListField(child=serializers.CharField()), required=False, help_text="Validation errors by field name.")


#########
# Rooms #
#########


class RoomSerializer(WarnOnUnknownFieldsMixin, serializers.ModelSerializer):
    # The model field predates the snake_case API names.
    display_order = serializers.IntegerField(source="displayOrder")

    class Meta:
        model = Room
        fields = ("id", "friendly_name", "display_order", "room_temp_provider", "room_temp_sensor")
        # Blank means no temperature sensor. required=True only affects the response schema, so the UI type isn't optional.
        extra_kwargs: ClassVar[dict] = {"room_temp_provider": {"allow_blank": True, "required": True}, "room_temp_sensor": {"allow_blank": True, "required": True}}


class RoomUpdateSerializer(RoomSerializer):
    """PUT /rest/rooms: the room's id, plus only the fields to change."""

    id = serializers.IntegerField()
    display_order = serializers.IntegerField(source="displayOrder", required=False)

    class Meta(RoomSerializer.Meta):
        extra_kwargs: ClassVar[dict] = {"friendly_name": {"required": False}, "room_temp_provider": {"allow_blank": True, "required": False}, "room_temp_sensor": {"allow_blank": True, "required": False}}


class RoomCreateSerializer(WarnOnUnknownFieldsMixin, serializers.Serializer):
    name = serializers.CharField(max_length=Room._meta.get_field("friendly_name").max_length)


class RoomListQuerySerializer(serializers.Serializer):
    id = serializers.IntegerField(required=False, help_text="Only return this room.")


class RoomListResponseSerializer(OkResponseSerializer):
    rooms = RoomSerializer(many=True)


class RoomIdResponseSerializer(OkResponseSerializer):
    room_id = serializers.IntegerField()
