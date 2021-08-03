#version 330 core
layout(location = 0) in vec3 coord3d;

uniform mat4 uniform_view_matrix;
uniform mat4 uniform_projection_matrix;

out float rand1; // goind to fragment shader.

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(void) 
{
	gl_Position = uniform_projection_matrix * uniform_view_matrix * vec4(coord3d, 1.0);
	//rand1 = rand(coord3d.xy);
}
