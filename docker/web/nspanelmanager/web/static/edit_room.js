function add_new_light_to_available_lights_list(light, type) {
  var icon = "";
  if (type == "home_assistant") {
    if (light.label.startsWith("light.")) {
      icon = "mdi-lightbulb";
    } else if (light.label.startsWith("switch.")) {
      // icon = "mdi-toggle-switch-off";
      icon = "mdi-lightning-bolt";
    } else {
      icon = "mdi-help";
    }
  } else {
    icon = "mdi-help";
  }

  $("#add_new_light_options").append(
    '<a class="panel-block" data-type="' +
      type +
      "\" data-items='" +
      JSON.stringify(light.items) +
      "'><span class=\"mdi " +
      icon +
      ' add_item_icon"></span>' +
      light.label +
      "</a>"
  );
}

function update_displayed_openhab_selectors() {
  var light_control_type = $('input[name="light_control_mode"]:checked').val();
  if (light_control_type == "dimmer") {
    $("#openhab_dimming_channel_selector").show();
    $("#openhab_switch_channel_selector").hide();
  } else {
    $("#openhab_switch_channel_selector").show();
    $("#openhab_dimming_channel_selector").hide();
  }

  if ($('input[name="color_temperature"]').prop("checked")) {
    $("#openhab_color_temp_channel_selector").show();
  } else {
    $("#openhab_color_temp_channel_selector").hide();
  }

  if ($('input[name="rgb"]').prop("checked")) {
    $("#openhab_rgb_channel_selector").show();
  } else {
    $("#openhab_rgb_channel_selector").hide();
  }
}

function populate_add_new_light_dialog() {
  $("#add_new_light_options").hide();
  $("#add_new_light_loader").show();

  $.get("/api/get_all_available_lights", function (data) {
    console.log(data);
    $("#add_new_light_errors").html("");
    $("#add_new_light_options").html("");
    data.errors.forEach((error) => {
      var error_element =
        '<div class="notification is-danger">' + error + "</div>";
      $("#add_new_light_errors").html(
        $("#add_new_light_errors").html() + error_element
      );
    });
    data.home_assistant_lights.forEach((light) => {
      add_new_light_to_available_lights_list(light, "home_assistant");
    });
    data.openhab_lights.forEach((light) => {
      add_new_light_to_available_lights_list(light, "openhab");
    });
    data.manual_lights.forEach((light) => {
      add_new_light_to_available_lights_list(light, "manual");
    });

    $("#add_new_light_loader").hide();
    $("#add_new_light_options").show();

    $("#add_new_light_options > a").click(add_new_light_show_light_page);
  }).fail(function () {
    alert("Failed to get available lights!");
  });
}

function add_new_light_show_light_page(light_element) {
  if ($(this).data("type") == "openhab") {
    $("#openhab_light_options").show();

    // Clear any previous options selected
    $("#openhab_dimming_channel_name").find("option").remove();
    $("#openhab_switch_channel_name").find("option").remove();
    $("#openhab_color_temperature_channel_name").find("option").remove();
    $("#openhab_RGB_channel_name").find("option").remove();

    var items = $(this).data("items");
    items.forEach((item) => {
      // Populate new options selected
      $("#openhab_dimming_channel_name").append(
        $("<option>", {
          value: item,
          text: item,
        })
      );
      $("#openhab_switch_channel_name").append(
        $("<option>", {
          value: item,
          text: item,
        })
      );
      $("#openhab_color_temperature_channel_name").append(
        $("<option>", {
          value: item,
          text: item,
        })
      );
      $("#openhab_RGB_channel_name").append(
        $("<option>", {
          value: item,
          text: item,
        })
      );
    });
  } else {
    $("#openhab_light_options").hide();
  }

  $("#edit_light_loader").hide();
  $("#modal-add-light-options-inputs").show();
  $("#edit_light_id").val("-1"); // Set text field
  $("#add_new_light_name").val($(this).text()); // Set text field
  $("#home_assistant_name").val($(this).text()); // Set text field
  $("#openhab_name").val($(this).text()); // Set text field
  $("#add_new_light_type").val($(this).data("type")); // Set the correct type

  $("#light_control_mode_dimmer").click(update_displayed_openhab_selectors);
  $("#light_control_mode_switch").click(update_displayed_openhab_selectors);
  $("#color_temperature").click(update_displayed_openhab_selectors);
  $("#rgb").click(update_displayed_openhab_selectors);
  update_displayed_openhab_selectors();

  // Finaly show the modal
  $("#add_save_light_button").text("Add");
  $("#modal-add-light").removeClass("is-active");
  $("#modal-add-light-options").addClass("is-active");
}

function add_new_lights_filter_for_type(type) {
  $("#add_new_light_tabs > a").removeClass("is-active");
  if (type == "all") {
    $("#filter_for_all").addClass("is-active");
  } else if (type == "home_assistant") {
    $("#filter_for_home_assistant").addClass("is-active");
  } else if (type == "openhab") {
    $("#filter_for_openhab").addClass("is-active");
  } else if (type == "manual") {
    $("#filter_for_manual").addClass("is-active");
  }
  add_new_lights_filter();
}

function add_new_lights_filter() {
  apply_tab_filter = !$("#filter_for_all").hasClass("is-active");
  tab_filter = "";
  if (apply_tab_filter) {
    if ($("#filter_for_home_assistant").hasClass("is-active")) {
      tab_filter = "home_assistant";
    } else if ($("#filter_for_openhab").hasClass("is-active")) {
      tab_filter = "openhab";
    } else if ($("#filter_for_manual").hasClass("is-active")) {
      tab_filter = "manual";
    }
  }

  var filter_for = $("#add_new_light_search").val();
  $("#add_new_light_options > a").each(function () {
    if ($(this).html().toLowerCase().includes(filter_for)) {
      if (!apply_tab_filter) {
        $(this).show();
      } else {
        // Also apply tab filter
        if ($(this).data("type") == tab_filter) {
          $(this).show();
        } else {
          $(this).hide();
        }
      }
    } else {
      $(this).hide();
    }
  });
}

function add_new_light() {
  $("#add_new_light_search").val(""); // Clear previous filter
  populate_add_new_light_dialog();
  $("#modal-add-light").addClass("is-active");
  $("#add_new_light_search").select();
}

function add_new_light_to_room_view(position_id) {
  $("#add_new_light_to_room_view_position").val(position_id);
  $("#modal-add-light-to-room-view").addClass("is-active");
}

function edit_light(light_id) {
  console.log("Edit light: " + light_id);
  $("#edit_light_loader").show();
  $("#modal-add-light-options-inputs").hide();
  $("#modal-add-light-options").addClass("is-active");

  $.get("/api/get_light_config/" + light_id, function (result) {
    if (result.type == "openhab") {
      $("#openhab_light_options").show();

      // Clear any previous options selected
      $("#openhab_dimming_channel_name").find("option").remove();
      $("#openhab_switch_channel_name").find("option").remove();
      $("#openhab_color_temperature_channel_name").find("option").remove();
      $("#openhab_RGB_channel_name").find("option").remove();

      $.get("/api/get_all_available_lights", function (data) {
        data.openhab_lights.forEach(function (light) {
          if (light.label == result.openhab_name) {
            light.items.forEach((item) => {
              // Populate new options selected
              $("#openhab_dimming_channel_name").append(
                $("<option>", {
                  value: item,
                  text: item,
                })
              );
              $("#openhab_switch_channel_name").append(
                $("<option>", {
                  value: item,
                  text: item,
                })
              );
              $("#openhab_color_temperature_channel_name").append(
                $("<option>", {
                  value: item,
                  text: item,
                })
              );
              $("#openhab_RGB_channel_name").append(
                $("<option>", {
                  value: item,
                  text: item,
                })
              );
              $("#openhab_dimming_channel_name")
                .val(result.openhab_item_dimmer)
                .change();
              $("#openhab_switch_channel_name")
                .val(result.openhab_item_switch)
                .change();
              $("#openhab_color_temperature_channel_name")
                .val(result.openhab_item_color_temp)
                .change();
              $("#openhab_RGB_channel_name")
                .val(result.openhab_item_rgb)
                .change();
            });
          }
        });
      });
    } else {
      $("#openhab_light_options").hide();
    }

    $("#edit_light_loader").hide();
    $("#modal-add-light-options-inputs").show();
    $("#edit_light_id").val(result.id); // Set text field
    $("#add_new_light_name").val(result.name); // Set text field
    $("#home_assistant_name").val(result.home_assistant_name); // Set text field
    $("#openhab_name").val(result.openhab_name); // Set text field
    $("#add_new_light_type").val(result.type); // Set the correct type

    $("#light_control_mode_dimmer").prop("checked", result.can_dim);
    $("#light_control_mode_switch").prop("checked", !result.can_dim);
    $("#color_temperature").prop("checked", result.can_color_temperature);
    $("#rgb").prop("checked", result.can_rgb);

    $("#light_type_ceiling").prop("checked", result.ceiling);
    $("#light_type_table").prop("checked", !result.ceiling);

    $("#light_control_mode_dimmer").click(update_displayed_openhab_selectors);
    $("#light_control_mode_switch").click(update_displayed_openhab_selectors);
    $("#color_temperature").click(update_displayed_openhab_selectors);
    $("#rgb").click(update_displayed_openhab_selectors);
    update_displayed_openhab_selectors();

    // Finaly show the modal
    $("#add_save_light_button").text("Save");
    $("#modal-add-light").removeClass("is-active");
    $("#modal-add-light-options").addClass("is-active");
  });
}

function add_new_scene() {
  $("#edit_scene_id").val("");
  $("#add_new_scene_to_room_modal").addClass("is-active");
}

function edit_scene(id, name) {
  $("#edit_scene_id").val(id);
  $("#scene_name").val(name);
  $("#add_new_scene_to_room_modal").addClass("is-active");
}

$(document).ready(function () {
  $("#add_new_light_search").keyup(add_new_lights_filter);

  if ($("#add_new_light_to_position_select option").length == 0) {
    $("#add_new_light_to_position_select").hide();
    $("#add_new_light_to_room_view_position_card_body").html(
      '<div class="notification is-danger">There are no available lights to add.</div>'
    );
  }
});
