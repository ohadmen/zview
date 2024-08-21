#pragma once
#include "zview/geometry/mvp_mat.h"
#include "zview/types/types.h"
namespace zview {
class Axis : public types::Mesh {
  const MVPmat &m_mvp;
  types::Matrix4x4 m_locOnScreen;
  static constexpr float d{0.1f};
  static constexpr float s{0.01f};

 public:
  explicit Axis(const MVPmat &mvp);
  bool init();
  void draw() const;
};
}  // namespace zview
