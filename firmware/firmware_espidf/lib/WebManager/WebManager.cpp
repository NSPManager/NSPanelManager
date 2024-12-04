#include <ConfigManager.hpp>
#include <MqttManager.hpp>
#include <NSPM_ConfigManager.hpp>
#include <NSPM_version.hpp>
#include <WebManager.hpp>
#include <WiFiManager.hpp>
#include <cJSON.h>
#include <esp_log.h>
#include <esp_wifi.h>

void WebManager::start() {
  if (WebManager::_server == NULL) {
    WebManager::_config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&WebManager::_server, &WebManager::_config) == ESP_OK) {
      httpd_register_uri_handler(WebManager::_server, &WebManager::_uri_index);                  // Register index view
      httpd_register_uri_handler(WebManager::_server, &WebManager::_uri_save_config);            // Register URI for saving config data
      httpd_register_uri_handler(WebManager::_server, &WebManager::_uri_config_data);            // Register URI for config data
      httpd_register_uri_handler(WebManager::_server, &WebManager::_uri_status_data);            // Register URI for status data
      httpd_register_uri_handler(WebManager::_server, &WebManager::_uri_get_available_networks); // Register URI for getting list of available networks
      httpd_register_uri_handler(WebManager::_server, &WebManager::_uri_static_css);             // Register static CSS uri
    } else {
      ESP_LOGE("Web", "Failed to start web server!");
    }
  }
}

esp_err_t WebManager::_handle_uri_index(httpd_req_t *req) {
  return WebManager::_read_file_and_response(req, "/littlefs/index.html", "text/html");
}

esp_err_t WebManager::_handle_uri_config_data(httpd_req_t *req) {
  cJSON *json = cJSON_CreateObject();
  if (json == NULL) {
    ESP_LOGE("WebManager", "Failed to create JSON-object when handling request for config data!");
    return ESP_ERR_NO_MEM;
  }
  cJSON_AddStringToObject(json, "version", NSPM_VERSION);
  cJSON_AddNumberToObject(json, "log_level", ConfigManager::log_level);
  cJSON_AddStringToObject(json, "wifi_hostname", ConfigManager::wifi_hostname.c_str());
  cJSON_AddStringToObject(json, "wifi_ssid", ConfigManager::wifi_ssid.c_str());
  cJSON_AddStringToObject(json, "wifi_psk", ConfigManager::wifi_psk.c_str());
  cJSON_AddStringToObject(json, "mqtt_server", ConfigManager::mqtt_server.c_str());
  cJSON_AddNumberToObject(json, "mqtt_port", ConfigManager::mqtt_port);
  cJSON_AddStringToObject(json, "mqtt_username", ConfigManager::mqtt_username.c_str());
  cJSON_AddStringToObject(json, "mqtt_psk", ConfigManager::mqtt_password.c_str());

  if (ConfigManager::use_latest_nextion_upload_protocol) {
    cJSON_AddTrueToObject(json, "use_latest_nextion_upload_protocol");
  } else {
    cJSON_AddFalseToObject(json, "use_latest_nextion_upload_protocol");
  }
  cJSON_AddNumberToObject(json, "nextion_upload_baudrate", ConfigManager::nextion_upload_baudrate);

  if (httpd_resp_set_type(req, "application/json") != ESP_OK) {
    ESP_LOGE("Web", "Failed to set content type for response!");
  }

  char *json_string = cJSON_Print(json);
  httpd_resp_send_chunk(req, json_string, strlen(json_string));
  httpd_resp_send_chunk(req, NULL, 0);

  cJSON_Delete(json);
  free(json_string);

  return ESP_OK;
}

esp_err_t WebManager::_handle_uri_save_config(httpd_req_t *req) {
  std::vector<char> data(req->content_len + 1);
  httpd_req_recv(req, data.data(), req->content_len);
  data[req->content_len] = '\0';

  // Decode form data into result unordered map
  std::unordered_map<std::string, std::string> result;
  if (WebManager::_decode_form_data(data.data(), &result) != ESP_OK) {
    ESP_LOGE("WebManager", "Failed to decode form data into parameters! Form data: %s", data.data());
    return ESP_ERR_INVALID_ARG;
  }

  if (result.find("wifi_hostname") != result.end()) {
    ConfigManager::wifi_hostname = result["wifi_hostname"];
  } else {
    ESP_LOGE("WebManager", "Could not find field name 'wifi_hostname' while saving config from web post request!");
  }

  if (result.find("log_level") != result.end()) {
    ConfigManager::log_level = atoi(result["log_level"].c_str());
  } else {
    ESP_LOGE("WebManager", "Could not find field name 'log_level' while saving config from web post request!");
  }

  if (result.find("wifi_ssid") != result.end()) {
    ConfigManager::wifi_ssid = result["wifi_ssid"];
  } else {
    ESP_LOGE("WebManager", "Could not find field name 'wifi_ssid' while saving config from web post request!");
  }

  if (result.find("wifi_psk") != result.end()) {
    ConfigManager::wifi_psk = result["wifi_psk"];
  } else {
    ESP_LOGE("WebManager", "Could not find field name 'wifi_psk' while saving config from web post request!");
  }

  if (result.find("mqtt_server") != result.end()) {
    ConfigManager::mqtt_server = result["mqtt_server"];
  } else {
    ESP_LOGE("WebManager", "Could not find field name 'mqtt_server' while saving config from web post request!");
  }

  if (result.find("mqtt_port") != result.end()) {
    ConfigManager::mqtt_port = atoi(result["mqtt_port"].c_str());
  } else {
    ESP_LOGE("WebManager", "Could not find field name 'mqtt_port' while saving config from web post request!");
  }

  if (result.find("mqtt_username") != result.end()) {
    ConfigManager::mqtt_username = result["mqtt_username"];
  } else {
    ESP_LOGE("WebManager", "Could not find field name 'mqtt_username' while saving config from web post request!");
  }

  if (result.find("mqtt_psk") != result.end()) {
    ConfigManager::mqtt_password = result["mqtt_psk"];
  } else {
    ESP_LOGE("WebManager", "Could not find field name 'mqtt_psk' while saving config from web post request!");
  }

  if (result.find("upload_protocol") != result.end()) {
    if (result["upload_protocol"].compare("latest") == 0) {
      ConfigManager::use_latest_nextion_upload_protocol = true;
    } else {
      ConfigManager::use_latest_nextion_upload_protocol = false;
    }
  } else {
    ESP_LOGE("WebManager", "Could not find field name 'upload_protocol' while saving config from web post request!");
  }

  if (result.find("upload_baud_rate") != result.end()) {
    ConfigManager::nextion_upload_baudrate = atoi(result["upload_baud_rate"].c_str());
  } else {
    ESP_LOGE("WebManager", "Could not find field name 'upload_baud_rate' while saving config from web post request!");
  }

  httpd_resp_set_status(req, "302 Found");  // Set the status code
  httpd_resp_set_hdr(req, "Location", "/"); // Set the Location header
  httpd_resp_send(req, NULL, 0);            // Send the response

  ConfigManager::save_config();
  esp_restart();
  return ESP_OK;
}

esp_err_t WebManager::_decode_form_data(char *source, std::unordered_map<std::string, std::string> *dest) {
  // Decode URL form data, example data:
  // wifi_hostname=NSPMPanel-FA5734&log_level=2&wifi_ssid=%C3%A5%C3%A4%C3%B6%21%22%23%C2%A4%25%26%2F%28%29%3D%3F%60%3B%3A_&wifi_psk=password&mqtt_server=10.2.0.4&mqtt_port=1883&mqtt_username=&mqtt_psk=&upload_protocol=latest&upload_buad_rate=115200

  std::string field;         // Currently reading field name
  std::string data;          // Currently reading field data
  bool reading_field = true; // Are we reading field name or data? True for field name

  while (*source) {
    if (*source == '=') {
      reading_field = !reading_field;
      source++;
    } else if (*source == '&') {
      // We've now read a whole field, this is the split between two fields
      reading_field = true; // Reset, the next field name is to be read
      (*dest)[field] = data;
      field.clear();
      data.clear();
      source++;
    } else if (*source == '%') { // We are reading base64 (ie. URI encoded) data
      int value;
      sscanf(source + 1, "%2X", &value); // Read the hex string from the source
      if (reading_field) {
        field.push_back((char)value);
      } else {
        data.push_back((char)value);
      }
      source += 3; // Move pas current hex string
    } else if (*source == '+') {
      if (reading_field) {
        field.push_back(' ');
      } else {
        data.push_back(' ');
      }
      source++;
    } else {
      if (reading_field) {
        field.push_back(*source++);
      } else {
        data.push_back(*source++);
      }
    }
  }
  // The last field doesn't contain an &-sign to separate it from other fields, therefore we add it manually here.
  (*dest)[field] = data;
  field.clear();
  data.clear();

  return ESP_OK;
}

esp_err_t WebManager::_handle_uri_status_data(httpd_req_t *req) {
  cJSON *json = cJSON_CreateObject();
  if (json == NULL) {
    ESP_LOGE("WebManager", "Failed to create JSON-object when handling request for status data!");
    return ESP_ERR_NO_MEM;
  }

  wifi_mode_t current_wifi_mode;
  if (esp_wifi_get_mode(&current_wifi_mode) == ESP_OK) {
    if (current_wifi_mode == wifi_mode_t::WIFI_MODE_STA) {
      if (WiFiManager::connected()) {
        cJSON_AddStringToObject(json, "wifi_status", "Connected");
      } else {
        cJSON_AddStringToObject(json, "wifi_status", "Disconnected");
      }
    } else if (current_wifi_mode == wifi_mode_t::WIFI_MODE_AP) {
      cJSON_AddStringToObject(json, "wifi_status", "Access Point");
    } else {
      cJSON_AddStringToObject(json, "wifi_status", "Unknown WiFi mode!");
    }
  } else {
    cJSON_AddStringToObject(json, "wifi_status", "Failed to get mode!");
  }

  if (MqttManager::connected()) {
    cJSON_AddStringToObject(json, "mqtt_status", "Connected");
  } else {
    cJSON_AddStringToObject(json, "mqtt_status", "Disconnected");
  }

  std::string manager_address = NSPM_ConfigManager::get_manager_address();
  if (MqttManager::connected()) {
    if (manager_address.empty()) {
      cJSON_AddStringToObject(json, "nspanelmanager_status", "Searching");
    } else {
      uint16_t manager_port = NSPM_ConfigManager::get_manager_port();
      std::string ret_string = "Connected to ";
      ret_string.append(manager_address);
      ret_string.append(":");
      ret_string.append(std::to_string(manager_port));
      cJSON_AddStringToObject(json, "nspanelmanager_status", ret_string.c_str());
    }
  } else {
    cJSON_AddStringToObject(json, "nspanelmanager_status", "Disconnected");
  }

  if (httpd_resp_set_type(req, "application/json") != ESP_OK) {
    ESP_LOGE("Web", "Failed to set content type for response!");
  }

  char *json_string = cJSON_Print(json);
  httpd_resp_send_chunk(req, json_string, strlen(json_string));
  httpd_resp_send_chunk(req, NULL, 0);

  cJSON_Delete(json);
  free(json_string);

  return ESP_OK;
}

esp_err_t WebManager::_handle_uri_main_css(httpd_req_t *req) {
  return WebManager::_read_file_and_response(req, "/littlefs/static/main.css", "text/css");
}

esp_err_t WebManager::_read_file_and_response(httpd_req_t *req, char *file_path, char *content_type) {
  FILE *f = fopen(file_path, "r");
  if (f == NULL) {
    ESP_LOGE("Web", "Failed to open '%s' for reading on LittleFS.", file_path);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  if (httpd_resp_set_type(req, content_type) != ESP_OK) {
    ESP_LOGE("Web", "Failed to set content type for response!");
  }

  // Read index.html and send it chunk by chunk
  char buffer[1024];
  size_t num_read_bytes;
  while ((num_read_bytes = fread(buffer, 1, sizeof(buffer), f)) > 0) {
    httpd_resp_send_chunk(req, buffer, num_read_bytes);
  }
  fclose(f);
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

esp_err_t WebManager::_handle_uri_get_available_networks(httpd_req_t *req) {
  cJSON *json = cJSON_CreateArray();
  if (json == NULL) {
    ESP_LOGE("WebManager", "Failed to create JSON-array when handling request for WiFi networks!");
    return ESP_ERR_NO_MEM;
  }

  // Get list of available networks
  std::vector<wifi_ap_record_t> networks = WiFiManager::search_available_networks();
  for (int i = 0; i < networks.size(); i++) {
    // Skip networks without names
    if (strlen((const char *)networks[i].ssid) == 0) {
      continue;
    }

    cJSON *network_item = cJSON_CreateObject();
    if (network_item == NULL) {
      ESP_LOGE("WebManager", "Failed to create JSON-object for AP info when handling request for WiFi networks.");
      continue;
    }

    cJSON_AddStringToObject(network_item, "ssid", (const char *)networks[i].ssid);
    cJSON_AddNumberToObject(network_item, "security", networks[i].authmode);
    cJSON_AddNumberToObject(network_item, "rssi", networks[i].rssi);
    cJSON_AddItemToArray(json, network_item);
  }

  if (httpd_resp_set_type(req, "application/json") != ESP_OK) {
    ESP_LOGE("Web", "Failed to set content type for response!");
  }

  char *json_string = cJSON_Print(json);
  httpd_resp_send_chunk(req, json_string, strlen(json_string));
  httpd_resp_send_chunk(req, NULL, 0);

  cJSON_Delete(json);
  free(json_string);

  return ESP_OK;
}