#include "zview/io/shared_memory/shared_memory_server.h"

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <cstring>
#include <iostream>  // Add this line
#include <iterator>

#include "pointer_shape_conversion.h"
#include "zview/io/shared_memory/shared_memory_types.h"
#include "zview/utils/recast.h"
namespace zview {

SharedMemoryServer::SharedMemoryServer(const AddShape &addShape,
                                       const RemoveShape &removeShape)
    : m_addShape{addShape}, m_removeShape{removeShape} {
  if (SHARED_MEMORY_SIZE <= sizeof(SharedMemoryInfo)) {
    throw std::runtime_error("Shared memory size too small");
  }

  if (boost::interprocess::shared_memory_object::remove(SHARED_MEMORY_NAME)) {
    std::cout << "Removing shared memory" << std::endl;
  }
  m_shm = std::make_unique<boost::interprocess::shared_memory_object>(
      boost::interprocess::create_only, SHARED_MEMORY_NAME,
      boost::interprocess::read_write);

  m_shm->truncate(SHARED_MEMORY_SIZE);
  m_region = boost::interprocess::mapped_region(
      *m_shm, boost::interprocess::read_write);
  std::memset(m_region.get_address(), 0, m_region.get_size());

  SharedMemoryInfo &info =
      *static_cast<SharedMemoryInfo *>(m_region.get_address());
  info.type = SharedMemMessageType::UNKNOWN;
}

template <typename T>
T *SharedMemoryServer::getRegionAddress(size_t offset_bytes) {
  return recast<T *>(
      std::next(static_cast<std::uint8_t *>(m_region.get_address()),
                ssize_t(offset_bytes + sizeof(SharedMemoryInfo))));
}

SharedMemoryServer::~SharedMemoryServer() {
  boost::interprocess::shared_memory_object::remove(SHARED_MEMORY_NAME);
}

void SharedMemoryServer::step() {
  SharedMemoryInfo &info =
      *static_cast<SharedMemoryInfo *>(m_region.get_address());

  boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
      lock(info.mutex);

  switch (info.type) {
    case SharedMemMessageType::ADD_PCL: {
      zview::types::Pcl pcl{std::string(info.name.begin(), info.name.end())};
      auto vertices_ptr = getRegionAddress<const float>(0);
      pcl.v() = ptr2vertData(vertices_ptr, info.n_vertices, info.dim_vertices);
      m_addShape(std::move(pcl));
      break;
    }
    case SharedMemMessageType::ADD_MESH: {
      zview::types::Mesh mesh{std::string(info.name.begin(), info.name.end())};
      auto vertices_ptr = getRegionAddress<const float>(0);
      mesh.v() = ptr2vertData(vertices_ptr, info.n_vertices, info.dim_vertices);
      auto indices_ptr = getRegionAddress<const std::uint32_t>(
          info.n_vertices * info.dim_vertices * sizeof(float));
      mesh.f() = ptr2indices<3>(indices_ptr, info.n_indices);
      m_addShape(std::move(mesh));
      break;
    }
    case SharedMemMessageType::ADD_EDGE: {
      zview::types::Edges edges{
          std::string(info.name.begin(), info.name.end())};
      auto vertices_ptr = getRegionAddress<const float>(0);
      edges.v() =
          ptr2vertData(vertices_ptr, info.n_vertices, info.dim_vertices);
      auto indices_ptr = getRegionAddress<const std::uint32_t>(
          info.n_vertices * info.dim_vertices * sizeof(float));
      edges.e() = ptr2indices<2>(indices_ptr, info.n_indices);
      m_addShape(std::move(edges));
      break;
    }
    case SharedMemMessageType::REMOVE_SHAPE:
      m_removeShape(std::string(info.name.begin(), info.name.end()));
      break;
    default:
      break;
  }
  info.type = SharedMemMessageType::UNKNOWN;
}

}  // namespace zview