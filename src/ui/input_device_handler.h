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
  // delete as field 'm_mvp' is of reference type 'zview::MVPmat &
  InputDeviceHandler(InputDeviceHandler &&) = delete;
  InputDeviceHandler(const InputDeviceHandler &) = delete;
  InputDeviceHandler &operator=(InputDeviceHandler &&) = delete;
  InputDeviceHandler &operator=(const InputDeviceHandler &) = delete;
  ~InputDeviceHandler();

 private:
  Eigen::AngleAxisf m_clickedViewRotation;
  Eigen::Translation3f m_clickedModelTranslation;
  types::Vector3 m_click_ray;
  std::array<std::array<float, 2>, 1024> m_hit_screen_lut;
  MVPmat &m_mvp;
};

}  // namespace zview
