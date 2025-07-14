function show_dropdown_menu(e, element) {
  var dropdown_id = $(element).attr("data-nspanel-dropdown-toggle");
  if (dropdown_id != "") {
    $("#" + dropdown_id).toggleClass("hidden");
    if (e.stopPropagation) e.stopPropagation();
  }
}

function show_popover(e) {
  var popover_target = $(e).data("popover-target");
  if (popover_target) {
    $("#" + popover_target).removeClass("invisible opacity-0");
  }
}

function hide_popover(e) {
  var popover_target = $(e).data("popover-target");
  if (popover_target) {
    $("#" + popover_target).addClass("invisible opacity-0");
  }
}

$(window).click(function () {
  $(".nspanel-actions-menu-dropdown").addClass("hidden");
});

function get_all_online_panel_macs() {
  var panel_macs = [];
  $(".nspanel_mac_container").each((index, obj) => {
    var panel_mac = $(obj).data("nspanel-mac");
    let mac_selector = panel_mac.replaceAll(":", "\\:");
    console.log(panel_mac);
    if ($("#panel_header_" + mac_selector).hasClass("bg-green-500")) {
      panel_macs.push(panel_mac);
    }
  });
  return panel_macs;
}

// Send a reboot command to all NSPanels
function reboot_all_nspanels() {
  $(".nspanel_info_tag").each((index, obj) => {
    stomp_send("nspanel/" + $(obj).data("nspanel-mac") + "/command", "reboot");
  });
}

// Send a firmware update command to all online NSPanels
function start_all_nspanels_firmware_update_ota() {
  $(".nspanel_info_tag[data-nspanel-state='online']").each((index, obj) => {
    stomp_send(
      "nspanel/" + $(obj).data("nspanel-mac") + "/command",
      "firmware_update",
    );
  });
}

// Send a GUI update command to all online NSPanels
function start_all_nspanels_gui_update_ota() {
  $(".nspanel_info_tag[data-nspanel-state='online']").each((index, obj) => {
    stomp_send(
      "nspanel/" + $(obj).data("nspanel-mac") + "/command",
      "gui_update",
    );
  });
}
