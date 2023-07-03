from .models import Settings, NSPanelSettings, NSPanel


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


def get_nspanel_setting_with_default(nspanel_id, name, default):
    panel = NSPanel.objects.filter(id=nspanel_id)
    if panel.count() > 0:
        objects = NSPanelSettings.objects.filter(nspanel=panel.first(), name=name)
        if objects.count() > 0:
            return objects.first().value
        else:
            return default
    else:
        return default


def delete_nspanel_setting(nspanel_id, name):
    panel = NSPanel.objects.filter(id=nspanel_id)
    if panel.count() > 0:
        objects = NSPanelSettings.objects.filter(nspanel=panel.first(), name=name)
        if objects.count() > 0:
            return objects.first().delete()


def set_nspanel_setting_value(nspanel_id, name, value):
    panel = NSPanel.objects.filter(id=nspanel_id)
    if panel.count() > 0:
        NSPanelSettings.objects.update_or_create(nspanel=panel.first(), name=name, defaults={
            "value": value
        })
