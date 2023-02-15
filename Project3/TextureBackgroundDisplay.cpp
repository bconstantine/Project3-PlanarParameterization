#include "TextureBackgroundDisplay.h"
#include "ResourcePath.h"


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
	if (!AddShader(GL_VERTEX_SHADER,"textureBackground.vs.glsl"))
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