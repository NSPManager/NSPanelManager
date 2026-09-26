"""
Creating and editing entities through the /rest/entities/* endpoints, using the same
payloads the React "add/edit entity" modal sends.
"""

from django.urls import reverse

from web.models import Entity, Scene

from .harness import NSPMTestCase


def placement(page, slot):
    return {"room_id": page.room_id, "entities_page_id": page.id, "room_view_position": slot}


class EntityRESTTestCase(NSPMTestCase):
    """Base for the per-type entity endpoints. Subclasses define url and payload()."""

    url = None
    model = Entity

    def setUp(self):
        super().setUp()
        self.room = self.make_room("Kitchen")
        self.page = self.entities_page(self.room)

    def payload(self, slot=0, **overrides):
        raise NotImplementedError

    def fields(self, payload):
        """The dict holding the entity fields (lights wrap them in "values")."""
        return payload

    def save(self, payload):
        return self.put_json(reverse(self.url), payload)

    def assertCreatesNothing(self, response):
        self.assertRejected(response)
        self.assertFalse(self.model.objects.exists())
        self.assertManagerNotReloaded()


class EntityRESTCommonTests:
    """Checks shared by every entity type. A mixin so it is not collected and run on its own."""

    def test_every_required_field_is_enforced(self):
        for field in list(self.fields(self.payload())):
            with self.subTest(missing=field):
                payload = self.payload()
                del self.fields(payload)[field]
                response = self.save(payload)
                self.assertEqual(response.status_code, 400)
                self.assertIn(field, response.json()["message"])
        self.assertFalse(self.model.objects.exists())
        self.assertManagerNotReloaded()

    def test_unknown_room_or_page_is_rejected(self):
        for field in ("room_id", "entities_page_id"):
            with self.subTest(field=field):
                self.assertRejected(self.save(self.payload(**{field: 999})))
        self.assertFalse(self.model.objects.exists())
        self.assertManagerNotReloaded()

    def test_malformed_body_is_rejected(self):
        self.assertCreatesNothing(self.put_json(reverse(self.url), "not json"))

    def test_edit_updates_in_place(self):
        self.save(self.payload(slot=1))
        obj = self.model.objects.get()
        self.mqttmanager.reset()

        response = self.save(self.payload(slot=1, id=obj.id, friendly_name="Renamed"))

        self.assertEqual(response.status_code, 200)
        self.assertEqual(self.model.objects.count(), 1)
        obj.refresh_from_db()
        self.assertEqual(obj.friendly_name, "Renamed")
        self.assertEqual(obj.room_view_position, 1)
        self.assertManagerReloaded(times=1)

    def test_edit_unknown_id_is_rejected(self):
        self.assertCreatesNothing(self.save(self.payload(id=999)))

    def test_other_methods_are_rejected(self):
        # POST carries a valid payload, so only the method check can stop it being saved.
        self.assertEqual(self.client.get(reverse(self.url)).status_code, 405)
        self.assertCreatesNothing(self.post_json(reverse(self.url), self.payload()))

    def test_add_to_occupied_slot_is_rejected(self):
        self.save(self.payload(slot=0))

        self.assertRejected(self.save(self.payload(slot=0)))
        self.assertEqual(self.model.objects.count(), 1)

    def test_add_to_slot_outside_page_is_rejected(self):
        self.assertCreatesNothing(self.save(self.payload(slot=7)))


class LightTests(EntityRESTCommonTests, EntityRESTTestCase):
    url = "rest_entities_lights"

    def payload(self, slot=0, **overrides):
        values = {
            **placement(self.page, slot),
            "controller": "home_assistant",
            "type": "light",
            "friendly_name": "Ceiling",
            "can_color_temperature": True,
            "can_dim": True,
            "can_rgb": False,
            "controlled_by_nspanel_main_page": True,
            "home_assistant_name": "light.kitchen_ceiling",
            "is_ceiling_light": True,
            "openhab_item_color_temp": "",
            "openhab_item_dimmer": "",
            "openhab_item_rgb": "",
        }
        return {"values": values | overrides}

    def fields(self, payload):
        return payload["values"]

    def test_add_home_assistant_light(self):
        response = self.save(self.payload(slot=2))

        self.assertEqual(response.status_code, 200)
        light = Entity.objects.get()
        self.assertEqual(light.entity_type, Entity.EntityType.LIGHT)
        self.assertEqual(light.friendly_name, "Ceiling")
        self.assertEqual((light.room, light.entities_page, light.room_view_position), (self.room, self.page, 2))
        self.assertEqual(light.entity_data["home_assistant_name"], "light.kitchen_ceiling")
        self.assertTrue(light.entity_data["can_dim"])
        self.assertTrue(light.entity_data["can_color_temperature"])
        self.assertFalse(light.entity_data["can_rgb"])
        self.assertTrue(light.entity_data["is_ceiling_light"])
        self.assertLoadableByManager(light)
        self.assertManagerReloaded(times=1)

    def test_add_openhab_light(self):
        payload = self.payload(controller="openhab", home_assistant_name="", can_dim=False, openhab_item_dimmer="Lamp_Switch")

        self.assertEqual(self.save(payload).status_code, 200)

        light = Entity.objects.get()
        self.assertEqual(light.entity_data["openhab_control_mode"], "switch")
        self.assertEqual(light.entity_data["openhab_item_dimmer"], "Lamp_Switch")
        self.assertLoadableByManager(light)

    def test_boolean_flags_accept_strings(self):
        self.save(self.payload(can_dim="false", can_rgb="true", is_ceiling_light="False"))

        data = Entity.objects.get().entity_data
        self.assertEqual((data["can_dim"], data["can_rgb"], data["is_ceiling_light"]), (False, True, False))

    def test_unknown_controller_is_rejected(self):
        self.assertCreatesNothing(self.save(self.payload(controller="zigbee")))

    def test_page_from_another_room_is_rejected(self):
        other_page = self.entities_page(self.make_room("Lounge"))

        self.assertCreatesNothing(self.save(self.payload(entities_page_id=other_page.id)))

    def test_entity_saved_before_placement_checks_can_still_be_edited(self):
        # Only a changed placement is validated, so rows already in a bad slot stay editable.
        light = self.make_light(self.room, slot=7)

        response = self.save(self.payload(slot=7, id=light.id, friendly_name="Renamed"))

        self.assertEqual(response.status_code, 200)
        light.refresh_from_db()
        self.assertEqual(light.friendly_name, "Renamed")


class SwitchTests(EntityRESTCommonTests, EntityRESTTestCase):
    url = "rest_entities_switches"

    def payload(self, slot=0, **overrides):
        return {
            **placement(self.page, slot),
            "controller": "home_assistant",
            "type": "switch",
            "friendly_name": "Fan",
            "home_assistant_name": "switch.kitchen_fan",
            "openhab_item_switch": "",
        } | overrides

    def test_add_switch(self):
        self.assertEqual(self.save(self.payload()).status_code, 200)

        switch = Entity.objects.get()
        self.assertEqual(switch.entity_type, Entity.EntityType.SWITCH)
        self.assertEqual(switch.entity_data["home_assistant_name"], "switch.kitchen_fan")
        self.assertLoadableByManager(switch)
        self.assertManagerReloaded(times=1)

    def test_add_openhab_switch(self):
        self.save(self.payload(controller="openhab", home_assistant_name="", openhab_item_switch="Kitchen_Fan"))

        switch = Entity.objects.get()
        self.assertEqual(switch.entity_data["openhab_item_switch"], "Kitchen_Fan")
        self.assertLoadableByManager(switch)


class ButtonTests(EntityRESTCommonTests, EntityRESTTestCase):
    url = "rest_entities_buttons"

    def payload(self, slot=0, **overrides):
        return {
            **placement(self.page, slot),
            "controller": "nspm",
            "type": "button",
            "friendly_name": "Gate",
            "home_assistant_name": "",
            "mqtt_topic": "gate/cmd",
            "mqtt_payload": "open",
        } | overrides

    def test_add_mqtt_button(self):
        self.assertEqual(self.save(self.payload()).status_code, 200)

        button = Entity.objects.get()
        self.assertEqual(button.entity_type, Entity.EntityType.BUTTON)
        self.assertEqual((button.entity_data["mqtt_topic"], button.entity_data["mqtt_payload"]), ("gate/cmd", "open"))
        self.assertLoadableByManager(button)
        self.assertManagerReloaded(times=1)

    def test_add_home_assistant_button(self):
        self.save(self.payload(controller="home_assistant", home_assistant_name="input_button.doorbell", mqtt_topic="", mqtt_payload=""))

        button = Entity.objects.get()
        self.assertEqual(button.entity_data["home_assistant_name"], "input_button.doorbell")
        self.assertLoadableByManager(button)

    def test_openhab_controller_is_rejected(self):
        # MQTTManager only has Home Assistant and MQTT ("nspm") buttons.
        self.assertCreatesNothing(self.save(self.payload(controller="openhab")))


class ThermostatTests(EntityRESTCommonTests, EntityRESTTestCase):
    url = "rest_entities_thermostats"

    def payload(self, slot=0, **overrides):
        return {
            **placement(self.page, slot),
            "controller": "home_assistant",
            "type": "thermostat",
            "friendly_name": "Heat pump",
            "step_size": "0.5",
            "use_current_temperature": "True",
            "home_assistant_name": "climate.kitchen",
            "openhab_fan_mode_item": "",
            "openhab_hvac_mode_item": "",
            "openhab_preset_mode_item": "",
            "openhab_swing_mode_item": "",
            "openhab_swingh_mode_item": "",
            "openhab_temperature_item": "",
            "fan_modes": [],
            "hvac_modes": [{"value": "heat", "icon": "fire", "label": "Heat"}, {"value": "cool", "icon": "snow", "label": "Cool"}],
            "preset_modes": [],
            "swing_modes": [],
            "swingh_modes": [],
        } | overrides

    def test_add_thermostat(self):
        self.assertEqual(self.save(self.payload()).status_code, 200)

        thermostat = Entity.objects.get()
        self.assertEqual(thermostat.entity_type, Entity.EntityType.THERMOSTAT)
        self.assertEqual(thermostat.entity_data["step_size"], 0.5)
        self.assertEqual([m["value"] for m in thermostat.entity_data["hvac_modes"]], ["heat", "cool"])
        self.assertEqual(thermostat.entity_data["home_assistant_name"], "climate.kitchen")
        self.assertIs(thermostat.entity_data["use_current_temperature"], True)
        self.assertLoadableByManager(thermostat)
        self.assertManagerReloaded(times=1)

    def test_mode_missing_a_field_is_rejected(self):
        for field in ("value", "icon", "label"):
            with self.subTest(missing=field):
                mode = {"value": "heat", "icon": "fire", "label": "Heat"}
                del mode[field]
                self.assertEqual(self.save(self.payload(hvac_modes=[mode])).status_code, 400)
        self.assertFalse(Entity.objects.exists())

    def test_non_numeric_step_size_is_rejected(self):
        self.assertCreatesNothing(self.save(self.payload(step_size="lots")))


class SceneTests(EntityRESTCommonTests, EntityRESTTestCase):
    url = "rest_entities_scenes"
    model = Scene

    def setUp(self):
        super().setUp()
        self.page = self.scenes_page(self.room)

    def payload(self, slot=0, **overrides):
        return {
            **placement(self.page, slot),
            "controller": "home_assistant",
            "type": "scene",
            "friendly_name": "Movie",
            "scene_type": "home_assistant",
            "backend_name": "scene.movie_time",
        } | overrides

    def test_add_room_scene(self):
        self.assertEqual(self.save(self.payload(slot=1)).status_code, 200)

        scene = Scene.objects.get()
        self.assertEqual((scene.room, scene.entities_page, scene.room_view_position), (self.room, self.page, 1))
        self.assertEqual(scene.backend_name, "scene.movie_time")
        self.assertLoadableByManager(scene)
        self.assertManagerReloaded(times=1)

    def test_add_global_nspm_scene(self):
        page = self.global_scenes_page()

        response = self.save(self.payload(room_id=None, entities_page_id=page.id, controller="nspm", scene_type="nspm_scene", backend_name=""))

        self.assertEqual(response.status_code, 200)
        scene = Scene.objects.get()
        self.assertIsNone(scene.room)
        self.assertLoadableByManager(scene)

    def test_home_assistant_scene_without_backend_name_is_rejected(self):
        self.assertCreatesNothing(self.save(self.payload(backend_name="")))

    def test_unknown_scene_type_is_rejected(self):
        self.assertCreatesNothing(self.save(self.payload(scene_type="zigbee")))
