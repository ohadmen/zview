#version 330 core

layout (location = 0) in vec3 a_xyz;
layout (location = 1) in vec4 a_rgb;

out vec4 vertexColor;
out vec3 xyz;
uniform mat4 u_transformation;


void main()
{
    gl_Position = u_transformation*vec4(a_xyz, 1.0);

	vertexColor = a_rgb/255.0f;
    xyz = a_xyz;
    
	
}
