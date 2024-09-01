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

    // Setup filename display when uploading files
    $("#firmware").change(() => {
      var filename = $('#firmware').val().replace(/C:\\fakepath\\/i, '')
      $("#save_new_firmware_modal_subtext").html("Selected file: '" + filename + "'.");
    });

    $("#data_upload_file_name").change(() => {
      var filename = $('#data_upload_file_name').val().replace(/C:\\fakepath\\/i, '')
      $("#save_new_data_file_modal_subtext").html("Selected file: '" + filename + "'.");
    });

    $("#tft_file").change(() => {
      var filename = $('#tft_file').val().replace(/C:\\fakepath\\/i, '')
      $("#save_new_tft_file_modal_subtext").html("Selected file: '" + filename + "'.");
    });

    // Setup drag and drop feature
    $('#firmware_dragdrop_target').on('dragover', (e) => {e.preventDefault(); e.stopPropagation();});
    $('#firmware_dragdrop_target').on('dragenter', (e) => {e.preventDefault(); e.stopPropagation();});
    $('#data_file_dragdrop_target').on('dragover', (e) => {e.preventDefault(); e.stopPropagation();});
    $('#data_file_dragdrop_target').on('dragenter', (e) => {e.preventDefault(); e.stopPropagation();});
    $('#tft_dragdrop_target').on('dragover', (e) => {e.preventDefault(); e.stopPropagation();});
    $('#tft_dragdrop_target').on('dragenter', (e) => {e.preventDefault(); e.stopPropagation();});

    $("#firmware_dragdrop_target").on('drop', (e) => {
      if(e.originalEvent.dataTransfer && e.originalEvent.dataTransfer.files.length) {
        e.preventDefault();
        e.stopPropagation();
        $("#firmware").prop("files", e.originalEvent.dataTransfer.files);
        var filename = $('#firmware').val().replace(/C:\\fakepath\\/i, '')
        $("#save_new_firmware_modal_subtext").html("Selected file: '" + filename + "'.");
        $("#save_new_firmware_modal_subtext").attr("class", "text-xs text-blue-500 dark:text-blue-400");
      }
    });

    $("#data_file_dragdrop_target").on('drop', (e) => {
      if(e.originalEvent.dataTransfer && e.originalEvent.dataTransfer.files.length) {
        e.preventDefault();
        e.stopPropagation();
        $("#data_upload_file_name").prop("files", e.originalEvent.dataTransfer.files);
        var filename = $('#data_upload_file_name').val().replace(/C:\\fakepath\\/i, '')
        $("#save_new_data_file_modal_subtext").html("Selected file: '" + filename + "'.");
        $("#save_new_data_file_modal_subtext").attr("class", "text-xs text-blue-500 dark:text-blue-400");
      }
    });

    $("#tft_dragdrop_target").on('drop', (e) => {
      if(e.originalEvent.dataTransfer && e.originalEvent.dataTransfer.files.length) {
        e.preventDefault();
        e.stopPropagation();
        $("#save_new_tft_file_modal_subtext").prop("files", e.originalEvent.dataTransfer.files);
        var filename = $('#tft_file').val().replace(/C:\\fakepath\\/i, '')
        $("#save_new_tft_file_modal_subtext").html("Selected file: '" + filename + "'.");
        $("#save_new_tft_file_modal_subtext").attr("class", "text-xs text-blue-500 dark:text-blue-400");
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