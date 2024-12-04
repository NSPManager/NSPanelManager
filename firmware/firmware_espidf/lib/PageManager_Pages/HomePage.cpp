#include <GUI_data.hpp>
#include <HomePage.hpp>
#include <MqttManager.hpp>
#include <NSPM_ConfigManager.hpp>
#include <Nextion.hpp>
#include <Nextion_event.hpp>
#include <PageManager.hpp>
#include <RoomManager.hpp>
#include <RoomManager_event.hpp>
#include <esp_log.h>
#include <protobuf_nspanel.pb-c.h>

void HomePage::show() {
  PageManager::set_current_page(PageManager::available_pages::HOME);
  Nextion::go_to_page(GUI_HOME_PAGE::page_name, 250);
  HomePage::_update_display();
  esp_event_handler_register(ROOMMANAGER_EVENT, ESP_EVENT_ANY_ID, HomePage::_handle_roommanager_event, NULL);
  esp_event_handler_register(NEXTION_EVENT, ESP_EVENT_ANY_ID, HomePage::_handle_nextion_event, NULL);

  if (HomePage::_special_mode_timer_handle == NULL) {
    // This is the first time this page is shown, create timers
    HomePage::_special_mode_create_timer_args = {
        .callback = &HomePage::_special_mode_timer_callback,
        .name = "special_mode_timer",
    };
    esp_err_t err = esp_timer_create(&HomePage::_special_mode_create_timer_args, &HomePage::_special_mode_timer_handle);
    if (err != ESP_OK) {
      ESP_LOGE("HomePage", "Failed to create 'Special mode timer'. Special mode will not be functional! Got error: %s", esp_err_to_name(err));
    }

    HomePage::_special_mode_timeout_create_timer_args = {
        .callback = &HomePage::_special_mode_timeout_timer_callback,
        .name = "special_mode_timeout_timer",
    };
    err = esp_timer_create(&HomePage::_special_mode_timeout_create_timer_args, &HomePage::_special_mode_timeout_timer_handle);
    if (err != ESP_OK) {
      ESP_LOGE("HomePage", "Failed to create 'Special mode timeout timer'. Special mode will not be functional! Got error: %s", esp_err_to_name(err));
    }
  }
}

void HomePage::unshow() {
  esp_event_handler_unregister(ROOMMANAGER_EVENT, ESP_EVENT_ANY_ID, HomePage::_handle_roommanager_event);
  esp_event_handler_unregister(NEXTION_EVENT, ESP_EVENT_ANY_ID, HomePage::_handle_nextion_event);
}

void HomePage::set_current_affect_mode(HomePageAffectMode mode) {
  HomePage::_current_affect_mode = mode;
  HomePage::_update_display();
}

void HomePage::set_current_edit_mode(HomePageEditMode mode) {
  HomePage::_current_edit_mode = mode;
  HomePage::_update_display();
}

void HomePage::_update_display() {
  NSPanelRoomStatus status;
  if (RoomManager::get_current_room_status(&status) == ESP_OK) {
    // Update displayed data
    if (HomePage::_current_affect_mode == HomePageAffectMode::ROOM) {
      Nextion::set_component_text(GUI_HOME_PAGE::mode_label_name, "Room", 100);
      Nextion::set_component_text(GUI_HOME_PAGE::room_label_name, status.name, 100);

      if (HomePage::_current_edit_mode == HomePageEditMode::ALL_LIGHTS) {
        if (HomePage::_cache_ceiling_light_brightness != status.ceiling_lights_dim_level) {
          Nextion::set_component_value(GUI_HOME_PAGE::label_ceiling_name, status.ceiling_lights_dim_level, 100);
          Nextion::set_component_value(GUI_HOME_PAGE::button_ceiling_name, status.ceiling_lights_dim_level > 0, 100);
          HomePage::_cache_ceiling_light_brightness = status.ceiling_lights_dim_level;
        }

        if (HomePage::_cache_table_light_brightness != status.table_lights_dim_level) {
          Nextion::set_component_value(GUI_HOME_PAGE::label_table_name, status.table_lights_dim_level, 100);
          Nextion::set_component_value(GUI_HOME_PAGE::button_table_name, status.table_lights_dim_level > 0, 100);
          HomePage::_cache_table_light_brightness = status.table_lights_dim_level;
        }

        if (HomePage::_cache_brightness_slider != status.average_dim_level) {
          Nextion::set_component_value(GUI_HOME_PAGE::dimmer_slider_name, status.average_dim_level, 100);
          HomePage::_cache_brightness_slider = status.average_dim_level;
        }

        if (HomePage::_cache_color_temperature_slider != status.average_color_temperature) {
          Nextion::set_component_value(GUI_HOME_PAGE::color_temperature_slider_name, status.average_color_temperature, 100);
          HomePage::_cache_color_temperature_slider = status.average_color_temperature;
        }
      } else if (HomePage::_current_edit_mode == HomePageEditMode::CEILING_LIGHTS) {
        if (HomePage::_cache_ceiling_light_brightness != status.ceiling_lights_dim_level) {
          Nextion::set_component_value(GUI_HOME_PAGE::label_ceiling_name, status.ceiling_lights_dim_level, 100);
          Nextion::set_component_value(GUI_HOME_PAGE::button_ceiling_name, status.ceiling_lights_dim_level > 0, 100);
          HomePage::_cache_ceiling_light_brightness = status.ceiling_lights_dim_level;
        }

        if (HomePage::_cache_brightness_slider != status.ceiling_lights_dim_level) {
          Nextion::set_component_value(GUI_HOME_PAGE::dimmer_slider_name, status.ceiling_lights_dim_level, 100);
          HomePage::_cache_brightness_slider = status.ceiling_lights_dim_level;
        }

        if (HomePage::_cache_color_temperature_slider != status.ceiling_lights_color_temperature_value) {
          Nextion::set_component_value(GUI_HOME_PAGE::color_temperature_slider_name, status.ceiling_lights_color_temperature_value, 100);
          HomePage::_cache_color_temperature_slider = status.ceiling_lights_color_temperature_value;
        }
      } else if (HomePage::_current_edit_mode == HomePageEditMode::TABLE_LIGHTS) {
        if (HomePage::_cache_table_light_brightness != status.table_lights_dim_level) {
          Nextion::set_component_value(GUI_HOME_PAGE::label_table_name, status.table_lights_dim_level, 100);
          Nextion::set_component_value(GUI_HOME_PAGE::button_table_name, status.table_lights_dim_level > 0, 100);
          HomePage::_cache_table_light_brightness = status.table_lights_dim_level;
        }

        if (HomePage::_cache_brightness_slider != status.table_lights_dim_level) {
          Nextion::set_component_value(GUI_HOME_PAGE::dimmer_slider_name, status.table_lights_dim_level, 100);
          HomePage::_cache_brightness_slider = status.table_lights_dim_level;
        }

        if (HomePage::_cache_color_temperature_slider != status.table_lights_color_temperature_value) {
          Nextion::set_component_value(GUI_HOME_PAGE::color_temperature_slider_name, status.table_lights_color_temperature_value, 100);
          HomePage::_cache_color_temperature_slider = status.table_lights_color_temperature_value;
        }
      } else {
        ESP_LOGE("HomePage", "Unknown HomePageEditMode!");
      }
    } else if (HomePage::_current_affect_mode == HomePageAffectMode::ALL) {
      // TODO: Implement "Affect all rooms"-mode
    }

  } else {
    ESP_LOGE("HomePage", "Failed to get current NSPanelRoomStatus when trying to update home page display.");
  }
}

void HomePage::_handle_roommanager_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  switch (event_id) {
  case roommanager_event_t::CURRENT_ROOM_UPDATED:
    HomePage::_update_display();
    break;
  case roommanager_event_t::ROOM_SWITCHED: {
    // We pressed the button and we are currently in a special mode, exit special mode.
    if (esp_timer_is_active(HomePage::_special_mode_timer_handle) == 1) {
      esp_timer_stop(HomePage::_special_mode_timer_handle); // Stop special mode timer in case it's running even though it shouldn't be.
    }
    HomePage::_special_mode_timer_activation_mode = HomePageEditMode::ALL_LIGHTS;
    HomePage::_special_mode_timeout_timer_callback(NULL); // Revert to "normal mode" in case we aren't already in normal mode
    HomePage::_update_display();                          // Update display with new values.
    break;
  }

  default:
    break;
  }
}

void HomePage::_handle_nextion_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  switch (event_id) {
  case nextion_event_t::TOUCH_EVENT: {
    nextion_event_touch_t *data = (nextion_event_touch_t *)event_data;
    if (data->component_id == GUI_HOME_PAGE::button_ceiling_id) {
      HomePage::_handle_nextion_event_master_ceiling_lights_button(data->pressed);
    } else if (data->component_id == GUI_HOME_PAGE::button_table_id) {
      HomePage::_handle_nextion_event_master_table_lights_button(data->pressed);
    } else if (data->component_id == GUI_HOME_PAGE::dimmer_slider_id) {
      HomePage::_update_brightness_slider_cache();
      HomePage::_handle_brightness_slider_event();
    } else if (data->component_id == GUI_HOME_PAGE::color_temperature_slider_id) {
      HomePage::_update_color_temperature_slider_cache();
      HomePage::_handle_color_temperature_slider_event();
    } else if (data->component_id == GUI_HOME_PAGE::button_next_room_id) {
      RoomManager::go_to_next_room();
    } else {
      ESP_LOGW("HomePage", "Got touch event from unknown ID: %d", data->component_id);
    }
    break;
  }

  default:
    break;
  }
}

void HomePage::_handle_nextion_event_master_ceiling_lights_button(bool pressed) {
  if (pressed) {
    if (HomePage::_current_edit_mode == HomePageEditMode::ALL_LIGHTS) {
      // We are currently pressing the "Ceiling lights master button", start timer for special mode. Only activate special mode if
      // we are currently not already in special mode.
      NSPanelConfig config;
      if (NSPM_ConfigManager::get_config(&config) == ESP_OK) {
        esp_timer_stop(HomePage::_special_mode_timer_handle); // Stop special mode timer in case it's running even though it shouldn't be.
        HomePage::_special_mode_timer_activation_mode = HomePageEditMode::CEILING_LIGHTS;
        esp_timer_start_once(HomePage::_special_mode_timer_handle, config.special_mode_trigger_time * 1000);
      } else {
        ESP_LOGE("HomePage", "Failed to get NSPanel Config from NSPM_ConfigManager!");
      }
    } else {
      // We pressed the button and we are currently in a special mode, exit special mode.
      if (esp_timer_is_active(HomePage::_special_mode_timer_handle) == 1) {
        esp_timer_stop(HomePage::_special_mode_timer_handle); // Stop special mode timer in case it's running even though it shouldn't be.
      }
      HomePage::_special_mode_timer_activation_mode = HomePageEditMode::ALL_LIGHTS;
      HomePage::_special_mode_timeout_timer_callback(NULL); // Revert to "normal mode"

      // This button press canceled the "special mode", ignore touch up event that follows this touch down event.
      HomePage::_ignore_next_ceiling_button_touch_up = true;
    }
  } else {
    // Should we ignore this event?
    if (HomePage::_ignore_next_ceiling_button_touch_up) {
      HomePage::_ignore_next_ceiling_button_touch_up = false;
      return;
    }
    // We are in "normal mode", proceed as normal
    if (HomePage::_current_edit_mode == HomePageEditMode::ALL_LIGHTS) {
      // We released the button and are currently not in special mode, therefore the timer
      // hasn't trigger yet so stop it ans the user hasn't held the button long enough
      // for special mode to trigger. This was a normal press of the button.
      if (esp_timer_is_active(HomePage::_special_mode_timer_handle) == 1) {
        esp_timer_stop(HomePage::_special_mode_timer_handle); // Stop special mode timer in case it's running even though it shouldn't be.
        HomePage::_special_mode_timer_activation_mode = HomePageEditMode::ALL_LIGHTS;
      }

      HomePage::_send_ceiling_master_button_command_to_manager();
    } else if (HomePage::_current_edit_mode == HomePage::TABLE_LIGHTS) {
      // We pressed the button and we are currently in a special mode, exit special mode.
      if (esp_timer_is_active(HomePage::_special_mode_timer_handle) == 1) {
        esp_timer_stop(HomePage::_special_mode_timer_handle); // Stop special mode timer in case it's running even though it shouldn't be.
      }
      HomePage::_special_mode_timer_activation_mode = HomePageEditMode::ALL_LIGHTS;
      HomePage::_special_mode_timeout_timer_callback(NULL); // Revert to "normal mode"
    }
  }
}

void HomePage::_handle_nextion_event_master_table_lights_button(bool pressed) {
  if (pressed) {
    if (HomePage::_current_edit_mode == HomePageEditMode::ALL_LIGHTS) {
      // We are currently pressing the "Table lights master button", start timer for special mode. Only activate special mode if
      // we are currently not already in special mode.
      NSPanelConfig config;
      if (NSPM_ConfigManager::get_config(&config) == ESP_OK) {
        esp_timer_stop(HomePage::_special_mode_timer_handle); // Stop special mode timer in case it's running even though it shouldn't be.
        HomePage::_special_mode_timer_activation_mode = HomePageEditMode::TABLE_LIGHTS;
        esp_timer_start_once(HomePage::_special_mode_timer_handle, config.special_mode_trigger_time * 1000);
      } else {
        ESP_LOGE("HomePage", "Failed to get NSPanel Config from NSPM_ConfigManager!");
      }
    } else {
      // We pressed the button and we are currently in a special mode, exit special mode.
      if (esp_timer_is_active(HomePage::_special_mode_timer_handle) == 1) {
        esp_timer_stop(HomePage::_special_mode_timer_handle); // Stop special mode timer in case it's running even though it shouldn't be.
      }
      HomePage::_special_mode_timer_activation_mode = HomePageEditMode::ALL_LIGHTS;
      HomePage::_special_mode_timeout_timer_callback(NULL); // Revert to "normal mode"

      // This button press canceled the "special mode", ignore touch up event that follows this touch down event.
      HomePage::_ignore_next_table_button_touch_up = true;
    }
  } else {
    // Should we ignore this event?
    if (HomePage::_ignore_next_table_button_touch_up) {
      HomePage::_ignore_next_table_button_touch_up = false;
      return;
    }
    // We are in "normal mode", proceed as normal
    if (HomePage::_current_edit_mode == HomePageEditMode::ALL_LIGHTS) {
      // We released the button and are currently not in special mode, therefore the timer
      // hasn't trigger yet so stop it ans the user hasn't held the button long enough
      // for special mode to trigger. This was a normal press of the button.
      if (esp_timer_is_active(HomePage::_special_mode_timer_handle) == 1) {
        esp_timer_stop(HomePage::_special_mode_timer_handle); // Stop special mode timer in case it's running even though it shouldn't be.
        HomePage::_special_mode_timer_activation_mode = HomePageEditMode::ALL_LIGHTS;
      }

      HomePage::_send_table_master_button_command_to_manager();
    } else if (HomePage::_current_edit_mode == HomePage::CEILING_LIGHTS) {
      // We pressed the button and we are currently in a special mode, exit special mode.
      if (esp_timer_is_active(HomePage::_special_mode_timer_handle) == 1) {
        esp_timer_stop(HomePage::_special_mode_timer_handle); // Stop special mode timer in case it's running even though it shouldn't be.
      }
      HomePage::_special_mode_timer_activation_mode = HomePageEditMode::ALL_LIGHTS;
      HomePage::_special_mode_timeout_timer_callback(NULL); // Revert to "normal mode"
    }
  }
}

void HomePage::_send_ceiling_master_button_command_to_manager() {
  if (HomePage::_current_affect_mode == HomePageAffectMode::ROOM) {
    NSPanelRoomStatus status;
    if (RoomManager::get_current_room_status(&status) == ESP_OK) {
      if (status.number_of_ceiling_lights > 0) {
        NSPanelMQTTManagerCommand__FirstPageTurnLightOn turn_light_on_cmd = NSPANEL_MQTTMANAGER_COMMAND__FIRST_PAGE_TURN_LIGHT_ON__INIT;
        turn_light_on_cmd.has_brightness_value = true;
        turn_light_on_cmd.brightness_slider_value = status.ceiling_lights_dim_level > 0 ? 0 : HomePage::_cache_brightness_slider;
        turn_light_on_cmd.affect_lights = NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__CEILING_LIGHTS;
        turn_light_on_cmd.has_kelvin_value = true;
        turn_light_on_cmd.kelvin_slider_value = HomePage::_cache_color_temperature_slider;
        turn_light_on_cmd.global = false;
        turn_light_on_cmd.selected_room = status.id;

        NSPanelMQTTManagerCommand cmd = NSPANEL_MQTTMANAGER_COMMAND__INIT;
        cmd.command_data_case = NSPANEL_MQTTMANAGER_COMMAND__COMMAND_DATA_FIRST_PAGE_TURN_ON;
        cmd.first_page_turn_on = &turn_light_on_cmd;

        uint32_t packed_length = nspanel_mqttmanager_command__get_packed_size(&cmd);
        std::vector<uint8_t> buffer(packed_length); // Use vector for automatic cleanup of data when going out of scope
        size_t packed_data_size = nspanel_mqttmanager_command__pack(&cmd, buffer.data());
        if (packed_data_size == packed_length) {
          if (MqttManager::publish(NSPM_ConfigManager::get_manager_command_topic(), (const char *)buffer.data(), packed_length, false) == ESP_OK) {
            NSPanelConfig config;
            if (NSPM_ConfigManager::get_config(&config) == ESP_OK) {
              if (config.optimistic_mode) {
                NSPanelRoomStatus *mutable_status;
                if (RoomManager::get_current_room_status_mutable(&mutable_status) == ESP_OK) {
                  // Does room contain ceiling lights?
                  // If all lights were off, they are now all on
                  if (status.ceiling_lights_dim_level == 0) {
                    status.ceiling_lights_dim_level = HomePage::_cache_brightness_slider;
                    status.number_of_ceiling_lights_on = status.number_of_ceiling_lights;
                  } else {
                    status.ceiling_lights_dim_level = 0;
                    status.number_of_ceiling_lights_on = 0;
                  }
                  // Calculate new average of total color temperature:
                  uint64_t total_light_level = 0;
                  uint16_t number_of_lights_on = 0;
                  total_light_level = status.ceiling_lights_dim_level * status.number_of_ceiling_lights_on;
                  number_of_lights_on += status.number_of_ceiling_lights_on;
                  total_light_level += status.table_lights_dim_level * status.number_of_table_lights_on;
                  number_of_lights_on += status.number_of_table_lights_on;

                  if (number_of_lights_on > 0) {
                    status.average_dim_level = total_light_level / number_of_lights_on;
                  }

                  RoomManager::replace_room_status(mutable_status);
                  HomePage::_update_display();
                } else {
                  ESP_LOGW("HomePage", "Failed to get mutable room status. Will wait for return result instead.");
                }
              }
            } else {
              ESP_LOGW("HomePage", "Failed to get NSPM config when checking if panel is optimistic. Will wait for return result instead.");
            }
          } else {
            ESP_LOGE("HomePage", "Failed to publish data while sending command to manager from 'ceiling master button'!");
          }
        } else {
          ESP_LOGE("HomePage", "Failed to pack data while sending command to manager from 'ceiling master button'!");
        }
      }
    } else {
      ESP_LOGE("HomePage", "Failed to get current page room status when trying to send 'ceiling master button' command to manager!");
    }
  }
}

void HomePage::_send_table_master_button_command_to_manager() {
  if (HomePage::_current_affect_mode == HomePageAffectMode::ROOM) {
    NSPanelRoomStatus status;
    if (RoomManager::get_current_room_status(&status) == ESP_OK) {
      if (status.number_of_table_lights > 0) {
        // Build and send command to turn on/off table lights
        NSPanelMQTTManagerCommand__FirstPageTurnLightOn turn_light_on_cmd = NSPANEL_MQTTMANAGER_COMMAND__FIRST_PAGE_TURN_LIGHT_ON__INIT;
        turn_light_on_cmd.has_brightness_value = true;
        turn_light_on_cmd.brightness_slider_value = status.table_lights_dim_level > 0 ? 0 : HomePage::_cache_brightness_slider;
        turn_light_on_cmd.affect_lights = NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__TABLE_LIGHTS;
        turn_light_on_cmd.has_kelvin_value = true;
        turn_light_on_cmd.kelvin_slider_value = HomePage::_cache_color_temperature_slider;
        turn_light_on_cmd.global = false;
        turn_light_on_cmd.selected_room = status.id;

        NSPanelMQTTManagerCommand cmd = NSPANEL_MQTTMANAGER_COMMAND__INIT;
        cmd.command_data_case = NSPANEL_MQTTMANAGER_COMMAND__COMMAND_DATA_FIRST_PAGE_TURN_ON;
        cmd.first_page_turn_on = &turn_light_on_cmd;

        uint32_t packed_length = nspanel_mqttmanager_command__get_packed_size(&cmd);
        std::vector<uint8_t> buffer(packed_length); // Use vector for automatic cleanup of data when going out of scope
        size_t packed_data_size = nspanel_mqttmanager_command__pack(&cmd, buffer.data());
        if (packed_data_size == packed_length) {
          if (MqttManager::publish(NSPM_ConfigManager::get_manager_command_topic(), (const char *)buffer.data(), packed_length, false) == ESP_OK) {
            NSPanelConfig config;
            if (NSPM_ConfigManager::get_config(&config) == ESP_OK) {
              // Panel is running in optimistic mode,
              if (config.optimistic_mode) {
                NSPanelRoomStatus *mutable_status;
                if (RoomManager::get_current_room_status_mutable(&mutable_status) == ESP_OK) {
                  // Does room contain table lights?
                  // If all lights were off, they are now all on
                  if (mutable_status->table_lights_dim_level == 0) {
                    mutable_status->table_lights_dim_level = HomePage::_cache_brightness_slider;
                    mutable_status->number_of_table_lights_on = mutable_status->number_of_table_lights;
                  } else {
                    mutable_status->table_lights_dim_level = 0;
                    mutable_status->number_of_table_lights_on = 0;
                  }
                  // Calculate new average of total color temperature:
                  uint64_t total_light_level = 0;
                  uint16_t number_of_lights_on = 0;
                  total_light_level = mutable_status->ceiling_lights_dim_level * mutable_status->number_of_ceiling_lights_on;
                  number_of_lights_on += mutable_status->number_of_ceiling_lights_on;
                  total_light_level += mutable_status->table_lights_dim_level * mutable_status->number_of_table_lights_on;
                  number_of_lights_on += mutable_status->number_of_table_lights_on;

                  if (number_of_lights_on > 0) {
                    mutable_status->average_dim_level = total_light_level / number_of_lights_on;
                  }

                  // As we replace an existing room we do not need to free the *status pointer.
                  // "Ownership" is handed over the RoomManager
                  RoomManager::replace_room_status(mutable_status);
                  HomePage::_update_display();
                } else {
                  ESP_LOGW("HomePage", "Failed to get mutable room status object. Will wait fro return result instead.");
                }
              }
            } else {
              ESP_LOGW("HomePage", "Failed to get NSPM config when checking if panel is optimistic. Will wait for return result instead.");
            }
          } else {
            ESP_LOGE("HomePage", "Failed to publish data while sending command to manager from 'table master button'!");
          }
        } else {
          ESP_LOGE("HomePage", "Failed to pack data while sending command to manager from 'table master button'!");
        }
      }
    } else {
      ESP_LOGE("HomePage", "Failed to get current page room status when trying to send 'table master button' command to manager!");
    }
  }
}

void HomePage::_update_brightness_slider_cache() {
  if (Nextion::get_component_integer_value(GUI_HOME_PAGE::dimmer_slider_name, &HomePage::_cache_brightness_slider, 1000, 250) != ESP_OK) {
    ESP_LOGE("HomePage", "Failed to get current color temperature value to update cache!");
    return;
  }
}

void HomePage::_update_color_temperature_slider_cache() {
  if (Nextion::get_component_integer_value(GUI_HOME_PAGE::color_temperature_slider_name, &HomePage::_cache_color_temperature_slider, 1000, 250) != ESP_OK) {
    ESP_LOGE("HomePage", "Failed to get current color temperature slider value to update cache!");
    return;
  }
}

void HomePage::_special_mode_timer_callback(void *arg) {
  ESP_LOGI("HomePage", "Special mode timer triggered!");
  HomePage::_current_edit_mode = HomePage::_special_mode_timer_activation_mode;

  if (HomePage::_current_edit_mode == HomePageEditMode::CEILING_LIGHTS) {
    Nextion::set_component_visibility(GUI_HOME_PAGE::highlight_ceiling_name, true, 100);
    Nextion::set_component_visibility(GUI_HOME_PAGE::highlight_table_name, false, 100);
    Nextion::set_component_foreground(GUI_HOME_PAGE::dimmer_slider_name, GUI_HOME_PAGE::slider_highlight_color, 100);
    Nextion::set_component_foreground(GUI_HOME_PAGE::color_temperature_slider_name, GUI_HOME_PAGE::slider_highlight_color, 100);

    HomePage::_cache_table_light_brightness = -1; // Set cache to -1 so that it updates once it special mode ends
    Nextion::set_component_value(GUI_HOME_PAGE::label_table_name, 0, 100);
    Nextion::set_component_value(GUI_HOME_PAGE::button_table_name, false, 100);
    HomePage::_ignore_next_ceiling_button_touch_up = true;
    HomePage::_ignore_next_table_button_touch_up = false;
  } else if (HomePage::_current_edit_mode == HomePageEditMode::TABLE_LIGHTS) {
    Nextion::set_component_visibility(GUI_HOME_PAGE::highlight_ceiling_name, false, 100);
    Nextion::set_component_visibility(GUI_HOME_PAGE::highlight_table_name, true, 100);
    Nextion::set_component_foreground(GUI_HOME_PAGE::dimmer_slider_name, GUI_HOME_PAGE::slider_highlight_color, 100);
    Nextion::set_component_foreground(GUI_HOME_PAGE::color_temperature_slider_name, GUI_HOME_PAGE::slider_highlight_color, 100);

    HomePage::_cache_ceiling_light_brightness = -1; // Set cache to -1 so that it updates once it special mode ends
    Nextion::set_component_value(GUI_HOME_PAGE::label_ceiling_name, 0, 100);
    Nextion::set_component_value(GUI_HOME_PAGE::button_ceiling_name, false, 100);
    HomePage::_ignore_next_ceiling_button_touch_up = false;
    HomePage::_ignore_next_table_button_touch_up = true;
  } else {
    ESP_LOGE("HomePage", "Unknown EditMode when processing special mode timer callback.");
  }

  NSPanelConfig config;
  if (NSPM_ConfigManager::get_config(&config) == ESP_OK) {
    esp_timer_stop(HomePage::_special_mode_timeout_timer_handle);
    esp_err_t result = esp_timer_start_once(HomePage::_special_mode_timeout_timer_handle, config.special_mode_release_time * 1000);
    if (result != ESP_OK) {
      ESP_LOGE("HomePage", "Error while starting timer to exit special mode! Got error: %s", esp_err_to_name(result));
    }
  } else {
    ESP_LOGW("HomePage", "Failed to get NSPanel Config from NSPM_ConfigManager. Will set default special mode timeout of 5 seconds.");
    esp_timer_stop(HomePage::_special_mode_timeout_timer_handle);
    esp_err_t result = esp_timer_start_once(HomePage::_special_mode_timeout_timer_handle, 5000 * 1000);
    if (result != ESP_OK) {
      ESP_LOGE("HomePage", "Error while restarting timer to exit special mode! Got error: %s", esp_err_to_name(result));
    }
  }

  HomePage::_update_display();
}

void HomePage::_special_mode_timeout_timer_callback(void *arg) {
  if (HomePage::_current_edit_mode != HomePageEditMode::ALL_LIGHTS) {
    // Reset to "normal mode"
    HomePage::_current_edit_mode = HomePageEditMode::ALL_LIGHTS; // Reset activation mode
    HomePage::_ignore_next_ceiling_button_touch_up = false;
    HomePage::_ignore_next_table_button_touch_up = false;

    Nextion::set_component_visibility(GUI_HOME_PAGE::highlight_ceiling_name, false, 100);
    Nextion::set_component_visibility(GUI_HOME_PAGE::highlight_table_name, false, 100);
    Nextion::set_component_foreground(GUI_HOME_PAGE::dimmer_slider_name, GUI_HOME_PAGE::slider_normal_color, 100);
    Nextion::set_component_foreground(GUI_HOME_PAGE::color_temperature_slider_name, GUI_HOME_PAGE::slider_normal_color, 100);

    HomePage::_update_display();
  }
}

void HomePage::_handle_brightness_slider_event() {
  NSPanelMQTTManagerCommand__FirstPageTurnLightOn turn_light_on_cmd = NSPANEL_MQTTMANAGER_COMMAND__FIRST_PAGE_TURN_LIGHT_ON__INIT;
  turn_light_on_cmd.has_brightness_value = true;
  turn_light_on_cmd.brightness_slider_value = HomePage::_cache_brightness_slider;
  turn_light_on_cmd.has_kelvin_value = false;
  turn_light_on_cmd.kelvin_slider_value = 0;
  RoomManager::get_current_room_id(&turn_light_on_cmd.selected_room);
  if (HomePage::_current_edit_mode == HomePageEditMode::ALL_LIGHTS) {
    turn_light_on_cmd.affect_lights = NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__ALL;
  } else if (HomePage::_current_edit_mode == HomePageEditMode::CEILING_LIGHTS) {
    turn_light_on_cmd.affect_lights = NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__CEILING_LIGHTS;
  } else if (HomePage::_current_edit_mode == HomePageEditMode::TABLE_LIGHTS) {
    turn_light_on_cmd.affect_lights = NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__TABLE_LIGHTS;
  } else {
    ESP_LOGE("HomePage", "Unknown edit mode when sending command from slider event, will cancel.");
    return;
  }

  if (HomePage::_current_affect_mode == HomePageAffectMode::ROOM) {
    turn_light_on_cmd.global = false;
  } else if (HomePage::_current_affect_mode == HomePageAffectMode::ROOM) {
    turn_light_on_cmd.global = true;
  } else {
    ESP_LOGE("HomePage", "Unknown affect mode when sending command from slider event, will cancel.");
    return;
  }

  NSPanelMQTTManagerCommand cmd = NSPANEL_MQTTMANAGER_COMMAND__INIT;
  cmd.command_data_case = NSPANEL_MQTTMANAGER_COMMAND__COMMAND_DATA_FIRST_PAGE_TURN_ON;
  cmd.first_page_turn_on = &turn_light_on_cmd;

  uint32_t packed_length = nspanel_mqttmanager_command__get_packed_size(&cmd);
  std::vector<uint8_t> buffer(packed_length); // Use vector for automatic cleanup of data when going out of scope
  size_t packed_data_size = nspanel_mqttmanager_command__pack(&cmd, buffer.data());

  if (packed_data_size == packed_length) {
    if (MqttManager::publish(NSPM_ConfigManager::get_manager_command_topic(), (const char *)buffer.data(), packed_length, false) == ESP_OK) {
      // TODO: This needs to be pointer to mutable room status in order for this to work.
      NSPanelConfig config;
      if (NSPM_ConfigManager::get_config(&config) == ESP_OK) {
        if (config.optimistic_mode) {
          NSPanelRoomStatus *mutable_status;
          if (RoomManager::get_current_room_status_mutable(&mutable_status) == ESP_OK) {

            if (mutable_status->number_of_ceiling_lights_on == 0 && mutable_status->number_of_table_lights_on == 0) {
              mutable_status->number_of_ceiling_lights_on = mutable_status->number_of_ceiling_lights;
              mutable_status->number_of_table_lights_on = mutable_status->number_of_table_lights;
            }
            if (mutable_status->number_of_ceiling_lights_on > 0) {
              mutable_status->ceiling_lights_dim_level = HomePage::_cache_brightness_slider;
            }
            if (mutable_status->number_of_table_lights_on > 0) {
              mutable_status->table_lights_dim_level = HomePage::_cache_brightness_slider;
            }

            uint64_t total_light_level = 0;
            uint16_t number_of_lights_on = 0;
            total_light_level = mutable_status->ceiling_lights_dim_level * mutable_status->number_of_ceiling_lights_on;
            number_of_lights_on += mutable_status->number_of_ceiling_lights_on;
            total_light_level += mutable_status->table_lights_dim_level * mutable_status->number_of_table_lights_on;
            number_of_lights_on += mutable_status->number_of_table_lights_on;

            if (number_of_lights_on > 0) {
              mutable_status->average_dim_level = total_light_level / number_of_lights_on;
            }

            RoomManager::replace_room_status(mutable_status);
            HomePage::_update_display();
          } else {
            ESP_LOGW("HomePage", "Couldn't get current room status while trying to update screen while in optimistic mode.");
          }
        }
      } else {
        ESP_LOGW("HomePage", "Failed to get NSPM config when checking if panel is optimistic. Will wait for return result instead.");
      }
    } else {
      ESP_LOGE("HomePage", "Failed to publish data while sending command to manager from 'brightness slider event'!");
    }
  } else {
    ESP_LOGE("HomePage", "Failed to pack data while sending command to manager from 'brightness slider event'!");
  }
}

void HomePage::_handle_color_temperature_slider_event() {
  NSPanelMQTTManagerCommand__FirstPageTurnLightOn turn_light_on_cmd = NSPANEL_MQTTMANAGER_COMMAND__FIRST_PAGE_TURN_LIGHT_ON__INIT;
  turn_light_on_cmd.has_brightness_value = false;
  turn_light_on_cmd.brightness_slider_value = 0;
  turn_light_on_cmd.has_kelvin_value = true;
  turn_light_on_cmd.kelvin_slider_value = HomePage::_cache_color_temperature_slider;
  RoomManager::get_current_room_id(&turn_light_on_cmd.selected_room);
  if (HomePage::_current_edit_mode == HomePageEditMode::ALL_LIGHTS) {
    turn_light_on_cmd.affect_lights = NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__ALL;
  } else if (HomePage::_current_edit_mode == HomePageEditMode::CEILING_LIGHTS) {
    turn_light_on_cmd.affect_lights = NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__CEILING_LIGHTS;
  } else if (HomePage::_current_edit_mode == HomePageEditMode::TABLE_LIGHTS) {
    turn_light_on_cmd.affect_lights = NSPANEL_MQTTMANAGER_COMMAND__AFFECT_LIGHTS_OPTIONS__TABLE_LIGHTS;
  } else {
    ESP_LOGE("HomePage", "Unknown edit mode when sending command from slider event, will cancel.");
    return;
  }

  if (HomePage::_current_affect_mode == HomePageAffectMode::ROOM) {
    turn_light_on_cmd.global = false;
  } else if (HomePage::_current_affect_mode == HomePageAffectMode::ROOM) {
    turn_light_on_cmd.global = true;
  } else {
    ESP_LOGE("HomePage", "Unknown affect mode when sending command from slider event, will cancel.");
    return;
  }

  NSPanelMQTTManagerCommand cmd = NSPANEL_MQTTMANAGER_COMMAND__INIT;
  cmd.command_data_case = NSPANEL_MQTTMANAGER_COMMAND__COMMAND_DATA_FIRST_PAGE_TURN_ON;
  cmd.first_page_turn_on = &turn_light_on_cmd;

  uint32_t packed_length = nspanel_mqttmanager_command__get_packed_size(&cmd);
  std::vector<uint8_t> buffer(packed_length); // Use vector for automatic cleanup of data when going out of scope
  size_t packed_data_size = nspanel_mqttmanager_command__pack(&cmd, buffer.data());
  if (packed_data_size == packed_length) {
    if (MqttManager::publish(NSPM_ConfigManager::get_manager_command_topic(), (const char *)buffer.data(), packed_length, false) == ESP_OK) {
      // TODO: Handle optimistic mode
      // NSPanelConfig config;
      // if (NSPM_ConfigManager::get_config(&config) == ESP_OK) {
      //   if (config.optimistic_mode) {
      //     if (status.ceiling_lights_dim_level == 0 && status.table_lights_dim_level == 0) {
      //       status.average_dim_level = HomePage::_cache_brightness_slider;
      //       status.ceiling_lights_dim_level = HomePage::_cache_brightness_slider;
      //       HomePage::_update_display();
      //     }
      //     // TODO: Update total average light with weighted values when average light is not 0
      //   }
      // } else {
      //   ESP_LOGW("HomePage", "Failed to get NSPM config when checking if panel is optimistic. Will wait for return result instead.");
      // }
    } else {
      ESP_LOGE("HomePage", "Failed to publish data while sending command to manager from 'color temperature slider'!");
    }
  } else {
    ESP_LOGE("HomePage", "Failed to pack data while sending command to manager from 'color temperature slider'!");
  }
}