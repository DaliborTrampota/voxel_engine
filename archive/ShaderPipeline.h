#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <map>
#include <string>

#include "Shader.h"


namespace gl {
    struct VertexLayout;

    class ShaderPipeline {
      public:
        uint32_t ID;
        std::array<Shader, 4> shaders;  // 0: Vertex, 1: Fragment, 2: Geometry,
        std::string name;

        ShaderPipeline(std::string name = "Unnamed");
        ~ShaderPipeline();

        bool registerShader(ShaderType type, const Shader& shader);
        bool link();

        void use();

        void setViewMatrix(glm::mat4 view) const;

        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setMat4(const std::string& name, const glm::mat4& value) const;
    };
}  // namespace gl
