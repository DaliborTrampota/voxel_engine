#pragma once

#include <glad/glad.h>

namespace gl {

    enum class ShaderType {
        Vertex = 0,
        Geometry = 1,
        Fragment = 2,


        Compute,
    };

    class Shader {
      public:
        unsigned int ID;

        Shader(const char* path, ShaderType type);
        Shader() = default;
        ~Shader();
    };
}  // namespace gl
