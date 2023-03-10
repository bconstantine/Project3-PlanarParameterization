#version 430

layout(location = 0) in vec3 vertex;

uniform mat4 um4mv;
uniform mat4 um4p;

void main()
{
	gl_Position = um4p * um4mv * vec4(vertex, 1.0);
}
