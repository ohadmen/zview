#pragma once
#include "src/types/types.h"
#include <GL/glew.h> // Initialize with glewInit()
#include <GLFW/glfw3.h>
#include "vp_mat.h"
#include "imgui.h"
namespace zview {


class InputDeviceHandler
{

    types::Vector3 getHitOnScreen(const ImVec2& xy,const float tb_radius);
public:
    InputDeviceHandler();
    void step();
    InputDeviceHandler(InputDeviceHandler &&) = default;
    InputDeviceHandler(const InputDeviceHandler &) = default;
    InputDeviceHandler &operator=(InputDeviceHandler &&) = default;
    InputDeviceHandler &operator=(const InputDeviceHandler &) = default;
    ~InputDeviceHandler();
    types::Matrix4x4 getVPmatrix() const;
    void setWinSize(int w, int h);
private:
    VPmat m_vp_mat;
    types::Transform m_clickViewMatrix;
};

}