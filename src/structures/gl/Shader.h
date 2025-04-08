#pragma once

#include <glad/glad.h>
#include "../Globals.h"

class Shader
{
public:
	unsigned int ID;

	Shader(const char* path, GLenum shaderType);
	Shader() = default;
	~Shader() {
		GL_GUARD
		glDeleteShader(ID);
	}
};

