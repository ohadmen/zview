#pragma once
#include "src/geometry/mvp_mat.h"
#include "src/types/types.h"
namespace zview {
class Axis : public types::Mesh {
  MVPmat &m_mvp;
  types::Matrix4x4 m_locOnScreen;
    static constexpr float d{0.1f};
  static constexpr float s{0.01f};

public:
  Axis(MVPmat &mvp);
  bool init();
  void draw() const;
};
} // namespace zview