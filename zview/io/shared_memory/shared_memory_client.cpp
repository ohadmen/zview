#include "zview/io/shared_memory/shared_memory_client.h"

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <cstring>
#include <string>
#include <vector>

#include "zview/io/shared_memory/shared_memory_types.h"
#include "zview/types/external_types.h"
#include "zview/utils/recast.h"

namespace zview {

SharedMemoryClient::SharedMemoryClient()
    : m_shm{boost::interprocess::open_only, SHARED_MEMORY_NAME,
            boost::interprocess::read_write} {
  m_region = boost::interprocess::mapped_region(
      m_shm, boost::interprocess::read_write);
}

bool SharedMemoryClient::plot(const std::string& name, const float* xyz,
                              size_t n_points, std::uint16_t dim_points,
                              const std::uint32_t* indices, size_t n_indices,
                              std::uint16_t dim_indices) {
  SharedMemoryInfo& info =
      *static_cast<SharedMemoryInfo*>(m_region.get_address());
  boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
      lock(info.mutex);
  const std::size_t available_size =
      m_region.get_size() - sizeof(SharedMemoryInfo);
  const std::size_t req_size = n_points * dim_points * sizeof(float) +
                               n_indices * dim_indices * sizeof(std::uint32_t);
  if (available_size < req_size) {
    return false;
  }
  std::fill(info.name.begin(), info.name.end(), '\0');
  std::memcpy(info.name.data(), name.data(),
              std::min(name.size(), MAX_NAME_LENGTH));
  info.type = SharedMemMessageType::ADD_PCL;
  info.n_vertices = n_points;
  info.dim_vertices = dim_points;
  info.n_indices = n_indices;
  info.dim_indices = dim_indices;
  std::uint8_t* vertices_ptr =
      std::next(static_cast<std::uint8_t*>(m_region.get_address()),
                sizeof(SharedMemoryInfo));
  std::memcpy(vertices_ptr, xyz, n_points * dim_points * sizeof(float));
  if (indices) {
    std::uint8_t* indices_ptr =
        std::next(vertices_ptr,
                  std::int64_t(n_points * ssize_t(dim_points) * sizeof(float)));
    std::memcpy(indices_ptr, indices,
                n_indices * dim_indices * sizeof(std::uint32_t));
  }
  return true;
}
bool SharedMemoryClient::removeShape(const std::string& name) {
  SharedMemoryInfo& info =
      *static_cast<SharedMemoryInfo*>(m_region.get_address());
  boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
      lock(info.mutex);
  info.type = SharedMemMessageType::REMOVE_SHAPE;
  std::fill(info.name.begin(), info.name.end(), '\0');
  std::memcpy(info.name.data(), name.data(),
              std::min(name.size(), MAX_NAME_LENGTH));
  return true;
}

}  // namespace zview