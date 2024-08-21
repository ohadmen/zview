#pragma once
#include "zview/types/types.h"
namespace zview {
struct ShapeUpdateVisitor {
  // operator must receive a reference to the variant object
  // NOLINTNEXTLINE[runtime/references]
  bool operator()(types::Pcl &oldObj, types::Pcl &&newObj) const;
  // NOLINTNEXTLINE[runtime/references]
  bool operator()(types::Edges &oldObj, types::Edges &&newObj) const;
  // NOLINTNEXTLINE[runtime/references]
  bool operator()(types::Mesh &oldObj, types::Mesh &&newObj) const;
};
}  // namespace zview
