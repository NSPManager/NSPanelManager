#pragma once
#include <cstdint>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <esp_err.h>
#include <esp_event.h>
#include <freertos/semphr.h>

class Nextion {
public:
  /**
   * Initialize the Nextion display and gather any facts about it.
   */
  static esp_err_t init();

  /**
   * @brief Go to the specified page
   * @param page: The name of the Nextion page to go to.
   * @param mutex_timeout: Timeout (in ms) of how long to wait for UART mutex
   * @return ESP_OK or ESP_ERR_TIMEOUT
   */
  static esp_err_t go_to_page(const char *page, uint16_t mutex_timeout);

  /**
   * @brief Send command to set Nextion display in "sleep mode"
   */
  static void sleep();

  /**
   * @brief Send command to wake Nextion display from "sleep mode"
   */
  static void wake();

  /**
   * @brief Set the text on the given component
   * @param component_id: The component to change
   * @param text: What text is to be displayed
   * @param mutex_timeout: Timeout (in ms) of how long to wait for UART mutex
   * @return ESP_OK or ESP_ERR_TIMEOUT
   */
  static esp_err_t set_component_text(const char *component_id, const char *text, uint16_t mutex_timeout);

  /**
   * @brief Set the integer value of the given component
   * @param component_id: The component to change
   * @param value: The new value for the component
   * @param mutex_timeout: Timeout (in ms) of how long to wait for UART mutex
   * @return ESP_OK or ESP_ERR_TIMEOUT
   */
  static esp_err_t set_component_value(const char *component_id, int16_t value, uint16_t mutex_timeout);

  /**
   * @brief Set the timeout value of the given timer, ie. how long should the timer run before triggering
   * @param timer_id: The timer to change
   * @param value: The new timeout for the timer
   * @param mutex_timeout: Timeout (in ms) of how long to wait for UART mutex
   * @return ESP_OK or ESP_ERR_TIMEOUT
   */
  static esp_err_t set_timer_value(const char *timer_id, uint16_t value, uint16_t mutex_timeout);

  /**
   * @brief Set the picture value of the given component (ie. the "pic" field in Nextion editor)
   * @param component_id: The component to change
   * @param value: The new value for the component
   * @param mutex_timeout: Timeout (in ms) of how long to wait for UART mutex
   * @return ESP_OK or ESP_ERR_TIMEOUT
   */
  static esp_err_t set_component_pic(const char *component_id, uint8_t value, uint16_t mutex_timeout);

  /**
   * @brief Set the second picture value of the given component (ie. the "pic1" field in Nextion editor)
   * @param component_id: The component to change
   * @param value: The new value for the component
   * @param mutex_timeout: Timeout (in ms) of how long to wait for UART mutex
   * @return ESP_OK or ESP_ERR_TIMEOUT
   */
  static esp_err_t set_component_pic1(const char *component_id, uint8_t value, uint16_t mutex_timeout);

  /**
   * @brief Set the component foreground color
   * @param component_id: The component to change
   * @param value: The new foreground color
   * @param mutex_timeout: Timeout (in ms) of how long to wait for UART mutex
   * @return ESP_OK or ESP_ERR_TIMEOUT
   */
  static esp_err_t set_component_foreground(const char *component_id, uint16_t value, uint16_t mutex_timeout);

  /**
   * @brief Set a component visibility
   * @param component_id: The component to show or hide
   * @param visible: "true" for shown, "false" for hidden
   * @param mutex_timeout: Timeout (in ms) of how long to wait for UART mutex
   * @return ESP_OK or ESP_ERR_TIMEOUT
   */
  static esp_err_t set_component_visibility(const char *component_id, bool visible, uint16_t mutex_timeout);

  /**
   * Request the current value of a component on the Nextion display
   * @param component_id: The ID of the component to request a value from
   * @param result: Pointer to where to save the value
   * @param timeout: The amount of time to wait for an answer in ms
   * @param mutex_timeout: Timeout (in ms) of how long to wait for UART mutex
   * @return: Return ESP_ERR_TIMEOUT if no value was returned within the given timeout
   */
  static esp_err_t get_component_integer_value(const char *component_id, int32_t *result, uint16_t timeout, uint16_t mutex_timeout);

  /**
   * Restart the Nextion display via GPIO power management
   */
  static void restart();

private:
  // Handle all events from UART
  static void _task_uart_event(void *param);

  // Process UART events
  static void _uart_data_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

  /**
   *  @brief Wait for a specific event to occur
   * @param event_base: The event base to listen for events from
   * @param event_id: The specific ID within the event base to wait for.
   * @param timeout: The amount of time to wait for event to happen, timeout in ms.
   * @param mutex_timeout: The amount of time to wait for mutex to be taken, timeout in ms.
   * @return ESP_OK on success, ESP_ERR_TIMEOUT if event did not occur within specified timeout or failed to take mutex.
   */
  static esp_err_t _wait_for_event(esp_event_base_t event_base, int32_t event_id, uint16_t timeout, uint16_t mutex_timeout);

  /**
   *  @brief When waiting for events, this is the function that will handle and trigger the event is done.
   */
  static void _wait_for_event_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

  /**
   * @brief Will attempt to write data onto serial.
   * @return ESP_OK on success, otherwise return ESP_ERR_NOT_FINISHED.
   */
  static esp_err_t _write_command(char *data);

  // VARIABLES:
  // Event loop handle for custom events that happens from UART read -> handle mqtt message
  static inline esp_event_loop_handle_t _handle_uart_data_event_loop;

  // Loop task arguments for custom event loop for UARt data
  static inline esp_event_loop_args_t _handle_uart_data_event_loop_args;

  // Mutex to only allow one task at the time to write to UART
  static inline SemaphoreHandle_t _uart_write_mutex;

  // Mutex to only allow one task at the time to wait for events
  static inline SemaphoreHandle_t _event_wait_mutex;

  // The Task that is waiting for an event to happen
  static inline TaskHandle_t _event_wait_task;

  // Mutex to only allow one task at the time to access the variables for "event_wait_integer"
  static inline SemaphoreHandle_t _event_wait_integer_mutex;

  // The task that requested an integer value from the Nextion display and is currently waiting
  static inline TaskHandle_t _event_wait_integer;

  // The value that was received from the display
  static inline int32_t *_event_wait_integer_result;

  // Configuration used to initialize uart
  static inline uart_config_t _uart_config;

  // Event queue for uart events
  static inline QueueHandle_t _uart_event_queue;

  // When starting the Nextion display, did we receive the "NSPM" flag indicating that our NSPanel Manager firmware is running on the Nextion display?
  static inline bool _has_received_nspm_flag;
};