#include "zview/io/shared_memory/shared_memory_client.h"

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <cstring>
#include <string>
#include <vector>

#include "zview/io/shared_memory/shared_memory_types.h"
#include "zview/types/external_types.h"
#include "zview/utils/recast.h"

namespace zview {

void populateRequestHeader(const zview::SharedMemMessageType type,
                           std::string &name, std::uint8_t *ptr) {
  name.resize(MAX_NAME_LENGTH, '\0');
  *zview::recast<zview::SharedMemMessageType *>(ptr) = type;
  std::advance(ptr, sizeof(zview::SharedMemMessageType));
  std::memcpy(ptr, name.data(), zview::MAX_NAME_LENGTH);
}

SharedMemoryClient::SharedMemoryClient()
    : m_shm{boost::interprocess::open_only, SHARED_MEMORY_NAME,
            boost::interprocess::read_write} {
  m_region = boost::interprocess::mapped_region(
      m_shm, boost::interprocess::read_write);
}

bool SharedMemoryClient::addRequest(const std::uint8_t *req_data,
                                    const std::size_t req_size) {
  SharedMemoryInfo *info =
      static_cast<SharedMemoryInfo *>(m_region.get_address());
  boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
      lock(info->mutex);
  const std::size_t available_size = m_region.get_size() - info->write_offset;

  if (available_size < req_size) {
    return false;
  }

  std::memcpy(
      static_cast<std::uint8_t *>(m_region.get_address()) + info->write_offset,
      req_data, req_size);
  info->write_offset += req_size;

  return true;
}

bool SharedMemoryClient::addPcl(std::string &name, const std::size_t n_points,
                                const zview::Vertex *const vertices) {
  const auto req_size =
      REQUEST_HEADER_SIZE + sizeof(std::size_t) + n_points * sizeof(Vertex);
  std::vector<std::uint8_t> req_data(req_size);

  std::uint8_t *ptr = req_data.data();

  populateRequestHeader(SharedMemMessageType::ADD_PCL, name, ptr);
  std::advance(ptr, REQUEST_HEADER_SIZE);

  *recast<std::size_t *>(ptr) = n_points;
  std::advance(ptr, sizeof(std::size_t));
  std::memcpy(ptr, vertices, n_points * sizeof(Vertex));

  return addRequest(req_data.data(), req_size);
}

}  // namespace zview