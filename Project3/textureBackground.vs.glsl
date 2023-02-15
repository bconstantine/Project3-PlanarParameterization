#version 430
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoord;
//uniform mat4 um4mv;
//uniform mat4 um4p;
//
//uniform vec2 offset;
//
//out VertexData
//{
//	vec2 texcoord;
//} vertexData;
//
//const vec2[] points = vec2[4](vec2(-0.5, -0.5), vec2(0.5, -0.5), vec2(-0.5, 0.5), vec2(0.5, 0.5));
//const vec2[] uv = vec2[4](vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1));
//
//void main()
//{
//	gl_Position = um4p * um4mv * vec4(aPos, 1.0);
//	vertexData.texcoord = aTexCoord + offset;
//}

uniform mat4 modelView;
uniform mat4 proj;

out VertexData
{
	vec2 texcoord;
} vertexData;

const vec2[] points = vec2[4](vec2(-0.5, -0.5), vec2(0.5, -0.5), vec2(-0.5, 0.5), vec2(0.5, 0.5));
const vec2[] uv = vec2[4](vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1));

void main()
{
	gl_Position = proj * modelView * vec4(points[gl_VertexID], 0.0, 1.0);
	vertexData.texcoord = uv[gl_VertexID];
}