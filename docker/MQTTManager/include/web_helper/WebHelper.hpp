#include <list>
#ifndef MQTT_MANAGER_WEB_HELPER
#include <string>

class WebHelper
{
public:
  /*
   * Perform a HTTP GET, POST or PUT request and collect response data.
   * @param url: The URL to perform the HTTP GET, POST or PUT request against.
   * @param response_data: Pointer to the variable to store the response data into. nullptr will result in not saving response data.
   * @param headers: Pointer to a std::list for any HTTP headers to set in the request.
   * @param post_data: Pointer to a std::string of POST/PUT data to send in the request. Setting this to nullptr will result in not saving response data.
   * returns: true if success else false.
   */
  static bool perform_get_request(std::string *url, std::string *response_data, std::list<const char *> *headers);
  static bool perform_post_request(std::string *url, std::string *response_data, std::list<const char *> *headers, std::string *post_data);
  static bool perform_put_request(std::string *url, std::string *response_data, std::list<const char *> *headers, std::string *put_data);

private:
  static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
};

#endif // !MQTT_MANAGER_WEB_HELPER
