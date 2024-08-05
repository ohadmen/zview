#version 330 core
#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif


out vec4 fragColor;

in vec4 vertexColor;

uniform int u_txt;
uniform vec3 u_lightDir;

varying vec3 v_xyz;
varying vec3 v_eyeDir;

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
//! [0]

