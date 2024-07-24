#pragma once
#include "src/types/types.h"
namespace zview {
struct ShapeInitVisitor {
  bool operator()(types::Pcl &obj);
  bool operator()(types::Edges &obj);
  bool operator()(types::Mesh &obj);
};
}  // namespace zview
