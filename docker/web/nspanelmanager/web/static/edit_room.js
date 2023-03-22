function add_new_light_to_available_lights_list(light, type) {
    $("#add_new_light_options").append('<a class="panel-block" data-type="' + type + '" data-items=\'' + JSON.stringify(light.items) + '\'>' + light.label + '</a>');
}

function populate_add_new_light_dialog() {
    $("#add_new_light_options").hide();
    $("#add_new_light_loader").show();

    $.get("/api/get_all_available_lights", function (data) {
        $("#add_new_light_options").html("");
        data.home_assistant_lights.forEach(light => {
            add_new_light_to_available_lights_list(light, "home_assistant");
        });
        data.openhab_lights.forEach(light => {
            add_new_light_to_available_lights_list(light, "openhab");
        });
        data.manual_lights.forEach(light => {
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
        $('#openhab_dimming_channel_name').find("option").remove();
        $('#openhab_color_temperature_channel_name').find("option").remove();
        $('#openhab_RGB_channel_name').find("option").remove();

        var items = $(this).data("items")
        items.forEach(item => {
            // Populate new options selected
            $('#openhab_dimming_channel_name').append($('<option>', {
                value: item,
                text: item
            }));
            $('#openhab_color_temperature_channel_name').append($('<option>', {
                value: item,
                text: item
            }));
            $('#openhab_RGB_channel_name').append($('<option>', {
                value: item,
                text: item
            }));
        });
    } else {
        $("#openhab_light_options").hide();
    }

    $("#add_new_light_name").val($(this).text()); // Set text field
    $("#add_new_light_type").val($(this).data("type")); // Set the correct type

    // Finaly show the modal
    $('#modal-add-light').removeClass("is-active");
    $('#modal-add-light-options').addClass("is-active");
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
    $('#modal-add-light').addClass("is-active");
    $('#add_new_light_search').select();
}

$(document).ready(function () {
    $("#add_new_light_search").keyup(add_new_lights_filter);
});