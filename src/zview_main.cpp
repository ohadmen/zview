#include "imgui.h"
#include "src/opengl_backend/imgui_impl_glfw.h"
#include "src/opengl_backend/imgui_impl_opengl3.h"
#include "src/opengl_backend/opengl_shader.h"
#include "src/ui/input_device_handler.h"
#include "src/drawables/shape_buffer.h"
#include "src/drawables/vp_mat.h"
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

void create_triangle(unsigned int &vbo, unsigned int &vao, unsigned int &ebo) {
 static constexpr std::array<types::VertData,3> tri_data {
      types::VertData{0.0f,   0.25f,  0.0f,      255,   0,   0, 255},
      types::VertData{0.25f,  -0.25f, 0.0f,      0,   255,   0, 255},
      types::VertData{-0.25f, -0.25f, 0.0f,      0,   0,   255, 255}
      };

  unsigned int triangle_indices[] = {0, 1, 2};
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tri_data), tri_data.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices),
               triangle_indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 4 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

int main(int argc, char *argv[]) {
  const auto args = get_args(argc, argv);
  ShapeBuffer buffer;
  

  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
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
  // int screen_width, screen_height;
  // glfwGetFramebufferSize(window, &screen_width, &screen_height);
  // glViewport(0, 0, screen_width, screen_height);

  // create our geometries
  unsigned int vbo, vao, ebo;
  create_triangle(vbo, vao, ebo);

  // init shader
  Shader triangle_shader;
  triangle_shader.init(read_file("shaders/simple-shader.vs"),
                       read_file("shaders/simple-shader.fs"));

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  zview::VPmat vpmat;
  InputDeviceHandler ui_input(window);
  while (!glfwWindowShouldClose(window)) {
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glfwPollEvents();
    glClearColor(0.1f, 0.1f, 0.1f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // rendering our geometries
    triangle_shader.use();

    buffer.draw();
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

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
    vpmat.scale(scale);
    vpmat.rotate(rotation, n);
    static types::Vector3 translation = {0.0, 0.0,0.0};
    ImGui::SliderFloat2("position", translation.data(), -1.0, 1.0);
    vpmat.translate(translation);
    static float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    // pass the parameters to the shader
    const types::Matrix4x4 transformation=vpmat.getVPmatrix();
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
