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
    } else {
      icon = "mdi-help";
    }
  } else {
    icon = "mdi-help";
  }

  $("#add_new_entity_options").append(
    '<a class="panel-block" data-type="' +
      type +
      "\" data-items='" +
      JSON.stringify(entity.items) + "' data-entity_id='" + entity.entity_id +
      "'><span class=\"mdi " +
      icon +
      ' add_item_icon"></span>' +
      entity.label +
      "</a>"
  );
}

function populate_add_new_entity_dialog(filter, callback) {
  $("#add_new_entity_options").hide();
  $("#add_new_entity_loader").show();

  $.get("/api/get_all_available_entities", filter, function (data) {
    $("#add_new_entity_errors").html("");
    $("#add_new_entity_options").html("");
    data.errors.forEach((error) => {
      var error_element =
        '<div class="notification is-danger">' + error + "</div>";
      $("#add_new_entity_errors").html(
        $("#add_new_entity_errors").html() + error_element
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

    $("#add_new_entity_options > a").click(callback);
  }).fail(function () {
    alert("Failed to get available entities!");
  });
}

function add_new_entities_filter_for_type(type) {
  $("#add_new_entity_tabs > a").removeClass("is-active");
  if (type == "all") {
    $("#filter_for_all").addClass("is-active");
  } else if (type == "home_assistant") {
    $("#filter_for_home_assistant").addClass("is-active");
  } else if (type == "openhab") {
    $("#filter_for_openhab").addClass("is-active");
  } else if (type == "manual") {
    $("#filter_for_manual").addClass("is-active");
  }
  add_new_entities_filter();
}

function add_new_entities_filter() {
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

  var filter_for = $("#add_new_entity_search").val();
  $("#add_new_entity_options > a").each(function () {
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
  $("#modal-select-entity").addClass("is-active");
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

  $("#modal-select-item").addClass("is-active");
}
