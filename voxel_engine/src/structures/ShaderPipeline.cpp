#include "ShaderPipeline.h"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>



ShaderPipeline::ShaderPipeline(std::string name) : m_activeCamera(nullptr)
{
    ID = glCreateProgram();
	this->name = name;
}

bool ShaderPipeline::registerShader(GLenum type, Shader shader)
{
	glAttachShader(ID, shader.ID);
	if (shaders.find(type) != shaders.end()) return false;
	shaders[type] = shader;
	return true;
}

bool ShaderPipeline::link()
{
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
        std::cout << "ERROR::SHADER::PROGRAM\n" << infoLog << std::endl;
    }
    return success;
}

void ShaderPipeline::use()
{
	glUseProgram(ID);
}

void ShaderPipeline::setCamera(Camera* camera)
{
	m_activeCamera = camera;
	setMat4("projection", camera->getProjection());
}

/* @param stride If tightly packed then = size*/
void ShaderPipeline::registerAttribute(uint32_t loc, uint32_t size, GLenum type, uint32_t stride, uint32_t offset)
{
	auto typeSize = sizeOfType(type);
	glVertexAttribPointer(loc, size, type, GL_FALSE, stride * typeSize, (void*)(offset * typeSize)); // always as floats in the shader (check glVertexAttribIPointer for ints and glVertexAttribLPointer for doubles)
	glEnableVertexAttribArray(loc);
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



GLsizei ShaderPipeline::sizeOfType(GLenum type)
{
	switch (type)
	{
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			return sizeof(GLbyte);
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
			return sizeof(GLshort);
		case GL_INT_2_10_10_10_REV:
		case GL_INT:
		case GL_UNSIGNED_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT:
			return sizeof(GLint);
		case GL_FLOAT:
			return sizeof(GLfloat);
		case GL_DOUBLE:
			return sizeof(GLdouble);
		case GL_HALF_FLOAT:
			return sizeof(GLhalf);
	}
	throw std::exception("Unknown type");
}
