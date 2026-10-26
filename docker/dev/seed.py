# Seeds the dev stack's database (run with `manage.py shell` by nspm-entrypoint.sh) so
# MQTTManager connects to the stack's own broker and there is a room to put panels in. It
# only fills in what is missing, so it is safe to run on every start.
from web.models import Room
from web.settings_helper import does_setting_exist, set_setting_value

settings = {
    "mqtt_server": "mosquitto",
    "mqtt_port": "1883",
    "manager_address": "nspm",
    "manager_port": "8000",
}
for name, value in settings.items():
    if not does_setting_exist(name):
        set_setting_value(name, value)

if not Room.objects.exists():
    Room.objects.create(friendly_name="Dev Room")

print("Dev stack database seeded.")
