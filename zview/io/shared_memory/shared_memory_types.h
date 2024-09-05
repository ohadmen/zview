#pragma once

#include <array>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <cstdint>
#include <memory>

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
static constexpr char const *SHARED_MEMORY_DATA_NAME = "zview_data_channel";
static constexpr char const *SHARED_MEMORY_CMD_NAME = "zview_cmd_channel";
static constexpr std::size_t SHARED_MEMORY_INIT_SIZE = 1 << 20;  // 1MB

struct SharedMemoryInfo {
  boost::interprocess::interprocess_mutex mutex;
  SharedMemMessageType type{0U};
  std::array<char, MAX_NAME_LENGTH> name;
  size_t n_vertices{0U};
  std::uint32_t dim_vertices{
      0U};  // 3 for xyz, 4 for xyzi, 6 for xyzrgb, 7 for xyzrgba
  std::uint32_t n_indices{0U};
  std::uint32_t dim_indices{0U};  // 2 for edges, 3 for faces
};

class SharedMemoryHandler {
  bool m_is_server;
  std::unique_ptr<boost::interprocess::shared_memory_object> m_data_shm;
  boost::interprocess::mapped_region m_data_region;

  std::unique_ptr<boost::interprocess::shared_memory_object> m_cmd_shm;
  boost::interprocess::mapped_region m_cmd_region;

 public:
  explicit SharedMemoryHandler(bool is_server);

  template <typename T>
  T *getDataPtr(size_t offset_bytes) {
    return recast<T *>(
        std::next(static_cast<std::uint8_t *>(m_data_region.get_address()),
                  ssize_t(offset_bytes)));
  }

  SharedMemoryInfo &getCmdInfo();
  void resizeData(size_t size);

  std::size_t getAvailableSize() const;

  SharedMemoryHandler(const SharedMemoryHandler &) = delete;
  SharedMemoryHandler &operator=(const SharedMemoryHandler &) = delete;
  SharedMemoryHandler(SharedMemoryHandler &&) = delete;
  SharedMemoryHandler &operator=(SharedMemoryHandler &&) = delete;
  ~SharedMemoryHandler();
};

}  // namespace zview