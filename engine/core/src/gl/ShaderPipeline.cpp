#include "core/gl/ShaderPipeline.h"

#include <glm/gtc/type_ptr.hpp>

#include "core/gl/Shader.h"
#include "core/gl/VertexLayout.h"
#include "../Globals.h"

using namespace gl;


ShaderPipeline::ShaderPipeline(std::string name)
{
	GL_GUARD
    ID = glCreateProgram();
	this->name = name;
}

ShaderPipeline::~ShaderPipeline()
{
    GL_GUARD
    glDeleteProgram(ID);
    for (auto& shader : shaders)
    {
        glDeleteShader(shader.second.ID);
    }
}

bool ShaderPipeline::registerShader(GLenum type, const Shader& shader)
{
	GL_GUARD
	glAttachShader(ID, shader.ID);
	if (shaders.find(type) != shaders.end()) return false;
	shaders[type] = shader;
	return true;
}

bool ShaderPipeline::link()
{
	GL_GUARD
    glLinkProgram(ID);

	for (auto& shader : shaders)
    {
		glDeleteShader(shader.second.ID);
	}

    int  success;
    char infoLog[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM\n%s", infoLog);
    }
    return success;
}

void ShaderPipeline::use()
{
	GL_GUARD
	glUseProgram(ID);
}

void ShaderPipeline::setViewMatrix(glm::mat4 view) const
{
	setMat4("view", view);
}

void ShaderPipeline::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ShaderPipeline::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderPipeline::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void ShaderPipeline::setMat4(const std::string& name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
