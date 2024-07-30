#pragma once

#include "src/drawables/axis.h"
#include "src/drawables/backdrop.h"
#include "src/drawables/grid.h"
#include "src/drawables/picking_texture.h"
#include "src/drawables/shape_buffer.h"
#include "src/geometry/mvp_mat.h"
#include "src/types/types.h"
#include "src/ui/input_device_handler.h"
#include "src/ui/tree_view.h"

namespace zview {
class ZviewMainApp {
  MVPmat m_mvp;
  ShapeBuffer m_buffer;
  PickingTexture m_picking;
  Backdrop m_backdrop;
  Grid m_grid;
  Axis m_axis;
  InputDeviceHandler m_idh;
  TreeView m_tree_view;

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
  void setCameraToViewSelectedKey(const std::vector<std::uint32_t> &keys);
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
  std::optional<types::Vector3> pickingPhase(const types::Matrix4x4 &mvp);
  /*
  @brief draw the parameters menu
  */
  void drawParamsMenu();
  /*
  @brief draw the status bar
  */
  void drawStatusBar();

 public:
  ZviewMainApp();

  bool init(const std::array<int, 2> &win_sz_wh);
  void loadFiles(const std::vector<std::string> &files);
  bool winResize(const std::array<int, 2> &wh);
  bool draw(const std::array<int, 2> &win_sz_wh);
  std::uint32_t plot(types::Shape &&shape);
};
}  // namespace zview