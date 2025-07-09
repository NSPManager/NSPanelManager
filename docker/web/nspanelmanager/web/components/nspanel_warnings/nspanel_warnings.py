from django_components import component
from django.template.context import Context
from django.core.files.storage import FileSystemStorage
import hashlib

from web.mqttmanager_ipc import send_ipc_request
from web.models import NSPanel
from web.settings_helper import get_nspanel_setting_with_default

@component.register("nspanel_warnings")
class NSPanelWarnings(component.Component):
    template_view = None

    def get_context_data(self, id, warnings):
        return {
            "id": id,
            "warnings": warnings
        }

    def get_template_name(self, context: Context):
        if self.template_view:
            url_name = self.template_view
        else:
            url_name = context.request.resolver_match.url_name
        if url_name == "htmx_partial_index_nspanels_section" or url_name == "index":
            return "nspanel_warnings/nspanel_warnings_index.html"

    # TODO: Move md5 checksum to DB instead of always calculating them on reqest.
    def get_file_md5sum(self, filename):
        fs = FileSystemStorage()
        if fs.exists(filename):
            return hashlib.md5(fs.open(filename).read()).hexdigest()
        else:
            return None

    def get(self, request, view, nspanel_id):
        panel_status = send_ipc_request(F"nspanel/{nspanel_id}/status", {"command": "get"})
        self.template_view = view

        md5_firmware = self.get_file_md5sum("firmware.bin")
        md5_data_file = self.get_file_md5sum("data_file.bin")
        tft_eu_checksums = {
            "tft1": self.get_file_md5sum("HMI_files/tft_automation/eu/output_tft1/gui.tft"),
            "tft2": self.get_file_md5sum("HMI_files/tft_automation/eu/output_tft2/gui.tft"),
            "tft3": self.get_file_md5sum("HMI_files/tft_automation/eu/output_tft3/gui.tft"),
            "tft4": self.get_file_md5sum("HMI_files/tft_automation/eu/output_tft4/gui.tft"),
        }
        tft_us_checksums = {
            "tft1": self.get_file_md5sum("HMI_files/tft_automation/us/output_tft1/gui.tft"),
            "tft2": self.get_file_md5sum("HMI_files/tft_automation/us/output_tft2/gui.tft"),
            "tft3": self.get_file_md5sum("HMI_files/tft_automation/us/output_tft3/gui.tft"),
            "tft4": self.get_file_md5sum("HMI_files/tft_automation/us/output_tft4/gui.tft"),
        }

        warnings = []
        nspanel = NSPanel.objects.get(id=nspanel_id)
        for panel in NSPanel.objects.filter(denied=False):
            if panel == nspanel:
                continue
            elif panel.friendly_name == nspanel.friendly_name:
                warnings.append({
                    "level": "error",
                    "text": "Two or more panels exists with the same name. This may have unintended consequences"
                })
                break
        if nspanel.md5_firmware != md5_firmware or nspanel.md5_data_file != md5_data_file:
            warnings.append({
                "level": "info",
                "text": "Firmware update available."
            })

        selected_tft = get_nspanel_setting_with_default(nspanel.id, "selected_tft", "tft1")
        is_us_panel = get_nspanel_setting_with_default(nspanel.id, "is_us_panel", "False")
        us_panel_orientation = get_nspanel_setting_with_default(nspanel.id, "us_panel_orientation", "vertical")
        if is_us_panel == "False" and nspanel.md5_tft_file != tft_eu_checksums[selected_tft]:
            warnings.append({
                "level": "info",
                "text": "GUI update available."
            })
        elif is_us_panel == "True" and us_panel_orientation == "horizontal" and nspanel.md5_tft_file != tft_eu_checksums[selected_tft]:
            # We use EU tft file for horizontal US panel with buttons on left as it's the same screen and orientation
            warnings.append({
                "level": "info",
                "text": "GUI update available."
            })
        elif is_us_panel == "True" and us_panel_orientation == "vertical" and nspanel.md5_tft_file != tft_us_checksums[selected_tft]:
            warnings.append({
                "level": "info",
                "text": "GUI update available."
            })

        args = {
            "id": nspanel_id,
            "warnings": warnings
        }
        return self.render_to_response(kwargs=args)

    class Media:
        css = "css/main.css"
