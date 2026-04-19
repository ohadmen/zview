#version 450

layout(push_constant) uniform PC {
    mat4     mvp;
    uint     objectIndex;
    float    pad0;
    float    pad1;
    float    pad2;
} pc;

layout(location = 0) out uvec4 fragColor;

void main() {
    uint primIndex = uint(gl_PrimitiveID);
    fragColor = uvec4(1u, pc.objectIndex, primIndex, 0u);
}
