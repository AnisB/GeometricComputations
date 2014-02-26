#version 410

layout(vertices = 3) out;
// in vec2 texCoordV[];
// in vec3 fnormalV[];
in vec3 positionV[];

out vec3 positionTC[];
// out vec2 texCoordTC[];
// out vec3 formalTC[];

uniform float TessLevelInner;
uniform float TessLevelOuter;
 
#define ID gl_InvocationID
void main()
{
    positionTC[ID] = positionV[ID].xyz;
    // formalTC[ID] = fnormalV[ID];
    // texCoordTC[ID] = texCoordV[ID];
    if (ID == 0) 
    {
        gl_TessLevelInner[0] = 16;
        gl_TessLevelOuter[0] = 16;
        gl_TessLevelOuter[1] = 16;
        gl_TessLevelOuter[2] = 16;
    }
}
