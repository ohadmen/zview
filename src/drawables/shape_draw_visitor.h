#pragma once
#include "src/types/types.h"
namespace zview {

struct ShapeDrawVisitor {
  void operator()(const types::Pcl &obj, const float *tform);
  void operator()(const types::Edges &obj, const float *tform);
  void operator()(const types::Mesh &obj, const float *tform);
};
}  // namespace zview