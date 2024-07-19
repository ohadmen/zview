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
  /*
  @brief get the window size
  @return the window size
  */
  std::array<int, 2> getWinSize() const;
  /*
  @brief move the camera to view the selected keys
  @param keys the keys to view
  */
  void setCameraToViewSelectedKey(const std::vector<std::uint32_t>& keys);
  /*
  @brief render the scene
  @param mvp the model view projection matrix
  */
  void renderPhase(const types::Matrix4x4 &mvp) const;
  /*
  @brief perform the picking phase
  @param mvp the model view projection matrix
  @return the 3d point that was picked (if any)
  */
  std::optional<types::Vector3> pickingPhase(const types::Matrix4x4 &mvp) ;
  /*
  @brief draw the parameters menu
  */
  void drawParamsMenu();
  /*
  @brief draw the status bar
  */
  void drawStatusBar();
  
  

public:
  MainApp();
  bool init();
  void loadFiles(const std::vector<std::string> &files);
  bool winResize(const std::array<int, 2> &wh);
  void loop();
};
} // namespace zview