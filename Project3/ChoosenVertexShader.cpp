#include "ChoosenVertexShader.h"
#include "ResourcePath.h"

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
