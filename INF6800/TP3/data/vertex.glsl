#version 410

// uniform mat4 model; 
// uniform mat4 view; 
// uniform mat4 projection;
// uniform mat4 modelviewprojection;

in vec3 position;

// out vec2 texCoordV;
// out vec3 fnormalV;
out vec3 positionV;


void main()
{
    positionV = position;
    // texCoordV = tex_coord;
    // fnormalV = normalize(view*model*vec4(normal,0.0)).xyz;
}
