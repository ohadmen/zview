#pragma once
#include "src/types/types.h"
namespace zview {
class Grid {
  types::Edges m_minor;
  types::Edges m_major;

 public:
  Grid();
  bool init();
  void draw(const types::Matrix4x4& mvp, const types::Vector3& model_loc,
            const float d) const;
};
}  // namespace zview
