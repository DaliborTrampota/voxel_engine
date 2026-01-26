#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "RenderContext.h"
#include "RenderPass.h"
#include "Window.h"

namespace engine {
    class Updateable;
    class Renderable;
    class Tickable;
    class Sun;
    class InputSystem;

    struct RenderStats {
        uint32_t drawCalls = 0;
        uint32_t vertices = 0;
        uint32_t triangles = 0;
        double frameTime = 0.0;
        double _frameStart = 0.0;

        void reset() {
            drawCalls = 0;
            vertices = 0;
            triangles = 0;
            frameTime = 0.0;
        }

        std::string toString() const;
    };

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
        void subscribeTick(std::shared_ptr<Tickable> tickable);
        void fireUpdate(float dt);
        void gameloop();

        virtual void beforeRender() {};
        virtual void render(double dt) = 0;
        virtual void afterRender() {};

        Window* window() const { return m_window.get(); }
        InputSystem* inputSystem() const { return m_inputSystem.get(); }

        void setDirectionalLightSource(std::shared_ptr<Sun> lightSource, uint8_t passPosition = 0);
        std::shared_ptr<Sun> directionalLightSource() const { return m_directionalLightSource; }

      protected:
        std::unique_ptr<InputSystem> m_inputSystem;
        std::shared_ptr<Sun> m_directionalLightSource;

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
        mutable RenderStats m_renderStats;
    };
}  // namespace engine