#include "imgui.h"
#include "src/graphics_backend/imgui_impl_glfw.h"
#include "src/graphics_backend/imgui_impl_opengl3.h"


#include <stdio.h>
#include <iostream>
#include <vector>

#include <GL/glew.h> // Initialize with glewInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include "src/zview.h"


#define PI 3.14159265358979323846

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


using namespace zview;
int main(int, char **)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

	// Create window with graphics context
	GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui - Conan", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	bool err = glewInit() != GLEW_OK;

	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	glViewport(0, 0, screen_width, screen_height);
  
 
    

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	auto context = ImGui::CreateContext();
	// ImGuiIO &io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

  bool show_window = false;
  std::unique_ptr<Zview> zviewInfPtr;
	while (!glfwWindowShouldClose(window))
	{
    
    
    
		glfwPollEvents();
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glfwMakeContextCurrent(window);
		glClear(GL_COLOR_BUFFER_BIT);

		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
    ImGui::SetCurrentContext(context);
		ImGui::NewFrame();
    

		// render your GUI
    
		ImGui::Begin("test window");
    ImGui::Text("Frame Rate: %.1f", ImGui::GetIO().Framerate);
      ImGui::Text("Hello, world!");
      ImGui::Checkbox("Demo Window", &show_window);
      
        ImGui::End();

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glfwSwapBuffers(window);
    if(show_window)
    {
      if(!zviewInfPtr)
      {
        zviewInfPtr = std::make_unique<Zview>();
        if(!zviewInfPtr->init())
        {
          std::cout << "Zview failed to initialize" << std::endl;
          return 1;
        }
      }
      if(!zviewInfPtr->loop())
      {
        show_window = false;
      }
    }
    if(!show_window && zviewInfPtr)
    {
      zviewInfPtr.reset();
    } 
    

	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}