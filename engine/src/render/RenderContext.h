#pragma once

#include <optional>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <LWGL/buffer/IBuffer.h>

#include "RenderPass.h"
// #include <glad/glad.h>

namespace gl {
    class FBO;
}  // namespace gl

namespace engine {

    class Camera;
    class Material;

    // Represents OpenGL render state that can be batched
    // struct RenderState {
    //     std::optional<GLenum> depthFunc = std::nullopt;

    //     // For future: blend mode, cull face, etc.
    //     // std::optional<GLenum> blendSrc, blendDst;
    //     // std::optional<GLenum> cullFace;

    //     bool operator==(const RenderState& other) const { return depthFunc == other.depthFunc; }

    //     bool operator<(const RenderState& other) const {
    //         // Compare depthFunc for ordering (nullopt < GL_LESS < GL_LEQUAL < ...)
    //         if (!depthFunc.has_value() && !other.depthFunc.has_value())
    //             return false;
    //         if (!depthFunc.has_value())
    //             return true;
    //         if (!other.depthFunc.has_value())
    //             return false;
    //         return depthFunc.value() < other.depthFunc.value();
    //     }
    // };

    struct RenderContext {
        gl::IBuffer* attributes;
        const Material* material;
        const Camera* camera = nullptr;
        const gl::FBO* fbo = nullptr;
        RenderPass::ID passMask = RenderPass::Scene;

        struct {
            std::optional<glm::mat4> projection;
            std::optional<glm::mat4> view;
            glm::mat4 model;
        } matrices;


        void setModelMatrix(const glm::vec3& position) {
            matrices.model = glm::translate(glm::mat4(1.0f), position);
        }

        void setModelMatrix(const glm::vec3& position, float angle, const glm::vec3& axis) {
            matrices.model = glm::translate(glm::rotate(glm::mat4(1.0f), angle, axis), position);
        }
    };

    // Minimal context for batching - only contains per-drawcall data
    struct MinimalRenderContext {
        gl::IBuffer* attributes;
        glm::mat4 model;
    };

    // Groups multiple draw calls that share material, FBO, camera, etc.
    // Only model matrices and attributes differ between draw calls.
    //
    // Performance notes:
    // - Vector is heap-allocated, so stack overflow risk is minimal
    // - Use reserve() when you know the approximate count to avoid reallocations
    // - Best for batching 10+ draw calls with shared state
    // - For very large batches (1000+), consider splitting into multiple groups
    struct GroupRenderContext {
        const Material* material;
        const Camera* camera = nullptr;
        const gl::FBO* fbo = nullptr;
        RenderPass::ID passMask = RenderPass::Scene;

        struct {
            std::optional<glm::mat4> projection;
            std::optional<glm::mat4> view;
        } matrices;

        std::vector<MinimalRenderContext> drawCalls;

        // Reserve space to avoid reallocations when building large groups
        void reserve(size_t count) { drawCalls.reserve(count); }

        // Add a draw call (helper for convenience)
        void addDrawCall(gl::IBuffer* attributes, const glm::mat4& model) {
            drawCalls.push_back({attributes, model});
        }
    };

}  // namespace engine