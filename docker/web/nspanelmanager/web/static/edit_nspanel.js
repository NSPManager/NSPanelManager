var mac_address;
var nspanel_id;

function update_shown_elements() {
  $("#button1_detached_mode_controls").addClass("hidden");
  $("#button2_detached_mode_controls").addClass("hidden");
  $("#button1_detached_mode_light").addClass("hidden");
  $("#button2_detached_mode_light").addClass("hidden");
  $("#button1_mqtt_mode_controls").addClass("hidden");
  $("#button2_mqtt_mode_controls").addClass("hidden");
  $("#button1_thermostat_mode_controls").addClass("hidden");
  $("#button2_thermostat_mode_controls").addClass("hidden");
  $("#button1_detached_mode_light").prop("required", false);
  $("#button2_detached_mode_light").prop("required", false);
  $("#button1_relay_lower_temperature").prop("required", false);
  $("#button1_relay_upper_temperature").prop("required", false);
  $("#button2_relay_lower_temperature").prop("required", false);
  $("#button2_relay_upper_temperature").prop("required", false);

  if ($("#button1_mode").val() == 0) {
    // Direct mode
  } else if ($("#button1_mode").val() == 1) {
    // Detached mode
    $("#button1_detached_mode_controls").removeClass("hidden");
    $("#button1_detached_mode_light").prop("required", true);
    $("#button1_detached_mode_light").removeClass("hidden");
  } else if ($("#button1_mode").val() == 2) {
    // MQTT mode
    $("#button1_mqtt_mode_controls").removeClass("hidden");
  } else if ($("#button1_mode").val() == 3) {
    // Follow mode
  } else if ($("#button1_mode").val() == 4 || $("#button1_mode").val() == 5) {
    // Thermostat mode
    $("#button1_thermostat_mode_controls").removeClass("hidden");
    $("#button1_relay_lower_temperature").prop("required", true);
    $("#button1_relay_upper_temperature").prop("required", true);
  }

  if ($("#button2_mode").val() == 0) {
    // Direct mode
  } else if ($("#button2_mode").val() == 1) {
    // Detached mode
    $("#button2_detached_mode_controls").removeClass("hidden");
    $("#button2_detached_mode_light").prop("required", true);
    $("#button2_detached_mode_light").removeClass("hidden");
  } else if ($("#button2_mode").val() == 2) {
    // MQTT mode
    $("#button2_mqtt_mode_controls").removeClass("hidden");
  } else if ($("#button2_mode").val() == 3) {
    // Follow mode
  } else if ($("#button2_mode").val() == 4 || $("#button2_mode").val() == 5) {
    // Thermostat mode
    $("#button2_thermostat_mode_controls").removeClass("hidden");
    $("#button2_relay_lower_temperature").prop("required", true);
    $("#button2_relay_upper_temperature").prop("required", true);
  }

  // Update shown lights depending on selected room
  var button1_selected_room_id = $("#button1_detached_mode_room").val();
  $(".button1_detached_mode_light_option[data-room-id='" + button1_selected_room_id + "']").show();
  $(".button1_detached_mode_light_option[data-room-id!='" + button1_selected_room_id + "']").hide();

  // Check if selected button1_detached_mode_light option is visible, if not, select the first of the options.
  if ($(".button1_detached_mode_light_option[data-room-id='" + button1_selected_room_id + "']:selected").length == 0) {
    var button1_detached_light_first_value = $(".button1_detached_mode_light_option[data-room-id='" + button1_selected_room_id + "']:first").val();
    if (button1_detached_light_first_value) {
      console.log(
        "Selected option for button1 detached light is no longer accessible in selected room, will select '",
        button1_detached_light_first_value,
        "' instead.",
      );
      $("#button1_detached_mode_light").val(button1_detached_light_first_value).change();
    } else {
      $("#button1_detached_mode_light").val("").change();
    }
  }

  var button2_selected_room_id = $("#button2_detached_mode_room").val();
  $(".button2_detached_mode_light_option[data-room-id='" + button2_selected_room_id + "']").show();
  $(".button2_detached_mode_light_option[data-room-id!='" + button2_selected_room_id + "']").hide();

  // Check if selected button2_detached_mode_light option is visible, if not, select the first of the options.
  if ($(".button2_detached_mode_light_option[data-room-id='" + button2_selected_room_id + "']:selected").length == 0) {
    var button2_detached_light_first_value = $(".button2_detached_mode_light_option[data-room-id='" + button2_selected_room_id + "']:first").val();
    if (button2_detached_light_first_value) {
      console.log(
        "Selected option for button2 detached light is no longer accessible in selected room, will select '",
        button2_detached_light_first_value,
        "' instead.",
      );
      $("#button2_detached_mode_light").val(button2_detached_light_first_value).change();
    } else {
      $("#button2_detached_mode_light").val("").change();
    }
  }

  // Update if screensaver outside temperature setting is available or not.
  var screensaver_mode = $("#screensaver_mode").val();
  if (screensaver_mode == "global") {
    screensaver_mode = global_screensaver_mode;
  }
  if (screensaver_mode == "datetime_with_background" || screensaver_mode == "datetime_without_background") {
    $("#screensaver_outside_temperature_setting").removeClass("hidden");
  } else {
    $("#screensaver_outside_temperature_setting").addClass("hidden");
  }
}

// Update the view of the example NSPanel model and orientation.
function update_nspanel_example_view() {
  var selected_type = $("#panel_type").val();
  if (selected_type == "eu") {
    $("#eu-panel-example").show();
  } else {
    $("#eu-panel-example").hide();
  }

  if (selected_type == "us_vertical") {
    $("#us-panel-vertical-example").show();
  } else {
    $("#us-panel-vertical-example").hide();
  }

  if (selected_type == "us_horizontal") {
    $("#us-panel-horizontal-example").show();
  } else {
    $("#us-panel-horizontal-example").hide();
  }

  if (selected_type == "us_horizontal_mirrored") {
    $("#us-panel-horizontal-mirrored-example").show();
  } else {
    $("#us-panel-horizontal-mirrored-example").hide();
  }
}

$(document).ready(() => {
  document.querySelectorAll(".nspanel_settings_container").forEach((element) => {
    // On message from manager on websocket.
    element.addEventListener("htmx:afterRequest", (event) => {
      if (event.detail.successful) {
        window.Location = "/";
      }
    });
  });
  update_shown_elements();

  $("#button1_mode").change(update_shown_elements);
  $("#button2_mode").change(update_shown_elements);
  $("#screensaver_mode").change(update_shown_elements);
  $("#button1_detached_mode_room").change(update_shown_elements);
  $("#button2_detached_mode_room").change(update_shown_elements);

  $("#panel_type").on("change", update_nspanel_example_view);
  update_nspanel_example_view(); // Update once manually depending on selected choice.
});
