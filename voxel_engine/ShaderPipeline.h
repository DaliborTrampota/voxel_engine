#pragma once

#include <glad/glad.h>
#include "Shader.h"

#include <string>
#include <map>

class ShaderPipeline
{
public:
	uint32_t ID;
	std::map<GLenum, Shader> shaders;
	std::string name;

	ShaderPipeline(std::string name = "Unnamed");

	bool registerShader(GLenum type, Shader shader);
	Shader* getShader(GLenum type) { return &shaders[type]; }
	bool link();

	void use() { glUseProgram(ID); }

	void registerAttribute(uint32_t loc, uint32_t size, GLenum type, uint32_t stride, uint32_t offset = 0);

    void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

	static GLsizei sizeOfType(GLenum type);
};

