#pragma once
#include <esp_http_server.h>
#include <unordered_map>

class WebManager {
public:
  /**
   * @brief Start the webserver task and register all URI endpoints
   */
  static void start();

private:
  static esp_err_t _handle_uri_index(httpd_req_t *req);
  static esp_err_t _handle_uri_save_config(httpd_req_t *req);
  static esp_err_t _handle_uri_config_data(httpd_req_t *req);
  static esp_err_t _handle_uri_status_data(httpd_req_t *req);
  static esp_err_t _handle_uri_get_available_networks(httpd_req_t *req);
  static esp_err_t _handle_uri_main_css(httpd_req_t *req);

  /**
   * @brief Read a file from LittleFS and send response
   * @param req: The request to respond to
   * @param file: The file path to read from
   * @param content_type: What content type to send, for example "text/html"
   */
  static esp_err_t _read_file_and_response(httpd_req_t *req, char *file, char *content_type);

  /**
   * @brief Decode URI data from base64 encoding to chars and split into fields with associated values
   * @param source: The source string to read from
   * @param destination: The map to write the result into
   * @return ESP_OK if everything was OK, else ESP_ERR_INVALID_ARG
   */
  static esp_err_t _decode_form_data(char *source, std::unordered_map<std::string, std::string> *destination);

  // Vars
  static inline httpd_handle_t _server = NULL;
  static inline httpd_config_t _config;

  // URIs
  static inline httpd_uri_t _uri_index = {
      .uri = "/",
      .method = HTTP_GET,
      .handler = _handle_uri_index,
      .user_ctx = NULL};
  static inline httpd_uri_t _uri_save_config = {
      .uri = "/save_config",
      .method = HTTP_POST,
      .handler = _handle_uri_save_config,
      .user_ctx = NULL};
  static inline httpd_uri_t _uri_config_data = {
      .uri = "/config_data",
      .method = HTTP_GET,
      .handler = _handle_uri_config_data,
      .user_ctx = NULL};
  static inline httpd_uri_t _uri_status_data = {
      .uri = "/status_data",
      .method = HTTP_GET,
      .handler = _handle_uri_status_data,
      .user_ctx = NULL};
  static inline httpd_uri_t _uri_get_available_networks = {
      .uri = "/available_networks",
      .method = HTTP_GET,
      .handler = _handle_uri_get_available_networks,
      .user_ctx = NULL};
  static inline httpd_uri_t _uri_static_css = {
      .uri = "/static/main.css",
      .method = HTTP_GET,
      .handler = _handle_uri_main_css,
      .user_ctx = NULL};
};