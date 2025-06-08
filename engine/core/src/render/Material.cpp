#include "render/Material.h"

#include <glm/gtc/type_ptr.hpp>
#include "gl/Shader.h"
#include "gl/UBO.h"

using namespace gl;


gl::Material::Material(
    const std::string& vertexPath,
    const std::string& geometryPath,
    const std::string& fragmentPath,
    std::string name
)
    : m_name(std::move(name)) {
    GL_GUARD
    m_id = glCreateProgram();

    Shader vert(vertexPath.c_str(), ShaderType::Vertex);
    Shader geom(geometryPath.c_str(), ShaderType::Geometry);
    Shader frag(fragmentPath.c_str(), ShaderType::Fragment);

    attach(vert, geom, frag);
    link();
}
gl::Material::Material(
    const std::string& vertexPath, const std::string& fragmentPath, std::string name
)
    : m_name(std::move(name)) {
    GL_GUARD
    m_id = glCreateProgram();

    Shader vert(vertexPath.c_str(), ShaderType::Vertex);
    Shader frag(fragmentPath.c_str(), ShaderType::Fragment);

    attach(vert, frag);
    link();
}

Material::~Material() {
    GL_GUARD
    glDeleteProgram(m_id);
}

void Material::use() const {
    GL_GUARD
    glUseProgram(m_id);
}


void Material::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void Material::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Material::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Material::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Material::bindUBO(const UBO& ubo) const {
    ubo.bindToProgram(m_id);
}


bool Material::link() {
    GL_GUARD
    glLinkProgram(m_id);

    int success;
    char infoLog[512];
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM\n%s", infoLog);
    }
    return success;
}