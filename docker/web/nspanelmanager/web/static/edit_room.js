function add_new_light_to_available_lights_list(light, type) {
  var icon = "";
  if (type == "home_assistant") {
    if (light.entity_id.startsWith("light.")) {
      icon = "mdi-lightbulb";
    } else if (light.entity_id.startsWith("switch.")) {
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
      JSON.stringify(light.items) + "' data-entity_id='" + light.entity_id +
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

  var types = {
    "home_assistant_type_filter": JSON.stringify([
      "light", "switch"
    ]),
    "openhab_type_filter": JSON.stringify([
      "things"
    ])
  };
  $.get("/api/get_all_available_entities", types, function (data) {
    $("#add_new_light_errors").html("");
    $("#add_new_light_options").html("");
    data.errors.forEach((error) => {
      var error_element =
        '<div class="notification is-danger">' + error + "</div>";
      $("#add_new_light_errors").html(
        $("#add_new_light_errors").html() + error_element
      );
    });
    data.home_assistant_entities.forEach((light) => {
      add_new_light_to_available_lights_list(light, "home_assistant");
    });
    data.openhab_entities.forEach((light) => {
      add_new_light_to_available_lights_list(light, "openhab");
    });
    data.manual_entities.forEach((light) => {
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
  var entity_name = $(light_element.currentTarget).text();
  var entity_id = $(light_element.currentTarget).data("entity_id");
  var entity_type = $(light_element.currentTarget).data("type");

  if (entity_type == "openhab") {
    $("#openhab_light_options").removeClass("hidden");

    // Clear any previous options selected
    $("#openhab_dimming_channel_name").find("option").remove();
    $("#openhab_switch_channel_name").find("option").remove();
    $("#openhab_color_temperature_channel_name").find("option").remove();
    $("#openhab_RGB_channel_name").find("option").remove();

    var items = $(light_element.currentTarget).data("items");
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
    $("#openhab_light_options").addClass("hidden");
  }

  close_modals();

  $("#edit_light_loader").addClass("hidden");
  $("#modal-add-light-options-inputs").removeClass("hidden");
  $("#edit_light_id").val("-1"); // Set text field
  $("#add_new_light_name").val(entity_name); // Set text field
  $("#home_assistant_name").val(entity_id); // Set text field
  $("#openhab_name").val(entity_name); // Set text field
  $("#add_new_light_type").val(entity_type); // Set the correct type

  $("#light_control_mode_dimmer").click(update_displayed_openhab_selectors);
  $("#light_control_mode_switch").click(update_displayed_openhab_selectors);
  $("#color_temperature").click(update_displayed_openhab_selectors);
  $("#rgb").click(update_displayed_openhab_selectors);
  update_displayed_openhab_selectors();

  // Finaly show the modal
  $("#add_save_light_button").text("Add");
  $("#modal-add-light").addClass("hidden");
  $("#modal-add-light-options").removeClass("hidden");
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
  var types = {
    "home_assistant_type_filter": JSON.stringify([
      "light", "switch"
    ]),
    "openhab_type_filter": JSON.stringify([
      "things"
    ])
  };
  select_new_entity(types, (element) => {
    add_new_light_show_light_page(element);
  });
}

function add_new_light_to_room_view(position_id) {
  $("#add_new_light_to_room_view_position").val(position_id);
  $("#modal-add-light-to-room-view").removeClass("hidden");
}

function edit_light(light_id) {
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

      var types = {
        "home_assistant_type_filter": JSON.stringify([
          "light", "switch"
        ]),
        "openhab_type_filter": JSON.stringify([
          "things"
        ])
      };
      $.get("/api/get_all_available_entities", types, function (data) {
        data.openhab_entities.forEach(function (light) {
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
    $("#modal-add-light").addClass("hidden");
    $("#modal-add-light-options").removeClass("hidden");
  });
}

function add_new_scene() {
  $("#edit_scene_id").val("");
  $("#add_new_scene_to_room_modal").removeClass("hidden");
}

function edit_scene(id, name) {
  $("#edit_scene_id").val(id);
  $("#scene_name").val(name);
  $("#add_new_scene_to_room_modal").removeClass("hidden");
}

// Add a scene from Home Assistant or OpenHAB
function add_existing_scene() {
  var types = {
    "home_assistant_type_filter": JSON.stringify([
      "scene"
    ]),
    "openhab_type_filter": JSON.stringify([
      "rules"
    ])
  };

  select_new_entity(types, (element) => {
    $("#add_existing_scene_entity_name").val($(element.currentTarget).data("entity_id"));
    $("#add_existing_scene_friendly_name").val($(element.currentTarget).data("label"));
    $("#add_existing_scene_type").val($(element.currentTarget).data("type"));
    $("#add_existing_scene_form").submit();
    //add_new_light_show_light_page(element);
  });
}

$(document).ready(function () {
  $("#add_new_light_search").keyup(add_new_lights_filter);
  $("#color_temperature").change(update_displayed_openhab_selectors);
  $("#rgb").change(update_displayed_openhab_selectors);

  if ($("#add_new_light_to_position_select option").length == 0) {
    $("#add_new_light_to_position_select").addClass("hidden");
    $("#add_new_light_to_room_view_position_card_body").html(
      '<div class="notification is-danger">There are no available lights to add.</div>'
    );
  }
});
