#version 430

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

uniform mat4 um4mv;
uniform mat4 um4p;

out VertexData
{
	vec3 vNormal;
} vertexOut;

void main()
{
	vertexOut.vNormal = normal; //normal
	gl_Position = um4p * um4mv * vec4(vertex, 1.0); //position
}
