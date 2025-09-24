#version 130

out vec2 tex_coord;
out vec4 color;

void main()
{
    // transform the vertex position
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    // transform the texture coordinates
    tex_coord = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy;

    // forward the vertex color
    color = vec4(1.0, 1.0, 1.0, 1.0);
}
