#pragma once

#include <memory>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core/buffer/IBuffer.h>

namespace gl {
    class Material;
}

namespace engine {

    class Camera;

    struct RenderContext {
        gl::IBuffer* attributes;
        const gl::Material* material;
        glm::mat4 modelMatrix;
        const Camera* camera = nullptr;

        void setModelMatrix(const glm::vec3& position) {
            modelMatrix = glm::translate(glm::mat4(1.0f), position);
        }

        void setModelMatrix(const glm::vec3& position, float angle, const glm::vec3& axis) {
            modelMatrix = glm::translate(glm::rotate(glm::mat4(1.0f), angle, axis), position);
        }
    };
}  // namespace engine