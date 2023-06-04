#ifndef HTTP_LIB_H
#define HTTP_LIB_H

#include <Arduino.h>
#include <ArduinoJson.h>

class HttpLib {
public:
  /// @brief Get the size of a remote file
  /// @param url The URL to the file to get the size of
  /// @return The size of the file or 0 if error
  static size_t GetFileSize(const char *url);
  /// @brief Download a chunk from a file to a buffer
  /// @param buffer The buffer to download into
  /// @param address The address to download the chunk from
  /// @param offset Where the chunk starts
  /// @param size The size of the chunk
  /// @return The number of bytes downloaded
  static size_t DownloadChunk(uint8_t *buffer, const char *address, size_t offset, size_t size);
  static bool GetMD5sum(const char *address, char *buffer);

  static bool DownloadJSON(const char *address, JsonDocument *document);
};

#endif
