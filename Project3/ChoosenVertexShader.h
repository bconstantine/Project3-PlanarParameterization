#pragma once

#include "../Include/ShaderObject.h"

class ChoosenVertexShader : public ShaderObject
{
public:
	ChoosenVertexShader();
	~ChoosenVertexShader();


	bool Init();
	void Enable();
	void Disable();
	//matrices
	void SetModelView(const glm::mat4&);
	void SetProj(const glm::mat4&);
	//color vector
	void SetColor(const glm::vec4&);
	//size(point)
	void SetSize(float pointSize);

private:
	GLuint projID;
	GLuint modelViewID;
	GLuint colorID;
};

