#pragma once
#include "zview/types/types.h"
#include "zview/geometry/mvp_mat.h"
namespace zview {

struct ShapeDrawVisitor {
  void operator()(const types::Pcl &obj,   const std::optional<MVPmat>& mvp=std::nullopt) const;
  void operator()(const types::Edges &obj, const std::optional<MVPmat>& mvp=std::nullopt) const;
  void operator()(const types::Mesh &obj,  const std::optional<MVPmat>& mvp=std::nullopt) const;
};
}  // namespace zview
