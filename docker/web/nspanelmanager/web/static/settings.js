function update_shown_elements() {
  // Update if screensaver outside temperature setting is available or not.
  var screensaver_mode = $("#screensaver_mode").val();
  if (screensaver_mode == "global") {
    screensaver_mode = global_screensaver_mode;
  }
  if (
    screensaver_mode == "datetime_with_background" ||
    screensaver_mode == "datetime_without_background"
  ) {
    $("#screensaver_outside_temperature_setting").removeClass("hidden");
  } else {
    $("#screensaver_outside_temperature_setting").addClass("hidden");
  }
}

$(document).ready(() => {
  update_shown_elements();

  $("#screensaver_mode").change(update_shown_elements);
});
