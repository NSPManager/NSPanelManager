var panels_that_are_updating = [];
const ws = new MQTTManager_WS();
var panel_warnings = {};
var status_header_base_classes =
  "min-h-1 overflow-hidden rounded-t-md w-full transission-all ease-linear duration-500 ";

function get_all_online_panel_macs() {
  var panel_macs = [];
  $(".nspanel_mac_container").each((index, obj) => {
    var panel_mac = $(obj).data("nspanel-mac");
    let mac_selector = panel_mac.replaceAll(":", "\\:");
    if ($("#panel_header_" + mac_selector).hasClass("bg-green-500")) {
      panel_macs.push(panel_mac);
    }
  });
  return panel_macs;
}

function startNSPanelOtaUpdateAll() {
  get_all_online_panel_macs().forEach((mac) => {
    startNSPanelOtaUpdate($("#nspanel_mac_container_" + mac));
  });
}

function startNSPanelOtaUpdate(dom) {
  var mac_selector = $(dom).data("nspanel-mac");
  $("#panel_header_" + mac_selector).attr(
    "class",
    status_header_base_classes + "bg-blue-500 dark:bg-blue-700 animate-pulse",
  );
  $("#panel_header_text_" + mac_selector).text("Awaiting status");
  var panel_id = $("#nspanel_id_" + mac_selector).text();
  ws.send_command("firmware_update_nspanels", { nspanels: [panel_id] }, null);
}

function send_nspanel_accept_request(dom) {
  let command_data = {};
  let nspanel_mac = $(dom).attr("data-nspanel-mac");
  // Convert selector-mac to real mac.
  let real_mac =
    nspanel_mac.substring(0, 2) +
    ":" +
    nspanel_mac.substring(2, 4) +
    ":" +
    nspanel_mac.substring(4, 6) +
    ":" +
    nspanel_mac.substring(6, 8) +
    ":" +
    nspanel_mac.substring(8, 10) +
    ":" +
    nspanel_mac.substring(10, 12);
  command_data.mac_address = real_mac;

  ws.send_command("nspanel_accept", command_data, (response) => {
    if (response.success) {
      location.reload();
    } else {
      console.log("Error! Failed to accept register request! Response: ");
      console.log(response);
    }
  });
}

function send_nspanel_deny_request(dom) {
  let command_data = {};
  let nspanel_mac = $(dom).attr("data-nspanel-mac");
  // Convert selector-mac to real mac.
  let real_mac =
    nspanel_mac.substring(0, 2) +
    ":" +
    nspanel_mac.substring(2, 4) +
    ":" +
    nspanel_mac.substring(4, 6) +
    ":" +
    nspanel_mac.substring(6, 8) +
    ":" +
    nspanel_mac.substring(8, 10) +
    ":" +
    nspanel_mac.substring(10, 12);
  command_data.mac_address = real_mac;

  ws.send_command("nspanel_deny", command_data, (response) => {
    if (response.success) {
      $(dom).closest(".nspanel-box").fadeOut(100);
      setTimeout(() => {
        $(dom).closest(".nspanel-box").remove();
      }, 100);
    } else {
      console.log("Error! Failed to accept register request! Response: ");
      console.log(response);
    }
  });
}

function send_nspanel_delete_request(dom) {
  let command_data = {};
  let nspanel_mac = $(dom).data("nspanel-mac");
  // Convert selector-mac to real mac.
  let real_mac =
    nspanel_mac.substring(0, 2) +
    ":" +
    nspanel_mac.substring(2, 4) +
    ":" +
    nspanel_mac.substring(4, 6) +
    ":" +
    nspanel_mac.substring(6, 8) +
    ":" +
    nspanel_mac.substring(8, 10) +
    ":" +
    nspanel_mac.substring(10, 12);
  command_data.mac_address = real_mac;

  ws.send_command("nspanel_delete", command_data, (response) => {
    if (response.success) {
      $(dom).closest(".nspanel-box").fadeOut(100);
      setTimeout(() => {
        $(dom).closest(".nspanel-box").remove();
      }, 100);
    }
  });
}

function startNSPanelTftUpdateAll() {
  get_all_online_panel_macs().forEach((mac) => {
    startNSPanelTftUpdate($("#nspanel_mac_container_" + mac));
  });
}

function startNSPanelTftUpdate(dom) {
  let mac_selector = $(dom).data("nspanel-mac");
  $("#panel_header_" + mac_selector).attr(
    "class",
    status_header_base_classes + "bg-blue-500 dark:bg-blue-700 animate-pulse",
  );
  $("#panel_header_text_" + mac_selector).text("Awaiting status");
  var panel_id = $("#nspanel_id_" + mac_selector).text();
  ws.send_command("tft_update_nspanels", { nspanels: [panel_id] }, null);
}

function rebootNSPanelAll() {
  get_all_online_panel_macs().forEach((mac) => {
    rebootNSPanel($("#nspanel_mac_container_" + mac));
  });
}

function rebootNSPanel(dom) {
  let mac_selector = $(dom).data("nspanel-mac");
  $("#panel_header_" + mac_selector).attr(
    "class",
    status_header_base_classes + "bg-blue-500 dark:bg-blue-700 animate-pulse",
  );
  $("#panel_header_text_" + mac_selector).text("Awaiting status");
  var panel_id = $("#nspanel_id_" + mac_selector).text();
  ws.send_command("reboot_nspanels", { nspanels: [panel_id] }, null);
}

function restart_mqtt_manager() {
  $.post("/api/restart_mqtt_manager", (data) => {});
}

function create_nspanel_from_template(data) {
  // Create a new NSPanel box from template. This will replace all data
  // neccesary for the update function to fill in the rest.
  var nspanel = $($("#nspanel_index_box_template").html());

  // Setup links
  nspanel
    .find(".nspanel-box")
    .attr("id", "panel_status_box_" + data.mac_address);
  nspanel.find("#nspanel_name").attr("href", "/nspanel/" + data.id);
  nspanel.find(".nspanel-settings-link").attr("href", "/nspanel/" + data.id);
  nspanel.find(".nspanel-visit-link").attr("href", "http://" + data.ip_address);

  // Setup dropdown
  nspanel
    .find("[data-dropdown-toggle]")
    .attr(
      "data-dropdown-toggle",
      "nspanelActionsMenuDropdown_" + data.mac_address,
    );
  nspanel
    .find("#nspanelActionsMenuDropdown")
    .attr("id", "nspanelActionsMenuDropdown_" + data.mac_address);

  nspanel.find("[data-dropdown-toggle]").click(show_dropdown_menu);
  nspanel.find("[data-nspanel-mac]").attr("data-nspanel-mac", data.mac_address);

  // Warnings
  nspanel
    .find("#nspanel_warnings")
    .attr("id", "nspanel_warnings_" + data.mac_address);
  nspanel
    .find("#nspanel_warning_indicator")
    .attr("id", "nspanel_warning_indicator_" + data.mac_address);

  // Setup warnings popover
  nspanel
    .find('[data-popover-target="nspanel_warning_popover"]')
    .attr("data-popover-target", "nspanel_warning_popover_" + data.mac_address);
  nspanel
    .find("#nspanel_warning_popover")
    .attr("id", "nspanel_warning_popover_" + data.mac_address);
  nspanel.find("[data-popover-target]").hover(
    (e) => {
      var target = $(e.currentTarget).data("popover-target");
      $("#" + target).removeClass("invisible opacity-0");
    },
    (e) => {
      var target = $(e.currentTarget).data("popover-target");
      $("#" + target).addClass("invisible opacity-0");
    },
  );

  nspanel.find("#panel_header").attr("id", "panel_header_" + data.mac_address);
  nspanel.find("#nspanel_id").text(data.id);
  nspanel.find("#nspanel_id").attr("id", "nspanel_id_" + data.mac_address);
  nspanel.find("#nspanel_mac_container").text(data.mac_address);
  nspanel
    .find("#nspanel_mac_container")
    .attr("id", "nspanel_mac_container_" + data.mac_address);
  nspanel
    .find("#panel_header_text")
    .attr("id", "panel_header_text_" + data.mac_address);
  nspanel
    .find("#wifi_signal_strength")
    .attr("id", "wifi_signal_strength_" + data.mac_address);
  nspanel
    .find("#wifi_signal_strength_text")
    .attr("id", "wifi_signal_strength_text_" + data.mac_address);
  nspanel.find("#temperature").attr("id", "temperature_" + data.mac_address);
  nspanel.find("#heap_used").attr("id", "heap_used_" + data.mac_address);
  nspanel.find("#nspanel_name").html(data.name);
  nspanel.find("#nspanel_name").attr("id", "nspanel_name_" + data.mac_address);

  if (data.state == "awaiting_accept") {
    nspanel.find(".accept-request-overlay").removeClass("hidden");
    nspanel
      .find(".accept-request-overlay")
      .attr("id", "accept_request_overlay_" + data.mac_address);
    nspanel
      .find(".accept-request-overlay span")
      .text('Discovered "' + data.name + '"!');
    nspanel
      .find(".accept-request-overlay button.is-success")
      .attr("data-nspanel-mac", data.mac_address);
    nspanel.find(".nspanel-status-view").addClass("hidden"); // Hide default view to make space for accept
  }

  $("#nspanels_container").append(nspanel);
  update_nspanel_status(data);
}

function update_nspanel_status(data) {
  if ("mac_address" in data) {
    if ($("#nspanel_mac_container_" + data.mac_address).length == 0) {
      create_nspanel_from_template(data);
      return;
    }

    if ("state" in data) {
      var new_html = "";

      if (data.state != "awaiting_accept") {
        $("#accept_request_overlay_" + data.mac_address).addClass("hidden");
      }

      if (data.state == "online") {
        $("#panel_header_" + data.mac_address).attr(
          "class",
          status_header_base_classes + "bg-green-500 dark:bg-green-700",
        );
        $("#panel_header_text_" + data.mac_address).text("");
        $("#panel_header_" + data.mac_address).css("width", "100%");
      } else if (data.state == "offline") {
        $("#panel_header_" + data.mac_address).attr(
          "class",
          status_header_base_classes + "bg-red-500 dark:bg-red-700",
        );
        $("#panel_header_text_" + data.mac_address).text("Offline");
        $("#panel_header_" + data.mac_address).css("width", "100%");
      } else if (data.state == "unknown") {
        $("#panel_header_" + data.mac_address).attr(
          "class",
          status_header_base_classes + "bg-gray-500 dark:bg-gray-700",
        );
        $("#panel_header_text_" + data.mac_address).text("Unknown");
        $("#panel_header_" + data.mac_address).css("width", "100%");
      } else if (data.state == "awaiting_accept") {
        $("#panel_header_" + data.mac_address).attr(
          "class",
          status_header_base_classes +
            "bg-blue-500 dark:bg-blue-700 animate-pulse",
        );
        $("#panel_header_text_" + data.mac_address).text("Awaiting accept");
        $("#panel_header_" + data.mac_address).css("width", "100%");
      } else if (data.state == "waiting") {
        $("#panel_header_text_" + data.mac_address).text("Waiting");
        $("#panel_header_" + data.mac_address).css("width", "100%");
        $("#panel_header_" + data.mac_address).attr(
          "class",
          status_header_base_classes +
            "bg-blue-500 dark:bg-blue-700 animate-pulse",
        );
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
            status_header_base_classes + "bg-green-500 dark:bg-green-700",
          );
        } else {
          $("#panel_header_" + data.mac_address).attr(
            "class",
            status_header_base_classes + "bg-blue-500 dark:bg-blue-700",
          );
        }

        $("#panel_header_" + data.mac_address).css(
          "width",
          update_progress + "%",
        );
        $("#panel_header_text_" + data.mac_address).text(
          update_text + ", " + update_progress + "%",
        );
      }
    }
    if ("rssi" in data) {
      var new_rssi_classes = "";
      if (
        data.state == "unknown" ||
        data.state == "waiting" ||
        data.state == "offline"
      ) {
        new_rssi_classes = "mdi mdi-wifi-strength-1-alert";
        data.rssi = "?";
      } else if (data.rssi <= -90) {
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
      $("#wifi_signal_strength_text_" + data.mac_address).html(
        data.rssi + " dBm",
      );
      $("#wifi_signal_strength_" + data.mac_address).attr(
        "class",
        new_rssi_classes,
      );
      $("#wifi_signal_strength_" + data.mac_address).attr(
        "title",
        data.rssi + " dBm",
      );
    }

    if (
      data.state == "unknown" ||
      data.state == "waiting" ||
      data.state == "offline"
    ) {
      $("#heap_used_" + data.mac_address).text("?%");
    } else {
      if ("heap_used_pct" in data) {
        $("#heap_used_" + data.mac_address)
          .text(data.heap_used_pct + "%")
          .text()
          .slice(-2);
      }
    }

    if (
      data.state == "unknown" ||
      data.state == "waiting" ||
      data.state == "offline"
    ) {
      var temperature_unit = $("#temperature_unit").text();
      $("#temperature_" + data.mac_address).html("? " + temperature_unit);
    } else if ("temperature" in data) {
      var temperature_unit = $("#temperature_unit").text();
      $("#temperature_" + data.mac_address).html(
        Math.round(data.temperature * 100) / 100 + " " + temperature_unit,
      );
    }

    if ("warnings" in data) {
      $("#nspanel_warnings_" + data.mac_address).html("");
      data["warnings"].split("\n").forEach((warning) => {
        if (warning != "") {
          console.log(warning);
          $("#nspanel_warnings_" + data.mac_address).append(
            $("<div>", {
              class:
                "p-4 mt-2 text-sm text-yellow-800 rounded-lg bg-yellow-50 dark:bg-yellow-300 dark:text-gray-800",
              role: "alert",
              text: warning,
            }),
          );
        }
      });

      if (data["warnings"] != "") {
        $("#nspanel_warning_indicator_" + data.mac_address).removeClass(
          "hidden",
        );
      } else {
        $("#nspanel_warning_indicator_" + data.mac_address).addClass("hidden");
      }
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
              $("<div>", {
                class:
                  "p-4 mt-2 text-sm text-red-800 rounded-lg bg-red-50 dark:bg-red-700 dark:text-white",
                id: "failed_to_connect_mqtt_error",
                role: "alert",
                text: "Failed to connect to MQTT server. Retrying...",
              }),
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
                $("<div>", {
                  class:
                    "p-4 mt-2 text-sm text-yellow-800 rounded-lg bg-yellow-50 dark:bg-yellow-300 dark:text-gray-800",
                  id: "failed_to_connect_home_assistant_error",
                  role: "alert",
                  text: "Failed to connect to Home Assistant websocket API. Retrying...",
                }),
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
                  $("<div>", {
                    class:
                      "p-4 mt-2 text-sm text-yellow-800 rounded-lg bg-yellow-50 dark:bg-yellow-300 dark:text-gray-800",
                    id: "failed_to_auth_home_assistant_error",
                    role: "alert",
                    text: "Authentication error while connecting to Home Assistant. Retrying...",
                  }),
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
                $("<div>", {
                  class:
                    "p-4 mt-2 text-sm text-yellow-800 rounded-lg bg-yellow-50 dark:bg-yellow-300 dark:text-gray-800",
                  id: "failed_to_connect_openhab_error",
                  role: "alert",
                  text: "Failed to connect to OpenHAB websocket API. Retrying...",
                }),
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
    $("#waiting_for_mqtt_manager_connection_error").remove();
    $("#loading-panels-spinner").css("opacity", "0%");

    ws.send_command("get_nspanels_status", {}, (response) => {
      for (const [id, nspanel] of Object.entries(response.nspanels)) {
        update_nspanel_status(nspanel);
      }
    });
  });
  ws.register_on_close_handler(() => {
    console.log("Disconnected from MQTTManager via websocket.");
    if ($("#failed_to_connect_error").length == 0) {
      $("#waiting_for_mqtt_manager_connection_error").remove(); // Remove the "Trying to connect to MQTTManager..." message.
      $("#notification_holder").append(
        $("<div>", {
          class:
            "p-4 mt-2 text-sm text-red-800 rounded-lg bg-red-50 dark:bg-red-700 dark:text-white",
          id: "failed_to_connect_error",
          role: "alert",
          text: "Failed to connect to MQTTManager. Retrying...",
        }),
      );
    }
  });
  console.log("Starting connect to WebSocket.");
  ws.connect();

  setTimeout(() => {
    if (!ws.is_connected()) {
      $("#loading-panels-spinner").removeClass("hidden");
      $("#notification_holder").append(
        $("<div>", {
          class:
            "p-4 mt-2 text-sm text-yellow-800 rounded-lg bg-yellow-50 dark:bg-yellow-300 dark:text-gray-800",
          id: "waiting_for_mqtt_manager_connection_error",
          role: "alert",
          text: "Trying to connect to MQTTManager...",
        }),
      );
    }
  }, 500);

  $("#firmware_upload_file_input").change(function () {
    var fileName = $(this).val().replace("C:\\fakepath\\", "");
    $("#firmware_upload_file_name").html(fileName);
  });

  $("#data_file").change(function () {
    var fileName = $(this).val().replace("C:\\fakepath\\", "");
    $("#data_file").html(fileName);
  });

  $("#tft_upload_file_input").change(function () {
    var fileName = $(this).val().replace("C:\\fakepath\\", "");
    $("#tft_upload_file_name").html(fileName);
  });

  $(window).click(function () {
    console.log("Window clicked");
    $(".dropdown").addClass("hidden");
  });
});
