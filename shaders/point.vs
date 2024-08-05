#version 330 core

layout (location = 0) in vec3 a_xyz;
layout (location = 1) in vec4 a_rgb;

out vec4 vertexColor;
uniform mat4 u_transformation;
uniform float u_ptsize;


varying vec3 v_xyz;
varying vec3 v_eyeDir;


void main()
{
    gl_Position = u_transformation*vec4(a_xyz, 1.0);
    
	vertexColor = a_rgb/255.0f;
    float z = gl_Position.z;
    gl_PointSize = max(0.1f,u_ptsize);
    v_xyz = a_xyz;
    v_eyeDir  =  -normalize(vec3(u_transformation[0]));


	
	
}
