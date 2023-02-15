#include "ShaderObject.h"



ShaderObject::ShaderObject()
{
}


ShaderObject::~ShaderObject()
{
	for (std::list<GLuint>::iterator it = shaderList.begin(); it != shaderList.end(); ++it)
	{
		glDetachShader(program, *it);
		glDeleteShader(*it);
	}
	shaderList.clear();

	if (program != 0)
	{
		glDeleteProgram(program);
		program = 0;
	}

}

bool ShaderObject::Init()
{
	program = glCreateProgram();
	if (program == 0)
	{
		puts("Create program error\n");
		return false;
	}

	return true;
}

bool ShaderObject::AddShader(GLenum shaderType, std::string fileName)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
	{
		printf("Create shader error: %d\n", shaderType);
		return false;
	}

	shaderList.push_back(shader);

	char **shaderSource = Common::LoadShaderSource((fileName).c_str());

	glShaderSource(shader, 1, shaderSource, NULL);
	Common::FreeShaderSource(shaderSource);

	glCompileShader(shader);
	bool isCompiled = Common::CheckShaderCompiled(shader);
	/*if (!isCompiled)
	{
		printf("Shader compile error %s \n", fileName.c_str());
		return false;
	}*/
	glAttachShader(program, shader);
	bool checkError = Common::CheckGLError();
	if (fileName == "picking.vs.glsl") {
		//std::cout << "entering2: " << checkError << std::endl;
	}
	return checkError;
}

bool ShaderObject::Finalize()
{
	glLinkProgram(program);
	bool isLinked = Common::CheckProgramLinked(program);
	
	for (std::list<GLuint>::iterator it = shaderList.begin(); it != shaderList.end(); ++it)
	{
		glDetachShader(program, *it);
		glDeleteShader(*it);
	}
	shaderList.clear();

	return isLinked;
}

void ShaderObject::Enable()
{
	glUseProgram(program);
}

void ShaderObject::Disable()
{
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint ShaderObject::GetUniformLocation(const GLchar* name)
{
	return glGetUniformLocation(program, name);
}