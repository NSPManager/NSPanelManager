function show_dropdown_menu(e) {
  var dropdown_id = $(this).attr("data-dropdown-toggle");
  if(dropdown_id != "") {
    $("#" + dropdown_id).toggleClass("hidden");
  }
  if(e.stopPropagation) e.stopPropagation();
}

function show_popover(e) {
  var popover_target = $(e).data("popover-target");
  if(popover_target) {
    $("#" + popover_target).removeClass("invisible opacity-0");
  }
}

function hide_popover(e) {
  var popover_target = $(e).data("popover-target");
  if(popover_target) {
    $("#" + popover_target).addClass("invisible opacity-0");
  }
}
