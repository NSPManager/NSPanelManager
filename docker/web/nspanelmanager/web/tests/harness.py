"""
Shared test harness for the NSPanelManager Django app.

Every test should inherit from NSPMTestCase. It gives you:

  * An isolated in-memory database per test (via django.test.TestCase).
  * self.mqttmanager -- a stand-in for the MQTTManager process. Views that change
    config call send_mqttmanager_reload_command(); here that is recorded instead of
    SIGUSR1-ing whatever nspm_mqttmanager happens to be running on the host. Use
    self.assertManagerReloaded() / self.assertManagerNotReloaded() to verify the
    manager would have been told to pick up the change.
  * self.home_assistant -- a fake Home Assistant /api/states endpoint. Add states with
    self.home_assistant.add("light.kitchen", friendly_name="Kitchen").
    self.openhab_items -- the item list returned by the (faked) OpenHAB API.
  * assertLoadableByManager(obj) -- checks a saved Entity/Scene has the shape MQTTManager
    parses when it reads the database, e.g. entity_data["controller"] is a known string.
  * self.client -- a Django test client that returns 500 responses instead of raising,
    so tests can assert that bad input is rejected the same way a browser would see it.
  * Builders for common fixtures (make_room, make_light, make_scene, ...) and put_json /
    post_json helpers that send requests the way the React UI does.
"""

import json
import sys
from typing import ClassVar
from unittest import mock

from django.test import Client, TestCase

import web.apps
from web.models import Entity, NSPanel, Room, RoomEntitiesPage, Scene
from web.settings_helper import get_setting_with_default, set_setting_value

# MQTTManager process-control helpers in web.apps. Modules import these by name
# (`from .apps import x`), which binds a separate reference, so every web.* module holding
# one is patched -- found at runtime so that adding or removing an import needs no change here.
_MANAGER_HOOKS = ("send_mqttmanager_reload_command", "restart_mqtt_manager_process", "start_mqtt_manager")


def _modules_holding(name):
    original = getattr(web.apps, name)
    return [module_name for module_name, module in list(sys.modules.items()) if (module_name == "web" or module_name.startswith("web.")) and getattr(module, name, None) is original]


class FakeMQTTManager:
    """Records the process-control calls the web app makes towards MQTTManager."""

    def __init__(self):
        self.reloads = 0
        self.restarts = 0

    def _reload(self):
        self.reloads += 1

    def _restart(self):
        self.restarts += 1

    def reset(self):
        self.reloads = 0
        self.restarts = 0


class FakeHomeAssistant:
    """Serves a canned /api/states response to web.home_assistant_api."""

    def __init__(self):
        self.states = []
        self.fail = False

    def add(self, entity_id, friendly_name=None, state="off", **attributes):
        if friendly_name is not None:
            attributes["friendly_name"] = friendly_name
        self.states.append({"entity_id": entity_id, "state": state, "attributes": attributes})

    def _get(self, url, *args, **kwargs):
        response = mock.Mock()
        if self.fail or not url.endswith("/api/states"):
            response.status_code = 500
            response.json.return_value = {}
        else:
            response.status_code = 200
            response.json.return_value = self.states
        return response


class NSPMTestCase(TestCase):
    def setUp(self):
        super().setUp()
        self.client = Client(raise_request_exception=False)

        # Data migrations seed a "Dummy room" (and pages for it). Start every test from an
        # empty configuration so tests only see what they create themselves.
        Room.objects.all().delete()
        RoomEntitiesPage.objects.all().delete()

        self.mqttmanager = FakeMQTTManager()
        hooks = {
            "send_mqttmanager_reload_command": self.mqttmanager._reload,
            "restart_mqtt_manager_process": self.mqttmanager._restart,
            "start_mqtt_manager": self.mqttmanager._restart,
        }
        for name in _MANAGER_HOOKS:
            for module in _modules_holding(name):
                self._patch(f"{module}.{name}", side_effect=hooks[name])
        # Safety net: even if a new code path finds another way to reach these, never
        # let a test signal or spawn a real MQTTManager on the host.
        self._patch("web.apps.psutil.process_iter", return_value=[])
        self._patch("web.apps.subprocess.Popen", side_effect=AssertionError("Tests must not start MQTTManager"))

        self.home_assistant = FakeHomeAssistant()
        set_setting_value("home_assistant_address", "http://home-assistant.test")
        set_setting_value("home_assistant_token", "test-token")

        # OpenHAB is faked one level higher, at its item-listing functions.
        self.openhab_items = []
        self._patch("web.openhab_api.get_all_openhab_items", side_effect=lambda *args, **kwargs: {"items": self.openhab_items, "errors": []})
        self._patch("web.openhab_api.get_all_openhab_scenes", side_effect=lambda *args, **kwargs: {"items": self.openhab_items, "errors": []})

        # Every module shares the one `requests` module, so route all HTTP through here:
        # Home Assistant goes to the fake, anything else fails the test.
        self._patch("requests.get", side_effect=self._fake_http_get)

    def _fake_http_get(self, url, *args, **kwargs):
        if url.startswith(get_setting_with_default("home_assistant_address")):
            return self.home_assistant._get(url, *args, **kwargs)
        raise AssertionError(f"Unexpected network request from test: GET {url}")

    def _patch(self, target, **kwargs):
        patcher = mock.patch(target, **kwargs)
        patcher.start()
        self.addCleanup(patcher.stop)

    # ---------------------------------------------------------------- assertions

    def assertManagerReloaded(self, times=None):
        if times is None:
            self.assertGreater(self.mqttmanager.reloads, 0, "Expected MQTTManager to be told to reload its config")
        else:
            self.assertEqual(self.mqttmanager.reloads, times, f"Expected MQTTManager to be told to reload {times} time(s)")

    def assertManagerNotReloaded(self):
        self.assertEqual(self.mqttmanager.reloads, 0, "MQTTManager was told to reload although nothing should have changed")

    # MQTTManager reads entities straight from the SQLite database. These are the values its
    # entity classes (docker/MQTTManager/include/{light,switch,button,thermostat}/) accept for
    # entity_data["controller"]; anything else is logged as an error, and a non-string throws.
    MANAGER_CONTROLLERS: ClassVar = {
        Entity.EntityType.LIGHT: {"home_assistant", "openhab"},
        Entity.EntityType.SWITCH: {"home_assistant", "openhab"},
        Entity.EntityType.BUTTON: {"home_assistant", "nspm"},
        Entity.EntityType.THERMOSTAT: {"home_assistant", "openhab"},
    }
    MANAGER_SCENE_TYPES: ClassVar = {"home_assistant", "openhab", "nspm_scene"}

    def assertLoadableByManager(self, obj):
        """Check a saved Entity/Scene row has the shape MQTTManager expects to parse."""
        obj.refresh_from_db()
        if isinstance(obj, Scene):
            self.assertIn(obj.scene_type, self.MANAGER_SCENE_TYPES)
            if obj.scene_type != "nspm_scene":
                self.assertTrue(obj.backend_name, "Home Assistant/OpenHAB scenes need a backend_name to activate")
        else:
            controller = obj.entity_data.get("controller")
            self.assertIsInstance(controller, str, f"entity_data['controller'] must be a string, got {controller!r}")
            self.assertIn(controller, self.MANAGER_CONTROLLERS[obj.entity_type])
        self.assertIsNotNone(obj.entities_page_id)
        self.assertLess(obj.room_view_position, obj.entities_page.page_type, "Entity is placed in a slot outside its page")

    def assertRejected(self, response):
        """The request was refused, however the view chose to signal it."""
        if response.status_code >= 400:
            return
        if response.get("Content-Type", "").startswith("application/json"):
            self.assertEqual(response.json().get("status"), "error", f"Expected an error response, got: {response.content!r}")
            return
        self.fail(f"Expected request to be rejected, got HTTP {response.status_code}")

    # ---------------------------------------------------------------- builders

    def make_room(self, name="Room"):
        """Create a room the way the UI does: with one entities page and one scenes page."""
        room = Room.objects.create(friendly_name=name)
        RoomEntitiesPage.objects.create(room=room, page_type=4, is_scenes_page=True, display_order=0)
        RoomEntitiesPage.objects.create(room=room, page_type=4, is_scenes_page=False, display_order=0)
        return room

    def entities_page(self, room, index=0):
        return RoomEntitiesPage.objects.filter(room=room, is_scenes_page=False).order_by("display_order")[index]

    def scenes_page(self, room, index=0):
        return RoomEntitiesPage.objects.filter(room=room, is_scenes_page=True).order_by("display_order")[index]

    def global_scenes_page(self):
        page, _ = RoomEntitiesPage.objects.get_or_create(room=None, is_scenes_page=True, defaults={"page_type": 4, "display_order": 0})
        return page

    def make_entity(self, room, entity_type=Entity.EntityType.LIGHT, name="Entity", page=None, slot=0, **entity_data):
        entity_data.setdefault("controller", "home_assistant")
        return Entity.objects.create(
            room=room,
            friendly_name=name,
            entity_type=entity_type,
            entities_page=page or self.entities_page(room),
            room_view_position=slot,
            entity_data=entity_data,
        )

    def make_light(self, room, name="Light", **kwargs):
        return self.make_entity(room, Entity.EntityType.LIGHT, name, **kwargs)

    def make_scene(self, room, name="Scene", page=None, slot=0, scene_type="home_assistant", backend_name="scene.test"):
        return Scene.objects.create(
            room=room,
            friendly_name=name,
            scene_type=scene_type,
            backend_name=backend_name,
            entities_page=page or (self.scenes_page(room) if room else self.global_scenes_page()),
            room_view_position=slot,
        )

    def make_panel(self, room, name="Panel", mac="AA:BB:CC:DD:EE:FF", **fields):
        return NSPanel.objects.create(room=room, friendly_name=name, mac_address=mac, model=NSPanel.NSPanelModel.SONOFF, **fields)

    # ---------------------------------------------------------------- REST helpers

    def put_json(self, url, data):
        """Send JSON the way the React UI does (fetch with a JSON body)."""
        return self.client.put(url, json.dumps(data) if not isinstance(data, str) else data, content_type="application/json")

    def post_json(self, url, data):
        return self.client.post(url, json.dumps(data) if not isinstance(data, str) else data, content_type="application/json")
