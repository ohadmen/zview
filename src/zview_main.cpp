#include "imgui.h"
#include "src/opengl_backend/imgui_impl_glfw.h"
#include "src/opengl_backend/imgui_impl_opengl3.h"
#include "src/opengl_backend/opengl_shader.h"
#include "src/ui/input_device_handler.h"
#include "src/drawables/shape_buffer.h"

#include "src/io/read_file.h"
#include "src/io/read_ply.h"
#include <GL/glew.h> // Initialize with glewInit()
#include <stdio.h>
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#define PI 3.14159265358979323846

static std::vector<std::string> get_args(int argc, char **argv) {
  std::vector<std::string> list;
  for (int i = 1; i != argc; ++i)
    list.push_back(argv[i]);

  return list;
}

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


int main(int argc, char *argv[]) {
  const auto args = get_args(argc, argv);
  ShapeBuffer buffer;
  

  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // 3.0+ only

  // Create window with graphics context
  GLFWwindow *window =
      glfwCreateWindow(1280, 720, "Dear ImGui - Conan", NULL, NULL);
  if (window == NULL)
    return 1;
  
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync
  
  
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    return 1;
  }
  
  for (const auto &f : args) {

    const auto shape_vector = io::read_ply(f);
    for (const auto &s : shape_vector) {
      buffer.push(s);
    }
  }
    types::Mesh example("triangle");
  example.v() = {
      types::VertData{0.0f,   0.25f,  10.0f,      255,   0,   0, 255},
      types::VertData{0.25f,  -0.25f, 10.0f,      0,   255,   0, 255},
      types::VertData{-0.25f, -0.25f, 1.0f,      0,   0,   255, 255}
      };
  example.f() = {types::FaceIndx{0,1,2}};
  buffer.push(example);

  // init shader
  Shader triangle_shader;
  triangle_shader.init(read_file("shaders/simple-shader.vs"),
                       read_file("shaders/simple-shader.fs"));

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  
  zview::InputDeviceHandler idh;
  std::array<int,2> display_wh;
  while (!glfwWindowShouldClose(window)) {
    auto display_wh_prev = display_wh;
    glfwGetFramebufferSize(window, &display_wh[0], &display_wh[1]);
    if(display_wh!=display_wh_prev)
    {
      idh.setWinSize(display_wh[0],display_wh[1]);
    }
    glViewport(0, 0, display_wh[0], display_wh[1]);
    
    glfwPollEvents();
    glClearColor(0.1f, 0.1f, 0.1f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);


    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // rendering our geometries
    triangle_shader.use();

    buffer.draw();

    // render your GUI
    ImGui::Begin("Triangle Position/Color");
    static float rotation = 0.0;
    static float rotation_phi = 0.0;
    static float rotation_theta = 0.0;
    static float scale= 1.0;
    
    ImGui::SliderFloat("scale", &scale, 0.9, 1.1);
    ImGui::SliderFloat("rotation", &rotation, 0, 2 * PI);
    ImGui::SliderFloat("rotation_phi", &rotation_phi, 0, 1);
    ImGui::SliderFloat("rotation_theta", &rotation_theta, 0, 1);
    types::Vector3 n{rotation_phi,rotation_theta,1.0};
    n.normalize();
    static types::Vector3 translation = {0.0, 0.0,0.0};
    ImGui::SliderFloat2("position", translation.data(), -1.0, 1.0);
    static float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    // pass the parameters to the shader
    idh.step();
    const types::Matrix4x4 transformation=idh.getVPmatrix();
    triangle_shader.setUniform("transformation",transformation.data());
    // color picker
    ImGui::ColorEdit3("color", color);
    // multiply triangle's color with this color
    triangle_shader.setUniform("color", color[0], color[1], color[2]);
    
    ImGui::End();

    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
