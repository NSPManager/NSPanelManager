function add_new_entity_to_list(entity, type) {
  var icon = "";
  if (type == "home_assistant") {
    if (entity.entity_id.startsWith("light.")) {
      icon = "mdi-lightbulb";
    } else if (entity.entity_id.startsWith("switch.")) {
      icon = "mdi-lightning-bolt";
    } else if (entity.entity_id.startsWith("weather.")) {
      icon = "mdi-weather-cloudy";
    } else if (entity.entity_id.startsWith("sun.")) {
      icon = "mdi-weather-sunny";
    } else if (entity.entity_id.startsWith("sensor.")) {
      icon = "mdi-import";
    } else if (entity.entity_id.startsWith("scene.")) {
      icon = "mdi-state-machine"
    } else {
      icon = "mdi-help";
    }
  } else {
    icon = "mdi-help";
  }

  $("#add_new_entity_options").append(
    '<tr class="select_entity_option hover cursor-pointer" data-type="' +
      type +
      "\" data-items='" +
      JSON.stringify(entity.items) + "' data-entity_id='" + entity.entity_id + "' data-label='" + entity.label +
      "'><td><span class=\"mdi " +
      icon +
      ' add_item_icon me-2"></span>' +
      entity.label +
      "</td></tr>"
  );
}

function populate_add_new_entity_dialog(filter, callback) {
  $("#add_new_entity_options").hide();
  $("#add_new_entity_loader").show();

  $.get("/api/get_all_available_entities", filter, function (data) {
    $("#add_new_entity_errors").html("");
    $("#add_new_entity_options").html("");
    data.errors.forEach((error) => {
      $("#add_new_entity_errors").append(
        $("<div>", {
          class: "p-4 mx-2 my-2 text-sm text-red-800 rounded-lg bg-red-50 dark:bg-red-700 dark:text-white",
          id: "failed_to_connect_mqtt_error",
          role: "alert",
          text: error,
        })
      );
    });
    data.home_assistant_entities.forEach((light) => {
      add_new_entity_to_list(light, "home_assistant");
    });
    data.openhab_entities.forEach((light) => {
      add_new_entity_to_list(light, "openhab");
    });
    data.manual_entities.forEach((light) => {
      add_new_entity_to_list(light, "manual");
    });

    $("#add_new_entity_loader").hide();
    $("#add_new_entity_options").show();

    $("#add_new_entity_options .select_entity_option").click(() => {
      modal_select_entity.close();
    });
    $("#add_new_entity_options .select_entity_option").click(callback);
  }).fail(function () {
    alert("Failed to get available entities!");
  });
}

function add_new_entities_filter_for_type(type) {
  $("#add_new_entity_tabs a").removeClass("tab-active");
  if (type == "all") {
    $("#filter_for_all").addClass("tab-active");
  } else if (type == "home_assistant") {
    $("#filter_for_home_assistant").addClass("tab-active");
  } else if (type == "openhab") {
    $("#filter_for_openhab").addClass("tab-active");
  } else if (type == "manual") {
    $("#filter_for_manual").addClass("tab-active");
  }
  add_new_entities_filter();
}

function add_new_entities_filter() {
  apply_tab_filter = !$("#filter_for_all").hasClass("tab-active");
  tab_filter = "";
  if (apply_tab_filter) {
    if ($("#filter_for_home_assistant").hasClass("tab-active")) {
      tab_filter = "home_assistant";
    } else if ($("#filter_for_openhab").hasClass("tab-active")) {
      tab_filter = "openhab";
    } else if ($("#filter_for_manual").hasClass("tab-active")) {
      tab_filter = "manual";
    }
  }

  var filter_for = $("#add_new_entity_search").val().toLowerCase();
  $("#add_new_entity_options > tr").each(function () {
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

function select_new_entity(filter, callback) {
  $("#add_new_entity_search").val(""); // Clear previous filter
  populate_add_new_entity_dialog(filter, callback);
  modal_select_entity.showModal();
  $("#add_new_entity_search").select();
}

$(document).ready(function () {
  $("#add_new_entity_search").keyup(add_new_entities_filter);
});

// Show a modal to select new items.
// heading: The title of the modal
// options: Array of what items are available and should be populated in the selects, example: [{text: 'item1', value: 1}]
// selects: An array of selects, example: [{text: 'Current weather item', id: 'weather-item'}]
function select_openhab_items(heading, options, selects, callback) {
  $("#select_item_options").html("");
  $("#modal-select-item-heading").html(heading);
  selects.forEach(element => {
    var field = $('<div class="field">').appendTo("#select_item_options");
    field.append($('<label class="label">').html(element.text));
    var select_div = $('<div class="select">').appendTo(field);
    var select = $('<select>').appendTo(select_div);
    select.attr("name", "modal-select-item-" + element.id);
    select.attr("id", "modal-select-item-" + element.id);
    options.forEach(option => {
      select.append($('<option>').attr("value", option.value).text(option.text));
    });
  });

  $("#select_item_ok_button").click((button) => {
    data = {};
    selects.forEach(element => {
      var value = $("#modal-select-item-" + element.id).val();
      data[element.id] = value;
    });

    callback(data);
  });

  $("#modal-select-item").removeClass("hidden");
}
