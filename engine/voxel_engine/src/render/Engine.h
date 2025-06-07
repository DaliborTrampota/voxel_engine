#pragma once

#include <deque>
#include <memory>

#include <core/Window.h>

#include "RenderContext.h"

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
        Engine(std::unique_ptr<gl::Window> window);
        ~Engine() = default;

        void submitRender(RenderContext&& ctx, bool immediate = false);
        virtual void flush();

        void subscribeUpdate(std::shared_ptr<Updateable> updateable);
        void fireUpdate(float dt);
        void gameloop();

        virtual void beforeRender() {};
        virtual void render(double dt) = 0;
        virtual void afterRender() {};

        gl::Window* window() const { return m_window.get(); }


      protected:
        std::vector<RenderContext> m_renderQueue;

      private:
        std::unique_ptr<gl::Window> m_window;
        std::deque<std::weak_ptr<Updateable>>
            m_updateSubscribers;  // Possibly revisit this to use a vector


        void render(RenderContext& ctx) const;
    };
}  // namespace engine