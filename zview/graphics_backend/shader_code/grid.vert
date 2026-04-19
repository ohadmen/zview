#version 450

layout(location = 0) in vec3 a_xyz;
layout(location = 1) in vec4 a_rgb;

layout(push_constant) uniform PC {
    mat4  mvp;
    vec2  shift;
    float scale;
    float pad;
} pc;

layout(location = 0) out vec4 vertexColor;
layout(location = 1) out vec3 xyz;

void main() {
    gl_Position = pc.mvp * vec4(a_xyz, 1.0);
    vertexColor = a_rgb;
    if (a_xyz.x == -pc.shift.x / pc.scale || a_xyz.y == -pc.shift.y / pc.scale) {
        vertexColor.a *= 2.0;
    }
    xyz = a_xyz;
}
