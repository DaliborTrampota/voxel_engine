#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <map>

#include "Shader.h"


namespace gl {
    struct VertexLayout;

    class ShaderPipeline
    {
    public:
        uint32_t ID;
        std::map<GLenum, Shader> shaders;
        std::string name;
    
        ShaderPipeline(std::string name = "Unnamed");
        ~ShaderPipeline();
    
        bool registerShader(GLenum type, const Shader& shader);
        Shader& getShader(GLenum type) { return shaders[type]; }
        bool link();
    
        void use();
    
        void setViewMatrix(glm::mat4 view) const;
    
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setMat4(const std::string& name, const glm::mat4 &value) const;
    };
}    
