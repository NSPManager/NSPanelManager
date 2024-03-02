var _last_dropdown_id_activated = "";
var _last_collapse_id_activated = "";

$(document).ready(function () {
    console.log('Document ready, attaching functions.');

    $('.modal-background').click(function () {
        $('.modal').removeClass('is-active');
    });

    $('.modal-card-head .delete').click(function () {
        $('.modal').removeClass('is-active');
    });

    $('.modal-cancel-button').click(function () {
        $('.modal').removeClass('is-active');
    });

    $('[data-dropdown-toggle]').click(function(e) {
      var toggle_id = $(this).data('dropdown-toggle');
      if(_last_dropdown_id_activated != "" && _last_dropdown_id_activated != toggle_id) {
        $("#" + _last_dropdown_id_activated).addClass("hidden");
      }
      $("#" + toggle_id).toggleClass("hidden");
      _last_dropdown_id_activated = toggle_id;
      if(e.stopPropagation) e.stopPropagation();
    });

    $('[data-collapse-toggle]').click(function(e) {
      var toggle_id = $(this).data('collapse-toggle');
      if(_last_collapse_id_activated != "" && _last_collapse_id_activated != toggle_id) {
        $("#" + _last_collapse_id_activated).addClass("max-md:hidden");
      }
      $("#" + toggle_id).toggleClass("max-md:hidden");
      _last_collapse_id_activated = toggle_id;
    });

    $(document).on('click', function (e) {
      // Close any open dropdown
      if(_last_dropdown_id_activated != "") {
        $("#" + _last_dropdown_id_activated).addClass("hidden");
      }
     });
});

function addNewRoom() {
    $('#modal-new-room').removeClass('hidden');
    $('#new_room_name').select();
}

function toggle_theme() {
  $("html").toggleClass("dark");

  data = {
    "dark": $("html").hasClass("dark")
  };
  $.post("/api/save_theme", data, (data) => {});
}

function close_modals() {
  $(".modal").addClass("hidden");
}