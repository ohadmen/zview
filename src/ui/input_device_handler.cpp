#include "src/ui/input_device_handler.h"

#include <imgui.h>

#include <Eigen/Dense>
#include <algorithm>
#include <limits>

#include "src/params/params.h"

std::complex<float> gaussianFunc(const float x) {
  static constexpr float tb_sigma = 0.25F;
  static constexpr float tb_height = +0.50F;
  const float y = tb_height * expf(-0.5f * x * x / (tb_sigma * tb_sigma));
  return {x, y};
}
namespace zview {
void setTextureTypeByKeyboard() {
  const bool isCtrlPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                             ImGui::IsKeyDown(ImGuiKey_RightCtrl);
  if (!isCtrlPressed) {
    return;
  }
  if (ImGui::IsKeyPressed(ImGuiKey_1)) {
    Params::i().texture_type = 1;
  } else if (ImGui::IsKeyPressed(ImGuiKey_2)) {
    Params::i().texture_type = 2;
  } else if (ImGui::IsKeyPressed(ImGuiKey_3)) {
    Params::i().texture_type = 3;
  } else if (ImGui::IsKeyPressed(ImGuiKey_4)) {
    Params::i().texture_type = 4;
  } else if (ImGui::IsKeyPressed(ImGuiKey_5)) {
    Params::i().texture_type = 5;
  } else if (ImGui::IsKeyPressed(ImGuiKey_6)) {
    Params::i().texture_type = 6;
  } else if (ImGui::IsKeyPressed(ImGuiKey_7)) {
    Params::i().texture_type = 7;
  } else if (ImGui::IsKeyPressed(ImGuiKey_8)) {
    Params::i().texture_type = 8;
  } else if (ImGui::IsKeyPressed(ImGuiKey_9)) {
    Params::i().texture_type = 9;
  }
}
void InputDeviceHandler::fillHitScreenLut() {
  for (size_t i{0}; i < m_hit_screen_lut.size(); ++i) {
    const auto z =
        gaussianFunc(static_cast<float>(i) /
                     static_cast<float>(m_hit_screen_lut.size()) * 3.0f);
    m_hit_screen_lut.at(i) = {z.real(), z.imag()};
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
InputDeviceHandler::InputDeviceHandler(MVPmat *mvpP) : m_mvp{*mvpP} {
  fillHitScreenLut();
}
void InputDeviceHandler::step(
    const std::optional<types::Vector3> &hover_point) {
  auto &io = ImGui::GetIO();

  if (io.WantCaptureMouse) return;

  setTextureTypeByKeyboard();

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
    static constexpr auto step_scale_down = 1.0f / 1.1f;
    static constexpr auto eps = std::numeric_limits<float>::epsilon() * 1e3f;

    const float d = std::max(
        eps, m_mvp.getViewDistance() *
                 (io.MouseWheel > 0 ? step_scale_up : step_scale_down));

    m_mvp.setViewDistance(d);
  }
}

InputDeviceHandler::~InputDeviceHandler() {}
}  // namespace zview
