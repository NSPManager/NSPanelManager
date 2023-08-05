const ws = new MQTTManager_WS();
var mac_address;
var nspanel_id;

function startNSPanelOtaUpdate() {
  ws.send_command("firmware_update_nspanels", { nspanels: [nspanel_id] }, null);
}

function startNSPanelTftUpdate() {
  ws.send_command("tft_update_nspanels", { nspanels: [nspanel_id] }, null);
}

function rebootNSPanel() {
  ws.send_command("reboot_nspanels", { nspanels: [nspanel_id] }, null);
}

function update_nspanel_status(data) {
  if ("mac" in data) {
    let mac_selector = data.mac;
    mac_selector = mac_selector.replaceAll(":", "\\:");
    if ("state" in data) {
      var new_html = "";
      if (data.state == "online") {
        $("#online_offline_tag_" + mac_selector).html("Online");
        $("#online_offline_tag_" + mac_selector).attr(
          "class",
          "tag is-success"
        );
      } else if (data.state == "offline") {
        $("#online_offline_tag_" + mac_selector).html("Offline");
        $("#online_offline_tag_" + mac_selector).attr("class", "tag is-danger");
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

        $("#online_offline_tag_" + mac_selector).html(
          update_text + ": " + update_progress + "%"
        );
        $("#online_offline_tag_" + mac_selector).attr("class", "tag is-info");
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
      $("#wifi_signal_strength_" + mac_selector).html(
        '<span class="' +
          new_rssi_classes +
          '" id="wifi_signal_strength_' +
          data.mac +
          '" title="' +
          data.rssi +
          ' dBm"></span>'
      );
    }

    if ("heap_used_pct" in data) {
      $("#heap_used_" + mac_selector)
        .text(data.heap_used_pct + "%")
        .text()
        .slice(-2);
    }

    if ("temperature" in data) {
      var temperature_unit = $("#temperature_" + mac_selector)
        .text()
        .slice(-2);
      $("#temperature_" + mac_selector).html(
        Math.round(data.temperature * 100) / 100 + " " + temperature_unit
      );
    }
  }
}

function update_shown_elements() {
  if ($("#button1_mode").val() == 0) {
    // Direct mode
    $("#button1_detached_mode_light_label").hide();
    $("#button1_detached_mode_light_select").hide();
    $("#button1_custom_mqtt_container").hide();
  } else if ($("#button1_mode").val() == 1) {
    $("#button1_detached_mode_light_label").show();
    $("#button1_detached_mode_light_select").show();
    $("#button1_custom_mqtt_container").hide();
  } else if ($("#button1_mode").val() == 2) {
    $("#button1_detached_mode_light_label").hide();
    $("#button1_detached_mode_light_select").hide();
    $("#button1_custom_mqtt_container").show();
  } else if ($("#button1_mode").val() == 3) {
    $("#button1_detached_mode_light_label").hide();
    $("#button1_detached_mode_light_select").hide();
    $("#button1_custom_mqtt_container").hide();
  }

  if ($("#button2_mode").val() == 0) {
    // Direct mode
    $("#button2_detached_mode_light_label").hide();
    $("#button2_detached_mode_light_select").hide();
    $("#button2_custom_mqtt_container").hide();
  } else if ($("#button2_mode").val() == 1) {
    $("#button2_detached_mode_light_label").show();
    $("#button2_detached_mode_light_select").show();
    $("#button2_custom_mqtt_container").hide();
  } else if ($("#button2_mode").val() == 2) {
    $("#button2_detached_mode_light_label").hide();
    $("#button2_detached_mode_light_select").hide();
    $("#button2_custom_mqtt_container").show();
  } else if ($("#button2_mode").val() == 3) {
    $("#button2_detached_mode_light_label").hide();
    $("#button2_detached_mode_light_select").hide();
    $("#button2_custom_mqtt_container").hide();
  }
}

$(document).ready(() => {
  mac_address = $("#nspanel_mac").text();
  nspanel_id = $("#nspanel_id").text();
  ws.register_message_handler((data) => {
    if (data.type == "status" || data.type == "status_report") {
      update_nspanel_status(data.payload);
    } else if (data.type == "log" && data.mac == mac_address) {
      var add_html = "<tr><td>";
      add_html += data.time;
      add_html += "</td><td>";
      if (data.level == "ERROR") {
        add_html += '<span class="tag is-danger">ERROR</span>';
      } else if (data.level == "WARNING") {
        add_html += '<span class="tag is-warning">WARNING</span>';
      } else if (data.level == "INFO") {
        add_html += '<span class="tag is-info">INFO</span>';
      } else if (data.level == "DEBUG") {
        add_html += '<span class="tag is-dark">DEBUG</span>';
      } else if (data.level == "TRACE") {
        add_html += '<span class="tag is-black">TRACE</span>';
      } else if (data.level == "WARNING") {
        add_html += '<span class="tag is-light">???UNKNOWN???</span>';
      }
      add_html += "</td>";
      add_html += "<td>";
      add_html += data.message;
      add_html += "</td>";
      add_html += "</tr>";

      var row_count = $("#log_body tr").length;
      if (row_count >= $("#max_live_log_messages").text()) {
        $("#log_body tr:last-child").remove();
      }

      $("#log_body").prepend(add_html);
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
  update_shown_elements();

  $("#button1_mode").change(update_shown_elements);
  $("#button2_mode").change(update_shown_elements);
});
