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
    : m_addShape{addShape}, m_removeShape{removeShape} {}

SharedMemoryServer::~SharedMemoryServer() {}

void SharedMemoryServer::step() {
  SharedMemoryInfo &info = m_handler.getCmdInfo();

  if (info.type == SharedMemMessageType::UNKNOWN) {
    return;
  }

  // no need to lock as the client is waiting for the server to finish

  switch (info.type) {
    case SharedMemMessageType::ADD_PCL: {
      zview::types::Pcl pcl{std::string(info.name.begin())};
      auto vertices_ptr = m_handler.getDataPtr<const float>(0);
      pcl.v() = ptr2vertData(vertices_ptr, info.n_vertices, info.dim_vertices);
      m_addShape(std::move(pcl));

      break;
    }
    case SharedMemMessageType::ADD_MESH: {
      zview::types::Mesh mesh{std::string(info.name.begin())};
      auto vertices_ptr = m_handler.getDataPtr<const float>(0);
      mesh.v() = ptr2vertData(vertices_ptr, info.n_vertices, info.dim_vertices);
      auto indices_ptr = m_handler.getDataPtr<const std::uint32_t>(
          info.n_vertices * info.dim_vertices * sizeof(float));
      mesh.f() = ptr2indices<3>(indices_ptr, info.n_indices);

      m_addShape(std::move(mesh));
      break;
    }
    case SharedMemMessageType::ADD_EDGE: {
      zview::types::Edges edges{std::string(info.name.begin())};
      auto vertices_ptr = m_handler.getDataPtr<const float>(0);
      edges.v() =
          ptr2vertData(vertices_ptr, info.n_vertices, info.dim_vertices);
      auto indices_ptr = m_handler.getDataPtr<const std::uint32_t>(
          info.n_vertices * info.dim_vertices * sizeof(float));
      edges.e() = ptr2indices<2>(indices_ptr, info.n_indices);
      m_addShape(std::move(edges));
      break;
    }
    case SharedMemMessageType::REMOVE_SHAPE: {
      m_removeShape(std::string(info.name.begin()));
      break;
    }
    case SharedMemMessageType::SM_RESIZE: {
      std::size_t target_size = info.n_vertices;
      if (target_size > (1 << 29)) {
        std::cerr << "Shared memory resize request too large: " << target_size
                  << std::endl
                  << std::flush;
        break;
      }
      m_handler.resizeData(target_size);
      break;
    }
    default:

      break;
  }

  boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>
      lock(info.mutex);
  info.type = SharedMemMessageType::UNKNOWN;
}

}  // namespace zview