#pragma once
#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>

#include "zview/drawables/statusbar.h"
#include "zview/geometry/mvp_mat.h"
#include "zview/types/types.h"

namespace zview {

class InputDeviceHandler {
  void fillHitScreenLut();
  types::Vector3 getHitOnScreen(types::Vector3 u);
  using AddShape = std::function<std::uint32_t(types::Shape &&)>;
  using RemoveShape = std::function<void(const std::string &)>;
  using StatusBarUpdate =
      std::function<void(const StatusBar::StatusField &, const std::string &)>;

 public:
  explicit InputDeviceHandler(MVPmat *mvpP, const AddShape &addShape,
                              const RemoveShape &removeShape,
                              const StatusBarUpdate &statusBarUpdate);
  void step(const std::optional<types::Vector3> &hover_point);
  // delete as field 'm_mvp' is of reference type 'zview::MVPmat &
  InputDeviceHandler(InputDeviceHandler &&) = delete;
  InputDeviceHandler(const InputDeviceHandler &) = delete;
  InputDeviceHandler &operator=(InputDeviceHandler &&) = delete;
  InputDeviceHandler &operator=(const InputDeviceHandler &) = delete;
  ~InputDeviceHandler();

 private:
  Eigen::AngleAxisf m_clickedViewRotation{};
  Eigen::Translation3f m_clickedModelTranslation{};
  types::Vector3 m_click_ray{};
  std::array<std::array<float, 2>, 1024> m_hit_screen_lut{};
  MVPmat &m_mvp;
  const AddShape m_addShape;
  const RemoveShape m_removeShape;
  const StatusBarUpdate m_statusBarUpdate;

  std::optional<types::Edges> m_measurement_edge;
};

}  // namespace zview
