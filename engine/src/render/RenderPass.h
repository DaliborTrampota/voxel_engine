#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <optional>


namespace gl {
    class FBO;
}  // namespace gl

namespace engine {
    class Material;
}  // namespace engine

namespace engine::RenderPass {
    using ID = uint16_t;

    /// @brief Configuration for a render pass.
    /// @note Material and FBO overrides are applied to all contexts in the pass.
    struct Config {
        ID id;
        const Material* materialOverride = nullptr;
        gl::FBO* fboOverride = nullptr;
        std::optional<glm::ivec2> viewportSize = std::nullopt;
    };

    constexpr ID DirectionalShadow = 1 << 0;
    constexpr ID OmniShadow = 1 << 1;
    constexpr ID SceneTransparent = 1 << 2;
    constexpr ID Scene = 1 << 3;

    /// @brief Creates a new render pass configuration.
    /// @return A new RenderPass::Config with id set to the next available ID.
    /// @note The first three render pass IDs are reserved for shadows and scene.
    /// @note The maximum number of render passes is 16.
    /// @note The render passes are rendered in the order of their creation.
    /// @note It is consumers responsibility to store the render pass ID and use it later.
    [[nodiscard]] inline Config make() {
        static ID s_passIndex = 4;
        return {.id = static_cast<ID>(1 << s_passIndex++)};
    }


}  // namespace engine::RenderPass