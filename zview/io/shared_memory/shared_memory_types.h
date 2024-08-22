#pragma once

#include <cstdint>
namespace zview {

enum class SharedMemMessageType : std::uint8_t {
  UNKNOWN,
  ADD_PCL,
  ADD_MESH,
  ADD_EDGE,
  REMOVE_SHAPE
};
static constexpr std::size_t MAX_NAME_LENGTH = 256;

static constexpr char const* SHARED_MEMORY_NAME = "zview_channel5";

}  // namespace zview