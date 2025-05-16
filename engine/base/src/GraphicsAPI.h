#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>

#include "gl/GLEventSource.h"

namespace gl {
    enum class GraphicsAPIType
    {
        None = 0,
        OpenGL,
    };
    
    class GraphicsAPI : public GLEventSource
    {
    public:
        GraphicsAPI() = default;
        ~GraphicsAPI() {
            destroy();
        };
    
        void init();
        void destroy();
        void registerCallbacks();
    
        void setRenderFlags();
        void clearScreen();
    
    
        GLFWwindow* window() const;
        void mouseLock(bool value);
        void setWindowSize(int x, int y) const;
        glm::ivec2 getWindowSize() const;
    
    private:
        GLFWwindow* m_window;
    };
}
