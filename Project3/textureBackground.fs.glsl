#version 430

out vec4 fragColor;

in VertexData
{
	vec2 texcoord;
} vertexData;

layout(binding = 0) uniform sampler2D displayTex;
uniform vec4 backgroundColor;

void main()
{
	vec4 texColor = texture(displayTex, vertexData.texcoord).rgba;
	fragColor = mix(texColor, backgroundColor, 0.5);
}
