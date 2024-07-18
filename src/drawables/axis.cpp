#include "src/params/params.h"
#include "src/drawables/axis.h"
#include "src/drawables/shape_draw_visitor.h"
#include "src/drawables/shape_init_visitor.h"

namespace zview {
Axis::Axis(MVPmat &mvp)
    : types::Mesh("grid"),m_mvp{mvp},
      m_locOnScreen{
          Eigen::Affine3f{Eigen::Translation3f(-0.90, -0.8, 0)}.matrix()} {}
bool Axis::init() {

  static constexpr std::uint8_t c{255U};

  m_v = {
    {-s,-s,-s,c,c,c,c},
    {s,-s,-s,c,0,0,c},
    {s,s,-s,c,c,0,c},
    {-s,s,-s,0,c,0,c},
    {-s,-s,s,0,0,c,c},
    {s,-s,s,c,0,c,c},
    {s,s,s,c,c,c,c},
    {-s,s,s,0,c,c,c},
      {d,0,0,c,0,0,c},
      {0,d,0,0,c,0,c},
      {0,0,d,0,0,c,c},
  };
  m_f= {//box
  {0,1,5},
  {0,5,4},
  {0,4,3},
  {3,4,7},
  //B
  {4,5,10},
  {5,6,10},
  {6,7,10},
  {7,4,10},
  //G
  {3,2,9},
  {2,6,9},
  {6,7,9},
  {7,3,9},
  //R
  {6,5,8},
  {1,5,8},
  {5,6,8},
  {6,2,8},
  };


  return ShapeInitVisitor()(*this);
}
void Axis::draw() const {
  
  Eigen::Affine3f r{m_mvp.getViewRotation()};
  const float distance = Params::i().camera_z_near+d;
  r.pretranslate(types::Vector3{0, 0, -distance});
  r.scale(distance);
  types::Matrix4x4 axisRotation = m_mvp.getProjectiveMatrix() * r.matrix();
  const types::Matrix4x4 m = m_locOnScreen * axisRotation;

  m_shader.use();
  m_shader.setUniform("u_transformation", m.data());
  ShapeDrawVisitor()(*this, nullptr);
}
} // namespace zview
