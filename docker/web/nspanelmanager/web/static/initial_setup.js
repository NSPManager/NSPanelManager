var current_step = "welcome";

var classes_step_completed = "flex w-full items-center text-blue-600 dark:text-blue-500 after:content-[''] after:w-full after:h-1 after:border-b after:border-blue-100 after:border-4 after:inline-block dark:after:border-blue-800";
var classes_last_step_completed = "flex items-center text-blue-600 dark:text-blue-500";
var classes_step_not_completed = "flex w-full items-center after:content-[''] after:w-full after:h-1 after:border-b after:border-gray-100 after:border-4 after:inline-block dark:after:border-gray-500";

var classes_step_icon_completed = "flex items-center justify-center w-10 h-10 bg-blue-100 rounded-full lg:h-12 lg:w-12 dark:bg-blue-800 shrink-0";
var classes_step_icon_not_completed = "flex items-center justify-center w-10 h-10 bg-gray-100 rounded-full lg:h-12 lg:w-12 dark:bg-gray-500 shrink-0";

function slideLeftOut(selector) {
  var initial_height = $(selector).height();
  $(selector).css("height", initial_height);
  $(selector).css("overflow", "hidden");
  $(selector).animate({width: 0, padding: 0, margin: 0, height: 0}, {duration: 500, queue: false});
}

function slideLeftIn(selector) {
  $(selector).height("0px");
  $(selector).width("0px");
  $(selector).animate({width: "100%", height: "100%"}, {duration: 500, queue: false});
}

function initial_setup_next() {
  if(current_step == "welcome") {
    $("#welcome_screen").addClass("hidden");
    $("#initial_setup_manager_config").removeClass("hidden");
    $("#initial_step_welcome").attr("class", classes_step_completed);
    $("#initial_step_welcome_icon").attr("class", classes_step_icon_completed);
    current_step = "manager_config";
  } else if (current_step == "manager_config") {
    if($("#manager_address").val() == "" || $("#manager_port").val() == "") {
      alert("Both address and port are required.");
      return false;
    }

    $.post("/initial_setup_manager_config", {
      manager_address: $("#manager_address").val(), 
      manager_port: $("#manager_port").val()
    });

    $("#initial_setup_manager_config").addClass("hidden");
    $("#mqtt_screen").removeClass("hidden");
    $("#initial_step_manager_config").attr("class", classes_step_completed);
    $("#initial_step_manager_config_icon").attr("class", classes_step_icon_completed);
    current_step = "mqtt_config";
  } else if (current_step == "mqtt_config") {
      if($("#mqtt_server").val() == "" || $("#mqtt_port").val() == "") {
        alert("Both address and port are required.");
        return false;
      }
  
      $.post("/initial_setup_mqtt_config", {
        mqtt_server: $("#mqtt_server").val(), 
        mqtt_port: $("#mqtt_port").val(),
        mqtt_username: $("#mqtt_username").val(),
        mqtt_password: $("#mqtt_password").val()
      });
  
      $("#mqtt_screen").addClass("hidden");
      $("#home_assistant_screen").removeClass("hidden");
      $("#initial_step_mqtt_config").attr("class", classes_step_completed);
      $("#initial_step_mqtt_config_icon").attr("class", classes_step_icon_completed);
      current_step = "home_assistant_config";
  } else if (current_step == "home_assistant_config") {
    if($("#home_assistant_address").val() != "" && $("#home_assistant_token").val() != "") {
      $.post("/initial_setup_home_assistant_config", {
        home_assistant_address: $("#home_assistant_address").val(), 
        home_assistant_token: $("#home_assistant_token").val()
      });
    }

    $("#home_assistant_screen").addClass("hidden");
    $("#openhab_screen").removeClass("hidden");
    $("#initial_step_home_assistant_config").attr("class", classes_step_completed);
    $("#initial_step_home_assistant_config_icon").attr("class", classes_step_icon_completed);
    current_step = "openhab_config";
  } else if (current_step == "openhab_config") {
    if($("#openhab_address").val() != "" && $("#openhab_token").val() != "") {
      $.post("/initial_setup_openhab_config", {
        openhab_address: $("#openhab_address").val(), 
        openhab_token: $("#openhab_token").val()
      });
    }

    $("#openhab_screen").addClass("hidden");
    $("#finish_screen").removeClass("hidden");
    $("#initial_step_openhab_config").attr("class", classes_step_completed);
    $("#initial_step_openhab_config_icon").attr("class", classes_step_icon_completed);

    // Also finish the last item on the list.
    $("#initial_step_finished").attr("class", classes_last_step_completed);
    $("#initial_step_finished_icon").attr("class", classes_step_icon_completed);
    $("#next_button").html("Finish");
    current_step = "finish";
  } else if (current_step == "finish") {
    $(".modal").addClass("hidden");
  } else {
    console.error("Unknown step: " + current_step);
  }
}