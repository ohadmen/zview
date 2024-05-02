#include "src/drawables/grid.h"
#include "src/drawables/shape_draw_visitor.h"
#include "src/drawables/shape_init_visitor.h"
#include "src/params/params.h"
#include <iostream>
namespace zview {
Grid::Grid() : types::Edges("grid") {}
bool Grid::init() {

  static const float max_z_far = 1e4;
  static const float max_fov_deg = 90;

  static const float q =
      float(std::pow(10, int(0.85 * std::log10(100 * max_z_far)))) / 100 * 0.1;
  static const float fp = max_z_far * tan(max_fov_deg / 2 * M_PIf / 180.0F) / q;
  static const float stride = 1.0f;
  static uint8_t col = 255;
  static uint8_t alpha = 50;
  for (float x{-fp}; x <= fp; x += stride) {
    m_v.push_back({x, -fp, 0, col, col, col, alpha});
    m_v.push_back({x, fp, 0, col, col, col, alpha});
  }
  for (float y{-fp}; y <= fp; y += stride) {
    m_v.push_back({-fp, y, 0, col, col, col, alpha});
    m_v.push_back({fp, y, 0, col, col, col, alpha});
  }
  for (unsigned int i{0}; i < m_v.size(); i += 2) {
    m_e.push_back({i, i + 1});
  }

  return ShapeInitVisitor()(*this);
}
void Grid::draw(const types::Matrix4x4 &mvp,
                const types::Vector3& model_loc,const float d) const {
  
  static constexpr float fov_factor = 0.25f;
  const float camfov = std::tan(Params::i().camera_fov_rad / 2.0f) * d * 2;
  float q = std::pow(10.0f, std::floor(std::log10(camfov * fov_factor)));
  types::Vector3 shift{std::floor(-model_loc.x() / q) * q,
                       std::floor(-model_loc.y() / q) * q, 0};
  
  Eigen::Affine3f m{Eigen::Matrix4f::Identity()};
  std::cout << "q: "<<q << " d: " << d  << std::endl;
  m.scale(q);
  m.pretranslate(shift);

  const types::Matrix4x4 mvp_grid = mvp * m.matrix();

  m_shader.use();
  m_shader.setUniform("u_transformation", mvp_grid.data());
  ShapeDrawVisitor()(*this, nullptr);
}
} // namespace zview
