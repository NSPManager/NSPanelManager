var _last_dropdown_id_activated = "";
var _last_collapse_id_activated = "";

function attach_cursor_change_events() {
  $(".cursor-grab")
    .on("mousedown", function () {
      $(this).removeClass("cursor-grab");
      $(this).addClass("cursor-grabbing");
    })
    .on("mouseup", function () {
      $(this).addClass("cursor-grab");
      $(this).removeClass("cursor-grabbing");
    });
}

$(document).ready(function () {
  console.log("Document ready, attaching functions.");

  stomp_subscribe("mqttmanager/events", (frame) => {
    var json_data = JSON.parse(frame.body);
    if (json_data.event_type) {
      console.log("Triggering custom event: " + json_data.event_type);
      const trigger_event = new CustomEvent(json_data.event_type, {
        detail: json_data,
      });
      document.getElementsByTagName("body")[0].dispatchEvent(trigger_event);
    }
  });

  // Listen for warnings from MQTTManager
  stomp_subscribe("mqttmanager/warnings", (data) => {
    var json = JSON.parse(data.body);
    var warnings = json.warnings;
    // Remove any removed warnings
    $("#error_toast_container > .alert").each(function () {
      let found = false;
      let match_text = $(this).find(".toast-text").text();
      for (let i = 0; i < warnings.length; i++) {
        if (warnings[i].text === match_text) {
          found = true;
          return;
        }
      }

      if (!found) {
        $(this).remove();
      }
    });

    for (let i = 0; i < warnings.length; i++) {
      // Check so that identical warnings are not shown multiple times
      let found = false;
      $("#error_toast_container > .alert").each(function () {
        if ($(this).find(".toast-text").text() == warnings[i].text) {
          found = true;
          return;
        }
      });

      if (!found) {
        show_error_toast(warnings[i].level, warnings[i].text);
      }
    }
  });

  // Something went wrong while sending/processing AJAX request via HTMX. Show error toast.
  document.addEventListener("htmx:responseError", (event) => {
    console.log("Got error: ");
    console.log(event.detail);

    try {
      const obj = JSON.parse(event.detail.xhr.response);
      if ("status" in obj) {
        if (obj.status == "error") {
          if ("text" in obj) {
            show_error_toast(0, obj["text"]);
          } else {
            show_error_toast(0, "Request error but no message specified.");
          }
        } else {
          show_error_toast(0, "Unknown request error.");
        }
      } else {
        show_error_toast(0, "Unknown request error.");
      }
    } catch (error) {
      show_error_toast(0, "Error while processing error response from server.");
    }
  });

  // Use custom confirm modal with HTMX
  document.addEventListener("htmx:confirm", function (e) {
    // The event is triggered on every trigger for a request, so we need to check if the element
    // that triggered the request has a hx-confirm attribute, if not we can return early and let
    // the default behavior happen
    if (!e.detail.target.hasAttribute("hx-confirm")) return;

    e.preventDefault();

    dialog = document.getElementById("modal_confirm");
    document
      .getElementById("modal_confirm_button")
      .addEventListener("click", function handle_confirm_click(event) {
        // Remove this event listener so that it doesn't trigger multiple times
        event.currentTarget.removeEventListener("click", handle_confirm_click);
        console.log("Sending request");
        document.getElementById("modal_confirm").close();
        e.detail.issueRequest(true);
      });

    document
      .getElementById("modal_confirm_cancel_button")
      .addEventListener("click", function handle_confirm_cancel_click(event) {
        event.currentTarget.removeEventListener(
          "click",
          handle_confirm_cancel_click,
        );
        document.getElementById("modal_confirm").close();
      });

    document.getElementById("modal_confirm_text").innerHTML = e.detail.question;
    dialog.showModal();
  });

  $("[data-dropdown-toggle]").click(function (e) {
    var toggle_id = $(this).data("dropdown-toggle");
    if (
      _last_dropdown_id_activated != "" &&
      _last_dropdown_id_activated != toggle_id
    ) {
      $("#" + _last_dropdown_id_activated).addClass("hidden");
    }
    $("#" + toggle_id).toggleClass("hidden");
    _last_dropdown_id_activated = toggle_id;
    if (e.stopPropagation) e.stopPropagation();
  });

  $("[data-collapse-toggle]").click(function (e) {
    var toggle_id = $(this).data("collapse-toggle");
    if (
      _last_collapse_id_activated != "" &&
      _last_collapse_id_activated != toggle_id
    ) {
      $("#" + _last_collapse_id_activated).addClass("max-md:hidden");
    }
    $("#" + toggle_id).toggleClass("max-md:hidden");
    _last_collapse_id_activated = toggle_id;
  });

  $(document).on("click", function (e) {
    // Close any open dropdown
    if (_last_dropdown_id_activated != "") {
      $("#" + _last_dropdown_id_activated).addClass("hidden");
    }
  });

  // Setup filename display when uploading files
  $("#firmware").change(() => {
    var filename = $("#firmware")
      .val()
      .replace(/C:\\fakepath\\/i, "");
    $("#save_new_firmware_modal_subtext").html(
      "Selected file: '" + filename + "'.",
    );
    $("#firmware_upload_submit_button").removeClass("btn-disabled");
  });

  $("#data_file").change(() => {
    var filename = $("#data_file")
      .val()
      .replace(/C:\\fakepath\\/i, "");
    $("#save_new_data_file_modal_subtext").html(
      "Selected file: '" + filename + "'.",
    );
    $("#data_file_upload_submit_button").removeClass("btn-disabled");
  });

  $("#tft_file").change(() => {
    var filename = $("#tft_file")
      .val()
      .replace(/C:\\fakepath\\/i, "");
    $("#save_new_tft_file_modal_subtext").html(
      "Selected file: '" + filename + "'.",
    );
    $("#tft_upload_submit_button").removeClass("btn-disabled");
  });

  // Setup drag and drop feature
  $("#firmware_dragdrop_target").on("dragover", (e) => {
    e.preventDefault();
    e.stopPropagation();
  });
  $("#firmware_dragdrop_target").on("dragenter", (e) => {
    e.preventDefault();
    e.stopPropagation();
  });
  $("#data_file_dragdrop_target").on("dragover", (e) => {
    e.preventDefault();
    e.stopPropagation();
  });
  $("#data_file_dragdrop_target").on("dragenter", (e) => {
    e.preventDefault();
    e.stopPropagation();
  });
  $("#tft_dragdrop_target").on("dragover", (e) => {
    e.preventDefault();
    e.stopPropagation();
  });
  $("#tft_dragdrop_target").on("dragenter", (e) => {
    e.preventDefault();
    e.stopPropagation();
  });

  $("#firmware_dragdrop_target").on("drop", (e) => {
    if (
      e.originalEvent.dataTransfer &&
      e.originalEvent.dataTransfer.files.length
    ) {
      e.preventDefault();
      e.stopPropagation();
      $("#firmware").prop("files", e.originalEvent.dataTransfer.files);
      var filename = $("#firmware")
        .val()
        .replace(/C:\\fakepath\\/i, "");
      $("#save_new_firmware_modal_subtext").html(
        "Selected file: '" + filename + "'.",
      );
      $("#firmware_upload_submit_button").removeClass("btn-disabled");
    }
  });

  $("#data_file_dragdrop_target").on("drop", (e) => {
    if (
      e.originalEvent.dataTransfer &&
      e.originalEvent.dataTransfer.files.length
    ) {
      e.preventDefault();
      e.stopPropagation();
      $("#data_file").prop("files", e.originalEvent.dataTransfer.files);
      var filename = $("#data_file")
        .val()
        .replace(/C:\\fakepath\\/i, "");
      $("#save_new_data_file_modal_subtext").html(
        "Selected file: '" + filename + "'.",
      );
      $("#data_file_upload_submit_button").removeClass("btn-disabled");
    }
  });

  $("#tft_dragdrop_target").on("drop", (e) => {
    if (
      e.originalEvent.dataTransfer &&
      e.originalEvent.dataTransfer.files.length
    ) {
      e.preventDefault();
      e.stopPropagation();
      $("#tft_file").prop("files", e.originalEvent.dataTransfer.files);
      var filename = $("#tft_file")
        .val()
        .replace(/C:\\fakepath\\/i, "");
      $("#tft_file").html("Selected file: '" + filename + "'.");
      $("#tft_upload_submit_button").removeClass("btn-disabled");
    }
  });

  // Attach event listener for cursor-grab to change it to grabing on mouse down for drag & drop targets
  attach_cursor_change_events();
});
