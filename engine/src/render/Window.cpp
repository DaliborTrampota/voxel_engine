#include "Window.h"

#include <glad/glad.h>

#include <stdexcept>


namespace {
    const char* getDebugSource(GLenum source) {
        switch (source) {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION: return "Application";
            case GL_DEBUG_SOURCE_OTHER: return "Other";
            default: return "Unknown";
        }
    }

    const char* getDebugType(GLenum type) {
        switch (type) {
            case GL_DEBUG_TYPE_ERROR: return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behavior";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behavior";
            case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
            case GL_DEBUG_TYPE_MARKER: return "Marker";
            case GL_DEBUG_TYPE_PUSH_GROUP: return "Push Group";
            case GL_DEBUG_TYPE_POP_GROUP: return "Pop Group";
            case GL_DEBUG_TYPE_OTHER: return "Other";
            default: return "Unknown";
        }
    }

    const char* getDebugSeverity(GLenum severity) {
        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH: return "High";
            case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
            case GL_DEBUG_SEVERITY_LOW: return "Low";
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
            default: return "Unknown";
        }
    }
}  // namespace

using namespace engine;

Window::Window(gl::Context* share) : gl::Context(share) {}

Window::~Window() {}

void Window::mouseLock(bool state) const {
    glfwSetInputMode(window(), GLFW_CURSOR, state ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}


int Window::init(int w, int h, const char* name) {
    int success = gl::Context::init(w, h, name);
    if (!success)
        return success;

    setDebugCallback(
        [](int source,
           int type,
           int id,
           int severity,
           int length,
           const GLchar* message,
           const void* userParam) -> void {
            if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
                return;
            printf(
                "GL DEBUG: source=%s, type=%s, id=%u, severity=%s\nMessage: %s\n",
                getDebugSource(source),
                getDebugType(type),
                id,
                getDebugSeverity(severity),
                message
            );
            if (type == GL_DEBUG_TYPE_ERROR)
                throw std::runtime_error("GL Error");
        }
    );

    setResizeCallback([](GLFWwindow* window, int width, int height) {
        // m_size.x = pEvent->width;
        // m_size.y = pEvent->height;
        // m_api->setWindowSize(m_size.x, m_size.y);
    });
    return 1;
}