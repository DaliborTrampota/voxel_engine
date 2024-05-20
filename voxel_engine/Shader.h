#pragma once

#include <glad/glad.h>

class Shader
{
public:
	unsigned int ID;

	Shader(const char* path, GLenum shaderType);
	Shader() = default;

};

