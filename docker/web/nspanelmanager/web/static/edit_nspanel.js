function connect_to_websocket() {
    webSocket = new WebSocket("ws://" + location.hostname + ":8001");

    webSocket.onmessage = (event) => {
        data = JSON.parse(event.data);
        if(data.type == "log" && data.mac == $('#nspanel_mac').text()) {
            var add_html = "<tr><td>";
            add_html += data.time;
            add_html += "</td><td>";
            if(data.level == "ERROR") {
                add_html += '<span class="tag is-danger">ERROR</span>';
            } else if (data.level == "WARNING") {
                add_html += '<span class="tag is-warning">WARNING</span>';
            } else if (data.level == "INFO") {
                add_html += '<span class="tag is-info">INFO</span>';
            } else if (data.level == "DEBUG") {
                add_html += '<span class="tag is-dark">DEBUG</span>';
            } else if (data.level == "TRACE") {
                add_html += '<span class="tag is-black">TRACE</span>';
            } else if (data.level == "WARNING") {
                add_html += '<span class="tag is-light">???UNKNOWN???</span>';
            }
            add_html += "</td>";
            add_html += '<td>';
            add_html += data.message;
            add_html += "</td>";
            add_html += "</tr>";
            
            var row_count = $('#log_body tr').length;
            if(row_count > 10) {
                $("#log_body tr:first-child").remove();
            }

            $("#log_body").append(add_html);

        }
    }

    webSocket.onclose = (event) => {
        console.log("Connected to websocket closed, trying in 1 second");
        setTimeout(function() {
            connect_to_websocket();
        }, 1000);
    };
}

function update_shown_elements() {
  if($("#button1_mode").val() == 0) { // Direct mode
    $("#button1_detached_mode_light_label").hide();
    $("#button1_detached_mode_light_select").hide();
  } else {
    $("#button1_detached_mode_light_label").show();
    $("#button1_detached_mode_light_select").show();
  }

  if($("#button2_mode").val() == 0) { // Direct mode
    $("#button2_detached_mode_light_label").hide();
    $("#button2_detached_mode_light_select").hide();
  } else {
    $("#button2_detached_mode_light_label").show();
    $("#button2_detached_mode_light_select").show();
  }

}

$(document).ready(function() {
  connect_to_websocket(); 
  update_shown_elements();

  $("#button1_mode").change(update_shown_elements);
  $("#button2_mode").change(update_shown_elements);
});
