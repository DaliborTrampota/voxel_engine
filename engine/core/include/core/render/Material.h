#pragma once

#include <string>
#include <glm/glm.hpp>
#include "../Globals.h"

#include <glad/glad.h>

namespace gl
{
    // Used to be named ShaderPipeline
    /// @brief Material class representing shader program.
    class Material {
        public:
            Material() = delete;
            Material(const std::string& vertexPath, const std::string& fragmentPath, std::string name);
            Material(const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath, std::string name);
            ~Material();

            void use() const;

            void setBool(const std::string& name, bool value) const;
            void setInt(const std::string& name, int value) const;
            void setFloat(const std::string& name, float value) const;
            void setMat4(const std::string& name, const glm::mat4& value) const;

            template <typename ...Shaders>
            void attach(Shaders&&... shaders) {
                GL_GUARD

                (glAttachShader(m_id, shaders.ID), ...);
            }

        protected:


        private:
            unsigned int m_id = 0;
            std::string m_name = "Unnamed";

            bool link();



    };

}