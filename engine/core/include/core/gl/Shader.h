#pragma once

#include <glad/glad.h>

namespace gl {

    enum class ShaderType {
        Vertex = 0,
        Geometry = 1,
        Fragment = 2,


        Compute,
    };

    struct Shader {
        unsigned int ID;

        Shader(const char* path, ShaderType type);
        ~Shader();
    };
}  // namespace gl
