#version 330 core

out vec4 fragColor;

in vec4 vertexColor;
in vec3 xyz;


#define THR_1 1e3f
#define THR_2 2e3f

//! [0]
void main()
{
		fragColor =vertexColor;
		
		
		float r2 = dot(xyz,xyz);
		if(r2>THR_1)
		{
			fragColor.a *=max(0,(THR_2-r2)/(THR_2-THR_1));
		}


    
}
//! [0]

