#version 330


uniform uint u_objectIndex;

out uvec3 fragColor;

void main()
{
    // fragColor = uvec3(u_objectIndex, u_drawIndex+1U,gl_PrimitiveID + 1U);
    uint primInndex = uint(gl_PrimitiveID);
    fragColor = uvec3(1U, u_objectIndex,primInndex);
}