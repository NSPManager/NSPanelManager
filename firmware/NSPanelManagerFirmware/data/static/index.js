function close_wifi_select_modal() {
  var select_wifi_modal = document.getElementById("select_wifi_modal");
  select_wifi_modal.classList.remove("is-active");
    doConnectionCheck = true;
}

function showWifiSelectModal() {
  var available_wifi_networks = document.getElementById("available_wifi_networks");
  var wifi_failed_to_get_networks = document.getElementById("wifi_failed_to_get_networks");
  var wifi_no_networks_found = document.getElementById("wifi_no_networks_found");
  var wifi_wait_div = document.getElementById("wifi_wait_div");
  var select_wifi_modal = document.getElementById("select_wifi_modal");

  available_wifi_networks.classList.add("hidden");
  wifi_failed_to_get_networks.classList.add("hidden");
  wifi_no_networks_found.classList.add("hidden");
  wifi_wait_div.classList.remove("hidden");
  select_wifi_modal.classList.add("is-active");
  getAvailableNetworks(0);
}

function getAvailableNetworks(attempt) {
  doConnectionCheck = false;
  // Make http request and get the available networks:
  const request = new XMLHttpRequest();
  request.onreadystatechange = () => {
    console.log(request.readyState);
    if(request.readyState === XMLHttpRequest.DONE && request.status === 200) {
      let data = JSON.parse(request.responseText);
      if (data.length > 0) {
        populateModalWithWiFiNetworks(data);
      } else if(attempt == 0) {
        // Got no networks back, try once more the first request usally returnes an empty list.
        console.log("Got empty list back from ESP32. Will try once more.");
        attempt++;
        getAvailableNetworks(attempt);
      }
    } else if(request.readyState === XMLHttpRequest.DONE && request.status != 200) {
      var wifi_wait_div = document.getElementById("wifi_wait_div");
      var wifi_failed_to_get_networks = document.getElementById("wifi_failed_to_get_networks");
      wifi_wait_div.classList.add("hidden");
      wifi_failed_to_get_networks.classList.remove("hidden");
    }
  };
  request.open("GET", "/available_wifi_networks");
  request.send();
}

function populateModalWithWiFiNetworks(networks) {
  var table_body_html = "";
  for(let i = 0; i < networks.length; i++) {
    let network = networks[i];
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
  }

  var wifi_wait_div = document.getElementById("wifi_wait_div");
  var available_wifi_networks = document.getElementById("available_wifi_networks");
  var available_wifi_networks_tbody = document.getElementById("available_wifi_networks_tbody");
  
  available_wifi_networks_tbody.innerHTML = table_body_html;
  wifi_wait_div.classList.add("hidden");
  available_wifi_networks.classList.remove("hidden");
}

function selectNetwork(network) {
  var wifi_ssid = document.getElementById("wifi_ssid");
  var select_wifi_modal = document.getElementById("select_wifi_modal");
  var wifi_psk = document.getElementById("wifi_psk");

  select_wifi_modal.classList.remove("is-active");
  wifi_ssid.value = network;
  wifi_psk.select();
  wifi_psk.focus();
  doConnectionCheck = true;
}
