#pragma once

#include "zview/drawables/axis.h"
#include "zview/drawables/backdrop.h"
#include "zview/drawables/grid.h"
#include "zview/drawables/picking_texture.h"
#include "zview/drawables/shape_buffer.h"
#include "zview/drawables/statusbar.h"
#include "zview/geometry/mvp_mat.h"
#include "zview/graphics_backend/frame_buffer.h"
#include "zview/io/shared_memory/shared_memory_server.h"
#include "zview/types/types.h"
#include "zview/ui/input_device_handler.h"
#include "zview/ui/tree_view.h"
#include "zview/zview_inf.h"

namespace zview {
class ZviewInfImpl : public ZviewInf {
  MVPmat m_mvp;
  ShapeBuffer m_buffer;
  FrameBuffer m_fbo{FrameBuffer::TextureType::RGBA8};
  PickingTexture m_picking;
  Backdrop m_backdrop;
  Grid m_grid;
  Axis m_axis;
  StatusBar m_status_bar;
  InputDeviceHandler m_idh;
  TreeView m_tree_view;
  SharedMemoryServer m_sms;
  bool m_show_params_menu{false};
  bool m_show_tree{false};
  bool m_show_help_menu{false};
  bool m_show_grid{true};

  std::optional<types::Vector3> m_hover_point;

  std::array<int, 2> getWinSize() const;

  void setCameraToViewSelectedKey(const std::vector<std::uint32_t> &keys);

  void renderPhase(const types::Matrix4x4 &mvp) const;

  std::optional<types::Vector3> pickingPhase(const types::Matrix4x4 &mvp);

  void drawParamsMenu();

  void updateStatusBar();

  void drawHelpMenu();

  void processInput();

  bool winResize(const ImVec2 &wh);

  std::uint32_t plotShape(types::Shape &&shape);

 public:
  ZviewInfImpl();

  bool init() override;
  bool draw() override;
  void loadFiles(const std::vector<std::string> &files) override;
  void plot(const std::string &name, std::vector<Vertex> &&vertices,
            std::vector<Face> &&faces) override;
  void plot(const std::string &name, std::vector<Vertex> &&vertices) override;
  void plot(const std::string &name, std::vector<Vertex> &&vertices,
            std::vector<Edge> &&edges) override;
  void remove(const std::string &name) override;
};
}  // namespace zview