#include "zview/io/shared_memory/shared_memory_server.h"

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

bool SharedMemoryServer::call_callback(std::uint8_t *ptr) {
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

  std::cout << "Processed message" << std::endl;
  return true;
}

SharedMemoryServer::SharedMemoryServer(const AddShape &addShape,
                                       const RemoveShape &removeShape)
    : m_addShape{addShape}, m_removeShape{removeShape} {}

void SharedMemoryServer::step() {}
}  // namespace zview