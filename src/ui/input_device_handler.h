#pragma once
#include <GL/glew.h> // Initialize with glewInit()
#include <GLFW/glfw3.h>
class InputDeviceHandler
{
    void cursor_pos_callback(GLFWwindow *window, double xposIn, double yposIn);
public:
    InputDeviceHandler(GLFWwindow* window);
    InputDeviceHandler(InputDeviceHandler &&) = default;
    InputDeviceHandler(const InputDeviceHandler &) = default;
    InputDeviceHandler &operator=(InputDeviceHandler &&) = default;
    InputDeviceHandler &operator=(const InputDeviceHandler &) = default;
    ~InputDeviceHandler();

private:
    
};

