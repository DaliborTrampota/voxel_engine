#pragma once

#include <memory>
#include <deque>

#include <core/Window.h>

namespace gl {
    class ShaderPipeline;
    class GraphicsAPI;
}

namespace engine {
    class Chunk;
    class World;
    class Camera;
    class Updateable;

    class Engine {
      public:
        Engine(std::unique_ptr<gl::Window> window);
        ~Engine() = default;

        void render(gl::ShaderPipeline* pipeline, Chunk* chunk);
        void render(gl::ShaderPipeline* pipeline, Camera* cam, std::shared_ptr<World> world);

        void subscribeUpdate(std::shared_ptr<Updateable> updateable);

        void fireUpdate(float dt);        

        virtual void beforeRender() {};
        virtual void render(double dt) = 0;
        virtual void afterRender() {};
        void gameloop();

        gl::Window* window() const { return m_window.get(); }

      private:
        std::unique_ptr<gl::Window> m_window;
        std::deque<std::weak_ptr<Updateable>> m_updateSubscribers; // Possibly revisit this to use a vector
    };
}  // namespace engine