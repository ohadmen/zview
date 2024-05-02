#version 330                                                                        
                                                                                    
layout (location = 0) in vec3 a_xyz;
layout (location = 1) in vec4 a_rgb;
                                                                                    
uniform mat4 u_transformation;
                                                                                    
void main()                                                                         
{                                                                                   
    gl_Position = u_transformation*vec4(a_xyz, 1.0);
    gl_PointSize = 10.0;
}