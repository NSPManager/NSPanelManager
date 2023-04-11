function startNSPanelOtaUpdate(ip_address) {
    $.post("http://" + ip_address + "/start_ota_update", function (data) {
        $('#modal-command-sent').addClass('is-active');
    });
}

function startNSPanelTftUpdate(ip_address) {
    $.post("http://" + ip_address + "/start_tft_ota_update", function (data) {
        $('#modal-command-sent').addClass('is-active');
    });
}

function connect_to_websocket() {
  webSocket = new WebSocket("ws://" + location.hostname + ":8001");

  webSocket.onmessage = (event) => {
    console.log("Got message.");
      data = JSON.parse(event.data);
      if(data.type == "status") {
        let mac_selector = data.payload.mac;
        mac_selector = mac_selector.replaceAll(":", "\\:");
        if(data.payload.state == "online" && $("#online_offline_state_" + mac_selector).text() == "Offline") {
          $("#online_offline_state_" + mac_selector).text("Online");
          $("#online_offline_state_" + mac_selector).removeClass("is-danger");
          $("#online_offline_state_" + mac_selector).addClass("is-success");
        } else if(data.payload.state == "offline" && $("#online_offline_state_" + mac_selector).text() == "Online") {
          $("#online_offline_state_" + mac_selector).text("Offline");
          $("#online_offline_state_" + mac_selector).addClass("is-danger");
          $("#online_offline_state_" + mac_selector).removeClass("is-success");
        }
      } else if (data.type == "status_report") {
        let mac_selector = data.payload.mac;
        mac_selector = mac_selector.replaceAll(":", "\\:");
        $("#heap_used_" + mac_selector).text(data.payload.heap_used_pct + "%");
        var new_rssi_classes = "";
        if(data.payload.rssi <= -90) {
          new_rssi_classes = "mdi mdi-wifi-strength-1-alert";
        } else if(data.payload.rssi <= -80) {
          new_rssi_classes = "mdi mdi-wifi-strength-1";
        } else if(data.payload.rssi <= -67) {
          new_rssi_classes = "mdi mdi-wifi-strength-2";
        } else if(data.payload.rssi <= -55) {
          new_rssi_classes = "mdi mdi-wifi-strength-3";
        } else {
          new_rssi_classes = "mdi mdi-wifi-strength-4";
        }
        $("#wifi_signal_strength_" + mac_selector).attr("class", new_rssi_classes);
        $("#wifi_signal_strength_" + mac_selector).attr("title", data.payload.rssi + " dBm");
      }
  }

  webSocket.onclose = (event) => {
      console.log("Websocket closed, trying in 1 second");
      setTimeout(function() {
          connect_to_websocket();
      }, 1000);
  };
}

$(document).ready(function() {
  connect_to_websocket(); 
});