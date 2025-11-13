#include "WebHelper.hpp"
#include <boost/exception/diagnostic_information.hpp>
#include <curl/curl.h>
#include <spdlog/spdlog.h>

size_t WebHelper::write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

bool WebHelper::perform_get_request(std::string *url, std::string *response_data, std::list<const char *> *headers)
{
  try
  {
    SPDLOG_TRACE("Performing CURL HTTP GET request to '{}'.", url->c_str());

    CURL *curl = curl_easy_init();
    CURLcode res;

    if (!curl)
    {
      SPDLOG_ERROR("Failed to create curl object!");
      return false;
    }

    // Place to store any errors that might occur within CURL itself.
    char curl_error_buffer[CURL_ERROR_SIZE];
    curl_error_buffer[0] = 0;

    curl_easy_setopt(curl, CURLOPT_URL, url->c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5); // Wait max 5 seconds for an answer
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error_buffer);
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 10000000); // Set max buffer
    // During cross-compilation the default path for CA certificates is not set and therefore any
    // request to an https endpoint (for example OpenMetoe weather service) fails.
    curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");

    // Build header list
    struct curl_slist *curl_headers = NULL;
    if (headers != nullptr)
    {
      for (auto it = headers->cbegin(); it != headers->cend(); it++)
      {
        SPDLOG_TRACE("Appending header '{}'.", (*it));
        curl_headers = curl_slist_append(curl_headers, (*it));
      }
      if (curl_headers != NULL)
      {
        SPDLOG_TRACE("Header list built succesfully. Setting CURLOPT_HTTPHEADER.");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
      }
      else
      {
        SPDLOG_ERROR("Failed to build curl_headers.");
        curl_easy_cleanup(curl);
        curl_slist_free_all(curl_headers);
        return false;
      }
    }

    if (response_data != nullptr)
    {
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WebHelper::write_callback);
    }

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    long http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    /* Check for errors */
    if (res != CURLE_OK || http_code != 200)
    {
      SPDLOG_ERROR("Failed to perform HTTP request to '{}'. Got response data: '{}'", url->c_str(), response_data != nullptr ? response_data->c_str() : "");
      SPDLOG_ERROR("curl_easy_perform() failed, got code: {}. Text interpretation: {}", (int)res, curl_easy_strerror(res));
      SPDLOG_ERROR("Curl error buffer: {}", curl_error_buffer);

      curl_easy_cleanup(curl);
      curl_slist_free_all(curl_headers);
      return false;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(curl_headers);
    return true;
  }
  catch (const std::exception &e)
  {
    SPDLOG_ERROR("Caught exception when trying to register NSPanel: {}", boost::diagnostic_information(e, true));
  }
  return false;
}

bool WebHelper::perform_put_request(std::string *url, std::string *response_data, std::list<const char *> *headers, std::string *put_data)
{
  try
  {
    SPDLOG_TRACE("Performing CURL HTTP PUT request to '{}'.", url->c_str());

    CURL *curl = curl_easy_init();
    CURLcode res;

    if (!curl)
    {
      SPDLOG_ERROR("Failed to create curl object!");
      return false;
    }

    // Place to store any errors that might occur within CURL itself.
    char curl_error_buffer[CURL_ERROR_SIZE];
    curl_error_buffer[0] = 0;

    curl_easy_setopt(curl, CURLOPT_URL, url->c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5); // Wait max 5 seconds for an answer
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error_buffer);
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 10000000); // Set max buffer
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); // Set HTTP method to PUT
    // During cross-compilation the default path for CA certificates is not set and therefore any
    // request to an https endpoint (for example OpenMetoe weather service) fails.
    curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");

    // Build header list
    struct curl_slist *curl_headers = NULL;
    if (headers != nullptr)
    {
      for (auto it = headers->cbegin(); it != headers->cend(); it++)
      {
        SPDLOG_TRACE("Appending header '{}'.", (*it));
        curl_headers = curl_slist_append(curl_headers, (*it));
      }
      if (curl_headers != NULL)
      {
        SPDLOG_TRACE("Header list built succesfully. Setting CURLOPT_HTTPHEADER.");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
      }
      else
      {
        SPDLOG_ERROR("Failed to build curl_headers.");
        curl_easy_cleanup(curl);
        curl_slist_free_all(curl_headers);
        return false;
      }
    }

    if (response_data != nullptr)
    {
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WebHelper::write_callback);
    }

    if (put_data != nullptr)
    {
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, put_data->c_str());
    }

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    long http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    /* Check for errors */
    if (res != CURLE_OK || http_code != 200)
    {
      SPDLOG_ERROR("Failed to perform HTTP request to '{}'. Got response data: '{}'", url->c_str(), response_data->c_str());
      SPDLOG_ERROR("curl_easy_perform() failed, got code: {}. Text interpretation: {}", (int)res, curl_easy_strerror(res));
      SPDLOG_ERROR("Curl error buffer: {}", curl_error_buffer);

      curl_easy_cleanup(curl);
      curl_slist_free_all(curl_headers);
      return false;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(curl_headers);
    return true;
  }
  catch (const std::exception &e)
  {
    SPDLOG_ERROR("Caught exception when trying to register NSPanel: {}", boost::diagnostic_information(e, true));
  }
  return false;
}

bool WebHelper::perform_post_request(std::string *url, std::string *response_data, std::list<const char *> *headers, std::string *post_data)
{
  try
  {
    SPDLOG_TRACE("Performing CURL HTTP POST request to '{}'.", url->c_str());

    CURL *curl = curl_easy_init();
    CURLcode res;

    if (!curl)
    {
      SPDLOG_ERROR("Failed to create curl object!");
      return false;
    }

    // Place to store any errors that might occur within CURL itself.
    char curl_error_buffer[CURL_ERROR_SIZE];
    curl_error_buffer[0] = 0;

    curl_easy_setopt(curl, CURLOPT_URL, url->c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5); // Wait max 5 seconds for an answer
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error_buffer);
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 10000000); // Set max buffer
    // During cross-compilation the default path for CA certificates is not set and therefore any
    // request to an https endpoint (for example OpenMetoe weather service) fails.
    curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt");

    // Build header list
    struct curl_slist *curl_headers = NULL;
    if (headers != nullptr)
    {
      for (auto it = headers->cbegin(); it != headers->cend(); it++)
      {
        SPDLOG_TRACE("Appending header '{}'.", (*it));
        curl_headers = curl_slist_append(curl_headers, (*it));
      }
      if (curl_headers != NULL)
      {
        SPDLOG_TRACE("Header list built succesfully. Setting CURLOPT_HTTPHEADER.");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
      }
      else
      {
        SPDLOG_ERROR("Failed to build curl_headers.");
        curl_easy_cleanup(curl);
        curl_slist_free_all(curl_headers);
        return false;
      }
    }

    if (response_data != nullptr)
    {
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_data);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WebHelper::write_callback);
    }

    if (post_data != nullptr)
    {
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data->c_str());
    }

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    long http_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    /* Check for errors */
    if (res != CURLE_OK || http_code != 200)
    {
      SPDLOG_ERROR("Failed to perform HTTP request to '{}'. Got response data: '{}'", url->c_str(), response_data->c_str());
      SPDLOG_ERROR("curl_easy_perform() failed, got code: {}. Text interpretation: {}", (int)res, curl_easy_strerror(res));
      SPDLOG_ERROR("Curl error buffer: {}", curl_error_buffer);

      curl_easy_cleanup(curl);
      curl_slist_free_all(curl_headers);
      return false;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(curl_headers);
    return true;
  }
  catch (const std::exception &e)
  {
    SPDLOG_ERROR("Caught exception when trying to register NSPanel: {}", boost::diagnostic_information(e, true));
  }
  return false;
}
