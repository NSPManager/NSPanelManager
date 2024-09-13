const ws = new MQTTManager_WS();
var mac_address;
var nspanel_id;

function requests_log_backtrace() {
  ws.send_command("get_nspanels_logs", { nspanels: [nspanel_id] }, null);
}

function push_log_message_to_view(data) {
  var add_html = "<tr class='hover:bg-base-300'><td class='py-1'>";
  add_html += data.time;
  add_html += "</td><td class='py-1'>";
  if (data.level == "ERROR") {
    add_html += '<span class="dark:text-red-400 text-red-600">ERROR</span>';
  } else if (data.level == "WARNING") {
    add_html +=
      '<span class="dark:text-yellow-400 text-yellow-600">WARNING</span>';
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

function update_shown_elements() {
  if ($("#button1_mode").val() == 0) {
    // Direct mode
    $("#button1_detached_mode_controls").addClass("hidden");
    $("#button1_detached_mode_light").prop("required", false);
    $("#button1_mqtt_mode_controls").addClass("hidden");
  } else if ($("#button1_mode").val() == 1) {
    // Detached mode
    $("#button1_detached_mode_controls").removeClass("hidden");
    $("#button1_detached_mode_light").prop("required", true);
    $("#button1_mqtt_mode_controls").addClass("hidden");
  } else if ($("#button1_mode").val() == 2) {
    // MQTT mode
    $("#button1_detached_mode_controls").addClass("hidden");
    $("#button1_detached_mode_light").prop("required", false);
    $("#button1_mqtt_mode_controls").removeClass("hidden");
  } else if ($("#button1_mode").val() == 3) {
    // Follow mode
    $("#button1_detached_mode_controls").addClass("hidden");
    $("#button1_detached_mode_light").prop("required", false);
    $("#button1_mqtt_mode_controls").addClass("hidden");
  }

  if ($("#button2_mode").val() == 0) {
    // Direct mode
    $("#button2_detached_mode_controls").addClass("hidden");
    $("#button2_detached_mode_light").prop("required", false);
    $("#button2_mqtt_mode_controls").addClass("hidden");
  } else if ($("#button2_mode").val() == 1) {
    // Detached mode
    $("#button2_detached_mode_controls").removeClass("hidden");
    $("#button2_detached_mode_light").prop("required", true);
    $("#button2_mqtt_mode_controls").addClass("hidden");
  } else if ($("#button2_mode").val() == 2) {
    // MQTT mode
    $("#button2_detached_mode_controls").addClass("hidden");
    $("#button2_detached_mode_light").prop("required", false);
    $("#button2_mqtt_mode_controls").removeClass("hidden");
  } else if ($("#button2_mode").val() == 3) {
    // Follow mode
    $("#button2_detached_mode_controls").addClass("hidden");
    $("#button2_detached_mode_light").prop("required", false);
    $("#button2_mqtt_mode_controls").addClass("hidden");
  }
}

$(document).ready(() => {
  document.querySelectorAll('.nspanel_settings_container').forEach((element) => {
    // On message from manager on websocket.
    element.addEventListener("htmx:afterRequest", (event) => {
      if(event.detail.successful) {
        window.Location = "/";
      }
    });
  });
  update_shown_elements();

  $("#button1_mode").change(update_shown_elements);
  $("#button2_mode").change(update_shown_elements);
});
