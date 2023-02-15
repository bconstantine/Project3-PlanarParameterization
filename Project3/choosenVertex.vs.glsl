#version 430

layout(location = 0) in vec3 pos;

uniform mat4 modelView;
uniform mat4 proj;

void main()
{
	gl_Position = proj * modelView * vec4(pos, 1.0);
}
