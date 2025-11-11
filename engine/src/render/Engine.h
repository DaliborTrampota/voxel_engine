#pragma once

#include <memory>
#include <vector>


#include "RenderContext.h"
#include "Window.h"


namespace gl {
    class ShaderPipeline;
    class GraphicsAPI;
}  // namespace gl

namespace engine {
    class Chunk;
    class World;
    class Camera;
    class Updateable;
    class Renderable;

    class Engine {
      public:
        Engine(std::unique_ptr<Window> window);
        ~Engine() = default;

        void submitRender(RenderContext&& ctx, bool immediate = false);
        virtual void flush();

        void subscribeUpdate(std::shared_ptr<Updateable> updateable);
        void fireUpdate(float dt);
        void gameloop();

        virtual void beforeRender() {};
        virtual void render(double dt) = 0;
        virtual void afterRender() {};

        Window* window() const { return m_window.get(); }

      protected:
        std::vector<RenderContext> m_renderQueue;

        void beginFrame();
        void endFrame();

      private:
        std::unique_ptr<Window> m_window;
        std::vector<std::weak_ptr<Updateable>> m_updateSubscribers;


        void render(RenderContext& ctx) const;
    };
}  // namespace engine