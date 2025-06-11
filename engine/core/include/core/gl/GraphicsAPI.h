#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>

#include "../events/GLEventSource.h"

struct GLFWwindow;

namespace gl {
    enum class GraphicsAPIType {
        None = 0,
        OpenGL,
    };

    class GraphicsAPI : public GLEventSource {
      public:
        GraphicsAPI() = default;
        ~GraphicsAPI() { destroy(); };

        void init();
        void destroy();
        void registerCallbacks();

        void setRenderFlags() const;
        void clearScreen() const;
        void swapBuffers() const;


        void mouseLock(bool state);
        void setWindowSize(int x, int y) const;
        glm::ivec2 getWindowSize() const;

      private:
        GLFWwindow* m_window = nullptr;

        friend class Window;
    };
}  // namespace gl
