from .models import Settings


def get_setting_with_default(name, default):
    objects = Settings.objects.filter(name=name)
    if objects.count() > 0:
        return objects.first().value
    else:
        return default


def set_setting_value(name, value):
    Settings.objects.update_or_create(name=name, defaults={
        "value": value
    })
