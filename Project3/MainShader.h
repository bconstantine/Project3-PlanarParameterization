#pragma once

#include "../Include/ShaderObject.h"

class MainShader : public ShaderObject
{
public:
	MainShader();
	~MainShader();

	bool Init();

	//matrices
	void SetModelView(const glm::mat4&);
	void SetProj(const glm::mat4&);
	void SetRotateTex(const glm::mat4&);

	//flags
	void SetLightingACK(bool);
	void SetDrawWireACK(bool);
	void SetDrawTexCoordACK(bool);
	void SetDrawTextureFragACK(bool);

	//color vectors
	void SetFaceColor(const glm::vec4&);
	void SetWireColor(const glm::vec4&);

	void setOffsetX(const float&);
	void setOffsetY(const float&);
	void setScalerXY(const float&);

	//texture binder
	void bindTexture(GLuint texture0);

private:
	//matrices ID
	GLuint projID;
	GLuint modelViewID;
	GLuint rotateTexID; //for uv rotation matrix
	//bool flag ID
	GLuint drawTexCoordACKID;
	GLuint drawTextureFragACKID;
	GLuint lightingACKID;
	GLuint drawWireACKID;
	//color vector ID
	GLuint faceColorID;
	GLuint wireColorID;
	//texture enumeration
	GLuint textureEnumerationID;

	GLuint offsetXID;
	GLuint offsetYID;

	GLuint scalerXYID;
};

class PickingShader : public ShaderObject
{
public:
	PickingShader();
	~PickingShader();

	bool Init();
	void SetMVMat(const GLfloat* value);
	void SetPMat(const GLfloat* value);

private:
	GLuint mvLocation;
	GLuint pLocation;
};

class DrawPickingFaceShader : public ShaderObject
{
public:
	DrawPickingFaceShader();
	~DrawPickingFaceShader();

	bool Init();
	void SetMVMat(const GLfloat* value);
	void SetPMat(const GLfloat* value);

private:
	GLuint mvLocation;
	GLuint pLocation;
};

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


