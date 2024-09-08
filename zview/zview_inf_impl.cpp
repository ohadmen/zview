#include "zview/zview_inf_impl.h"

#include <ImGuiFileDialog.h>
#include <imgui.h>

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "zview/io/read_ply.h"
#include "zview/params/params.h"
#include "zview/utils/recast.h"

namespace zview {

ZviewInfImpl::ZviewInfImpl()
    : m_axis(m_mvp),
      m_idh{&m_mvp,
            std::bind(&ZviewInfImpl::plotShape, this, std::placeholders::_1),
            std::bind(&ZviewInfImpl::remove, this, std::placeholders::_1),
            std::bind(&StatusBar::update, &m_status_bar, std::placeholders::_1,
                      std::placeholders::_2)},
      m_tree_view{
          std::bind(&ShapeBuffer::shapeVisibility, &m_buffer,
                    std::placeholders::_1),
          std::bind(&ZviewInfImpl::setCameraToViewSelectedKey, this,
                    std::placeholders::_1),
          std::bind(&ShapeBuffer::erase, &m_buffer, std::placeholders::_1)},
      m_sms{std::bind(&ZviewInfImpl::plotShape, this, std::placeholders::_1),
            std::bind(&ZviewInfImpl::remove, this, std::placeholders::_1)} {}

void ZviewInfImpl::processInput() {
  const bool isCtrlPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                             ImGui::IsKeyDown(ImGuiKey_RightCtrl);

  if (!isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_P)) {
    m_show_params_menu = !m_show_params_menu;
  }
  if (!isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_G)) {
    m_show_grid = !m_show_grid;
  }
  if (!isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_L)) {
    m_show_tree = !m_show_tree;
  }

  if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_1)) {
    Params::i().texture_type = 1;
  } else if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_2)) {
    Params::i().texture_type = 2;
  } else if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_3)) {
    Params::i().texture_type = 3;
  } else if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_4)) {
    Params::i().texture_type = 4;
  } else if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_5)) {
    Params::i().texture_type = 5;
  } else if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_6)) {
    Params::i().texture_type = 6;
  }
  if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_L)) {
    static const IGFD::FileDialogConfig config{".", {}, {}, {}, {},
                                               {},  {}, {}, {}};

    ImGuiFileDialog::Instance()->OpenDialog("LoadFileDlgKey", "Choose File",
                                            ".ply", config);
  }
  if (isCtrlPressed && ImGui::IsKeyPressed(ImGuiKey_S)) {
    static const IGFD::FileDialogConfig config{".", {}, {}, {}, {},
                                               {},  {}, {}, {}};

    ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Choose File",
                                            ".ply", config);
  }

  // display
  if (ImGuiFileDialog::Instance()->Display(
          "LoadFileDlgKey", ImGuiWindowFlags_Modal, ImVec2(700, 310))) {
    if (ImGuiFileDialog::Instance()->IsOk()) {  // action if OK
      std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
      loadFiles({filePathName});
    }
    // close
    ImGuiFileDialog::Instance()->Close();
  }
  if (ImGuiFileDialog::Instance()->Display(
          "SaveFileDlgKey", ImGuiWindowFlags_Modal, ImVec2(700, 310))) {
    if (ImGuiFileDialog::Instance()->IsOk()) {  // action if OK
      std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

      m_buffer.writeBufferToFile({filePathName});
    }
    // close
    ImGuiFileDialog::Instance()->Close();
  }
}

bool ZviewInfImpl::init() {
  if (!m_backdrop.init(Params::i().background_color)) {
    std::cerr << "Failed to init backdrop" << std::endl;
    return false;
  }
  if (!m_grid.init()) {
    std::cerr << "Failed to init grid" << std::endl;
    return false;
  }
  if (!m_axis.init()) {
    std::cerr << "Failed to init axis" << std::endl;
    return false;
  }

  return true;
}

void ZviewInfImpl::loadFiles(const std::vector<std::string> &files) {
  for (const auto &f : files) {
    // temporary object to read the ply file
    try {
      auto shape_vector = io::read_ply(f);

      for (types::Shape &s : shape_vector) {
        plotShape(std::move(s));
      }
    } catch (const std::exception &e) {
      std::cerr << e.what() << '\n';
    }
  }

  setCameraToViewSelectedKey({});
}
bool ZviewInfImpl::winResize(const ImVec2 &wh) {
  if (wh.x == m_mvp.getWinSize().x && wh.y == m_mvp.getWinSize().y) {
    return true;
  }
  m_mvp.setWinSize(wh);
  std::array<int, 2> whint = {static_cast<int>(wh.x), static_cast<int>(wh.y)};
  if (!m_picking.resize(whint)) {
    std::cerr << "picking texture init failed" << std::endl;
    return false;
  }
  if (!m_fbo.resize(whint)) {
    std::cerr << "frame buffer init failed" << std::endl;
    return false;
  }

  return true;
}

void ZviewInfImpl::setCameraToViewSelectedKey(
    const std::vector<std::uint32_t> &keys) {
  auto bbox = m_buffer.getBbox(keys);
  if (bbox.min() == bbox.max()) {
    return;
  }
  const auto obj_center = (bbox.max() + bbox.min()) / 2.0F;
  const auto verts = bbox.getVertices();

  m_mvp.setModelTranslation(Eigen::Translation3f(-obj_center));
  float step = (bbox.max() - bbox.min()).minCoeff() / 10;

  m_mvp.setViewDistance(step);
  for (int i{0}; i < 100; ++i) {
    bool all_inside = true;
    const auto mvp = m_mvp.getMVPmatrix();
    for (const auto &v : verts) {
      Eigen::Vector4f v1 = mvp * v.homogeneous();
      v1 /= v1.w();
      if (std::abs(v1.x()) > 1 || std::abs(v1.y()) > 1) {
        all_inside = false;
        break;
      }
    }
    if (all_inside) {
      break;
    }
    m_mvp.setViewDistance(m_mvp.getViewDistance() + step);
  }
}

bool ZviewInfImpl::draw() {
  processInput();

  if (m_show_tree) {
    m_tree_view.draw(&m_show_tree);
  }
  const auto sz = ImGui::GetContentRegionAvail();

  const auto transformation = m_mvp.getMVPmatrix();
  drawHelpMenu();
  drawParamsMenu();
  m_status_bar.draw(sz);
  updateStatusBar();

  if (!winResize(sz)) {
    return false;
  }

  if (ImGui::IsWindowHovered()) {
    m_idh.step(m_hover_point);
  }
  m_sms.step();

  m_fbo.bind();
  renderPhase(transformation);
  m_fbo.unbind();
  m_hover_point = pickingPhase(transformation);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::Image(
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast,performance-no-int-to-ptr)
      reinterpret_cast<void *>(m_fbo.txt()),
      ImVec2{static_cast<float>(sz[0]), static_cast<float>(sz[1])},
      ImVec2(0, 1), ImVec2{1, 0});
  ImGui::PopStyleVar();

  return true;
}

void ZviewInfImpl::renderPhase(const types::Matrix4x4 &mvp) const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_backdrop.draw();
  if (m_show_grid) {
    m_grid.draw(mvp, m_mvp.getModelTranslation().translation(),
                m_mvp.getViewDistance());
  }
  m_axis.draw();

  m_buffer.draw(mvp.data());
}
std::optional<types::Vector3> ZviewInfImpl::pickingPhase(
    const types::Matrix4x4 &mvp) {
  m_picking.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_picking.setTransform(mvp);
  auto &picking = m_picking;
  // QUESTION: In m_buffer::draw we iterate over all shapes and call the
  // preDrawFunction for each. Doesn't this mean that the object index in
  // m_picking is set to the last object in the buffer? What even is the purpose
  // of this function? Is this such that when we have overlapping shapes, the
  // object index refers to the last shape drawn? If so, where in setObjectIndex
  // do we use the dimension of the shape? This is what I was imagining how the
  // mask in picking looks like if we have two overlapping shapes:
  //
  //   ..................       ......yyyy........      ......yyyy........
  //   ...xxxxx..........       ......yyyy........      ...xxxyyyy........
  //   ...xxxxx..........   +   ......yyyy........  =   ...xxxyyyy........
  //   ...xxxxx..........       ..................      ...xxxxx..........
  //   ...xxxxx..........       ..................      ...xxxxx..........
  //   ..................       ..................      ..................
  //
  // Where x is the first shape and y is the second shape. Based on the
  // resulting mask we would then know which shape is being hovered over.
  const auto preDrawFunction =
      [&picking](const std::pair<std::uint32_t, types::Shape> &s) {
        picking.setObjectIndex(s.first);
      };
  m_buffer.draw(nullptr, preDrawFunction);
  m_picking.unbind();
  // get mouse position relative to the current window, reducing the window
  // offset
  // QUESTION: What is the purpose of the 10, 10 offset?
  const auto mouse_rel =
      ImGui::GetMousePos() - ImGui::GetWindowPos() - ImVec2{10, 10};

  const auto pix = m_picking.readPixel(static_cast<int>(mouse_rel.x),
                                       static_cast<int>(mouse_rel.y));

  if (pix.valid == 1) {
    const auto r = m_mvp.getRay(mouse_rel, MVPmat::CoordinateSystem::GLOBAL);

    auto pt = m_buffer.get3dLocation(pix.object_id, pix.prim_id, r);

    return pt;
  }

  return {};
}
void ZviewInfImpl::updateStatusBar() {
  {
    std::stringstream ss;
    ss << "FPS: " << std::fixed << std::setprecision(1)
       << ImGui::GetIO().Framerate;
    m_status_bar.update(StatusBar::StatusField::FPS, ss.str());
  }
  {
    std::stringstream ss;
    ss << "[";
    if (m_hover_point.has_value()) {
      // set float point to fixed width
      ss << std::fixed << std::setprecision(3) << std::setfill(' ')
         << std::setw(8) << m_hover_point.value().x() << ",";
      ss << std::fixed << std::setprecision(3) << std::setfill(' ')
         << std::setw(8) << m_hover_point.value().y() << ",";
      ss << std::fixed << std::setprecision(3) << std::setfill(' ')
         << std::setw(8) << m_hover_point.value().z();

    } else {
      ss << "  No point under cursor   ";
    }
    ss << "]";
    m_status_bar.update(StatusBar::StatusField::OBJECT_END_CURSOR, ss.str());
  }
}

void ZviewInfImpl::drawHelpMenu() {
  if (ImGui::IsKeyPressed(ImGuiKey_H)) {
    m_show_help_menu = !m_show_help_menu;
  }
  if (!m_show_help_menu) {
    return;
  }
  ImGui::OpenPopup("Help");
  if (ImGui::BeginPopupModal("Help", &m_show_help_menu,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text(" H - open this menu");
    ImGui::Text(" P - show/hide  parameters menu");
    ImGui::Text(" L - show/hide  layers view");
    ImGui::Text(" G - show/hide  grid");
    ImGui::Text(" D - start/stop  distance measurement");
    ImGui::Text(" CTRL+S - save current buffer to file to .ply file");
    ImGui::Text(" CTRL+L - load .ply file");
    ImGui::Text(" CTRL+1 - set the texture type to 1");
    ImGui::Text(" CTRL+2 - set the texture type to 2");
    ImGui::Text(" CTRL+3 - set the texture type to 3");
    ImGui::Text(" CTRL+4 - set the texture type to 4");
    ImGui::Text(" CTRL+5 - set the texture type to 5");
    ImGui::Text(" CTRL+6 - set the texture type to 6");
    if (ImGui::Button("Close", ImVec2(120, 0))) {
      m_show_help_menu = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}
void ZviewInfImpl::drawParamsMenu() {
  // render your GUI
  if (!m_show_params_menu) {
    return;
  }
  ImGui::Begin(
      "Parameters", &m_show_params_menu,
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing);
  auto &params = zview::Params::i();

  static constexpr float deg2rad = static_cast<float>(M_PI) / 180.0f;
  float cam_fov_deg = params.camera_fov_rad / deg2rad;

  if (ImGui::SliderFloat("Field of view", &cam_fov_deg, 10, 90)) {
    params.camera_fov_rad = cam_fov_deg * deg2rad;
    m_mvp.updatePmat();
  }

  if (ImGui::SliderInt("background color", &params.background_color, 0, 3)) {
    m_backdrop.init(params.background_color);
  }
  ImGui::SliderInt("Texture type", &params.texture_type, 1, 6);
  ImGui::SliderFloat("Point size", &params.point_size, 0.01, 2.0);

  ImGui::End();
}
std::uint32_t ZviewInfImpl::plotShape(types::Shape &&shape) {
  const auto shape_name =
      std::visit([](const auto &v) { return v.getName(); }, shape);

  const auto key = m_buffer.emplace(std::move(shape));
  if (key == 0) {
    // only update
    return 0;
  }
  m_tree_view.push(shape_name, key);
  return key;
}

void ZviewInfImpl::plot(const std::string &name,
                        std::vector<Vertex> &&vertices) {
  types::Pcl pcl{name};
  pcl.v() = std::move(*recast<std::vector<types::VertData> *>(&vertices));
  plotShape(pcl);
}
void ZviewInfImpl::plot(const std::string &name, std::vector<Vertex> &&vertices,
                        std::vector<Face> &&faces) {
  types::Mesh mesh{name};
  mesh.v() = std::move(*recast<std::vector<types::VertData> *>(&vertices));
  // Face and types::FaceIndx are the same type and have the same memory layout.
  // To save copying, we can reinterpret_cast the vector of faces to a vector of
  // types::FaceIndx NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  mesh.f() = std::move(*recast<std::vector<types::FaceIndx> *>(&faces));
  plotShape(mesh);
}
void ZviewInfImpl::plot(const std::string &name, std::vector<Vertex> &&vertices,
                        std::vector<Edge> &&edges) {
  types::Edges graph{name};
  // Vertex and types::VertData are the same type and have the same memory
  // layout. To save copying, we can reinterpret_cast the vector of vertices to
  // a vector of types::VertData
  graph.v() = std::move(*recast<std::vector<types::VertData> *>(&vertices));
  // Edge and types::EdgeIndx are the same type and have the same memory layout.
  // To save copying, we can reinterpret_cast the vector of edges to a vector of
  // types::EdgeIndx NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  graph.e() = std::move(*recast<std::vector<types::EdgeIndx> *>(&edges));
  plotShape(graph);
}

void ZviewInfImpl::remove(const std::string &name) {
  if (name.empty()) {
    // delete all
    m_tree_view.removeAll();
  } else {
    std::uint32_t key = m_buffer.getKey(name);
    if (key != 0) {
      m_tree_view.remove(key);
    }
  }
}
}  // namespace zview
