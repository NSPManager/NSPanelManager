var panels_that_are_updating = [];

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
      data = JSON.parse(event.data);
      if(data.type == "status") {
        let mac_selector = data.payload.mac;
        mac_selector = mac_selector.replaceAll(":", "\\:");

        if (!$("#online_offline_tag_parent_" + mac_selector).length) {
          // Got a state update for a panel that does not exist in this page, reload to show it if it has registered
          setTimeout(function() {
            location.reload();
          }, 2000);
        }

        if(data.payload.state == "online") {
          var new_html = '<span class="tag is-success" id="online_offline_state_' + data.payload.mac + '">Online</span>';
          
          if(panels_that_are_updating.includes(data.payload.mac)) {
            if($("#online_offline_tag_parent_" + mac_selector).text().trim() == "Offline") {
              setTimeout(function() {
                location.reload();
              }, 1000);
            }
          } else {
          $("#online_offline_tag_parent_" + mac_selector).html(new_html);
          }
        } else if(data.payload.state == "offline") {
          var new_html = '<span class="tag is-danger" id="online_offline_state_' + data.payload.mac + '">Offline</span>';
          $("#online_offline_tag_parent_" + mac_selector).html(new_html);
        }
      } else if (data.type == "status_report") {
        let mac_selector = data.payload.mac;
        mac_selector = mac_selector.replaceAll(":", "\\:");
        $("#heap_used_" + mac_selector).text(data.payload.heap_used_pct + "%");
        var temperature_unit = $("#temperature_" + mac_selector).text().slice(-2);
        $("#temperature_" + mac_selector).text(Math.round(data.payload.temperature*100)/100 + " " + temperature_unit);


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

        if (data.payload.state == "online") {
          var new_html = '<span class="tag is-success" id="online_offline_state_' + data.payload.mac + '">Online</span>';
          if(!panels_that_are_updating.includes(data.payload.mac)) {
            if($("#online_offline_tag_parent_" + mac_selector).text().trim() == "Online") {
              $("#online_offline_tag_parent_" + mac_selector).html(new_html);
            } else {
              setTimeout(function() {
                location.reload();
              }, 1000);
            }
          }
        } else if (data.payload.state == "offline") {
          var new_html = '<span class="tag is-danger" id="online_offline_state_' + data.payload.mac + '">Offline</span>';
          $("#online_offline_tag_parent_" + mac_selector).html(new_html);
        } else {
          // Update panel tag to show update progress if any
          update_text = ""
          update_progress = 0
          if (data.payload.state == "updating_fw") {
            update_text = "Updating firmware"
            update_progress = data.payload.progress
            if(!panels_that_are_updating.includes(data.payload.mac)) {
              console.log("Adding " + data.payload.mac + " to updating panels.");
              panels_that_are_updating.push(data.payload.mac);
            }
          } else if (data.payload.state == "updating_fs") {
            update_text = "Updating LittleFS"
            update_progress = data.payload.progress
            if(!panels_that_are_updating.includes(data.payload.mac)) {
              console.log("Adding " + data.payload.mac + " to updating panels.");
              panels_that_are_updating.push(data.payload.mac);
            }
          } else if (data.payload.state == "updating_tft") {
            update_text = "Updating GUI"
            update_progress = data.payload.progress
            if(!panels_that_are_updating.includes(data.payload.mac)) {
              console.log("Adding " + data.payload.mac + " to updating panels.");
              panels_that_are_updating.push(data.payload.mac);
            }
          }

          var new_html = '<div class="tags has-addons"><span class="tag is-dark">' + update_text + '</span><span class="tag is-info">' + update_progress + '%</span></div>';
          $("#online_offline_tag_parent_" + mac_selector).html(new_html);
        }
      }
  }

  webSocket.onclose = (event) => {
      console.log("Websocket closed, trying in 1 second");
      setTimeout(function() {
          connect_to_websocket();
          // location.reload();
      }, 1000);
  };
}

function show_dropdown_menu(event) {
  $(this).closest('.dropdown').toggleClass("is-active");
  event.stopPropagation();
}

$(document).ready(function() {
  connect_to_websocket(); 

  $("#firmware_upload_file_input").change(function (){
    var fileName = $(this).val().replace("C:\\fakepath\\", "");
    $("#firmware_upload_file_name").html(fileName);
  });


  $("#data_upload_file_input").change(function (){
    var fileName = $(this).val().replace("C:\\fakepath\\", "");
    $("#data_upload_file_name").html(fileName);
  });


  $("#tft_upload_file_input").change(function (){
    var fileName = $(this).val().replace("C:\\fakepath\\", "");
    $("#tft_upload_file_name").html(fileName);
  });

  $(".dropdown").click(show_dropdown_menu);
  $(window).click(function() {
    $(".dropdown").removeClass("is-active");
  });
});


