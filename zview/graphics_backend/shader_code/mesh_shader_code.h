#pragma once
namespace zview {
namespace shader_code {
namespace mesh {
const char* const vertex_shader = R"(
#version 330 core

layout (location = 0) in vec3 a_xyz;
layout (location = 1) in vec4 a_rgb;

out vec4 vertexColor;
uniform mat4 u_transformation;

varying vec3 v_xyz;
varying vec3 v_eyeDir;


void main()
{
    gl_Position = u_transformation*vec4(a_xyz, 1.0);
	vertexColor = a_rgb/255.0f;
    v_xyz = a_xyz;
    v_eyeDir  =  -1.0 *normalize(vec3(u_transformation[0]));
}


)";
const char* const fragment_shader = R"(
#version 330 core

out vec4 fragColor;

in vec4 vertexColor;

uniform int u_txt;
uniform vec3 u_lightDir;

varying vec3 v_xyz;
varying vec3 v_eyeDir;

void main()
{	
	switch(u_txt)
	{
		case 1:
		{
		 fragColor = vertexColor;
		 break;
		}
		case 2:
		{
			const vec3 lightColor=vec3(1,1,1);
			const float ambientFactor = 0.5;
			const float diffuseFactor =0.8;
			const float specFactor = 1.1;
			//calc normal
			vec3 ec_pos=v_xyz.xyz;
			vec3 light_dir = normalize(u_lightDir+v_eyeDir);
			vec3 ec_normal = normalize(cross(dFdx(ec_pos),dFdy(ec_pos)));
			//calc diffuse
			float diffuse = max(0.0,dot(ec_normal, light_dir));
			//calc specularity
			vec3 reflectDir = -reflect(light_dir, ec_normal);
			float spec = pow(max(dot(v_eyeDir, reflectDir), 0.0),2.0);
			vec3 result = vertexColor.xyz*(lightColor*ambientFactor+diffuse*diffuseFactor+spec*specFactor);
			fragColor = vec4(result,vertexColor.w); 
			break;
		}
		case 3:
		{
			vec3 ec_pos=v_xyz.xyz;
			vec3 ec_normal = normalize(cross(dFdx(ec_pos),dFdy(ec_pos)));
			fragColor = vec4((1.0-ec_normal)*0.5,1.0);
			break;

		}
		case 4:
		{
			const vec3 lightColor=vec3(1,1,1);
			const float ambientFactor = 0.5;
			const float diffuseFactor =0.8;
			const float specFactor = 1.1;
						vec3 ec_pos=v_xyz.xyz;
			vec3 ec_normal = normalize(cross(dFdx(ec_pos),dFdy(ec_pos)));
			vec3 norm_color = (1.0-ec_normal)*0.5;

			vec3 light_dir = normalize(u_lightDir+v_eyeDir);
			//calc diffuse
			float diffuse = max(0.0,dot(ec_normal, light_dir));
			//calc specularity
			vec3 reflectDir = -reflect(light_dir, ec_normal);
			float spec = pow(max(dot(v_eyeDir, reflectDir), 0.0),2.0);
			vec3 result = norm_color*(lightColor*ambientFactor+diffuse*diffuseFactor+spec*specFactor);
			fragColor = vec4(result,1.0); 
			break;
		}
		case 5:
		{
			const vec4 specularColor1=vec4 (0.1,0.08,0.05,1.0);
  		const  vec4 specularColor2=vec4 (0.1,0.1,0.05,1.0);
  		const  vec4 glassColor=vec4(0.5,0.5,0.6,0.15);
  		const  float specularFactor1=2.0;
  		const float specularFactor2=2.0;
  		vec3 ec_pos=v_xyz.xyz;
		vec3 ec_normal = normalize(cross(dFdx(ec_pos),dFdy(ec_pos)));
  	    vec3 reflect_dir = -reflect(u_lightDir, ec_normal);
  	    float spec = max(dot(v_eyeDir, reflect_dir), 0.0);
  	    spec = spec * spec;
  		vec4 color = glassColor + specularFactor1 * spec * specularColor1;
  	    spec = pow(spec, 8.0) * specularFactor2;
  		color += spec * specularColor2;
  		fragColor = min(color, vec4(1.0));
		
		break;
	}
		case 6:

		
		{
		const vec3 base3 = vec3(0.99, 0.96, 0.89);
		const vec3 base2 = vec3(0.92, 0.91, 0.83);
		const vec3 base00 = vec3(0.40, 0.48, 0.51);
		const vec3 va = vec3(0.0, 0.0, 1.0);
		const vec3 vb = vec3(-0.57, -0.57, 0.57);
		vec3 ec_pos=v_xyz.xyz;
		vec3 ec_normal = normalize(cross(dFdx(ec_pos), dFdy(ec_pos)));
		ec_normal = normalize(ec_normal);
		float a = dot(ec_normal, va);
		float b = dot(ec_normal, vb);
		fragColor = vec4(((a*base2 + (1.0-a)*base00) + (b*base3 + (1.0-b)*base00))*0.5, 1.0); 

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
}  // namespace mesh
}  // namespace shader_code

}  // namespace zview