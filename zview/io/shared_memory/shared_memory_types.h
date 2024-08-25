#pragma once

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <cstdint>

namespace zview {

struct SharedMemoryInfo {
  std::size_t read_offset{0U};
  std::size_t write_offset{0U};
  boost::interprocess::interprocess_mutex mutex;
};

enum class SharedMemMessageType : std::uint8_t {
  UNKNOWN,
  ADD_PCL,
  ADD_MESH,
  ADD_EDGE,
  REMOVE_SHAPE
};
static constexpr std::size_t MAX_NAME_LENGTH = 256;
static constexpr std::size_t REQUEST_HEADER_SIZE =
    sizeof(SharedMemMessageType) + MAX_NAME_LENGTH;

static constexpr char const* SHARED_MEMORY_NAME = "zview_channel5";

static constexpr std::size_t SHARED_MEMORY_SIZE = 1 << 29;  // 512MB

}  // namespace zview