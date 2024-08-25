#pragma once

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <functional>

#include "zview/types/types.h"

namespace zview {

class SharedMemoryServer {
  using AddShape = std::function<std::uint32_t(types::Shape &&)>;
  using RemoveShape = std::function<void(const std::string &)>;

  AddShape m_addShape;
  RemoveShape m_removeShape;

  boost::interprocess::shared_memory_object m_shm;
  boost::interprocess::mapped_region m_region;

  bool call_callback(std::uint8_t *&ptr);

 public:
  explicit SharedMemoryServer(const AddShape &addShape,
                              const RemoveShape &removeShape);
  SharedMemoryServer(const SharedMemoryServer &) = delete;
  SharedMemoryServer &operator=(const SharedMemoryServer &) = delete;
  SharedMemoryServer(SharedMemoryServer &&) = delete;
  SharedMemoryServer &operator=(SharedMemoryServer &&) = delete;
  ~SharedMemoryServer();

  void step();
};

}  // namespace zview