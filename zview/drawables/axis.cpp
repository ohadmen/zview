#include "zview/drawables/axis.h"

#include <iostream>

#include "zview/drawables/shape_draw_visitor.h"
#include "zview/drawables/shape_init_visitor.h"
#include "zview/params/params.h"

namespace zview {
Axis::Axis(const MVPmat &mvp)
    : types::Mesh("mesh"),
      m_mvp{mvp},
      m_locOnScreen{
          Eigen::Affine3f{Eigen::Translation3f(-0.90, -0.8, 0)}.matrix()} {}
bool Axis::init() {
  static constexpr std::uint8_t c{255U};

  v() = {
      {-s, -s, -s, c, c, c, c}, {s, -s, -s, c, 0, 0, c}, {s, s, -s, c, c, 0, c},
      {-s, s, -s, 0, c, 0, c},  {-s, -s, s, 0, 0, c, c}, {s, -s, s, c, 0, c, c},
      {s, s, s, c, c, c, c},    {-s, s, s, 0, c, c, c},  {d, 0, 0, c, 0, 0, c},
      {0, d, 0, 0, c, 0, c},    {0, 0, d, 0, 0, c, c},
  };
  f() = {
      // box
      {0, 1, 5},
      {0, 5, 4},
      {0, 4, 3},
      {3, 4, 7},
      // B
      {4, 5, 10},
      {5, 6, 10},
      {6, 7, 10},
      {7, 4, 10},
      // G
      {3, 2, 9},
      {2, 6, 9},
      {6, 7, 9},
      {7, 3, 9},
      // R
      {6, 5, 8},
      {1, 5, 8},
      {5, 6, 8},
      {6, 2, 8},
  };

  return ShapeInitVisitor()(*this);
}
void Axis::draw() const {
  static constexpr float rad_30 = static_cast<float>(M_PI) / 180.0f * 30.0f;
  Eigen::Affine3f r{m_mvp.getViewRotation()};
  float distance = (m_mvp.getViewDistance() * Params::i().zmin_factor) / d;
  r.pretranslate(types::Vector3{0, 0, -distance});
  float zoom_scale =
      std::tan(Params::i().camera_fov_rad / 2.0f) / std::tan(rad_30);
  r.scale(distance * zoom_scale);
  types::Matrix4x4 axisRotation = m_mvp.getProjectiveMatrix() * r.matrix();
  const types::Matrix4x4 m = m_locOnScreen * axisRotation;

  shader().use();
  shader().setUniform("u_txt", 2);
  shader().setUniform("u_transformation", m.data());
  ShapeDrawVisitor()(*this);
}
}  // namespace zview
