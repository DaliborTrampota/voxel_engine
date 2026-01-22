#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>


namespace gl {
    class FBO;
}  // namespace gl


namespace engine {
    class Engine;
    class Material;

    class RenderPass {
      public:
        using ID = uint16_t;

        // Built-in pass IDs
        static constexpr ID DirectionalShadow = 1 << 0;
        static constexpr ID OmniShadow = 1 << 1;
        static constexpr ID Scene = 1 << 2;
        static constexpr ID SceneTransparent = 1 << 3;

        virtual ~RenderPass() = default;

        virtual void beforeRender(Engine& engine, uint8_t pass) = 0;
        virtual void afterRender(Engine& engine, uint8_t pass) = 0;

        static std::unique_ptr<RenderPass> create(const std::string& name);

        ID id() const { return m_id; }
        uint8_t passes() const { return m_passes; }

        const Material* material = nullptr;
        const gl::FBO* fbo = nullptr;
        std::optional<glm::ivec2> viewportSize = std::nullopt;

      protected:
        glm::ivec2 m_resolution;

        /// @brief Constructs pass with next available ID, use only for custom passes.
        explicit RenderPass(glm::ivec2 resolution, uint8_t passes = 1)
            : m_id(1 << s_nextPassIndex++),
              m_passes(passes),
              m_resolution(resolution) {}

        /// @brief Constructs pass with specific ID, use only for built-in passes.
        explicit RenderPass(glm::ivec2 resolution, ID id, uint8_t passes = 1)
            : m_id(id),
              m_passes(passes),
              m_resolution(resolution) {}


      private:
        ID m_id;
        uint8_t m_passes;
        inline static unsigned int s_nextPassIndex = 4;
    };

}  // namespace engine