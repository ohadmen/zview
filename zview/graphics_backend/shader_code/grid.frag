#version 450

layout(location = 0) in vec4 vertexColor;
layout(location = 1) in vec3 xyz;

layout(location = 0) out vec4 fragColor;

#define THR_1 1e3
#define THR_2 2e3

void main() {
    fragColor = vertexColor;
    float r2 = dot(xyz, xyz);
    if (r2 > THR_1) {
        fragColor.a *= max(0.0, (THR_2 - r2) / (THR_2 - THR_1));
    }
}
