#include "Shader.h"
#include "../Globals.h"

#include <fstream>
#include <string>

using namespace gl;

namespace {
    constexpr GLenum shaderTypeToGL(ShaderType type) {
        switch (type) {
            case ShaderType::Vertex: return GL_VERTEX_SHADER;
            case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
            case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
            case ShaderType::Compute: return GL_COMPUTE_SHADER;
            default: return GL_NONE;  // Invalid type
        }
    }
}  // namespace

Shader::Shader(const char* path, ShaderType type) {
    GL_GUARD
    ID = glCreateShader(shaderTypeToGL(type));

    std::ifstream R(path);
    if (!R.is_open()) {
        printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
    }
    const std::string content(
        (std::istreambuf_iterator<char>(R)), std::istreambuf_iterator<char>()
    );
    R.close();


    const char* code = content.c_str();
    glShaderSource(ID, 1, &code, NULL);
    glCompileShader(ID);


    int success;
    char infoLog[512];
    glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ID, 512, NULL, infoLog);
        printf("ERROR::SHADER::COMPILATION_FAILED::%s\n%s", path, infoLog);
    }
    printf("Shader %s compiled %d\n", path, ID);
}

Shader::~Shader() {
    GL_GUARD
    glDeleteShader(ID);
}