#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "gl/GLEventSite.h"

using namespace engine;

namespace gl {

    class GraphicsAPI;

    class Window : public GLEventSite {
      public:
        Window(std::unique_ptr<GraphicsAPI> api);
        ~Window();

        void windowResizeEvent(ResizeEvent* pEvent) override;

        void mouseLock(bool state) const;
        float time() const;
        void close();
        bool shouldClose() const;

        void beginFrame() const;
        void endFrame() const;
        void setRenderFlags() const;

        const glm::ivec2& size() const;

        GraphicsAPI* graphicsAPI() const { return m_api.get(); }

      private:
        std::unique_ptr<GraphicsAPI> m_api;
        glm::ivec2 m_size;
        bool m_close = false;
    };
}  // namespace gl