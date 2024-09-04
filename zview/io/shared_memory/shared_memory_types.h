#pragma once

#include <array>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <cstdint>

namespace zview {

enum class SharedMemMessageType : std::uint8_t {
  UNKNOWN,
  ADD_PCL,
  ADD_MESH,
  ADD_EDGE,
  REMOVE_SHAPE,
  SM_RESIZE
};
static constexpr std::size_t MAX_NAME_LENGTH = 256;
struct SharedMemoryInfo {
  boost::interprocess::interprocess_mutex mutex;
  SharedMemMessageType type{0U};
  std::array<std::uint8_t, MAX_NAME_LENGTH> name;
  size_t n_vertices{0U};
  std::uint32_t dim_vertices{
      0U};  // 3 for xyz, 4 for xyzi, 6 for xyzrgb, 7 for xyzrgba
  std::uint32_t n_indices{0U};
  std::uint32_t dim_indices{0U};  // 2 for edges, 3 for faces
};

static constexpr char const* SHARED_MEMORY_DATA_NAME = "zview_data_channel";
static constexpr char const* SHARED_MEMORY_CMD_NAME = "zview_cmd_channel";

static constexpr std::size_t SHARED_MEMORY_INIT_SIZE = 1 << 10;  // 1MB

}  // namespace zview