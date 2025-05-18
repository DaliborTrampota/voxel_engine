#pragma once

#include <glm/glm.hpp>

#include "GLEventSite.h"
#include "Controls.h"

using namespace engine;

namespace gl {

    class GraphicsAPI;

    class Window : public GLEventSite {
        public:
            Window(GraphicsAPI* api);
            ~Window();

            void windowResizeEvent(ResizeEvent* pEvent) override;
            void mouseMoveEvent(MouseEvent* pEvent) override;

            KeyState getKeyState(Key k) const;
            virtual void processMouse(double x, double y) = 0;
            void mouseLock(bool state) const;

            void close();
            bool shouldClose() const;

            virtual void beforeRender() {};
            virtual void render(double dt) = 0;
            virtual void afterRender() {};
            void gameloop();

        private:
            GraphicsAPI* m_api;
            glm::ivec2 m_size;
            bool m_close = false;
    };
}