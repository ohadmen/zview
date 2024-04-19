#include "input_device_handler.h"


#include <Eigen/Dense>
#include <iostream>
#include "src/params/params.h"
namespace zview {
types::Vector3 InputDeviceHandler::getHitOnScreen(const ImVec2 &xy,
                                                  const float tb_radius) {
  /*
  hit area is represented by two function:
  za(x) = sqrt(r*r-x*x)
  zb(x) = a/(x+b) -1
  the switch between the function happens on x0=alpha*r
  a and b are calculated s.t. zb(x) passes through (x0,z0) and that the
  derivative in this point is equal
  */

  static const float switch_p = 0.75;
  float tbradius2 = tb_radius * tb_radius;
  assert(tbradius2 < 1.0);
  float x0 = tb_radius * switch_p;
  float y0 = std::sqrt(tbradius2 - x0 * x0);
  float parama = y0 * y0 * y0 / x0;
  float paramb = y0 * y0 / x0 - x0;

  auto hitonscreen = m_vp_mat.xy2screen(xy[0],xy[1]);

  float xyr2 =
      hitonscreen.x() * hitonscreen.x() + hitonscreen.y() * hitonscreen.y();
  float xyr = std::sqrt(xyr2);
  if (xyr >= x0)
    // use za(x)
    hitonscreen[2] = parama / (xyr + paramb) - 1;
  else
    // use za(x)
    hitonscreen[2] = sqrt(tbradius2 - xyr2) - 1;
  // substract (0,0,-1) to get the vector
  hitonscreen[2] += 1;
  return hitonscreen;
}
InputDeviceHandler::InputDeviceHandler() {}
void InputDeviceHandler::step() {
  auto &io = ImGui::GetIO();
  if (io.MouseClicked[0]) {
    m_clickViewMatrix = m_vp_mat.getViewMatrix();
  }
  if (io.MouseDown[0]) {
    {
      // rotating
      const auto &tbradius = Params::i().trackball_radius;
      auto hitClick =
          getHitOnScreen(io.MouseClickedPos[0], tbradius).normalized();
      auto hitnew = getHitOnScreen(io.MousePos, tbradius);
      float angleScale = hitnew.norm() / tbradius;
      hitnew.normalize();
      const auto axis = hitnew.cross(hitClick).normalized();

      float phi = -std::acos(hitnew.dot(hitClick)) * angleScale;
      
      auto m = Eigen::AngleAxis(phi, axis);
    //   m(0, 3) = m(0, 2);
    //   m(1, 3) = m(1, 2);
    //   m(2, 3) = m(2, 2) - 1;
      m_vp_mat.setViewMatrix(m * m_clickViewMatrix);

      auto res= m_vp_mat.getVPmatrix()*types::Vector4{0,0,-0.25,1};
      std::cout << "phi: " << phi << " res"<<  std::endl << res<<  std::endl;
    }
  }
}
types::Matrix4x4 InputDeviceHandler::getVPmatrix()  const
{
    return m_vp_mat.getVPmatrix();

}
void InputDeviceHandler::setWinSize(int w, int h) { m_vp_mat.setWinSize(w, h); }

InputDeviceHandler::~InputDeviceHandler() {}
} // namespace zview