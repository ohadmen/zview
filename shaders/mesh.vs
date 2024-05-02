#version 330 core

layout (location = 0) in vec3 a_xyz;
layout (location = 1) in vec4 a_rgb;

out vec4 vertexColor;
uniform mat4 u_transformation;

varying vec3 v_xyz;
varying vec3 v_eye_dir;


void main()
{
    gl_Position = u_transformation*vec4(a_xyz, 1.0);
	vertexColor = a_rgb/255.0f;
    v_xyz = a_xyz;
    v_eye_dir  =  -normalize(vec3(u_transformation[0]));


	
	
}
