#include <cstdint>
#include <list>
#ifndef CHUNK_DOWNLOADER_HPP
#include <inttypes.h>
#include <string>

class ChunkDownloader {
public:
  struct DownloadChunk {
    uint32_t location;
    uint16_t size;
    uint8_t *data;
  };

  ChunkDownloader(std::string address, uint16_t chunk_size, uint8_t download_chunks);
  ~ChunkDownloader();
  uint32_t getCurrentChunkPosition();
  size_t getTotalFileSize();
  void seek(uint32_t chunk_location);
  uint16_t readNextChunk(uint8_t *buffer);

private:
  uint16_t _chunk_size;
  uint8_t _download_chunks;
  uint32_t _current_chunk_position;
  size_t _total_file_size;
  std::string _address;
  std::list<DownloadChunk *> _chunks;

  bool _downloadChunks();
  void _clearDownloadedChunks();
  void _clearChunk(DownloadChunk *chunk);
};

#endif // !CHUNK_DOWNLOADER_HPP
