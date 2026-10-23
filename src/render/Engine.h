#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "RenderContext.h"
#include "RenderPassRegistry.h"
#include "Window.h"

#include "../scene/AmbientLight.h"


namespace gl {
    class CubeMapArray;
    class TextureArray;
}  // namespace gl

namespace engine {
    class Updateable;
    class Renderable;
    class Tickable;
    class DirectionalLight;
    class InputSystem;
    class PointLightManager;

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
        void submitRender(IndirectRenderContext&& ctx, bool immediate = false);

        /// @brief Flushes the render queue; loops over all render passes and renders all contexts.
        /// @note This is called between beforeRender() and afterRender().
        /// @note Can be overridden, consumer must clear the m_renderQueue.
        virtual void flush();

        void subscribeUpdate(std::shared_ptr<Updateable> updateable);
        void unsubscribeUpdate(Updateable* updateable);
        void subscribeTick(std::shared_ptr<Tickable> tickable);
        void unsubscribeTick(Tickable* tickable);
        void fireUpdate(float dt);
        void gameloop();

        virtual void beforeRender() {};
        virtual void render(double dt) = 0;
        virtual void afterRender() {};

        Window* window() const { return m_window.get(); }
        InputSystem* inputSystem() const { return m_inputSystem.get(); }

        DirectionalLight* directionalLightSource() const { return m_activeDirectionalLightSource; }
        gl::TextureArray& setDirectionalLightSource(DirectionalLight* lightSource, Camera* camera);
        gl::CubeMapArray& setPointLightSource(PointLightManager* pointLightManager, Camera* camera);
        /// @brief Pass nullptr to fall back to AmbientLight defaults
        void setAmbientLight(const AmbientLight* ambientLight);
        /// @brief Float uniform set on every material before it renders; keeps its value until set again.
        void setGlobalUniform(const std::string& name, float value) { m_globalUniforms[name] = value; }

        void clearDirectionalLightSource();
        void clearPointLightSource();

        // RenderPassRegistry* passRegistry() const { return m_passRegistry; }

      protected:
        std::unique_ptr<InputSystem> m_inputSystem;
        DirectionalLight* m_activeDirectionalLightSource = nullptr;
        PointLightManager* m_activePointLightManager = nullptr;
        const AmbientLight* m_ambientLight = nullptr;
        std::unordered_map<std::string, float> m_globalUniforms;

        RenderPassRegistry* m_passRegistry;
        std::vector<std::variant<RenderContext, GroupRenderContext, IndirectRenderContext>>
            m_renderQueue;
        std::unique_ptr<Window> m_window;

        void beginFrame();
        void endFrame();

        void configureMaterialBeforeRender(
            const Material* material,
            const Camera* camera,
            const std::optional<glm::mat4>& view,
            const std::optional<glm::mat4>& projection
        ) const;

      private:
        void initUtilityShaders();

        std::vector<std::weak_ptr<Updateable>> m_updateSubscribers;
        std::vector<std::weak_ptr<Tickable>> m_tickSubscribers;
        float m_tickAccumulator = 0.0f;


        void render(RenderContext& ctx, const RenderPass* renderPass) const;
        void render(GroupRenderContext& ctx, const RenderPass* renderPass) const;
        void render(IndirectRenderContext& ctx, const RenderPass* renderPass) const;
        mutable RenderStats m_renderStats;
    };
}  // namespace engine