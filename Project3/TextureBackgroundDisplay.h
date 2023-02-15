#pragma once
#pragma once

#include "../Include/ShaderObject.h"

class TextureBackgroundDisplay : public ShaderObject
{
public:
	TextureBackgroundDisplay();
	~TextureBackgroundDisplay();

	bool Init();
	void SetModelView(const glm::mat4&);
	void SetProj(const glm::mat4&);
	void SetTex(GLuint texID);
	void SetBackgroundColor(const glm::vec4&);

private:
	//transform matrices
	GLuint modelViewID;
	GLuint projID;

	//texture enumeration
	GLuint texEnumerationID;

	//Color vector
	GLuint backgroundColorID;
};

