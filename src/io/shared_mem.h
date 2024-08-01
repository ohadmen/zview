#pragma once
#include <functional>

#include "src/types/types.h"
namespace zview {

class SharedMem {
  std::function<std::uint32_t(types::Shape&&)> m_plot;
  std::function<void(std::uint32_t)> m_erase;

 public:
  SharedMem(const std::function<std::uint32_t(types::Shape&&)>& plot_function,
            const std::function<void(std::uint32_t)>& erase_function);
};

}  // namespace zview