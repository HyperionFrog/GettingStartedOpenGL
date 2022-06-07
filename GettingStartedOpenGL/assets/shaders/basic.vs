// Vertex Shader
#version 460 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec3 i_color;
layout (location = 2) in vec2 i_tex_coord;

out vec3 o_frag_coord;
out vec2 o_tex_coord;

//uniform float uOffset;
//uniform mat4 uTransform;
//uniform mat4 uProjection;
//uniform mat4 uView;

void main()
{
    //fragCoord = aPosition.xy + vec2(0.5);
    //gl_Position = uProjection * uView * (uTransform * vec4(aPosition, 1.0));

	gl_Position = vec4(i_position, 1.0);
    o_frag_coord = i_color;
    o_tex_coord = i_tex_coord;

}
