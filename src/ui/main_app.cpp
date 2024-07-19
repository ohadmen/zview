#include "src/ui/main_app.h"

#include "src/io/read_ply.h"

#include "src/opengl_backend/imgui_impl_glfw.h"
#include "src/opengl_backend/imgui_impl_opengl3.h"
#include "src/params/params.h"
#include <iostream>
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
namespace zview {

static void glfw_error_callback(int error, const char *description) {
  std::cerr << "Glfw Error " << error << ": " << description << std::endl;
}

std::array<int, 2> MainApp::getWinSize() const {
  std::array<int, 2> size;
  glfwGetWindowSize(m_window, &size[0], &size[1]);
  return size;
}

MainApp::MainApp()
    : m_axis(m_mvp), m_idh{m_mvp},
      m_tree_view{std::bind(&ShapeBuffer::shapeVisibility, &m_buffer,
                            std::placeholders::_1),
                  std::bind(&MainApp::setCameraToViewSelectedKey, this,
                            std::placeholders::_1)} {}
bool MainApp::init() {

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    std::cerr << "Failed to initialize glfw" << std::endl;
    return false;
  }

  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 460";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only

  // Create window with graphics context
  m_window = glfwCreateWindow(1280, 720, "ZVIEW", nullptr, nullptr);
  if (m_window == nullptr) {
    std::cerr << "Failed to create window" << std::endl;
    return false;
  }

  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(1); // Enable vsync

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
    return false;
  }

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_DEPTH_TEST); // draw object back tp front
  glEnable(GL_LINE_SMOOTH);

  glEnable(GL_MULTISAMPLE);
  glEnable(GL_POLYGON_SMOOTH);

  glEnable(GL_BLEND);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDepthFunc(GL_LESS);

  glClearColor(0, 1, 0, 1);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(m_window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  if (!winResize(getWinSize())) {
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

void MainApp::loadFiles(const std::vector<std::string> &files) {
  for (const auto &f : files) {
    auto shape_vector = io::read_ply(f);

    for (const auto &s : shape_vector) {
      const auto key = m_buffer.push(s);
      m_tree_view.push(std::visit([](const auto &v) { return v.getName(); }, s),
                       key);
    }
  }

  setCameraToViewSelectedKey({});
}
bool MainApp::winResize(const std::array<int, 2> &wh) {
  m_mvp.setWinSize(wh);
  if (!m_picking.init(wh)) {
    std::cerr << "picking texture init failed" << std::endl;
    return false;
  }
  return true;
}

void MainApp::setCameraToViewSelectedKey(
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

void MainApp::loop() {
  
  while (!glfwWindowShouldClose(m_window)) {
    
    const auto wh = getWinSize();
    if (wh != m_mvp.getWinSize()) {
      m_mvp.setWinSize(wh);
      if (!m_picking.init(wh)) {
        std::cerr << "picking texture init failed" << std::endl;
        return;
      }
    }
    glViewport(0, 0, wh[0], wh[1]);

    glfwPollEvents();

    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    drawParamsMenu();
    drawStatusBar();

    m_tree_view.draw();

    m_idh.step(m_hover_point);

    const auto transformation = m_mvp.getMVPmatrix();

    // render phase

    m_hover_point = pickingPhase(transformation);

    renderPhase(transformation);

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(m_window);
  glfwTerminate();
}

void MainApp::renderPhase(const types::Matrix4x4 &mvp) const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_backdrop.draw();
  m_grid.draw(mvp, m_mvp.getModelTranslation().translation(),
              m_mvp.getViewDistance());
  m_axis.draw();

  m_buffer.draw(mvp.data());
}
std::optional<types::Vector3>
MainApp::pickingPhase(const types::Matrix4x4 &mvp) {
  const auto &io = ImGui::GetIO();
  m_picking.enableWriting();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_picking.setTransform(mvp);
  auto &picking = m_picking;
  const auto preDrawFunction =
      [&picking](std::pair<std::uint32_t, types::Shape> s) {
        picking.setObjectIndex(s.first);
      };
  m_buffer.draw(nullptr, preDrawFunction);
  m_picking.disableWriting();

  const auto pix = m_picking.readPixel(io.MousePos.x, io.MousePos.y);

  if (pix.valid == 1) {
    const auto r = m_mvp.getRay(io.MousePos, MVPmat::CoordinateSystem::GLOBAL);

    const auto pt = m_buffer.get3dLocation(pix.object_id, pix.prim_id, r);

    return pt;
  }

  return {};
}
void MainApp::drawStatusBar() {
  const auto sz = 40; // ImGui::GetTextLineHeightWithSpacing();

  ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - sz), 0);
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 0), 0);
  ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

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
void MainApp::drawParamsMenu() {

  // render your GUI
  ImGui::Begin("Parameters");
  auto &params = zview::Params::i();
  const float object_distance = m_mvp.getViewDistance();
  params.camera_z_near = object_distance * 1e-3;
  params.camera_z_far = object_distance * 1e3;

  static constexpr float deg2rad = M_PIf / 180.0f;
  float cam_fov_deg = params.camera_fov_rad / deg2rad;

  if (ImGui::SliderFloat("Field of view", &cam_fov_deg, 10, 90)) {
    params.camera_fov_rad = cam_fov_deg * deg2rad;
  }

  if (ImGui::SliderInt("background color", &params.background_color, 0, 3)) {
    m_backdrop.init(params.background_color);
  }
  ImGui::SliderInt("Texture type", &params.texture_type, 0, 4);
  ImGui::SliderFloat("Point size", &params.point_size, 0.1, 10.0);
  m_mvp.updatePmat();

  ImGui::End();
}
} // namespace zview