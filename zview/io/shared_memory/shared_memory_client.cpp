#include "zview/io/shared_memory/shared_memory_client.h"

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "zview/io/shared_memory/shared_memory_types.h"
#include "zview/types/external_types.h"
#include "zview/utils/recast.h"

namespace zview {

SharedMemoryClient::SharedMemoryClient()
    : m_data_shm{boost::interprocess::open_only, SHARED_MEMORY_DATA_NAME,
                 boost::interprocess::read_write},
      m_cmd_shm{boost::interprocess::open_only, SHARED_MEMORY_CMD_NAME,
                boost::interprocess::read_write},
      m_data_region{m_data_shm, boost::interprocess::read_write},
      m_cmd_region{m_cmd_shm, boost::interprocess::read_write} {}

bool SharedMemoryClient::checkResponse() {
  SharedMemoryInfo &info =
      *static_cast<SharedMemoryInfo *>(m_cmd_region.get_address());

  // wait 10 ms
  for (int i = 0; i < 20; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    {
      boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
          lock(info.mutex);
      if (info.type == SharedMemMessageType::UNKNOWN) {
        return true;
      }
    }
  }
  return false;
}
bool SharedMemoryClient::sendServerResizeRequest(std::size_t size) {
  {  // mutex scope
    SharedMemoryInfo &info =
        *static_cast<SharedMemoryInfo *>(m_cmd_region.get_address());
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
        lock(info.mutex);
    info.type = SharedMemMessageType::SM_RESIZE;
    info.n_vertices = size;
  }
  if (!checkResponse()) {
    return false;
  }
  m_data_shm.truncate(ssize_t(size));

  m_data_region = boost::interprocess::mapped_region(
      m_data_shm, boost::interprocess::read_write);
  return true;
}
bool SharedMemoryClient::plot(const std::string &name, const float *xyz,
                              size_t n_points, std::uint16_t dim_points,
                              const std::uint32_t *indices, size_t n_indices,
                              std::uint16_t dim_indices) {
  const std::size_t available_size = m_data_region.get_size();
  const std::size_t req_size = n_points * dim_points * sizeof(float) +
                               n_indices * dim_indices * sizeof(std::uint32_t);
  if (available_size < req_size) {
    bool ok = sendServerResizeRequest(req_size);
    if (!ok) {
      std::cerr << "Shared memory is too small (available: " << available_size
                << ", required: " << req_size
                << "). Server could not resize shared memory" << std::endl;
      return false;
    }
  }

  {  // mutex scope
    SharedMemoryInfo &info =
        *static_cast<SharedMemoryInfo *>(m_cmd_region.get_address());

    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
        lock(info.mutex);

    std::fill(info.name.begin(), info.name.end(), '\0');
    std::memcpy(info.name.data(), name.data(),
                std::min(name.size(), MAX_NAME_LENGTH));
    info.type = SharedMemMessageType::ADD_PCL;
    info.n_vertices = n_points;
    info.dim_vertices = dim_points;
    info.n_indices = n_indices;
    info.dim_indices = dim_indices;
    std::uint8_t *vertices_ptr =
        static_cast<std::uint8_t *>(m_data_region.get_address());
    std::memcpy(vertices_ptr, xyz, n_points * dim_points * sizeof(float));
    if (indices) {
      std::uint8_t *indices_ptr = std::next(
          vertices_ptr,
          std::int64_t(n_points * ssize_t(dim_points) * sizeof(float)));
      std::memcpy(indices_ptr, indices,
                  n_indices * dim_indices * sizeof(std::uint32_t));
    }
  }
  return checkResponse();
}
bool SharedMemoryClient::removeShape(const std::string &name) {
  {  // mutex scope
    SharedMemoryInfo &info =
        *static_cast<SharedMemoryInfo *>(m_cmd_region.get_address());
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
        lock(info.mutex);
    info.type = SharedMemMessageType::REMOVE_SHAPE;
    std::fill(info.name.begin(), info.name.end(), '\0');
    std::memcpy(info.name.data(), name.data(),
                std::min(name.size(), MAX_NAME_LENGTH));
  }
  return checkResponse();
}

}  // namespace zview