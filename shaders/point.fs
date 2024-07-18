#version 330 core
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


out vec4 fragColor;

in vec4 vertexColor;

uniform int u_txt;
uniform vec3 u_light_dir;

varying vec3 v_xyz;
varying vec3 v_eye_dir;

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
	case 0:
	{
		fragColor =vertexColor;
		break;
	}
	case 1:
	{
		vec4 specularColor1=vec4 (0.1,0.08,0.05,1.0);
  		vec4 specularColor2=vec4 (0.1,0.1,0.05,1.0);
  		vec4 glassColor=vec4(0.5,0.5,0.6,0.15);
  		float specularFactor1=2.0;
  		float specularFactor2=2.0;
  		vec3 ec_normal = normalize(rand3(v_xyz));
  	    vec3 reflectDir = -reflect(u_light_dir, ec_normal);
  	    float spec = max(dot(v_eye_dir, reflectDir), 0.0);
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

	// else if(u_txt==2)
	// {
		
	// 	float specularFactor =0.5;
	// 	vec3 ec_pos=v_xyz.xyz;
	// 	vec3 ec_normal = normalize(cross(dFdx(ec_pos),dFdy(ec_pos)));
	// 	float v = abs(dot(ec_normal, u_lightDir));
	// 	float spec = max(v, 0.0)*specularFactor/2.0+
	// 				 max(-v, 0.0)*specularFactor/2.0+
	// 				 (1.0-specularFactor);
	// 	gl_FragColor = vec4(vec3(v_rgb*spec),1.0); 
		

	// }
	// else if(u_txt==4)
	// {
		
  	// 	vec4 specularColor1=vec4 (0.1,0.08,0.05,1.0);
  	// 	vec4 specularColor2=vec4 (0.1,0.1,0.05,1.0);
  	// 	vec4 glassColor=vec4(0.5,0.5,0.6,0.15);
  	// 	float specularFactor1=2.0;
  	// 	float specularFactor2=2.0;
  	// 	vec3 ec_pos=vec3(v_xyz[0],v_xyz[1],v_xyz[2]);
  	// 	vec3 ec_normal = normalize(cross(dFdx(ec_pos),dFdy(ec_pos)));
  	//     vec3 reflectDir = -reflect(u_lightDir, ec_normal);
  	//     float spec = max(dot(v_eyeDir, reflectDir), 0.0);
  	//     spec = spec * spec;
  	// 	vec4 color = glassColor + specularFactor1 * spec * specularColor1;
  	//     spec = pow(spec, 8.0) * specularFactor2;
  	// 	color += spec * specularColor2;
  	// 	gl_FragColor = min(color, vec4(1.0));
	// }
	// else if(u_txt==5)
	// {
	// 	gl_FragColor = vec4(v_z,v_z,v_z,1.0); 
	// }
	// else
	// {
	// 	gl_FragColor=vec4(0.0,0.0,0.0,1.0);	
	// }
	// 		 fragColor = vertexColor;

	
    
}
//! [0]

