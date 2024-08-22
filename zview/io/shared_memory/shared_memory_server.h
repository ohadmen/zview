#pragma once

#include <functional>

#include "zview/types/types.h"
namespace zview {
class SharedMemoryServer {
  using AddShape = std::function<std::uint32_t(types::Shape &&)>;
  using RemoveShape = std::function<void(const std::string &)>;

  AddShape m_addShape;
  RemoveShape m_removeShape;

  bool call_callback(std::uint8_t *ptr);

 public:
  explicit SharedMemoryServer(const AddShape &addShape,
                              const RemoveShape &removeShape);
  void step();
};

}  // namespace zview