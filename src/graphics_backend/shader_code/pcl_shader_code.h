#pragma once
namespace zview {
namespace shader_code {
namespace pcl {
const char *const vertex_shader = R"(
#version 330 core

layout (location = 0) in vec3 a_xyz;
layout (location = 1) in vec4 a_rgb;

out vec4 vertexColor;
uniform mat4 u_transformation;
uniform float u_ptsize;


varying vec3 v_xyz;
varying vec3 v_eyeDir;
varying float v_depth;


void main()
{
    gl_Position = u_transformation*vec4(a_xyz, 1.0);
    
	vertexColor = a_rgb/255.0f;
    v_depth = gl_Position.z;
    gl_PointSize = max(0.1f,u_ptsize);
    v_xyz = a_xyz;
    v_eyeDir  =  -normalize(vec3(u_transformation[0]));


	
	
}

)";
const char *const fragment_shader = R"(
#version 330 core

out vec4 fragColor;

in vec4 vertexColor;

uniform int u_txt;
uniform vec3 u_lightDir;

varying vec3 v_xyz;
varying vec3 v_eyeDir;
varying float v_depth;
uint wang_hash(uint seed)
{
	
    seed = (seed ^ 61U) ^ (seed >> 16U);
    seed *= 9U;
    seed = seed ^ (seed >> 4U);
    seed *= 0x27d4eb2dU;
    seed = seed ^ (seed >> 15U);
    return seed;
}
vec3 rand3(const vec3 xyz)
{	
	
	uint seed_a = uint(floatBitsToInt(xyz.x)^floatBitsToInt(xyz.y)^floatBitsToInt(xyz.z));
	uint seed_b = wang_hash(seed_a);
	uint seed_c = wang_hash(seed_b);
	return vec3(
		intBitsToFloat(int(seed_a)),
		intBitsToFloat(int(seed_b)),
		intBitsToFloat(int(seed_c))
	);

	return vec3(
		floatBitsToInt(xyz.x),
		floatBitsToInt(xyz.y),
		floatBitsToInt(xyz.z)
	);
}

void main()
{

	switch(u_txt)
	{
	case 1:
	{
		fragColor =vertexColor;
		break;
	}
	case 2:
	{	
		float z = max(0,1-v_depth/100);
		fragColor  =vec4(z,z,z,1.0);
		break;
	}
	case 5:
	{
	vec4 specularColor1=vec4 (0.1,0.08,0.05,1.0);
  		vec4 specularColor2=vec4 (0.1,0.1,0.05,1.0);
  		vec4 glassColor=vec4(0.5,0.5,0.6,0.15);
  		float specularFactor1=0.1;
  		float specularFactor2=0.3;
  		vec3 ec_normal = normalize(rand3(v_xyz));
  	    vec3 reflectDir = -reflect(u_lightDir, ec_normal);
  	    float spec = max(dot(v_eyeDir, reflectDir), 0.0);
  	    spec = spec * spec;
  		vec4 color = glassColor + specularFactor1 * spec * specularColor1;
  	    spec = pow(spec, 8.0) * specularFactor2;
  		color += spec * specularColor2;
  		fragColor = min(color, vec4(1.0));
		break;
	}
	
	default:
	{
		fragColor = vec4(0.0,0.0,0.0,1.0);
		break;
	}
	}
    
}


)";
}  // namespace pcl
}  // namespace shader_code

}  // namespace zview