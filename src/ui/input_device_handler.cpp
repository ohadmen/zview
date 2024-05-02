#include "input_device_handler.h"

#include "imgui.h"
#include "src/params/params.h"
#include <Eigen/Dense>
#include <iostream>

std::complex<float> gaussianFunc(const float x) {
  static constexpr float tb_sigma = 0.5F;
  static constexpr float tb_height = +0.50F;
  const float y = tb_height * std::exp(-0.5 * x * x / (tb_sigma * tb_sigma));
  return {x, y};
}
namespace zview {
void InputDeviceHandler::fillHitScreenLut() {

  for (size_t i{0}; i < m_hit_screen_lut.size(); ++i) {
    const auto z = gaussianFunc(float(i) / m_hit_screen_lut.size() * 3.0);
    m_hit_screen_lut[i] = {z.real(), z.imag()};
  }
}
types::Vector3 InputDeviceHandler::getHitOnScreen(types::Vector3 u) {

  u.normalize();
  const auto u_xy = std::sqrt(u.x() * u.x() + u.y() * u.y());

  auto it = std::find_if(m_hit_screen_lut.begin(), m_hit_screen_lut.end(),
                         [&u_xy](const auto &c) { return c[0] > u_xy; });
  if (it == m_hit_screen_lut.end()) {
    return types::Vector3(0, 0, 0);
  }
  u.z() = (*it)[1];
  u.normalize();
  return u;

  // }
}
InputDeviceHandler::InputDeviceHandler(MVPmat &mvp) : m_mvp{mvp} {
  fillHitScreenLut();
}
void InputDeviceHandler::step(
    const std::optional<types::Vector3> &hover_point) {
  auto &io = ImGui::GetIO();

  if (io.WantCaptureMouse)
    return;

  if (io.MouseClicked[0] || io.MouseClicked[1]) {
    m_clickedViewRotation = m_mvp.getViewRotation();
    m_clickedModelTranslation = m_mvp.getModelTranslation();
    m_click_ray =
        m_mvp.getRay(io.MousePos, MVPmat::CoordinateSystem::SCREEN)[1];
  }
  if (io.MouseDoubleClicked[0]) {
    if (hover_point) {

      m_mvp.setModelTranslation(Eigen::Translation3f(-hover_point.value()));
      m_clickedModelTranslation = m_mvp.getModelTranslation();
    }
    // std::cout << "double click" <<m_vp_mat.getViewMatrix().translation()<<
    // std::endl;

  } else if (io.MouseDown[0]) {
    {
      // rotating
      const auto clickHit_normalized = getHitOnScreen(m_click_ray);
      const auto ray =
          m_mvp.getRay(io.MousePos, MVPmat::CoordinateSystem::SCREEN)[1];
      auto hitnew = getHitOnScreen(ray);

      const auto axis = hitnew.cross(clickHit_normalized).normalized();
      const float cos_phi = std::min(hitnew.dot(clickHit_normalized), 1.0F);
      const float phi = -std::acos(cos_phi);

      const auto m = Eigen::AngleAxis(phi, axis);
      // rotate around the object center, so we need to shift,rotate,shift back

      const auto t = m * m_clickedViewRotation;

      m_mvp.setViewRotation(t);
      // std::cout << "rotating" <<m_vp_mat.getViewMatrix().translation()<<
      // std::endl;
    }
  } else if (io.MouseDown[1]) {
    // //translating
    const float d = m_mvp.getViewDistance();
    auto hitnew =
        m_mvp.getRay(io.MousePos, MVPmat::CoordinateSystem::SCREEN)[1];

    const auto delta = m_clickedViewRotation.inverse() *
                       (-hitnew / hitnew.z() + m_click_ray / m_click_ray.z()) *
                       d;
    m_mvp.setModelTranslation(Eigen::Translation3f(delta) *
                              m_clickedModelTranslation);

  } else if (io.MouseWheel != 0) {

    static constexpr auto step_scale_up = 1.1f;
    static constexpr auto step_scale_down = 1.0f/1.1f;
    static constexpr auto eps = std::numeric_limits<float>::epsilon() * 1e3f;

    const float d = std::max(eps, m_mvp.getViewDistance() *
                                      (io.MouseWheel>0 ? step_scale_up : step_scale_down));

    m_mvp.setViewDistance(d);
  }
}

void InputDeviceHandler::setCameraToViewAll(types::Bbox3d bbox) {

  const auto obj_center = (bbox.max() + bbox.min()) / 2.0F;
  m_mvp.setModelTranslation(Eigen::Translation3f(-obj_center));
  auto vm = m_mvp.getViewRotation();
  const auto t = (vm * Eigen::Translation3f(-obj_center));
  bbox.applyTransform(t);

  const auto tan_h_fov_x = std::tan(Params::i().camera_fov_rad / 2);
  const auto tan_h_fov_y = tan_h_fov_x / m_mvp.getAspect();
  const auto req_distance_x =
      std::max(std::abs(bbox.max().x()), std::abs(bbox.min().x())) /
      tan_h_fov_x;
  const auto req_distance_y =
      std::max(std::abs(bbox.max().y()), std::abs(bbox.min().y())) /
      tan_h_fov_y;
  const auto req_distance = std::max(req_distance_x, req_distance_y);
  m_mvp.setViewDistance(req_distance);
}

InputDeviceHandler::~InputDeviceHandler() {}
} // namespace zview