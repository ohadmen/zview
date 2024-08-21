#include "zview/zview_inf.h"

#include <memory>

#include "zview/zview_inf_impl.h"

namespace zview {
std::shared_ptr<ZviewInf> ZviewInf::create() {
  return std::make_shared<ZviewInfImpl>();
}

}  // namespace zview
