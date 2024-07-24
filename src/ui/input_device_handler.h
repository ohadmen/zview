#pragma once
#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>

#include "src/geometry/mvp_mat.h"
#include "src/types/types.h"

namespace zview {

class InputDeviceHandler {
  void fillHitScreenLut();
  types::Vector3 getHitOnScreen(types::Vector3 u);

 public:
  explicit InputDeviceHandler(MVPmat *mvpP);
  void step(const std::optional<types::Vector3> &hover_point);
  InputDeviceHandler(InputDeviceHandler &&) = default;
  InputDeviceHandler(const InputDeviceHandler &) = default;
  InputDeviceHandler &operator=(InputDeviceHandler &&) = default;
  InputDeviceHandler &operator=(const InputDeviceHandler &) = default;
  ~InputDeviceHandler();

 private:
  Eigen::AngleAxisf m_clickedViewRotation;
  Eigen::Translation3f m_clickedModelTranslation;
  types::Vector3 m_click_ray;
  std::array<std::array<float, 2>, 1024> m_hit_screen_lut;
  MVPmat &m_mvp;
};

}  // namespace zview
