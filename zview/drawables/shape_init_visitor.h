#pragma once
#include "zview/types/types.h"
namespace zview {
struct ShapeInitVisitor {
  // operator must receive a reference to the variant object
  // NOLINTNEXTLINE[runtime/references]
  bool operator()(types::Pcl &obj) const;
  // NOLINTNEXTLINE[runtime/references]
  bool operator()(types::Edges &obj) const;
  // NOLINTNEXTLINE[runtime/references]
  bool operator()(types::Mesh &obj) const;
};
}  // namespace zview
