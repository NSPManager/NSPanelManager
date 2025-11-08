#include <MqttManager.hpp>
#include <esp_log.h>

void MqttManager::start(std::string *server, uint16_t *port, std::string *username, std::string *password) {
  ESP_LOGI("MQTT", "Starting MQTTManager, will connect to %s:%d", server->c_str(), *port);
  MqttManager::_connected = false;
  MqttManager::_mqtt_config.broker.address.hostname = server->c_str();
  MqttManager::_mqtt_config.broker.address.port = *port;
  MqttManager::_mqtt_config.broker.address.transport = esp_mqtt_transport_t::MQTT_TRANSPORT_OVER_TCP;
  if (username->size() > 0 && password->size() > 0) {
    MqttManager::_mqtt_config.credentials.username = username->c_str();
    MqttManager::_mqtt_config.credentials.authentication.password = password->c_str();
  }
  MqttManager::_mqtt_config.credentials.client_id = "NSPMPanel-TODO"; // TODO: Replace "TODO" with actual unique name.
  MqttManager::_mqtt_config.buffer.size = 4096;
  MqttManager::_mqtt_config.buffer.out_size = 512;
  MqttManager::_mqtt_client = esp_mqtt_client_init(&MqttManager::_mqtt_config);
  if (MqttManager::_mqtt_client == NULL) {
    ESP_LOGE("MQTT", "Failed to create MQTT client!");
    esp_restart();
    return;
  }

  // Register event handler for MQTT events
  esp_err_t result = esp_mqtt_client_register_event(MqttManager::_mqtt_client, esp_mqtt_event_id_t::MQTT_EVENT_ANY, MqttManager::_mqtt_event_handler, NULL);
  switch (result) {
  case ESP_ERR_NO_MEM:
    ESP_LOGE("MQTT", "Failed to allocate MQTT event handler!");
    esp_restart();
    break;

  case ESP_ERR_INVALID_ARG:
    ESP_LOGE("MQTT", "Failed to initialize MQTT event handler!");
    esp_restart();
    break;

  case ESP_OK:
    ESP_LOGV("MQTT", "Attached MQTT event handler.");
    break;

  default:
    ESP_LOGW("MQTT", "Unknown status code when registering MQTT event handler: %s", esp_err_to_name(result));
    break;
  }

  // Start the MQTT client
  result = esp_mqtt_client_start(MqttManager::_mqtt_client);
  switch (result) {
  case ESP_ERR_INVALID_ARG:
    ESP_LOGE("MQTT", "Failed to start MQTT client!");
    esp_restart();
    break;

  case ESP_OK:
    ESP_LOGI("MQTT", "Started MQTT client.");
    break;

  default:
    ESP_LOGW("MQTT", "Unknown status code when registering MQTT event handler: %s", esp_err_to_name(result));
    break;
  }
}

void MqttManager::_mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
  // esp_mqtt_client_handle_t client = event->client;

  switch ((esp_mqtt_event_id_t)event_id) {
  case MQTT_EVENT_CONNECTED:
    ESP_LOGI("MQTT", "Connected to MQTT server.");
    MqttManager::_connected = true;
    break;

  case MQTT_EVENT_DISCONNECTED:
    ESP_LOGW("MQTT", "Lost connection to MQTT server.");
    MqttManager::_connected = false;
    break;

  case MQTT_EVENT_ERROR:
    ESP_LOGI("MQTT", "MQTT_EVENT_ERROR");
    if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
      if (event->error_handle->esp_tls_last_esp_err != 0) {
        ESP_LOGE("MQTT", "Error report from esp-tls!");
      } else if (event->error_handle->esp_tls_stack_err != 0) {
        ESP_LOGE("MQTT", "Error report from tls stack!");
      } else if (event->error_handle->esp_transport_sock_errno != 0) {
        ESP_LOGE("MQTT", "Captured as transport's socket errno!");
      }
      ESP_LOGI("MQTT", "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
    }
    break;

  default:
    break;
  }
}

bool MqttManager::connected() {
  return MqttManager::_connected;
}

esp_err_t MqttManager::subscribe(std::string topic) {
  if (MqttManager::connected()) {
    int result_code = esp_mqtt_client_subscribe_single(MqttManager::_mqtt_client, topic.c_str(), 2);
    if (result_code >= 0) {
      return ESP_OK;
    } else {
      ESP_LOGE("MqttManager", "Failed to subscribe to '%s'. Got return code: %d", topic.c_str(), result_code);
    }
  } else {
    ESP_LOGE("MqttManager", "Failed to subscribe to MQTT topic. Not connected to MQTT server.");
  }
  return ESP_ERR_NOT_FINISHED;
}

esp_err_t MqttManager::unsubscribe(std::string topic) {
  if (MqttManager::connected()) {
    int result_code = esp_mqtt_client_unsubscribe(MqttManager::_mqtt_client, topic.c_str());
    if (result_code >= 0) {
      return ESP_OK;
    } else {
      ESP_LOGE("MqttManager", "Failed to unsubscribe from '%s'. Got return code: %d", topic.c_str(), result_code);
    }
  } else {
    ESP_LOGE("MqttManager", "Failed to unsubscribe from MQTT topic. Not connected to MQTT server.");
  }
  return ESP_ERR_NOT_FINISHED;
}

esp_err_t MqttManager::publish(std::string topic, const char *data, size_t length, bool retain) {
  if (MqttManager::connected()) {
    int result_code = esp_mqtt_client_publish(MqttManager::_mqtt_client, topic.c_str(), data, length, 0, retain);
    if (result_code >= 0) {
      return ESP_OK;
    } else {
      ESP_LOGE("MqttManager", "Failed to publish to '%s'. Got return code: %d", topic.c_str(), result_code);
    }
  } else {
    ESP_LOGE("MqttManager", "Failed to public to MQTT topic. Not connected to MQTT server.");
  }
  return ESP_ERR_NOT_FINISHED;
}

esp_err_t MqttManager::register_handler(esp_mqtt_event_id_t event_id, esp_event_handler_t event_handler, void *event_handler_arg) {
  return esp_mqtt_client_register_event(MqttManager::_mqtt_client, event_id, event_handler, event_handler_arg);
}

esp_err_t MqttManager::unregister_handler(esp_mqtt_event_id_t event_id, esp_event_handler_t event_handler) {
  return esp_mqtt_client_unregister_event(MqttManager::_mqtt_client, event_id, event_handler);
}