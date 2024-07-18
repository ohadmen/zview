#pragma once
#include "imgui.h"
#include "src/drawables/axis.h"
#include "src/geometry/mvp_mat.h"
#include "src/drawables/backdrop.h"
#include "src/drawables/shape_buffer.h"
#include "src/ui/input_device_handler.h"
#include "src/drawables/grid.h"
#include "src/ui/tree_view.h"
#include <GL/glew.h> // Initialize with glewInit()
namespace zview {
class MainApp {
  MVPmat m_mvp;
  ShapeBuffer m_buffer;
  PickingTexture m_picking;
  Backdrop m_backdrop;
  Grid m_grid;
  Axis m_axis;
  InputDeviceHandler m_idh;
  TreeView m_tree_view;
  GLFWwindow *m_window;
  std::optional<types::Vector3> m_hover_point;

  std::array<int, 2> getWinSize() const;
  void renderPhase(const types::Matrix4x4 &mvp) const;
  std::optional<types::Vector3> pickingPhase(const types::Matrix4x4 &mvp) ;

  void drawParamsMenu();
  void drawStatusBar();
  
  

public:
  MainApp();
  bool init();
  void loadFiles(const std::vector<std::string> &files);
  bool winResize(const std::array<int, 2> &wh);
  void loop();
};
} // namespace zview