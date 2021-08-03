#version 330 core
layout(location = 0) out vec4 out_color;

uniform vec3 uniform_color;
uniform float particles_type;

in float rand1;


void main(void) 
{
	vec3 color = uniform_color;
	
	out_color = vec4( color, 0.5);	//semi transparent.
}

