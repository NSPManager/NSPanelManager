#include <algorithm>
#include <charconv>
#include <cstdint>
#include <entity/entity.hpp>
#include <entity_manager/entity_manager.hpp>
#include <ixwebsocket/IXHttpServer.h>
#include <media_player/media_player.hpp>
#include <memory>
#include <nextion_image_server/nextion_image_server.hpp>
#include <regex>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

// Album art comes from Home Assistant integrations, keep the decoders to the formats they serve and cap the size.
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_MAX_DIMENSIONS 4096
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

void NextionImageServer::start() {
  if (NextionImageServer::_server == nullptr) {
    SPDLOG_DEBUG("Creating new ix::HttpServer for Nextion image server.");
    NextionImageServer::_server = new ix::HttpServer(8003, "0.0.0.0");
    NextionImageServer::_server->setOnConnectionCallback(&NextionImageServer::_handle_request);
  }

  auto res = NextionImageServer::_server->listen();
  if (!res.first) {
    SPDLOG_ERROR("Failed to start Nextion image server: {}", res.second);
    return;
  }
  NextionImageServer::_server->start();
  NextionImageServer::_server->wait();
}

ix::HttpResponsePtr NextionImageServer::_handle_request(ix::HttpRequestPtr request, std::shared_ptr<ix::ConnectionState> connection_state) {
  // Requests come from anything on the network, never let a bad one take down the manager.
  try {
    return NextionImageServer::_route_request(request, connection_state);
  } catch (const std::exception &e) {
    SPDLOG_ERROR("Nextion image server failed to handle {} {}: {}", request->method, request->uri, e.what());
  } catch (...) {
    SPDLOG_ERROR("Nextion image server failed to handle {} {}: unknown exception", request->method, request->uri);
  }
  return NextionImageServer::_text_response(500, "Internal Server Error", "Internal server error.");
}

ix::HttpResponsePtr NextionImageServer::_route_request(ix::HttpRequestPtr request, std::shared_ptr<ix::ConnectionState> connection_state) {
  SPDLOG_DEBUG("Nextion image server request from {}: {} {}", connection_state->getRemoteIp(), request->method, request->uri);
  if (request->method.compare("GET") != 0) {
    return NextionImageServer::_text_response(405, "Method Not Allowed", "Only GET is supported.");
  }

  std::string path = request->uri;
  std::string query;
  size_t query_start = path.find('?');
  if (query_start != std::string::npos) {
    query = path.substr(query_start + 1);
    path = path.substr(0, query_start);
  }

  static const std::regex album_art_path(R"(^/nextion-img/media_player/(\d+)/album_art$)");
  std::smatch match;
  if (std::regex_match(path, match, album_art_path)) {
    uint32_t media_player_id = 0;
    std::string id_string = match[1].str();
    auto [ptr, ec] = std::from_chars(id_string.data(), id_string.data() + id_string.size(), media_player_id);
    if (ec != std::errc()) {
      return NextionImageServer::_text_response(404, "Not Found", "Unknown media player.");
    }
    return NextionImageServer::_handle_album_art_request(media_player_id, NextionImageServer::_parse_query(query));
  }

  return NextionImageServer::_text_response(404, "Not Found", "Not found.");
}

ix::HttpResponsePtr NextionImageServer::_handle_album_art_request(uint32_t media_player_id, const std::map<std::string, std::string> &query) {
  // Read and validate the requested size and format.
  uint16_t size[2] = {NextionImageServer::default_image_dimension, NextionImageServer::default_image_dimension};
  const char *size_keys[2] = {"w", "h"};
  for (int i = 0; i < 2; i++) {
    auto it = query.find(size_keys[i]);
    if (it == query.end()) {
      continue;
    }
    uint32_t value = 0;
    auto [ptr, ec] = std::from_chars(it->second.data(), it->second.data() + it->second.size(), value);
    if (ec != std::errc() || ptr != it->second.data() + it->second.size() || value < 1 || value > NextionImageServer::max_image_dimension) {
      return NextionImageServer::_text_response(400, "Bad Request", fmt::format("{} must be between 1 and {}.", size_keys[i], NextionImageServer::max_image_dimension));
    }
    size[i] = value;
  }

  ImageFormat format = ImageFormat::RGB565;
  auto format_it = query.find("format");
  if (format_it != query.end()) {
    if (format_it->second.compare("rgb565") == 0) {
      format = ImageFormat::RGB565;
    } else if (format_it->second.compare("png") == 0) {
      format = ImageFormat::PNG;
    } else {
      return NextionImageServer::_text_response(400, "Bad Request", "format must be rgb565 or png.");
    }
  }

  auto media_player = EntityManager::get_entity_by_id<MediaPlayerEntity>(MQTT_MANAGER_ENTITY_TYPE::MEDIA_PLAYER, media_player_id);
  if (!media_player) {
    return NextionImageServer::_text_response(404, "Not Found", "Unknown media player.");
  }

  std::shared_ptr<const std::string> album_art = (*media_player)->get_album_art();
  if (!album_art) {
    // Either the media player has no album art right now or it could not be downloaded, the log tells which.
    return NextionImageServer::_text_response(404, "Not Found", "No album art available.");
  }

  std::shared_ptr<const ConvertedImage> image;
  {
    // Held while converting so that panels requesting the same image at the same time share one conversion.
    std::lock_guard<std::mutex> lock_guard(NextionImageServer::_converted_images_mutex);
    ConvertedImageKey key = {.media_player_id = media_player_id, .width = size[0], .height = size[1], .format = format};
    auto cached = NextionImageServer::_converted_images.find(key);
    if (cached != NextionImageServer::_converted_images.end() && cached->second.source == album_art) {
      image = cached->second.image;
    } else {
      std::optional<ConvertedImage> converted = NextionImageServer::_convert_image(*album_art, size[0], size[1], format);
      if (!converted) {
        return NextionImageServer::_text_response(502, "Bad Gateway", "Failed to convert album art.");
      }
      image = std::make_shared<const ConvertedImage>(std::move(*converted));
      // Entries for album art that has since changed are only replaced when the same size is requested again, start over rather than grow without bound.
      if (cached == NextionImageServer::_converted_images.end() && NextionImageServer::_converted_images.size() >= NextionImageServer::max_converted_images) {
        NextionImageServer::_converted_images.clear();
      }
      NextionImageServer::_converted_images[key] = {.source = album_art, .image = image};
    }
  }

  ix::WebSocketHttpHeaders headers;
  headers["Content-Type"] = format == ImageFormat::PNG ? "image/png" : "application/octet-stream";
  headers["X-Image-Width"] = std::to_string(image->width);
  headers["X-Image-Height"] = std::to_string(image->height);
  return std::make_shared<ix::HttpResponse>(200, "OK", ix::HttpErrorCode::Ok, headers, image->data);
}

std::optional<NextionImageServer::ConvertedImage> NextionImageServer::_convert_image(const std::string &image_data, uint16_t width, uint16_t height, ImageFormat format) {
  int source_width, source_height, source_channels;
  std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> source(
      stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(image_data.data()), image_data.size(), &source_width, &source_height, &source_channels, 3),
      &stbi_image_free);
  if (!source) {
    SPDLOG_ERROR("Failed to decode image: {}", stbi_failure_reason());
    return std::nullopt;
  }

  // Crop the largest centered area of the source with the same aspect ratio as the requested size, then scale it to fill the requested size.
  int64_t crop_width = source_width;
  int64_t crop_height = source_height;
  if (static_cast<int64_t>(source_width) * height > static_cast<int64_t>(source_height) * width) {
    crop_width = std::max<int64_t>(1, static_cast<int64_t>(source_height) * width / height);
  } else {
    crop_height = std::max<int64_t>(1, static_cast<int64_t>(source_width) * height / width);
  }
  int64_t crop_x = (source_width - crop_width) / 2;
  int64_t crop_y = (source_height - crop_height) / 2;
  const stbi_uc *crop_start = source.get() + (crop_y * source_width + crop_x) * 3;

  std::vector<stbi_uc> pixels(static_cast<size_t>(width) * height * 3);
  if (stbir_resize_uint8_srgb(crop_start, crop_width, crop_height, source_width * 3, pixels.data(), width, height, width * 3, STBIR_RGB) == nullptr) {
    SPDLOG_ERROR("Failed to resize image from {}x{} to {}x{}.", crop_width, crop_height, width, height);
    return std::nullopt;
  }

  ConvertedImage image = {.width = width, .height = height, .data = ""};
  if (format == ImageFormat::PNG) {
    auto append_to_string = [](void *context, void *data, int size) {
      static_cast<std::string *>(context)->append(static_cast<const char *>(data), size);
    };
    if (stbi_write_png_to_func(append_to_string, &image.data, width, height, 3, pixels.data(), width * 3) == 0) {
      SPDLOG_ERROR("Failed to encode image as PNG.");
      return std::nullopt;
    }
  } else {
    image.data.resize(static_cast<size_t>(width) * height * 2);
    for (size_t i = 0; i < static_cast<size_t>(width) * height; i++) {
      uint16_t red = (pixels[i * 3] * 31 + 127) / 255;
      uint16_t green = (pixels[i * 3 + 1] * 63 + 127) / 255;
      uint16_t blue = (pixels[i * 3 + 2] * 31 + 127) / 255;
      uint16_t rgb565 = (red << 11) | (green << 5) | blue;
      image.data[i * 2] = static_cast<char>(rgb565 & 0xFF);
      image.data[i * 2 + 1] = static_cast<char>(rgb565 >> 8);
    }
  }
  return image;
}

std::map<std::string, std::string> NextionImageServer::_parse_query(const std::string &query) {
  std::map<std::string, std::string> result;
  size_t start = 0;
  while (start < query.size()) {
    size_t end = query.find('&', start);
    if (end == std::string::npos) {
      end = query.size();
    }
    std::string pair = query.substr(start, end - start);
    size_t equals = pair.find('=');
    if (equals != std::string::npos) {
      result[pair.substr(0, equals)] = pair.substr(equals + 1);
    } else if (!pair.empty()) {
      result[pair] = "";
    }
    start = end + 1;
  }
  return result;
}

ix::HttpResponsePtr NextionImageServer::_text_response(int status_code, const std::string &description, const std::string &body) {
  ix::WebSocketHttpHeaders headers;
  headers["Content-Type"] = "text/plain";
  return std::make_shared<ix::HttpResponse>(status_code, description, ix::HttpErrorCode::Ok, headers, body);
}
