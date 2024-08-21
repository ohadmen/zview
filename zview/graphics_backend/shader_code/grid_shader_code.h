#pragma once
namespace zview {
namespace shader_code {
namespace grid {
const char* const vertex_shader = R"(
#version 330 core

layout (location = 0) in vec3 a_xyz;
layout (location = 1) in vec4 a_rgb;

out vec4 vertexColor;
out vec3 xyz;
uniform mat4 u_transformation;
uniform vec2 u_shift;
uniform float u_scale;


void main()
{
    gl_Position = u_transformation*vec4(a_xyz, 1.0);

	vertexColor = a_rgb/255.0f;
    if(a_xyz.x==-u_shift.x/u_scale || a_xyz.y==-u_shift.y/u_scale)
		{
			vertexColor.a *=2;
		}

    xyz = a_xyz;
    
	
}

)";
const char* const fragment_shader = R"(#version 330 core

out vec4 fragColor;

in vec4 vertexColor;
in vec3 xyz;

#define THR_1 1e3f
#define THR_2 2e3f
void main()
{
		fragColor =vertexColor;
		
		
		float r2 = dot(xyz,xyz);
		if(r2>THR_1)
		{
			fragColor.a *=max(0,(THR_2-r2)/(THR_2-THR_1));
		}
}



)";
}  // namespace grid
}  // namespace shader_code

}  // namespace zview