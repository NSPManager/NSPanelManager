#include <Arduino.h>
#include <ChunkDownloader.hpp>
#include <HTTPClient.h>
#include <HttpLib.hpp>
#include <MqttLog.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

ChunkDownloader::ChunkDownloader(std::string address, uint16_t chunk_size, uint8_t download_chunks) {
  this->_address = address;
  this->_chunk_size = chunk_size;
  this->_download_chunks = download_chunks;
  this->_current_chunk_position = 0;
  this->_total_file_size = HttpLib::GetFileSize(this->_address.c_str());
}

bool ChunkDownloader::_downloadChunks() {
  HTTPClient httpClient;
  httpClient.begin(this->_address.c_str());

  std::string rangeHeader = "bytes=";
  rangeHeader.append(std::to_string(this->_current_chunk_position));
  rangeHeader.append("-");
  rangeHeader.append(std::to_string(this->_current_chunk_position + (this->_chunk_size * this->_download_chunks)));
  httpClient.addHeader("Range", rangeHeader.c_str());
  rangeHeader.clear();

  int httpReturnCode = httpClient.GET();
  // LOG_TRACE("Downloading TFT ", rangeHeader.c_str(), " got return code: ", httpReturnCode);
  if (httpReturnCode != 200 && httpReturnCode != 206) {
    httpClient.end();
    LOG_ERROR("Failed to retrive file chunk. Got code: ", httpReturnCode);
    return false;
  }

  uint8_t num_failed_reads = 0;
  uint read_chunks = 0;
  while (read_chunks < this->_download_chunks) {
    if (!httpClient.getStreamPtr()->available()) { // No data avilable from WiFi, wait 1000ms and try again
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      num_failed_reads++;
      continue;
    }

    size_t next_read_chunk_size = this->_total_file_size - this->_current_chunk_position;
    if (next_read_chunk_size > this->_chunk_size) {
      next_read_chunk_size = this->_chunk_size;
    }
    if (next_read_chunk_size <= 0) {
      // We have reached the end of the remote file, break.
      break;
    }
    DownloadChunk *chunk = new DownloadChunk();
    chunk->data = new uint8_t[next_read_chunk_size];
    chunk->location = this->_current_chunk_position;
    uint16_t num_read_bytes = 0;
    while (num_read_bytes < next_read_chunk_size) {
      while (httpClient.getStreamPtr()->available() <= 0) {
        vTaskDelay(250 / portTICK_PERIOD_MS);
        num_failed_reads++;
        if (num_failed_reads >= 10) {
          // We have had 10 failed reads in a row for a total of 2.5 seconds, cancel this request and try again.
          this->_clearChunk(chunk);
          return false;
        }
      }
      num_read_bytes += httpClient.getStreamPtr()->readBytes(chunk->data, next_read_chunk_size);
    }
    chunk->size = num_read_bytes;
    this->_current_chunk_position += chunk->size;
    this->_chunks.push_back(chunk);
    read_chunks++;
  }
  httpClient.end();
  return true;
}

void ChunkDownloader::seek(uint32_t _seek_position) {
  this->_clearDownloadedChunks();
  this->_current_chunk_position = _seek_position;
  LOG_DEBUG("Seeking to position: ", this->_current_chunk_position);
}

uint16_t ChunkDownloader::readNextChunk(uint8_t *buffer) {
  if (this->_chunks.size() <= 0) {
    while (!this->_downloadChunks()) {
      LOG_ERROR("Failed to download any chunks, will try again.");
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
  if (this->_chunks.size() > 0) {
    DownloadChunk *chunk = this->_chunks.front();
    memcpy(buffer, chunk->data, chunk->size);
    uint16_t return_size = this->_chunks.front()->size;
    this->_chunks.erase(this->_chunks.begin());
    this->_clearChunk(chunk);
    return return_size;
  } else {
    return 0;
  }
}

void ChunkDownloader::_clearDownloadedChunks() {
  for (auto it = this->_chunks.cbegin(); it != this->_chunks.cend();) {
    DownloadChunk *chunkToRm = *it;
    it = this->_chunks.erase(it);
    this->_clearChunk(chunkToRm);
  }
}

void ChunkDownloader::_clearChunk(DownloadChunk *chunk) {
  delete chunk->data;
  delete chunk;
}

uint32_t ChunkDownloader::getCurrentChunkPosition() {
  if (this->_chunks.size() <= 0) {
    this->_downloadChunks();
  }
  if (this->_chunks.size() > 0) {
    DownloadChunk *chunk = this->_chunks.front();
    return chunk->location;
  } else {
    return 0;
  }
}

size_t ChunkDownloader::getTotalFileSize() {
  return this->_total_file_size;
}
