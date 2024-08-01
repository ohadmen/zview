
#include "src/io/shared_mem.h"
namespace zview {

SharedMem::SharedMem(
    const std::function<std::uint32_t(types::Shape&&)>& plot_function,
    const std::function<void(const std::uint32_t)>& erase_function)
    : m_plot{plot_function}, m_erase{erase_function} {}

}  // namespace zview
