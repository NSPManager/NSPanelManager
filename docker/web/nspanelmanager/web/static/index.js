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

function startNSPanelOtaUpdate(dom) {
  let mac_selector = $(dom).closest(".nspanel-box").find(".nspanel_mac_container").text();
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

function startNSPanelTftUpdate(dom) {
  let mac_selector = $(dom).closest(".nspanel-box").find(".nspanel_mac_container").text();
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

function rebootNSPanel(dom) {
  let mac_selector = $(dom).closest(".nspanel-box").find(".nspanel_mac_container").text();
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

function restart_mqtt_manager() {
  $.post("/api/restart_mqtt_manager", (data) => {

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
    let mac_selector = mac.replaceAll(":", ""); // Convert real MAC to DOM-MAC
    if (total_warning_string == "") {
      $("#nspanel_warninigs_" + mac_selector).addClass("is-hidden");
    } else if (total_warning_string != "") {
      $("#nspanel_warnings_" + mac_selector).removeClass("is-hidden");
      $("#nspanel_warnings_" + mac_selector).attr(
        "data-tooltip",
        total_warning_string
      );
    }
  }
}

function show_dropdown_menu(dom) {
  $(dom).closest(".dropdown").toggleClass("is-active");
  event.stopPropagation();
}

function create_nspanel_from_template(data) {
  // Create a new NSPanel box from template. This will replace all data
  // neccesary for the update function to fill in the rest.
  var nspanel = $($("#nspanel_index_box_template").html());

  // Setup links
  nspanel.find("#nspanel_name").attr("href", "/nspanel/" + data.id);
  nspanel.find(".nspanel-settings-link").attr("href", "/nspanel/" + data.id);
  nspanel.find(".nspanel-visit-link").attr("href", "http://" + data.ip_address);

  nspanel.find("#panel_header").attr("id", "panel_header_" + data.mac_address);
  nspanel.find("#nspanel_id").text(data.id);
  nspanel.find("#nspanel_id").attr("id", "nspanel_id_" + data.mac_address);
  nspanel.find("#nspanel_mac_container").text(data.mac_address);
  nspanel.find("#nspanel_mac_container").attr("id", "nspanel_mac_container_" + data.mac_address);
  nspanel.find("#panel_header_text").attr("id", "panel_header_text_" + data.mac_address);
  nspanel.find("#wifi_signal_strength").attr("id", "wifi_signal_strength_" + data.mac_address);
  nspanel.find("#wifi_signal_strength_text").attr("id", "wifi_signal_strength_text_" + data.mac_address);
  nspanel.find("#temperature").attr("id", "temperature_" + data.mac_address);
  nspanel.find("#heap_used").attr("id", "heap_used_" + data.mac_address);
  nspanel.find("#nspanel_warnings").attr("id", "nspanel_warnings_" + data.mac_address);
  nspanel.find("#nspanel_name").html(data.name);
  nspanel.find("#nspanel_name").attr("id", "nspanel_name_" + data.mac_address);

  $("#nspanels_container").append(nspanel);
  update_nspanel_status(data);
}

function update_nspanel_status(data) {
  if ("mac_address" in data) {
    
    if($("#panel_header_" + data.mac_address).length == 0) {
      create_nspanel_from_template(data);
      return;
    }

    if ("state" in data) {
      var new_html = "";
      if (data.state == "online") {
        if ($("#panel_header_" + data.mac_address).length == 0) {
          // We got an online message from a newly registed panel. Updated page in about 1 second.
          setTimeout(() => {
            location.reload();
          }, 1000);
        }
        if (
          $("#panel_header_" + data.mac_address).hasClass(
            "has-background-danger"
          ) ||
          $("#panel_header_" + data.mac_address).hasClass(
            "has-background-success-dark"
          )
        ) {
          // Current state is offline, just about to update to online. Check if the panel has any warnings.
          updateNSPanelsWarnings();
        }

        $("#panel_header_" + data.mac_address).attr(
          "class",
          "nspanel-status-header has-background-success"
        );
        $("#panel_header_text_" + data.mac_address).text("");
        $("#panel_header_" + data.mac_address).css("width", "100%");
      } else if (data.state == "offline") {
        $("#panel_header_" + data.mac_address).attr(
          "class",
          "nspanel-status-header has-background-danger"
        );
        $("#panel_header_text_" + data.mac_address).text("Offline");
        $("#panel_header_" + data.mac_address).css("width", "100%");
      } else if (data.state == "unknown") {
        $("#panel_header_" + data.mac_address).attr(
          "class",
          "nspanel-status-header has-background-grey"
        );
        $("#panel_header_text_" + data.mac_address).text("Unknown");
        $("#panel_header_" + data.mac_address).css("width", "100%");
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
          $("#panel_header_" + data.mac_address).attr(
            "class",
            "nspanel-status-header has-background-success-dark"
          );
        } else {
          $("#panel_header_" + data.mac_address).attr(
            "class",
            "nspanel-status-header has-background-info"
          );
        }

        $("#panel_header_" + data.mac_address).css("width", update_progress + "%");
        $("#panel_header_text_" + data.mac_address).text(
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
      $("#wifi_signal_strength_text_" + data.mac_address).html(data.rssi + " dBm");
      $("#wifi_signal_strength_" + data.mac_address).attr(
        "class",
        new_rssi_classes
      );
      $("#wifi_signal_strength_" + data.mac_address).attr(
        "title",
        data.rssi + " dBm"
      );
    }

    if ("heap_used_pct" in data) {
      $("#heap_used_" + data.mac_address)
        .text(data.heap_used_pct + "%")
        .text()
        .slice(-2);
    }

    if ("temperature" in data) {
      var temperature_unit = $("#temperature_unit").text();
      $("#temperature_" + data.mac_address).html(
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

function add_nspanel(data) {}

$(document).ready(function () {

  ws.register_message_handler((message) => {
    if ("type" in message) {
      if (message.type == "status") {
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

    ws.send_command("get_nspanels_status", {}, (response) => {
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
  console.log("Starting connect to WebSocket.");
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

  $(window).click(function () {
    $(".dropdown").removeClass("is-active");
  });

  updateNSPanelsWarnings();
});
