#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData
{
	vec3 inverseTransposeNormal;
	vec3 fragPos;
	vec2 vTexCoord;
} vertexIn[];

out VertexData
{
	vec3 inverseTransposeNormal;
	vec3 fragPos;
	vec2 vTexCoord;
	vec3 barycentric;
} vertexOut;

void main()
{
	vertexOut.inverseTransposeNormal = vertexIn[0].inverseTransposeNormal;
	vertexOut.fragPos = vertexIn[0].fragPos;
	vertexOut.vTexCoord = vertexIn[0].vTexCoord;
	vertexOut.barycentric = vec3(1, 0, 0);
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	vertexOut.inverseTransposeNormal = vertexIn[1].inverseTransposeNormal;
	vertexOut.fragPos = vertexIn[1].fragPos;
	vertexOut.vTexCoord = vertexIn[1].vTexCoord;
	vertexOut.barycentric = vec3(0, 1, 0);
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	vertexOut.inverseTransposeNormal = vertexIn[2].inverseTransposeNormal;
	vertexOut.fragPos = vertexIn[2].fragPos;
	vertexOut.vTexCoord = vertexIn[2].vTexCoord;
	vertexOut.barycentric = vec3(0, 0, 1);
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}