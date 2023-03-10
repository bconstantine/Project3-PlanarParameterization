#pragma once

#include "../Include/Common.h"

class PickingTexture
{
public:
	PickingTexture();
	~PickingTexture();

	bool Init(int width, int height);
	void Enable();
	void Disable();
	GLuint ReadTexture(unsigned int x, unsigned int y);
	GLuint GetColorTex() { return fboColor; }

private:
	GLuint fbo;
	GLuint fboColor;
	GLuint fboDepth;

	void Clear();

};