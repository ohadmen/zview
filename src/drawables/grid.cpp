#include "src/drawables/grid.h"

#include <iostream>

#include "src/drawables/shape_draw_visitor.h"
#include "src/drawables/shape_init_visitor.h"
#include "src/params/params.h"
namespace zview {
Grid::Grid() : m_minor("minor"), m_major{"major"} {}
bool Grid::init() {
  static constexpr float max_z_far = 1e4;
  static constexpr float max_fov_deg = 90;

  static const float q =
      std::pow(10.0f, 0.85f * std::log10(100 * max_z_far)) / 100 * 0.1f;
  static const float fp = max_z_far * tanf(max_fov_deg / 2 * M_PI / 180.0F) / q;
  static const float stride = 1.0f;
  static constexpr uint8_t col{255U};
  static constexpr uint8_t alpha{50};
  static constexpr uint8_t major_alpha{100};

  int n_gridlines = static_cast<int>(fp / stride);
  // minor
  for (int i{-n_gridlines}; i <= n_gridlines; ++i) {
    float x = static_cast<float>(i) * stride;
    m_minor.v().push_back({x, -fp, 0, col, col, col, alpha});
    m_minor.v().push_back({x, fp, 0, col, col, col, alpha});
  }
  for (int i{-n_gridlines}; i <= n_gridlines; ++i) {
    float y = static_cast<float>(i) * stride;
    m_minor.v().push_back({-fp, y, 0, col, col, col, alpha});
    m_minor.v().push_back({fp, y, 0, col, col, col, alpha});
  }
  for (unsigned int i{0}; i < m_minor.v().size(); i += 2) {
    m_minor.e().push_back({i, i + 1});
  }
  // major - only main axes
  m_major.v().push_back({-fp, 0, 0, col, col, col, major_alpha});
  m_major.v().push_back({fp, 0, 0, col, col, col, major_alpha});
  m_major.v().push_back({0, -fp, 0, col, col, col, major_alpha});
  m_major.v().push_back({0, fp, 0, col, col, col, major_alpha});
  m_major.e().push_back({0, 1});
  m_major.e().push_back({2, 3});

  return ShapeInitVisitor()(m_minor) && ShapeInitVisitor()(m_major);
}
void Grid::draw(const types::Matrix4x4 &mvp, const types::Vector3 &model_loc,
                const float d) const {
  static constexpr float fov_factor = 0.25f;
  const float camfov = std::tan(Params::i().camera_fov_rad / 2.0f) * d * 2;
  float q = std::pow(10.0f, std::floor(std::log10(camfov * fov_factor)));
  // minor
  {
    types::Vector3 shift{std::floor(-model_loc.x() / q) * q,
                         std::floor(-model_loc.y() / q) * q, 0};

    Eigen::Affine3f m{Eigen::Matrix4f::Identity()};

    m.scale(q);
    m.pretranslate(shift);

    const types::Matrix4x4 mvp_grid = mvp * m.matrix();

    m_minor.shader().use();
    m_minor.shader().setUniform("u_transformation", mvp_grid.data());
    ShapeDrawVisitor()(m_minor, nullptr);
  }
  // major
  {
    Eigen::Affine3f m{Eigen::Matrix4f::Identity()};
    m.scale(q);

    const types::Matrix4x4 mvp_grid = mvp * m.matrix();
    m_major.shader().use();
    m_major.shader().setUniform("u_transformation", mvp_grid.data());
    ShapeDrawVisitor()(m_major, nullptr);
  }
}
}  // namespace zview
