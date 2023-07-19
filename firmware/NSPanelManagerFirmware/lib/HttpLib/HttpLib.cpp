#include "esp_http_client.h"
#include <HTTPClient.h>
#include <HttpLib.hpp>
#include <MqttLog.hpp>

size_t HttpLib::GetFileSize(const char *url) {
  HTTPClient httpClient;
  httpClient.begin(url);
  const char *header_names[] = {"Content-Length"};
  httpClient.collectHeaders(header_names, 1);
  int httpReturnCode = httpClient.GET();

  if (httpReturnCode != 200) {
    LOG_ERROR("Failed to retrive file size for URL '", url, "'. Got return code: ", httpReturnCode);
    return 0;
  }

  try {
    size_t content_length = httpClient.header("Content-Length").toInt();
    httpClient.end();
    return content_length;
  } catch (...) {
    LOG_ERROR("Something went wrong when checking file size for URL '", url, "'.");
  }
  httpClient.end();
  return 0;
}

size_t HttpLib::DownloadChunk(uint8_t *buffer, const char *address, size_t offset, size_t size) {
  HTTPClient httpClient;
  httpClient.begin(address);

  std::string rangeHeader = "bytes=";
  rangeHeader.append(std::to_string(offset));
  rangeHeader.append("-");
  rangeHeader.append(std::to_string(offset + size));
  httpClient.addHeader("Range", rangeHeader.c_str());
  rangeHeader.clear();

  int httpReturnCode = httpClient.GET();
  // LOG_TRACE("Downloading TFT ", rangeHeader.c_str(), " got return code: ", httpReturnCode);
  if (httpReturnCode != 200 && httpReturnCode != 206) {
    httpClient.end();
    LOG_ERROR("Failed to retrive file chunk from URL '", address, "'. Got return code: ", httpReturnCode);
    return 0;
  }

  size_t sizeReceived = 0;
  while (sizeReceived < size) {
    if (!httpClient.getStreamPtr()->available()) { // No data avilable from WiFi, wait 100ms and try again
      vTaskDelay(500 / portTICK_PERIOD_MS);
      LOG_DEBUG("Still waiting for data from address '", address, "'.");
      continue;
    }
    sizeReceived += httpClient.getStreamPtr()->readBytes(&buffer[sizeReceived], httpClient.getStreamPtr()->available() >= size - sizeReceived ? size - sizeReceived : httpClient.getStreamPtr()->available());
  }
  httpClient.end();

  return sizeReceived;
}

bool HttpLib::GetMD5sum(const char *address, char *buffer) {
  HTTPClient http;
  http.begin(address);
  int responseCode = http.GET();

  if (responseCode == 200) {
    http.getString().toCharArray(buffer, 33);
    http.end();
    return true;
  }
  http.end();
  return false;
}

bool HttpLib::DownloadJSON(const char *address, JsonDocument *document) {
  HTTPClient http;
  http.begin(address);
  int responseCode = http.GET();

  if (responseCode == 200) {
    DeserializationError error = deserializeJson(*document, http.getStream());
    http.end();
    return !error; // Return true or false depending on there was a deserialization error
  }
  http.end();
  return false;
}

esp_err_t HttpLib::_http_event_handler(esp_http_client_event_t *evt) {
  static char *output_buffer; // Buffer to store response of http request from event handler
  static int output_len;      // Stores number of bytes read
  switch (evt->event_id) {
  case HTTP_EVENT_ERROR:
    ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
    /*
     *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
     *  However, event handler can also be used in case chunked encoding is used.
     */
    if (!esp_http_client_is_chunked_response(evt->client)) {
      // If user_data buffer is configured, copy the response into the buffer
      int copy_len = 0;
      if (evt->user_data) {
        copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
        if (copy_len) {
          memcpy(evt->user_data + output_len, evt->data, copy_len);
        }
      } else {
        const int buffer_len = esp_http_client_get_content_length(evt->client);
        if (output_buffer == NULL) {
          output_buffer = (char *)malloc(buffer_len);
          output_len = 0;
          if (output_buffer == NULL) {
            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
            return ESP_FAIL;
          }
        }
        copy_len = MIN(evt->data_len, (buffer_len - output_len));
        if (copy_len) {
          memcpy(output_buffer + output_len, evt->data, copy_len);
        }
      }
      output_len += copy_len;
    }

    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
    if (output_buffer != NULL) {
      // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
      // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
      free(output_buffer);
      output_buffer = NULL;
    }
    output_len = 0;
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
    int mbedtls_err = 0;
    esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
    if (err != 0) {
      ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
      ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
    }
    if (output_buffer != NULL) {
      free(output_buffer);
      output_buffer = NULL;
    }
    output_len = 0;
    break;
  case HTTP_EVENT_REDIRECT:
    ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
    esp_http_client_set_header(evt->client, "From", "user@example.com");
    esp_http_client_set_header(evt->client, "Accept", "text/html");
    esp_http_client_set_redirection(evt->client);
    break;
  }
  return ESP_OK;
}

bool HttpLib::http_post_json(const char *host, uint16_t port, const char *path, const char *post_data, char *response_buffer) {
  esp_http_client_config_t config = {
      .host = host,
      .path = path,
      .port = port,
      .query = "esp",
      .event_handler = _http_event_handler,
      .user_data = response_buffer, // Pass address of local buffer to get response
      .disable_auto_redirect = true,
      .method = HTTP_METHOD_POST,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_header(client, "Content-Type", "application/json");
  esp_http_client_set_post_field(client, post_data, strlen(post_data));
  esp_err_t err = esp_http_client_perform(client);
  if (err != ESP_OK) {
    printf("HTTP POST request failed: %s", esp_err_to_name(err));
  }
  return err == ESP_OK;
}
