#pragma once

#include <cassert>
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
        static constexpr ID Scene = 1 << 0;
        static constexpr ID SceneTransparent = 1 << 1;
        static constexpr ID DirectionalShadow = 1 << 2;
        static constexpr ID Composite = 1 << 3;
        static constexpr ID PointLightShadow = 1 << 4;
        static constexpr ID ClusterBuildPass = 1 << 5;
        static constexpr ID Tonemap = 1 << 6;

        virtual ~RenderPass() = default;

        RenderPass(const RenderPass&) = delete;
        RenderPass& operator=(const RenderPass&) = delete;
        RenderPass(RenderPass&&) noexcept = delete;
        RenderPass& operator=(RenderPass&&) noexcept = delete;

        virtual void beforeRender(Engine& engine, uint8_t pass) = 0;
        virtual void afterRender(Engine& engine, uint8_t pass) = 0;

        virtual void resize(glm::ivec2 resolution) { m_resolution = resolution; }

        ID id() const { return m_id; }
        virtual uint8_t passes() const { return m_passes; }
        virtual bool shouldRun() const { return true; }

        const Material* material = nullptr;
        const gl::FBO* fbo = nullptr;
        std::optional<glm::ivec2> viewportSize = std::nullopt;

      protected:
        glm::ivec2 m_resolution;

        /// @brief Reserves a free ID bit; call once per custom pass class, e.g. `inline static const ID Id = allocateId();`.
        static ID allocateId() {
            assert(s_nextPassIndex < 16 && "out of render pass ids");
            return static_cast<ID>(1u << s_nextPassIndex++);
        }

        /// @brief `id` is a built-in ID or one from allocateId(), shared by every instance of the class.
        explicit RenderPass(glm::ivec2 resolution, ID id, uint8_t passes = 1)
            : m_id(id),
              m_passes(passes),
              m_resolution(resolution) {}


      private:
        ID m_id;
        uint8_t m_passes;
        inline static unsigned int s_nextPassIndex = 7;
    };

}  // namespace engine