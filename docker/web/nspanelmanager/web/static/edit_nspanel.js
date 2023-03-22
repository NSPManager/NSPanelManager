$(document).ready(function() {
    webSocket = new WebSocket("ws://" + location.hostname + ":8001", "proto1");

    webSocket.onmessage = (event) => {
        data = JSON.parse(event.data);
        if(data.type == "log") {
            var add_html = "<tr><td>";
            add_html += data.time;
            add_html += "</td><td>";
            if(data.level == "ERROR") {
                add_html += '<span class="tag is-danger">ERROR</span>';
            } else if (data.level == "WARNING") {
                add_html += '<span class="tag is-danger">WARNING</span>';
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
            console.log(add_html);
            var row_count = $('#log_body tr').length;
            if(row_count > 25) {
                $("#log_body tr:first-child").remove();
            }

            $("#log_body").append(add_html);

        }
    }
});