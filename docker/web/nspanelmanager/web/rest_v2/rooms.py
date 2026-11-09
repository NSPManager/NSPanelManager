import logging

from django.db import transaction
from django.shortcuts import get_object_or_404
from rest_framework import serializers, status
from rest_framework.response import Response
from rest_framework.views import APIView, exception_handler

from web.apps import send_mqttmanager_reload_command
from web.models import NSPanel, Room

from .schema import extend_schema

logger = logging.getLogger(__name__)

# The Strict* fields accept only the matching JSON type: DRF's own fields also take "5000" for an
# integer, "yes" or 1 for a boolean and 5 for a string, which would hide a client sending the wrong type.


class StrictIntegerField(serializers.IntegerField):
    def to_internal_value(self, data):
        if isinstance(data, bool) or not isinstance(data, int):
            self.fail("invalid")
        return super().to_internal_value(data)


class StrictCharField(serializers.CharField):
    def __init__(self, **kwargs):
        # Stored exactly as sent; TrimmedCharField below overrides this for the fields that want trimming.
        kwargs.setdefault("trim_whitespace", False)
        super().__init__(**kwargs)

    def to_internal_value(self, data):
        if not isinstance(data, str):
            self.fail("invalid")
        return super().to_internal_value(data)


class TrimmedCharField(StrictCharField):
    """Leading and trailing whitespace is removed. Only-whitespace input is an error, not blank, so a
    stray space can't be stored in place of a real value. max_length applies to the trimmed value."""

    default_error_messages = {"whitespace": "Only whitespace was entered."}

    def to_internal_value(self, data):
        value = super().to_internal_value(data)
        trimmed = value.strip()
        if value and not trimmed:
            self.fail("whitespace")
        return trimmed


def _max_length(name):
    return Room._meta.get_field(name).max_length


_TRIMMED = "Leading and trailing whitespace is removed."


class RoomSerializer(serializers.ModelSerializer):
    """A room, for reading and (with partial=True) for PATCH."""

    # Declared rather than left to ModelSerializer so they use the strict field classes.
    friendly_name = TrimmedCharField(allow_blank=False, max_length=_max_length("friendly_name"), help_text=_TRIMMED)
    # Not required on create: the model's default picks the next free number.
    displayOrder = StrictIntegerField(min_value=1, max_value=65535, required=False)
    room_temp_provider = TrimmedCharField(allow_blank=True, max_length=_max_length("room_temp_provider"), required=False, help_text=_TRIMMED)
    room_temp_sensor = TrimmedCharField(allow_blank=True, max_length=_max_length("room_temp_sensor"), required=False, help_text=_TRIMMED)

    class Meta:
        model = Room
        fields = ["id", "friendly_name", "displayOrder", "room_temp_provider", "room_temp_sensor"]
        read_only_fields = ["id"]

    def to_internal_value(self, data):
        # Unknown keys are an error rather than ignored: the UI is built with the server, so one it
        # doesn't know is a bug (or a typo) that a 200 would hide.
        if isinstance(data, dict):
            writable = {name for name, field in self.fields.items() if not field.read_only}
            errors = {key: ["This field is read-only."] if key in self.fields else ["Unknown field."] for key in data if key not in writable}
            if errors:
                raise serializers.ValidationError(errors)
        return super().to_internal_value(data)


def _build_errors_serializer():
    messages = {"__module__": __name__, "__doc__": "A 400 from POST/PATCH: error messages by field name. Maps onto react-hook-form's setError()."}
    for name in [*RoomSerializer().fields, "non_field_errors"]:
        messages[name] = serializers.ListField(child=serializers.CharField(), required=False)
    # Instead of the above, for a body that isn't valid JSON.
    messages["detail"] = serializers.CharField(required=False)
    return type("RoomErrorsSerializer", (serializers.Serializer,), messages)


RoomErrorsSerializer = _build_errors_serializer()


def create_entity_pages_for_room(room):
    from web.models import RoomEntitiesPage

    # Check if an existing scenes page exists, if it does not, create one
    existing_pages = RoomEntitiesPage.objects.filter(room=room, is_scenes_page=True)
    if not existing_pages.exists():
        page = RoomEntitiesPage()
        page.room = room
        page.display_order = 0
        page.page_type = 4
        page.is_scenes_page = True
        page.save()

    # Check if an existing entities page exists, if it doesn not, create one.
    existing_pages = RoomEntitiesPage.objects.filter(room=room, is_scenes_page=False)
    if not existing_pages.exists():
        page = RoomEntitiesPage()
        page.room = room
        page.display_order = 0
        page.page_type = 4
        page.is_scenes_page = False
        page.save()


class RoomDetailSerializer(serializers.Serializer):
    """DRF's error body for a request it rejects before validation, e.g. invalid JSON or a failed CSRF check."""

    detail = serializers.CharField()


def _error_message(data):
    """Flatten DRF's error body ({"detail": ...}, {"field": [...]}, or a list) into one string."""
    if isinstance(data, dict):
        parts = []
        for field, messages in data.items():
            if isinstance(messages, (list, tuple)):
                messages = " ".join(str(m) for m in messages)
            parts.append(str(messages) if field in ("detail", "non_field_errors") else f"{field}: {messages}")
        return "; ".join(parts)
    if isinstance(data, (list, tuple)):
        return " ".join(str(m) for m in data)
    return str(data)


def error_exception_handler(exc, context):
    response = exception_handler(exc, context)
    if response is not None:
        response.data = {"status": "error", "message": _error_message(response.data)}
    return response


class JsonErrorAPIView(APIView):
    """An APIView whose errors are {"status": "error", "message": ...} instead of {"detail": ...}."""

    def get_exception_handler(self):
        return error_exception_handler


class RoomListView(JsonErrorAPIView):
    http_method_names = ["get", "post", "head"]

    @extend_schema(responses=RoomSerializer(many=True))
    def get(self, request):
        rooms = Room.objects.order_by("displayOrder", "id")
        return Response(RoomSerializer(rooms, many=True).data)

    @extend_schema(
        request=RoomSerializer,
        responses={201: RoomSerializer, 400: RoomErrorsSerializer, 403: RoomDetailSerializer, 415: RoomDetailSerializer},
    )
    def post(self, request):
        serializer = RoomSerializer(data=request.data)
        serializer.is_valid(raise_exception=True)

        room = serializer.save()
        logger.info("Room created: %s", room)

        with transaction.atomic():
            create_entity_pages_for_room(room)

        send_mqttmanager_reload_command()
        return Response(RoomSerializer(room).data, status=status.HTTP_201_CREATED)


class RoomDetailView(JsonErrorAPIView):
    http_method_names = ["get", "patch", "delete", "head"]

    @extend_schema(responses={200: RoomSerializer, 404: RoomDetailSerializer})
    def get(self, request, pk):
        return Response(RoomSerializer(get_object_or_404(Room, pk=pk)).data)

    @extend_schema(
        request=RoomSerializer,
        responses={200: RoomSerializer, 400: RoomErrorsSerializer, 403: RoomDetailSerializer, 404: RoomDetailSerializer, 415: RoomDetailSerializer},
    )
    def patch(self, request, pk):
        with transaction.atomic():
            room = get_object_or_404(Room.objects.select_for_update(), pk=pk)
            serializer = RoomSerializer(room, data=request.data, partial=True)
            serializer.is_valid(raise_exception=True)

            # Write only what differs, so a client sending back the values it was given changes nothing.
            changed = [name for name, value in serializer.validated_data.items() if getattr(room, name) != value]
            if changed:
                serializer.save()
        if changed:
            logger.info("Room %s changed: %s", room.id, ", ".join(changed))
            send_mqttmanager_reload_command()
        return Response(RoomSerializer(room).data)

    @extend_schema(responses={204: None, 404: RoomDetailSerializer})
    def delete(self, request, pk):
        room = get_object_or_404(Room, pk=pk)
        try:
            # Panels are not deleted with their room; move them to another room.
            new_room = Room.objects.exclude(id=room.id).first()
            if new_room:
                NSPanel.objects.filter(room=room).update(room=new_room)
            elif Room.objects.count() <= 1:
                # Panels must belong to a room, so the last room can not be deleted.
                return Response(status=status.HTTP_400_BAD_REQUEST, data={"status": "error", "message": "The last room cannot be deleted."})

            logger.info("Room deleted: %s", room)
            room.delete()
            send_mqttmanager_reload_command()
            return Response(status=status.HTTP_204_NO_CONTENT)
        except Exception as ex:
            logging.exception(ex)
            return Response(status=status.HTTP_400_BAD_REQUEST, data={"status": "error", "message": ex})
