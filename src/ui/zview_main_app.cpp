#include "src/ui/zview_main_app.h"

#include <ImGuiFileDialog.h>
#include <imgui.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "src/io/read_ply.h"
#include "src/params/params.h"

namespace zview {

ZviewMainApp::ZviewMainApp()
    : m_axis(m_mvp),
      m_idh{&m_mvp, std::bind(&ZviewMainApp::plot, this, std::placeholders::_1),
            std::bind(&ZviewMainApp::remove, this, std::placeholders::_1)},
      m_tree_view{
          std::bind(&ShapeBuffer::shapeVisibility, &m_buffer,
                    std::placeholders::_1),
          std::bind(&ZviewMainApp::setCameraToViewSelectedKey, this,
                    std::placeholders::_1),
          std::bind(&ShapeBuffer::erase, &m_buffer, std::placeholders::_1)} {}

void ZviewMainApp::processInput() {
  const bool isCtrlPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) ||
                             ImGui::IsKeyDown(ImGuiKey_RightCtrl);

  if (ImGui::IsKeyPressed(ImGuiKey_P)) {
    m_show_params_menu = !m_show_params_menu;
  }
  if (ImGui::IsKeyPressed(ImGuiKey_G)) {
    m_show_grid = !m_show_grid;
  }
  if (ImGui::IsKeyPressed(ImGuiKey_L)) {
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

bool ZviewMainApp::init(const std::array<int, 2> &win_sz_wh) {
  if (!winResize(win_sz_wh)) {
    std::cerr << "Failed to resize window" << std::endl;
    return false;
  }

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
  // {
  //     types::Mesh example("triangle");
  //   example.v() = {types::VertData{10.0f, 0, 0.0f, 255, 0, 0, 255},
  //                  types::VertData{0, 10, 0.0f, 0, 255, 0, 128},
  //                  types::VertData{0.0f, -10.0f, 0.0f, 0, 0, 255, 0}};
  //   example.f() = {types::FaceIndx{0, 1, 2}};
  //   m_buffer.push(example);

  // }

  return true;
}

void ZviewMainApp::loadFiles(const std::vector<std::string> &files) {
  for (const auto &f : files) {
    // temporary object to read the ply file
    try {
      auto shape_vector = io::read_ply(f);

      for (types::Shape &s : shape_vector) {
        plot(std::move(s));
      }
    } catch (const std::exception &e) {
      std::cerr << e.what() << '\n';
    }
  }

  setCameraToViewSelectedKey({});
}
bool ZviewMainApp::winResize(const std::array<int, 2> &wh) {
  if (wh == m_mvp.getWinSize()) {
    return true;
  }
  m_mvp.setWinSize(wh);
  if (!m_picking.init(wh)) {
    std::cerr << "picking texture init failed" << std::endl;
    return false;
  }
  if (!m_fbo.init(wh)) {
    std::cerr << "frame buffer init failed" << std::endl;
    return false;
  }

  return true;
}

void ZviewMainApp::setCameraToViewSelectedKey(
    const std::vector<std::uint32_t> &keys) {
  auto vm = m_mvp.getViewRotation();
  auto bbox = m_buffer.getBbox(keys);
  const auto obj_center = (bbox.max() + bbox.min()) / 2.0F;
  m_mvp.setModelTranslation(Eigen::Translation3f(-obj_center));
  bbox.applyTransform(vm);

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

bool ZviewMainApp::draw(const std::array<int, 2> &win_sz_wh) {
  processInput();

  if (m_show_tree) {
    m_tree_view.draw(&m_show_tree);
  }

  const auto transformation = m_mvp.getMVPmatrix();
  drawHelpMenu();
  drawParamsMenu();
  drawStatusBar();
  ImGui::Begin("window", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_NoFocusOnAppearing);
  ImGui::SetWindowPos(ImVec2(0, 0));
  ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);
  if (ImGui::IsWindowHovered()) {
    m_idh.step(m_hover_point);
  }

  auto sz = ImGui::GetContentRegionAvail();

  sz[0] = std::max(1.0f, sz[0]);
  sz[1] = std::max(1.0f, sz[1]);

  if (!winResize({static_cast<int>(sz.x), static_cast<int>(sz.y)})) {
    return false;
  }

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

  ImGui::End();

  return true;
}

void ZviewMainApp::renderPhase(const types::Matrix4x4 &mvp) const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_backdrop.draw();
  if (m_show_grid) {
    m_grid.draw(mvp, m_mvp.getModelTranslation().translation(),
                m_mvp.getViewDistance());
  }
  m_axis.draw();

  m_buffer.draw(mvp.data());
}
std::optional<types::Vector3> ZviewMainApp::pickingPhase(
    const types::Matrix4x4 &mvp) {
  m_picking.enableWriting();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_picking.setTransform(mvp);
  auto &picking = m_picking;
  const auto preDrawFunction =
      [&picking](const std::pair<std::uint32_t, types::Shape> &s) {
        picking.setObjectIndex(s.first);
      };
  m_buffer.draw(nullptr, preDrawFunction);
  m_picking.disableWriting();
  // get mouse position relative to the current window, reducing the window
  // offset
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
void ZviewMainApp::drawStatusBar() {
  const auto sz = 40;  // ImGui::GetTextLineHeightWithSpacing();

  ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - sz), 0);
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 0), 0);
  ImGui::SetNextWindowBgAlpha(0.35f);  // Transparent background

  ImGui::Begin("Status Bar", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoSavedSettings |
                   ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_NoFocusOnAppearing |
                   ImGuiWindowFlags_NoNav);

  ImGui::Text("FPS:%4.1f", ImGui::GetIO().Framerate);
  ImGui::SameLine(70);
  if (m_hover_point.has_value()) {
    ImGui::Text("[% 8.3f,% 8.3f,% 8.3f]", m_hover_point.value().x(),
                m_hover_point.value().y(), m_hover_point.value().z());
  } else {
    { ImGui::Text("[  No point under cursor   ]"); }
  }

  ImGui::End();
}

void ZviewMainApp::drawHelpMenu() {
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
void ZviewMainApp::drawParamsMenu() {
  // render your GUI
  if (!m_show_params_menu) {
    return;
  }
  ImGui::Begin("Parameters", &m_show_params_menu,
               ImGuiWindowFlags_AlwaysAutoResize);
  auto &params = zview::Params::i();

  static constexpr float deg2rad = M_PIf / 180.0f;
  float cam_fov_deg = params.camera_fov_rad / deg2rad;

  if (ImGui::SliderFloat("Field of view", &cam_fov_deg, 10, 90)) {
    params.camera_fov_rad = cam_fov_deg * deg2rad;
    m_mvp.updatePmat();
  }

  if (ImGui::SliderInt("background color", &params.background_color, 0, 3)) {
    m_backdrop.init(params.background_color);
  }
  ImGui::SliderInt("Texture type", &params.texture_type, 1, 6);
  ImGui::SliderFloat("Point size", &params.point_size, 0.1, 10.0);

  ImGui::End();
}
std::uint32_t ZviewMainApp::plot(types::Shape &&shape) {
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
void ZviewMainApp::remove(const std::string &name) {
  std::uint32_t key = m_buffer.getKey(name);
  if (key != 0) {
    m_tree_view.remove(key);
  }
}
}  // namespace zview
