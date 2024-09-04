#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <functional>
#include <memory>

#include "zview/types/types.h"

namespace zview {

class SharedMemoryServer {
  using AddShape = std::function<std::uint32_t(types::Shape &&)>;
  using RemoveShape = std::function<void(const std::string &)>;

  AddShape m_addShape;
  RemoveShape m_removeShape;

  std::unique_ptr<boost::interprocess::shared_memory_object> m_data_shm;
  boost::interprocess::mapped_region m_data_region;

  std::unique_ptr<boost::interprocess::shared_memory_object> m_cmd_shm;
  boost::interprocess::mapped_region m_cmd_region;

  template <typename T>
  T *getRegionAddress(size_t offset_bytes);

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