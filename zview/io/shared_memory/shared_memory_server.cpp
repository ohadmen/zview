#include "zview/io/shared_memory/shared_memory_server.h"

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <cstring>
#include <iostream>  // Add this line
#include <iterator>

#include "zview/io/shared_memory/shared_memory_types.h"
#include "zview/utils/recast.h"

namespace zview {

template <typename T>
void getAndAdvance(std::uint8_t *&ptr, std::vector<T> &vals) {
  const std::size_t sz = *recast<std::size_t *>(ptr);
  std::advance(ptr, sizeof(std::size_t));
  vals.resize(sz);
  std::memcpy(vals.data(), ptr, sz * sizeof(T));
  std::advance(ptr, sz * sizeof(T));
}

bool SharedMemoryServer::call_callback(std::uint8_t *&ptr) {
  const auto type = *recast<zview::SharedMemMessageType *>(ptr);
  std::advance(ptr, sizeof(zview::SharedMemMessageType));
  const std::string name(recast<const char *>(ptr), MAX_NAME_LENGTH);
  std::advance(ptr, MAX_NAME_LENGTH);

  switch (type) {
    case zview::SharedMemMessageType::ADD_PCL: {
      auto obj = zview::types::Pcl{name};
      getAndAdvance(ptr, obj.v());
      m_addShape(obj);
      break;
    }
    case zview::SharedMemMessageType::ADD_MESH: {
      auto obj = zview::types::Mesh{name};
      getAndAdvance(ptr, obj.v());
      getAndAdvance(ptr, obj.f());
      m_addShape(obj);
      break;

      break;
    }
    case zview::SharedMemMessageType::ADD_EDGE: {
      auto obj = zview::types::Edges{name};
      getAndAdvance(ptr, obj.v());
      getAndAdvance(ptr, obj.e());
      m_addShape(obj);
      break;
    }
    case zview::SharedMemMessageType::REMOVE_SHAPE: {
      const std::string name(recast<const char *>(ptr), MAX_NAME_LENGTH);
      m_removeShape(name);
      break;
    }
    default:
      std::cerr << "Unknown type" << std::endl;
      return false;
  }

  return true;
}

SharedMemoryServer::SharedMemoryServer(const AddShape &addShape,
                                       const RemoveShape &removeShape)
    : m_addShape{addShape},
      m_removeShape{removeShape},
      m_shm{boost::interprocess::create_only, SHARED_MEMORY_NAME,
            boost::interprocess::read_write} {
  if (SHARED_MEMORY_SIZE <= sizeof(SharedMemoryInfo)) {
    throw std::runtime_error("Shared memory size too small");
  }

  m_shm.truncate(SHARED_MEMORY_SIZE);
  m_region = boost::interprocess::mapped_region(
      m_shm, boost::interprocess::read_write);
  std::memset(m_region.get_address(), 0, m_region.get_size());

  SharedMemoryInfo *info =
      static_cast<SharedMemoryInfo *>(m_region.get_address());

  info->read_offset = sizeof(SharedMemoryInfo);
  info->write_offset = info->read_offset;
}

SharedMemoryServer::~SharedMemoryServer() {
  boost::interprocess::shared_memory_object::remove(SHARED_MEMORY_NAME);
}

void SharedMemoryServer::step() {
  SharedMemoryInfo *info =
      static_cast<SharedMemoryInfo *>(m_region.get_address());

  boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
      lock(info->mutex);
  auto ptr =
      static_cast<std::uint8_t *>(m_region.get_address()) + info->read_offset;

  if (*ptr == 0) {
    return;
  }

  if (call_callback(ptr)) {
    info->read_offset =
        std::distance(static_cast<std::uint8_t *>(m_region.get_address()), ptr);
  } else {
    std::uint8_t *end = static_cast<std::uint8_t *>(m_region.get_address()) +
                        m_region.get_size();
    std::memset(ptr, 0, std::distance(ptr, end));

    info->write_offset = info->read_offset;
  }
}

}  // namespace zview