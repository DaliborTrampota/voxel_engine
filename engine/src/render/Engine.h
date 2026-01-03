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
    class Tickable;
    class Sun;

    class Engine {
      public:
        static inline float TickRate = 1.0f / 30.0f;

        Engine(std::unique_ptr<Window> window);
        ~Engine() = default;

        void submitRender(RenderContext&& ctx, bool immediate = false);
        void submitRender(GroupRenderContext&& ctx, bool immediate = false);

        /// @brief Registers a custom render pass at the specified position.
        /// @param pass The concrete RenderPass instance.
        /// @param position Where to insert the pass (default: after scene)
        void registerRenderPass(std::unique_ptr<RenderPass> pass, uint8_t position);

        void registerDefaultRenderPasses();

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

        void setDirectionalLightSource(
            std::shared_ptr<engine::Sun> lightSource, uint8_t passPosition = 0
        );
        std::shared_ptr<engine::Sun> directionalLightSource() const {
            return m_directionalLightSource;
        }

      protected:
        std::shared_ptr<engine::Sun> m_directionalLightSource;

        std::vector<std::variant<RenderContext, GroupRenderContext>> m_renderQueue;
        std::unique_ptr<Window> m_window;

        void beginFrame();
        void endFrame();

      private:
        void initUtilityShaders();

        std::vector<std::unique_ptr<RenderPass>> m_renderPasses;
        std::vector<std::weak_ptr<Updateable>> m_updateSubscribers;
        std::vector<std::weak_ptr<Tickable>> m_tickSubscribers;
        float m_tickAccumulator = 0.0f;


        void render(RenderContext& ctx, const RenderPass* renderPass) const;
        void render(GroupRenderContext& ctx, const RenderPass* renderPass) const;
    };
}  // namespace engine