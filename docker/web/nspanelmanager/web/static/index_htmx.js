function show_dropdown_menu(e) {
  var dropdown_id = $(this).attr("data-dropdown-toggle");
  if(dropdown_id != "") {
    $("#" + dropdown_id).toggleClass("hidden");
  }
  if(e.stopPropagation) e.stopPropagation();
}
