#pragma once

#include <memory>

namespace gl {
    class ShaderPipeline;
}

namespace engine {
    class Chunk;
    class World;
    class Camera;

    class Engine
    {
        public:
            Engine() = default;
            ~Engine() = default;

            void render(gl::ShaderPipeline* pipeline, Chunk* chunk);
            void render(gl::ShaderPipeline* pipeline, Camera* cam, std::shared_ptr<World> world);

    };
}