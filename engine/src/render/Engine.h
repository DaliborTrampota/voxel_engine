#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "RenderContext.h"
#include "Window.h"



namespace gl {
    class ShaderPipeline;
    class GraphicsAPI;
    class Material;
    class FBO;
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
        void submitRender(GroupRenderContext&& ctx, bool immediate = false);
        void registerRenderPass(RenderPass::ID pass);

        void subscribeUpdate(std::shared_ptr<Updateable> updateable);
        void fireUpdate(float dt);
        void gameloop();

        virtual void beforeRender() {};
        virtual void render(double dt) = 0;
        virtual void afterRender() {};

        Window* window() const { return m_window.get(); }

        void setRenderOverride(const gl::Material* material, gl::FBO* fbo) {
            m_renderOverride.material = material;
            m_renderOverride.fbo = fbo;
        }

        void clearRenderOverride() {
            m_renderOverride.material = nullptr;
            m_renderOverride.fbo = nullptr;
        }

      protected:
        struct {
            const gl::Material* material = nullptr;
            const gl::FBO* fbo = nullptr;
        } m_renderOverride;
        std::vector<std::variant<RenderContext, GroupRenderContext>> m_renderQueue;
        std::unique_ptr<Window> m_window;

        void beginFrame();
        void endFrame();

      private:
        std::vector<std::weak_ptr<Updateable>> m_updateSubscribers;


        void render(RenderContext& ctx) const;
        void render(GroupRenderContext& ctx) const;
    };
}  // namespace engine