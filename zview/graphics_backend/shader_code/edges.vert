#version 450

layout(location = 0) in vec3 a_xyz;
layout(location = 1) in vec4 a_rgb;

layout(push_constant) uniform PC {
    mat4 mvp;
} pc;

layout(location = 0) out vec4 vertexColor;

void main() {
    gl_Position = pc.mvp * vec4(a_xyz, 1.0);
    vertexColor = a_rgb;
}
