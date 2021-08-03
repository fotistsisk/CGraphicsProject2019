#version 330
layout(location = 0) out vec4 out_color;

uniform sampler2D uniform_texture;
uniform mat4 uniform_projection_inverse_matrix;

in vec2 f_texcoord;

void main(void)
{
	vec4 color = texture2D(uniform_texture, f_texcoord);
	out_color = color;	
}