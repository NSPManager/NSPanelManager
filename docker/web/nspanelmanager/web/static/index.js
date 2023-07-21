var panels_that_are_updating = [];
const ws = new MQTTManager_WS();
var panel_warnings = {};

function get_all_online_panel_macs() {
  var panel_macs = [];
  $(".nspanel_mac_container").each((index, obj) => {
    var panel_mac = $(obj).text();
    let mac_selector = panel_mac.replaceAll(":", "\\:");
    if ($("#panel_header_" + mac_selector).hasClass("has-background-success")) {
      panel_macs.push(panel_mac);
    }
  });
  return panel_macs;
}

function startNSPanelOtaUpdateAll() {
  get_all_online_panel_macs().forEach((mac) => {
    startNSPanelOtaUpdate(mac);
  });
}

function startNSPanelOtaUpdate(mac) {
  let mac_selector = mac;
  mac_selector = mac_selector.replaceAll(":", "\\:");
  $("#panel_header_" + mac_selector).attr(
    "class",
    "nspanel-status-header has-background-info nspanel-status-header-await"
  );
  $("#panel_header_text_" + mac_selector).text("Awaiting status");
  var panel_id = $("#nspanel_id_" + mac_selector).text();
  ws.send_command("firmware_update_nspanels", { nspanels: [panel_id] }, null);
}

function startNSPanelTftUpdateAll() {
  get_all_online_panel_macs().forEach((mac) => {
    startNSPanelTftUpdate(mac);
  });
}

function startNSPanelTftUpdate(mac) {
  let mac_selector = mac;
  mac_selector = mac_selector.replaceAll(":", "\\:");
  $("#panel_header_" + mac_selector).attr(
    "class",
    "nspanel-status-header has-background-info nspanel-status-header-await"
  );
  $("#panel_header_text_" + mac_selector).text("Awaiting status");
  var panel_id = $("#nspanel_id_" + mac_selector).text();
  ws.send_command("tft_update_nspanels", { nspanels: [panel_id] }, null);
}

function rebootNSPanelAll() {
  get_all_online_panel_macs().forEach((mac) => {
    rebootNSPanel(mac);
  });
}

function rebootNSPanel(mac) {
  let mac_selector = mac;
  mac_selector = mac_selector.replaceAll(":", "\\:");
  $("#panel_header_" + mac_selector).attr(
    "class",
    "nspanel-status-header has-background-info nspanel-status-header-await"
  );
  $("#panel_header_text_" + mac_selector).text("Awaiting status");
  var panel_id = $("#nspanel_id_" + mac_selector).text();
  ws.send_command("reboot_nspanels", { nspanels: [panel_id] }, null);
}

function updateNSPanelsWarnings() {
  $.get("/api/get_nspanels_warnings", (data) => {
    data.panels.forEach((panel) => {
      if (!(panel.nspanel.mac in panel_warnings)) {
        panel_warnings[panel.nspanel.mac] = {
          api: "",
          websocket: "",
        };
      }
      panel_warnings[panel.nspanel.mac]["api"] = panel.warnings;
    });

    updateDisplayedWarnings();
  });
}

function updateDisplayedWarnings() {
  for (const [mac, data] of Object.entries(panel_warnings)) {
    var total_warning_string = "";
    if (data["api"] != "") {
      total_warning_string = data["api"];
    }
    if (data["websocket"] != "") {
      if (total_warning_string != "") {
        total_warning_string += "\n";
      }
      total_warning_string += data["websocket"];
    }
    let mac_selector = mac.replaceAll(":", "\\:");
    if (total_warning_string == "") {
      $("#nspanel_" + mac_selector + "_warnings").addClass("is-hidden");
    } else if (total_warning_string != "") {
      $("#nspanel_" + mac_selector + "_warnings").removeClass("is-hidden");
      $("#nspanel_" + mac_selector + "_warnings").attr(
        "data-tooltip",
        total_warning_string
      );
    }
  }
}

function show_dropdown_menu(event) {
  $(this).closest(".dropdown").toggleClass("is-active");
  event.stopPropagation();
}

function update_nspanel_status(data) {
  if ("mac" in data) {
    let mac_selector = data.mac;
    mac_selector = mac_selector.replaceAll(":", "\\:");
    if ("state" in data) {
      var new_html = "";
      if (data.state == "online") {
        if ($("#panel_header_" + mac_selector).length == 0) {
          // We got an online message from a newly registed panel. Updated page in about 1 second.
          setTimeout(() => {
            location.reload();
          }, 1000);
        }
        if (
          $("#panel_header_" + mac_selector).hasClass("has-background-danger")
        ) {
          // Current state is offline, just about to update to online. Check if the panel has any warnings.
          updateNSPanelsWarnings();
        }
        $("#panel_header_" + mac_selector).attr(
          "class",
          "nspanel-status-header has-background-success"
        );
        $("#panel_header_text_" + mac_selector).text("");
      } else if (data.state == "offline") {
        $("#panel_header_" + mac_selector).attr(
          "class",
          "nspanel-status-header has-background-danger"
        );
        $("#panel_header_text_" + mac_selector).text("Offline");
      } else {
        // Update panel tag to show update progress if any
        update_text = "";
        update_progress = 0;
        if (data.state == "updating_fw") {
          update_text = "Updating firmware";
          update_progress = data.progress;
        } else if (data.state == "updating_fs") {
          update_text = "Updating LittleFS";
          update_progress = data.progress;
        } else if (data.state == "updating_tft") {
          update_text = "Updating GUI";
          update_progress = data.progress;
        }

        if (update_progress == 100) {
          update_text = "Rebooting";
          $("#panel_header_" + mac_selector).attr(
            "class",
            "nspanel-status-header has-background-success-dark"
          );
        } else {
          $("#panel_header_" + mac_selector).attr(
            "class",
            "nspanel-status-header has-background-info"
          );
        }

        $("#panel_header_" + mac_selector).css("width", update_progress + "%");
        $("#panel_header_text_" + mac_selector).text(
          update_text + ", " + update_progress + "%"
        );
      }
    }
    if ("rssi" in data) {
      var new_rssi_classes = "";
      if (data.rssi <= -90) {
        new_rssi_classes = "mdi mdi-wifi-strength-1-alert";
      } else if (data.rssi <= -80) {
        new_rssi_classes = "mdi mdi-wifi-strength-1";
      } else if (data.rssi <= -67) {
        new_rssi_classes = "mdi mdi-wifi-strength-2";
      } else if (data.rssi <= -55) {
        new_rssi_classes = "mdi mdi-wifi-strength-3";
      } else {
        new_rssi_classes = "mdi mdi-wifi-strength-4";
      }
      $("#wifi_signal_strength_text_" + mac_selector).html(data.rssi + " dBm");
      $("#wifi_signal_strength_" + mac_selector).attr(
        "class",
        new_rssi_classes
      );
      $("#wifi_signal_strength_" + mac_selector).attr(
        "title",
        data.rssi + " dBm"
      );
    }

    if ("heap_used_pct" in data) {
      $("#heap_used_" + mac_selector)
        .text(data.heap_used_pct + "%")
        .text()
        .slice(-2);
    }

    if ("temperature" in data) {
      var temperature_unit = $("#temperature_unit").text();
      $("#temperature_" + mac_selector).html(
        Math.round(data.temperature * 100) / 100 + " " + temperature_unit
      );
    }

    if ("warnings" in data) {
      if (!(data["mac"] in panel_warnings)) {
        panel_warnings[data["mac"]] = {
          api: "",
          websocket: "",
        };
      }
      panel_warnings[data["mac"]]["websocket"] = data.warnings;
      updateDisplayedWarnings();
    }
  }
}

$(document).ready(function () {
  ws.register_message_handler((message) => {
    if ("type" in message) {
      if (message.type == "status" || message.type == "status_report") {
        update_nspanel_status(message.payload);
      } else if (message.type == "mqttmanager_status") {
        if (message.mqtt.connected == false) {
          if ($("#failed_to_connect_mqtt_error").length == 0) {
            $("#notification_holder").append(
              '<div class="notification is-danger" id="failed_to_connect_mqtt_error">Failed to connect to MQTT server. Retrying...</div>'
            );
          }
        } else if (message.mqtt.connected) {
          $("#failed_to_connect_mqtt_error").remove();
          if (
            message.home_assistant.configured &&
            !message.home_assistant.connected
          ) {
            if ($("#failed_to_connect_home_assistant_error").length == 0) {
              $("#notification_holder").append(
                '<div class="notification is-danger" id="failed_to_connect_home_assistant_error">Failed to connect to Home Assistant websocket API. Retrying...</div>'
              );
            }
          } else if (
            message.home_assistant.configured &&
            message.home_assistant.connected
          ) {
            $("#failed_to_connect_home_assistant_error").remove();
            if (!message.home_assistant.auth_ok) {
              if ($("#failed_to_auth_home_assistant_error").length == 0) {
                $("#notification_holder").append(
                  '<div class="notification is-danger" id="failed_to_auth_home_assistant_error">Authentication error while connecting to Home Assistant. Retrying...</div>'
                );
              }
            } else {
              $("#failed_to_auth_home_assistant_error").remove();
            }
          } else if (
            !message.home_assistant.configured ||
            message.home_assistant.connected
          ) {
            $("#failed_to_connect_home_assistant_error").remove();
          }

          if (message.openhab.configured && !message.openhab.connected) {
            if ($("#failed_to_connect_openhab_error").length == 0) {
              $("#notification_holder").append(
                '<div class="notification is-danger" id="failed_to_connect_openhab_error">Failed to connect to OpenHAB websocket API. Retrying...</div>'
              );
            }
          } else if (!message.openhab.configured || message.openhab.connected) {
            $("#failed_to_connect_openhab_error").remove();
          }
        }
      } else {
      }
    }
  });

  ws.register_on_connect_handler(() => {
    console.log("Connected to MQTTManager via websocket.");
    // Remove any connection error notification
    $("#failed_to_connect_error").remove();

    ws.send_command("get_nspanel_status", {}, (response) => {
      for (const [id, nspanel] of Object.entries(response.nspanels)) {
        update_nspanel_status(nspanel);
      }
    });
  });
  ws.register_on_close_handler(() => {
    console.log("Disconnected from MQTTManager via websocket.");
    if ($("#failed_to_connect_error").length == 0) {
      $("#notification_holder").append(
        '<div class="notification is-danger" id="failed_to_connect_error">Failed to connect to MQTTManager. Retrying...</div>'
      );
    }
  });
  ws.connect();

  $("#firmware_upload_file_input").change(function () {
    var fileName = $(this).val().replace("C:\\fakepath\\", "");
    $("#firmware_upload_file_name").html(fileName);
  });

  $("#data_upload_file_input").change(function () {
    var fileName = $(this).val().replace("C:\\fakepath\\", "");
    $("#data_upload_file_name").html(fileName);
  });

  $("#tft_upload_file_input").change(function () {
    var fileName = $(this).val().replace("C:\\fakepath\\", "");
    $("#tft_upload_file_name").html(fileName);
  });

  $(".dropdown").click(show_dropdown_menu);
  $(window).click(function () {
    $(".dropdown").removeClass("is-active");
  });

  updateNSPanelsWarnings();
});
