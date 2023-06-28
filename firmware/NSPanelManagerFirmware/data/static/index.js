function setupWifiSelectModal() {
  $("#select_wifi_modal_close_background").click(function () {
    $("#select_wifi_modal").removeClass("is-active");
    doConnectionCheck = true;
  });

  $("#select_wifi_modal_close_button").click(function () {
    $("#select_wifi_modal").removeClass("is-active");
    doConnectionCheck = true;
  });

  $("#select_wifi_modal_cancel_button").click(function () {
    $("#select_wifi_modal").removeClass("is-active");
    doConnectionCheck = true;
  });
}

function showWifiSelectModal() {
  $("#available_wifi_networks").hide();
  $("#wifi_failed_to_get_networks").hide();
  $("#wifi_no_networks_found").hide();
  $("#wifi_wait_div").show();
  $("#select_wifi_modal").addClass("is-active");
  // setTimeout(function() {
  //     $("#wifi_wait_div").hide();
  //     $("#available_wifi_networks").show();
  // }, 1000);
  // The first scan will always return 0 results. If we get 0 results, wait 5 seconds and try again.
  doConnectionCheck = false;
  $.get("/available_wifi_networks", function (data) {
    if (data.length <= 0) {
      setTimeout(function () {
        $.get("/available_wifi_networks", function (data) {
          if (data.length <= 0) {
            $("#wifi_wait_div").hide();
            $("#wifi_no_networks_found").show();
          } else {
            populateModalWithWiFiNetworks(data);
          }
        }).fail(function () {
          $("#wifi_wait_div").hide();
          $("#wifi_failed_to_get_networks").show();
        });
      }, 5000)
    } else {
      populateModalWithWiFiNetworks(data);
    }
  }).fail(function () {
    $("#wifi_wait_div").hide();
    $("#wifi_failed_to_get_networks").show();
  });
}

function populateModalWithWiFiNetworks(networks) {
  var table_body_html = "";
  $.each(networks, function (index, network) {
    table_body_html += "<tr>";

    table_body_html += "<th>";
    table_body_html += `<a onclick="selectNetwork('${network.ssid}');">${network.ssid}</a>`;
    table_body_html += "</th>";

    table_body_html += "<td>";
    table_body_html += network.rssi;
    table_body_html += "</td>";

    table_body_html += "<td>";
    table_body_html += network.channel;
    table_body_html += "</td>";

    table_body_html += "<td>";
    table_body_html += network.security;
    table_body_html += "</td>";

    table_body_html += "</tr>";
  });
  $("#available_wifi_networks_tbody").html(table_body_html);
  $("#wifi_wait_div").hide();
  $("#available_wifi_networks").show();
}

function selectNetwork(network) {
    $("#wifi_ssid").val(network);
    $("#select_wifi_modal").removeClass("is-active");
    $("#wifi_psk").select(); // Select the PSK field to accept the PSK for the select WiFi
    doConnectionCheck = true;
}

$(document).ready(function () {
  setupWifiSelectModal();
});
