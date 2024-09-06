#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <algorithm>
#include <iostream>
#include <random>

#include "zview/graphics_backend/imgui_impl_glfw.h"
#include "zview/graphics_backend/imgui_impl_opengl3.h"
#include "zview/zview_inf.h"

static void glfw_error_callback(int error, const char *description) {
  std::cerr << "Glfw Error " << error << ": " << description << std::endl;
}

std::vector<zview::Face> generateRandomFaces(int n_vertices, int n_faces) {
  // random integer generator
  static std::default_random_engine gen;
  static std::uniform_int_distribution<std::uint32_t> dist(0, n_vertices - 1);

  std::vector<zview::Face> faces{static_cast<std::size_t>(n_faces)};
  std::for_each(faces.begin(), faces.end(),
                [&](zview::Face &f) { f = {dist(gen), dist(gen), dist(gen)}; });
  return faces;
}

std::vector<zview::Edge> generateRandomEdges(int n_vertices, int n_edges) {
  // random integer generator
  static std::default_random_engine gen;
  static std::uniform_int_distribution<std::uint32_t> dist(0, n_vertices - 1);

  std::vector<zview::Edge> edges{static_cast<std::size_t>(n_edges)};
  std::for_each(edges.begin(), edges.end(),
                [&](zview::Edge &e) { e = {dist(gen), dist(gen)}; });
  return edges;
}
std::vector<zview::Vertex> generateRandomVertices(int n_vertices,
                                                  float z_offset) {
  std::vector<zview::Vertex> vertices{static_cast<std::size_t>(n_vertices)};
  static std::default_random_engine gen;
  static std::uniform_real_distribution<float> dist(-1, 1);
  auto random_color = []() {
    return static_cast<std::uint8_t>((dist(gen) / 2 + 0.5) * 255);
  };
  auto random_pos = [&]() { return dist(gen); };
  std::for_each(vertices.begin(), vertices.end(), [&](zview::Vertex &v) {
    v = {random_pos(),
         random_pos(),
         random_pos() + z_offset,
         random_color(),
         random_color(),
         random_color(),
         255U};
  });

  return vertices;
}

GLFWwindow *initGL() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    return nullptr;
  }

  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 460";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

  // Create window with graphics context
  auto m_window = glfwCreateWindow(1280, 720, "ZVIEW", nullptr, nullptr);
  if (m_window == nullptr) {
    std::cerr << "Failed to create window" << std::endl;
    return nullptr;
  }

  glfwMakeContextCurrent(m_window);

  glfwSwapInterval(1);  // Enable vsync

  if (glewInit() != GLEW_OK) {
    std::cerr << "Failed to initialize OpenGL loader!" << std::endl;
    return nullptr;
  }
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_DEPTH_TEST);  // draw object back tp front
  glEnable(GL_LINE_SMOOTH);

  glEnable(GL_MULTISAMPLE);
  glEnable(GL_POLYGON_SMOOTH);

  glEnable(GL_BLEND);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDepthFunc(GL_LESS);

  glClearColor(0, 1, 0, 1);

  ImGuiIO &io = ImGui::GetIO();
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui_ImplOpenGL3_Init(glsl_version);
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(m_window, true);
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  return m_window;
}

static std::vector<std::string> get_args(int argc, char **argv) {
  std::vector<std::string> list;
  for (int i = 1; i != argc; ++i) {
    list.push_back(*std::next(argv, i));
  }

  return list;
}

class AppWrapper {
  std::shared_ptr<zview::ZviewInf> app{zview::ZviewInf::create()};
  AppWrapper() = default;

 public:
  static zview::ZviewInf &i() {
    static AppWrapper instance;
    return *instance.app;
  }
  static void fileDropCallback(GLFWwindow *window, int count,
                               const char **paths) {
    std::vector<std::string> files(count);
    for (int i = 0; i < count; ++i) {
      files[i] = *std::next(paths, i);
    }

    i().loadFiles(files);
  }
};

int main(int argc, char *argv[]) {
  auto &app = AppWrapper::i();

  const auto files = get_args(argc, argv);

  auto window = initGL();

  if (!app.init()) {
    return 1;
  }

  glfwSetDropCallback(window, AppWrapper::fileDropCallback);
  app.loadFiles(files);
#ifdef DEBUG_DATA_INJECTION
  bool add_pcl{true};
  bool add_mesh{false};
  bool add_edges{false};
  int n_vertices{1000};
  int n_edges{1000};
  int n_faces{1000};
#endif

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glfwMakeContextCurrent(window);

    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("window", nullptr,
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoDocking |
                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

    if (!app.draw()) {
      break;
    }
    ImGui::End();
#ifdef DEBUG_DATA_INJECTION

    ImGui::Begin("interface inject");
    ImGui::Checkbox("add pcl", &add_pcl);
    ImGui::SameLine();
    ImGui::SliderInt("n vertices", &n_vertices, 0, 10000);
    ImGui::Checkbox("add mesh", &add_mesh);
    ImGui::SameLine();
    ImGui::SliderInt("n faces", &n_faces, 0, 1000);
    ImGui::Checkbox("add edges", &add_edges);
    ImGui::SameLine();
    ImGui::SliderInt("n edges", &n_edges, 0, 1000);
    ImGui::End();
    if (add_pcl) {
      auto vertices = generateRandomVertices(n_vertices, 0);
      app.plot("test_interface/pcl", std::move(vertices));
    }
    if (add_mesh) {
      auto vertices = generateRandomVertices(n_vertices, 2);
      auto faces = generateRandomFaces(n_vertices, n_faces);
      app.plot("test_interface/mesh", std::move(vertices), std::move(faces));
    }
    if (add_edges) {
      auto vertices = generateRandomVertices(n_vertices, 4);
      auto edges = generateRandomEdges(n_vertices, n_edges);
      app.plot("test_interface/edges", std::move(vertices), std::move(edges));
    }
#endif
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
