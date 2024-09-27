#pragma once
#include "zview/geometry/mvp_mat.h"
#include "zview/types/types.h"
namespace zview {
class Grid {
  types::Edges m_minor;
  Shader m_shader;

 public:
  Grid();
  bool init();
  void draw(const MVPmat& mvp) const;
};
}  // namespace zview
