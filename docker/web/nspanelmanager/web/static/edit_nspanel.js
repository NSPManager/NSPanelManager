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

function requests_log_backtrace() {
  ws.send_command("get_nspanels_logs", { nspanels: [nspanel_id] }, null);
}

function push_log_message_to_view(data) {
  var add_html = "<tr><td class='py-1'>";
  add_html += data.time;
  add_html += "</td><td class='py-1'>";
  if (data.level == "ERROR") {
    add_html += '<span class="dark:text-red-400 text-red-600">ERROR</span>';
  } else if (data.level == "WARNING") {
    add_html += '<span class="dark:text-yellow-400 text-yellow-600">WARNING</span>';
  } else if (data.level == "INFO") {
    add_html += '<span class="dark:text-blue-400 text-blue-600">INFO</span>';
  } else if (data.level == "DEBUG") {
    add_html += '<span class="dark:text-black text-black">DEBUG</span>';
  } else if (data.level == "TRACE") {
    add_html += '<span class="dark:text-gray-400 text-gray-800">TRACE</span>';
  } else {
    add_html += '<span class="tag is-light">???UNKNOWN???</span>';
  }
  add_html += "</td>";
  add_html += "<td class='py-1'>";
  add_html += data.message;
  add_html += "</td>";
  add_html += "</tr>";

  var row_count = $("#log_body tr").length;
  if (row_count >= $("#max_live_log_messages").text()) {
    $("#log_body tr:last-child").remove();
  }

  $("#log_body").prepend(add_html);
}

function update_nspanel_status(data) {
  if ("mac_address" in data) {
    if(data.mac_address == mac_address) { // Check that the status data corresponds to the panel currently in view.
      if ("state" in data) {
        $("#panel_visit_link").attr("href", "http://" + data.ip_address);
        var new_html = "";
        if (data.state == "online") {
          $("#state_text").html("Online");
          $("#state_text").attr(
            "class",
            "dark:text-green-400 text-green-600"
          );
          $("#update_progress_container").addClass("hidden");
          $("#update_progress_container").slideUp("slow", () => {});
        } else if (data.state == "offline") {
          $("#state_text").html("Offline");
          $("#state_text").attr(
            "class",
            "dark:text-red-400 text-red-600"
          );
          $("#update_progress_container").addClass("hidden");
          $("#update_progress_container").slideUp("slow", () => {});
        } else {
          // Update panel tag to show update progress if any
          update_text = "";
          update_progress = 0;
          if (data.state == "updating_fw") {
            update_text = "Updating FW";
            update_progress = data.progress;
          } else if (data.state == "updating_fs") {
            update_text = "Updating FS";
            update_progress = data.progress;
          } else if (data.state == "updating_tft") {
            update_text = "Updating TFT";
            update_progress = data.progress;
          }

          $("#update_progress_container").slideDown("slow", () => {});
          $("#update_progress_container").removeClass("hidden");
          $("#update_progress_pct_progressbar").css("width", update_progress + "%");
          $("#update_progress_pct_text").html(update_progress + "%");
          $("#state_text").html(update_text);
          $("#state_text").attr(
            "class",
            "dark:text-green-400 text-green-600"
          );
        }
      }
      if ("rssi" in data) {
        var new_rssi_classes = "";
        var new_rssi_text = "";
        var new_rssi_text_classes = "";
        if(data.state == "offline") {
          new_rssi_classes = "mdi mdi-wifi-strength-1-alert me-2";
          new_rssi_text = "Unknown";
          new_rssi_text_classes = "dark:text-red-400 text-red-600";
        }
        else if (data.rssi <= -90) {
          new_rssi_classes = "mdi mdi-wifi-strength-1-alert me-2";
          new_rssi_text = "Poor";
          new_rssi_text_classes = "dark:text-red-400 text-red-600";
        } else if (data.rssi <= -80) {
          new_rssi_classes = "mdi mdi-wifi-strength-1 me-2";
          new_rssi_text = "Low";
          new_rssi_text_classes = "dark:text-yellow-400 text-yellow-600";
        } else if (data.rssi <= -67) {
          new_rssi_classes = "mdi mdi-wifi-strength-2 me-2";
          new_rssi_text = "Medium";
          new_rssi_text_classes = "dark:text-green-400 text-green-600";
        } else if (data.rssi <= -55) {
          new_rssi_classes = "mdi mdi-wifi-strength-3 me-2";
          new_rssi_text = "Good";
          new_rssi_text_classes = "dark:text-green-400 text-green-600";
        } else {
          new_rssi_classes = "mdi mdi-wifi-strength-4 me-2";
          new_rssi_text = "Excellent";
          new_rssi_text_classes = "dark:text-green-400 text-green-600";
        }
        $("#wifi_signal_strength_icon").attr("class", new_rssi_classes);
        $("#wifi_signal_strength").attr("class", new_rssi_text_classes);
        $("#wifi_signal_strength").html(new_rssi_text);
      }

      if ("heap_used_pct" in data) {
        var heap_used = data.heap_used_pct + "%";
        var new_heap_used_text_color = "";

        if(data.state == "offline") {
          heap_used = "Unknown";
          new_heap_used_text_color = "dark:text-red-400 text-red-600";
        }else if(data.heap_used_pct < 70) {
          new_heap_used_text_color = "dark:text-green-400 text-green-600";
        } else if (data.heap_used_pct < 85) {
          new_heap_used_text_color = "dark:text-yellow-400 text-yellow-600";
        } else {
          new_heap_used_text_color = "dark:text-red-400 text-red-600";
        }

        $("#heap_used").text(heap_used)
        $("#heap_used").attr("class", new_heap_used_text_color);
      }

      if(data.state == "offline") {
        $("#temperature").html("Unknown");
      } else if ("temperature" in data) {
        var temperature_unit = $("#temperature")
          .text()
          .slice(-2);
        $("#temperature").html(
          Math.round(data.temperature * 100) / 100 + " " + temperature_unit
        );
      }

      if ("warnings" in data) {
        $("#warning_container").html("");
        data["warnings"].split("\n").forEach((warning) => {
          if(warning != "") {
            console.log(warning);
            $("#warning_container").append(
              $("<div>", {
                class: "p-4 mt-2 text-sm text-yellow-800 rounded-lg bg-yellow-50 dark:bg-yellow-300 dark:text-gray-800",
                role: "alert",
                text: warning,
              })
            );
          }
        });
      }
    }
  }
}

function update_shown_elements() {
  if ($("#button1_mode").val() == 0) {
    // Direct mode
    $("#button1_detached_mode_controls").addClass("hidden");
    $("#button1_mqtt_mode_controls").addClass("hidden");
  } else if ($("#button1_mode").val() == 1) {
    // Detached mode
    $("#button1_detached_mode_controls").removeClass("hidden");
    $("#button1_mqtt_mode_controls").addClass("hidden");
  } else if ($("#button1_mode").val() == 2) {
    // MQTT mode
    $("#button1_detached_mode_controls").addClass("hidden");
    $("#button1_mqtt_mode_controls").removeClass("hidden");
  } else if ($("#button1_mode").val() == 3) {
    // Follow mode
    $("#button1_detached_mode_controls").addClass("hidden");
    $("#button1_mqtt_mode_controls").addClass("hidden");
  }

  if ($("#button2_mode").val() == 0) {
    // Direct mode
    $("#button2_detached_mode_controls").addClass("hidden");
    $("#button2_mqtt_mode_controls").addClass("hidden");
  } else if ($("#button2_mode").val() == 1) {
    // Detached mode
    $("#button2_detached_mode_controls").removeClass("hidden");
    $("#button2_mqtt_mode_controls").addClass("hidden");
  } else if ($("#button2_mode").val() == 2) {
    // MQTT mode
    $("#button2_detached_mode_controls").addClass("hidden");
    $("#button2_mqtt_mode_controls").removeClass("hidden");
  } else if ($("#button2_mode").val() == 3) {
    // Follow mode
    $("#button2_detached_mode_controls").addClass("hidden");
    $("#button2_mqtt_mode_controls").addClass("hidden");
  }
}

$(document).ready(() => {
  mac_address = $("#nspanel_mac").text().replaceAll(":", ""); // Convert from Django mac format to MQTTManager mac format.
  nspanel_id = $("#nspanel_id").text();
  ws.register_message_handler((data) => {
    if (data.type == "status" || data.type == "status_report") {
      update_nspanel_status(data.payload);
    } else if (data.type == "log" && data.mac.replaceAll(":", "") == mac_address) {
      push_log_message_to_view(data);
    }
  });

  ws.register_on_connect_handler(() => {
    console.log("Connected to MQTTManager via websocket.");
    // Remove any connection error notification
    $("#failed_to_connect_error").remove();

    ws.send_command("get_nspanels_status", { nspanel_id: nspanel_id }, (response) => {
      update_nspanel_status(response.nspanels[0]);
    });

    ws.send_command(
      "get_nspanel_logs",
      { nspanel_id: nspanel_id },
      (response) => {
        for (const [id, log] of Object.entries(response.logs)) {
          push_log_message_to_view(log);
        }
      }
    );
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
