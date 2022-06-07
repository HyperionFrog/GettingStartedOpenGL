// Fragment shader
#version 460 core

in vec3 o_frag_coord;
in vec2 o_tex_coord;

out vec4 frag_color;

//uniform vec3 u_color;
uniform sampler2D u_texture;

void main()
{
	frag_color = texture(u_texture, o_tex_coord);
}
