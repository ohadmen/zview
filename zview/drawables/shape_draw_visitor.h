#pragma once
#include "zview/types/types.h"
namespace zview {

struct ShapeDrawVisitor {
  void operator()(const types::Pcl &obj, const float *tform) const;
  void operator()(const types::Edges &obj, const float *tform) const;
  void operator()(const types::Mesh &obj, const float *tform) const;
};
}  // namespace zview
