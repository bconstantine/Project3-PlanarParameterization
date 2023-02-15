#version 430

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 modelView;
uniform mat4 rotateTex;
uniform bool drawTexCoordACK;
uniform float xOffset;
uniform float yOffset;
uniform float scalerXY;


out VertexData
{
	vec3 inverseTransposeNormal;
	vec3 fragPos;
	vec2 vTexCoord;
} vertexOut;

void main()
{
	vec4 pos;
	if (drawTexCoordACK)
	{
		//for drawing on the right subwindow, drawing planar texture mapping
		vec4 offset = vec4(-0.5, -0.5, 0,0);
		pos = rotateTex * vec4(texCoord, 0.0, 1.0);
		pos = pos+offset;

//		vec2 offset = vec2(-0.5, -0.5);
//		pos = rotateTex*vec4(texCoord+offset, 0, 1);
		gl_Position = modelView* pos;
		gl_Position.x += (xOffset*scalerXY);
		gl_Position.y += (yOffset*scalerXY);
	}
	else
	{
		//drawing model on left subwindow
		pos = vec4(vertex, 1.0);
		gl_Position = proj * modelView * pos;
	}

	
	mat3 cutOffModelView = mat3(modelView);
	vertexOut.inverseTransposeNormal = normalize(transpose(inverse(cutOffModelView)) * normal); 
	vec4 viewModelPos = modelView * vec4(vertex, 1.0);
	vertexOut.fragPos = viewModelPos.xyz;
	vec4 rotatedTexCoord = rotateTex * vec4(texCoord, 0.0, 1.0);
	vertexOut.vTexCoord = rotatedTexCoord.xy;
}
