#ifndef MQTTMANAGER_NEXTION_IMAGE_SERVER_HPP
#define MQTTMANAGER_NEXTION_IMAGE_SERVER_HPP

#include <cstdint>
#include <ixwebsocket/IXHttpServer.h>
#include <map>
#include <optional>
#include <string>

/**
 * HTTP server that serves images to the NSPanels, converted into a format the panel can draw.
 * nginx forwards /nextion-img/ on the manager to this server.
 *
 * Endpoints:
 *   GET /nextion-img/media_player/<id>/album_art[?w=<width>&h=<height>&format=<rgb565|png>]
 *     Album art for the media player, scaled and center cropped to fill w x h (default 100 x 100).
 *     format=rgb565 (default): raw pixels, row by row from the top left, 2 bytes per pixel,
 *                              little endian RGB565. The size is in the X-Image-Width/X-Image-Height headers.
 *     format=png:              the same image as a PNG, to check the conversion in a browser.
 */
class NextionImageServer {
public:
  /**
   * Start the HTTP server on 0.0.0.0:8003 and block while it is running.
   */
  static void start();

private:
  enum class ImageFormat {
    RGB565,
    PNG,
  };

  struct ConvertedImage {
    uint16_t width;
    uint16_t height;
    std::string data;
  };

  /**
   * Handle an HTTP request to the server.
   */
  static ix::HttpResponsePtr _handle_request(ix::HttpRequestPtr request, std::shared_ptr<ix::ConnectionState> connection_state);

  /**
   * Handle a request for media player album art.
   */
  static ix::HttpResponsePtr _handle_album_art_request(uint32_t media_player_id, const std::map<std::string, std::string> &query);

  /**
   * Decode an image (JPEG or PNG), scale and center crop it to fill width x height and encode it in the given format.
   * @return The converted image or std::nullopt if the image could not be decoded or encoded.
   */
  static std::optional<ConvertedImage> _convert_image(const std::string &image_data, uint16_t width, uint16_t height, ImageFormat format);

  /**
   * Parse the query string of a URI ("a=1&b=2") into a map.
   */
  static std::map<std::string, std::string> _parse_query(const std::string &query);

  /**
   * Create a plain text HTTP response.
   */
  static ix::HttpResponsePtr _text_response(int status_code, const std::string &description, const std::string &body);

  static inline ix::HttpServer *_server;

  // Largest image size that can be requested. The NSPanel display is 480x320 in either orientation.
  static constexpr uint16_t max_image_dimension = 480;
  static constexpr uint16_t default_image_dimension = 100;
};

#endif
