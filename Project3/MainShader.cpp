#include "MainShader.h"
#include "ResourcePath.h"


MainShader::MainShader()
{

}


MainShader::~MainShader()
{

}

bool MainShader::Init()
{
	if (!ShaderObject::Init())
	{
		return false;
	}
	//std::cout << "enter here\n";
	if (!AddShader(GL_VERTEX_SHADER, "main.vs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_GEOMETRY_SHADER, "main.gs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "main.fs.glsl"))
	{
		//std::cout << "enter here\n";
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	//matrices
	//model view matrix
	modelViewID = GetUniformLocation("modelView");
	//projection matrix
	projID = GetUniformLocation("proj");
	//uv rotationMatrix
	rotateTexID = GetUniformLocation("rotateTex");
	
	//bool flags
	//boolean to draw or not texture coordinate
	drawTexCoordACKID = GetUniformLocation("drawTexCoordACK");
	//on fragment, whether to replace face color with texture
	drawTextureFragACKID = GetUniformLocation("drawTextureFragACK");
	//use lighting
	lightingACKID = GetUniformLocation("lightingACK");
	//wire drawn or not
	drawWireACKID= GetUniformLocation("drawWireACK");

	//color vectors
	//face (when no texture displayed)
	faceColorID = GetUniformLocation("faceColor");
	wireColorID = GetUniformLocation("wireColor");

	//texture enumeration
	textureEnumerationID = GetUniformLocation("tex0");
	glUseProgram(program);
	glUniform1i(textureEnumerationID, 0);


	offsetXID = GetUniformLocation("xOffset");
	offsetYID = GetUniformLocation("yOffset");
	scalerXYID = GetUniformLocation("scalerXY");
	/*if (modelViewID == -1 || projID == -1 || rotateTexID == -1 || drawTexCoordACKID == -1 || drawTextureFragACKID == -1 || lightingACKID == -1 || drawWireACKID == -1 || faceColorID == -1 || wireColorID = -1) {
		std::cout << "Fault\n";
	}*/

	//if (modelViewID == -1) {
	//	std::cout << "Fault\n";
	//}
	//std::cout << "enter her2e\n";
	return true;
}

void MainShader::SetModelView(const glm::mat4& mat)
{
	glUniformMatrix4fv(modelViewID, 1, GL_FALSE, glm::value_ptr(mat));
}

void MainShader::SetProj(const glm::mat4& mat)
{
	glUniformMatrix4fv(projID, 1, GL_FALSE, glm::value_ptr(mat));
}


void MainShader::SetRotateTex(const glm::mat4& mat)
{
	glUniformMatrix4fv(rotateTexID, 1, GL_FALSE, glm::value_ptr(mat));
}

void MainShader::SetLightingACK(bool in)
{
	glUniform1i(lightingACKID, in);
}

void MainShader::SetDrawWireACK(bool in)
{
	glUniform1i(drawWireACKID, in);
}

void MainShader::SetDrawTexCoordACK(bool in)
{
	glUniform1i(drawTexCoordACKID, in);
}

void MainShader::SetDrawTextureFragACK(bool in)
{
	glUniform1i(drawTextureFragACKID, in);
}

void MainShader::SetFaceColor(const glm::vec4& color)
{
	glUniform4fv(faceColorID, 1, glm::value_ptr(color));
}

void MainShader::SetWireColor(const glm::vec4& color)
{
	glUniform4fv(wireColorID, 1, glm::value_ptr(color));
}

void MainShader::bindTexture(GLuint texture0) {
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texture0);
}

void MainShader::setOffsetX(const float &in) {
	glUniform1fv(offsetXID, 1, &in);
}

void MainShader::setOffsetY(const float &in) {
	glUniform1fv(offsetYID, 1, &in);
}

void MainShader::setScalerXY(const float &in) {
	glUniform1fv(scalerXYID, 1, &in);
}

ChoosenVertexShader::ChoosenVertexShader()
{
}

ChoosenVertexShader::~ChoosenVertexShader()
{
}

bool ChoosenVertexShader::Init()
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "choosenVertex.vs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "choosenVertex.fs.glsl"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	projID = GetUniformLocation("proj");
	modelViewID = GetUniformLocation("modelView");
	colorID = GetUniformLocation("color");
	return true;
}

void ChoosenVertexShader::Enable()
{
	ShaderObject::Enable();
	glPushAttrib(GL_POINT_BIT);
}

void ChoosenVertexShader::Disable()
{
	glPopAttrib();
	ShaderObject::Disable();
}

void ChoosenVertexShader::SetModelView(const glm::mat4& in)
{
	glUniformMatrix4fv(modelViewID, 1, GL_FALSE, glm::value_ptr(in));
}

void ChoosenVertexShader::SetProj(const glm::mat4& in)
{
	glUniformMatrix4fv(projID, 1, GL_FALSE, glm::value_ptr(in));
}

void ChoosenVertexShader::SetColor(const glm::vec4& in)
{
	glUniform4fv(colorID, 1, glm::value_ptr(in));
}

void ChoosenVertexShader::SetSize(float in)
{
	glPointSize(in);
}

DrawPickingFaceShader::DrawPickingFaceShader()
{
}


DrawPickingFaceShader::~DrawPickingFaceShader()
{
}

bool DrawPickingFaceShader::Init()
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "drawPickingFace.vs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_GEOMETRY_SHADER, "drawPickingFace.gs.glsl"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "drawPickingFace.fs.glsl"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	mvLocation = GetUniformLocation("um4mv");
	if (mvLocation == -1)
	{
		puts("Get uniform loaction error: um4mv");
		return false;
	}

	pLocation = GetUniformLocation("um4p");
	if (pLocation == -1)
	{
		puts("Get uniform loaction error: um4p");
		return false;
	}

	return true;
}

void DrawPickingFaceShader::SetMVMat(const GLfloat* value)
{
	glUniformMatrix4fv(mvLocation, 1, GL_FALSE, value);
}

void DrawPickingFaceShader::SetPMat(const GLfloat* value)
{
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, value);
}

PickingShader::PickingShader()
{
}


PickingShader::~PickingShader()
{
}

bool PickingShader::Init()
{
	if (!ShaderObject::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "picking.vs.glsl"))
	{
		std::cout << "1\n";
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "picking.fs.glsl"))
	{
		std::cout << "2\n";
		return false;
	}

	if (!Finalize())
	{
		std::cout << "3\n";
		return false;
	}

	mvLocation = GetUniformLocation("um4mv");
	if (mvLocation == -1)
	{
		puts("Get uniform loaction error: um4mv");
		return false;
	}

	pLocation = GetUniformLocation("um4p");
	if (pLocation == -1)
	{
		puts("Get uniform loaction error: um4p");
		return false;
	}

	return true;
}

void PickingShader::SetMVMat(const GLfloat* value)
{
	glUniformMatrix4fv(mvLocation, 1, GL_FALSE, value);
}

void PickingShader::SetPMat(const GLfloat* value)
{
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, value);
}

TextureBackgroundDisplay::TextureBackgroundDisplay()
{
}


TextureBackgroundDisplay::~TextureBackgroundDisplay()
{
}

bool TextureBackgroundDisplay::Init()
{
	//std::cout << "a-1\n";
	if (!ShaderObject::Init())
	{
		return false;
	}
	//std::cout << "a-1.1\n";
	if (!AddShader(GL_VERTEX_SHADER, "textureBackground.vs.glsl"))
	{
		//std::cout << "entering\n";
		return false;
	}
	//std::cout << "a-1.25\n";
	if (!AddShader(GL_FRAGMENT_SHADER, "textureBackground.fs.glsl"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}
	//std::cout << "a-1.5\n";
	modelViewID = GetUniformLocation("modelView");
	//std::cout << "a-2\n";

	projID = GetUniformLocation("proj");

	texEnumerationID = GetUniformLocation("displayTex");
	glUseProgram(program);
	glUniform1i(texEnumerationID, 0);

	backgroundColorID = GetUniformLocation("backgroundColor");
	//std::cout << "a-3\n";
	return true;
}

void TextureBackgroundDisplay::SetModelView(const glm::mat4& in)
{
	glUniformMatrix4fv(modelViewID, 1, GL_FALSE, glm::value_ptr(in));
}

void TextureBackgroundDisplay::SetProj(const glm::mat4& in)
{
	glUniformMatrix4fv(projID, 1, GL_FALSE, glm::value_ptr(in));
}

void TextureBackgroundDisplay::SetTex(GLuint texID) {
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, texID);
}

void TextureBackgroundDisplay::SetBackgroundColor(const glm::vec4& in) {
	glUniform4fv(backgroundColorID, 1, glm::value_ptr(in));
}