#version 430

out vec4 fragColor;

in VertexData
{
	vec3 inverseTransposeNormal;
	vec3 fragPos;
	vec2 vTexCoord;
	vec3 barycentric;
} vertexIn;

layout(binding = 0) uniform sampler2D tex0;
uniform bool lightingACK;
uniform bool drawWireACK;
uniform bool drawTextureFragACK;
uniform vec4 faceColor;
uniform vec4 wireColor;

float edgeFactor()
{
	//for drawing the edges
	vec3 d = fwidth(vertexIn.barycentric); //return the sum of the absolute value of derivatives in x and y
	vec3 a3 = smoothstep(vec3(0.0), d * 1.5, vertexIn.barycentric);
	return min(min(a3.x, a3.y), a3.z);
}

void main(void) 
{
	//vec4 faceColor = vec4(vec3(1.0) * lightIntense, 1.0);
	vec4 newFaceColor = faceColor;
	if (drawTextureFragACK)
	{
		//decide whether face is plain or apply texture
		
		vec4 texColor = texture(tex0, vertexIn.vTexCoord);
		newFaceColor = texColor;
//		if(texColor.x < 0.7 && texColor.y < 0.7 && texColor.z < 0.7){
//			//newFaceColor = vec4(0.9,0.9,0.9,1);
//		}

//		if(vertexIn.vTexCoord[0] < 0 || vertexIn.vTexCoord[1] < 0 || vertexIn.vTexCoord[0] >1 || vertexIn.vTexCoord[1] > 1){
//			newFaceColor = vec4(0.5,0.5,0.5,1);
//		}

//		if(vertexIn.vTexCoord[0] >= 0 || vertexIn.vTexCoord[1] >= 0){
//			newFaceColor = vec4(0.75,0.75,0.75,1);
//		}
	}

	if (lightingACK)
	{
		//phong lighting
		vec3 viewVector = -vertexIn.fragPos;
		vec3 lightDir = vec3(0, 0, -1);

		vec3 L = -lightDir;
		vec3 V = normalize(viewVector);
		vec3 N = normalize(vertexIn.inverseTransposeNormal);

		float ambient = 0.01;
		float diffuse = max(dot(N, L), 0);
		float specular = 0;
		if (diffuse > 0)
		{
			vec3 H = normalize(L + V);
			specular = pow(dot(N, H), 128);
		}

		newFaceColor = vec4(vec3(1.0) * (ambient + specular) + newFaceColor.xyz * diffuse, newFaceColor.a);
	}

	

	vec4 color = newFaceColor;
	//vec4 color = faceColor;
	if (drawWireACK)
	{
		float ef = edgeFactor();
		color = mix(wireColor, newFaceColor, ef); //return value is computed as x×(1−a)+y×a.
	}

	fragColor = color; //final result of frag color
}