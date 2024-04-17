#include "input_device_handler.h"
#include "imgui.h"
#include <iostream>

void InputDeviceHandler::cursor_pos_callback(GLFWwindow* window, double x, double y)
{
    std::cout << x << " " << y << std::endl;
    return;
}


InputDeviceHandler::InputDeviceHandler(GLFWwindow* window)
{
    
    glfwSetCursorPosCallback(window,cursor_pos_callback);
}

InputDeviceHandler::~InputDeviceHandler()
{
}