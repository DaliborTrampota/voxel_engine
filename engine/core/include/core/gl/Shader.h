#pragma once

#include <glad/glad.h>

namespace gl
{
    class Shader
    {
    public:
        unsigned int ID;
    
        Shader(const char* path, GLenum shaderType);
        Shader() = default;
        ~Shader();
    };
}


