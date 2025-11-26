#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "RenderContext.h"
#include "RenderPass.h"
#include "Window.h"


namespace gl {
    class ShaderPipeline;
    class GraphicsAPI;
    class FBO;
}  // namespace gl

namespace engine {
    class Chunk;
    class World;
    class Camera;
    class Material;
    class Updateable;
    class Renderable;
    class Sun;


    class Engine {
      public:
        Engine(std::unique_ptr<Window> window);
        ~Engine() = default;

        void submitRender(RenderContext&& ctx, bool immediate = false);
        void submitRender(GroupRenderContext&& ctx, bool immediate = false);

        /// @brief Registers a render pass with optional material and FBO overrides.
        /// @param config Pass configuration including ID and optional overrides.
        void registerRenderPass(const RenderPass::Config& config);
        void setRenderPassOrder(const std::vector<RenderPass::ID>& order);

        /// @brief Flushes the render queue; loops over all render passes and renders all contexts.
        /// @note This is called between beforeRender() and afterRender().
        /// @note Can be overridden, consumer must clear the m_renderQueue.
        virtual void flush();

        void subscribeUpdate(std::shared_ptr<Updateable> updateable);
        void fireUpdate(float dt);
        void gameloop();

        virtual void beforeRender() {};
        virtual void render(double dt) = 0;
        virtual void afterRender() {};

        Window* window() const { return m_window.get(); }

        void setDirectionalLightSource(std::shared_ptr<engine::Sun> lightSource) {
            m_directionalLightSource = lightSource;
        }
        std::shared_ptr<engine::Sun> directionalLightSource() const {
            return m_directionalLightSource;
        }

      protected:
        std::shared_ptr<engine::Sun> m_directionalLightSource;

        struct {
            const Material* material = nullptr;
            const gl::FBO* fbo = nullptr;
        } m_renderOverride;

        std::vector<std::variant<RenderContext, GroupRenderContext>> m_renderQueue;
        std::unique_ptr<Window> m_window;

        void beginFrame();
        void endFrame();

        void setRenderOverride(const Material* material, gl::FBO* fbo) {
            m_renderOverride.material = material;
            m_renderOverride.fbo = fbo;
        }

        void clearRenderOverride() {
            m_renderOverride.material = nullptr;
            m_renderOverride.fbo = nullptr;
        }

      private:
        std::vector<RenderPass::Config> m_renderPasses;
        std::vector<std::weak_ptr<Updateable>> m_updateSubscribers;


        void render(RenderContext& ctx, RenderPass::ID renderPass) const;
        void render(GroupRenderContext& ctx, RenderPass::ID renderPass) const;
    };
}  // namespace engine