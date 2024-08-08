#pragma once
namespace zview {
namespace shader_code {
namespace picking {
const char* const vertex_shader = R"(
#version 330                                                                        
                                                                                    
layout (location = 0) in vec3 a_xyz;
layout (location = 1) in vec4 a_rgb;
                                                                                    
uniform mat4 u_transformation;
                                                                                    
void main()                                                                         
{                                                                                   
    gl_Position = u_transformation*vec4(a_xyz, 1.0);
    gl_PointSize = 10.0;
}
)";
const char* const fragment_shader = R"(
#version 330


uniform uint u_objectIndex;

out uvec3 fragColor;

void main()
{
    // fragColor = uvec3(u_objectIndex, u_drawIndex+1U,gl_PrimitiveID + 1U);
    uint primInndex = uint(gl_PrimitiveID);
    fragColor = uvec3(1U, u_objectIndex,primInndex);
}

)";
}  // namespace picking
}  // namespace shader_code

}  // namespace zview