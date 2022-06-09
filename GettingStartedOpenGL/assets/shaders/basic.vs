#version 460 core

layout (location = 0) in vec3 i_pos_coord;
layout (location = 1) in vec2 i_tex_coord;

out vec2 o_tex_coord;

uniform mat4 u_model_mat;
uniform mat4 u_view_mat;
uniform mat4 u_projection_mat;
uniform mat4 u_comp_mat;

void main()
{
	// gl_Position = u_projection_mat * u_view_mat * u_model_mat * vec4(i_pos_coord, 1.0);
	gl_Position = u_comp_mat * vec4(i_pos_coord, 1.0);
    o_tex_coord = i_tex_coord;
}
