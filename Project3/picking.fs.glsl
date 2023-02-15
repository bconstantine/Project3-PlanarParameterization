#version 430

out uint fragColor;

void main(void) 
{
	fragColor = gl_PrimitiveID + 1;
}