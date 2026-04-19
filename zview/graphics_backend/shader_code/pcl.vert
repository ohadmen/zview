#version 450

layout(location = 0) in vec3 a_xyz;
layout(location = 1) in vec4 a_rgb;

layout(push_constant) uniform PC {
    mat4  mvp;
    float ptsize;
    float nearPlaneDist;
    int   txt;
    float pad;
    vec4  lightDir;
} pc;

layout(location = 0) out vec4 vertexColor;
layout(location = 1) out vec3 v_eyeDir;
layout(location = 2) out float v_normDepth;

void main() {
    gl_Position  = pc.mvp * vec4(a_xyz, 1.0);
    vertexColor  = a_rgb;
    gl_PointSize = pc.ptsize * pc.nearPlaneDist / gl_Position.w;
    v_normDepth  = gl_Position.w / pc.nearPlaneDist * 2.0;
    v_eyeDir     = -normalize(vec3(pc.mvp[0]));
}
