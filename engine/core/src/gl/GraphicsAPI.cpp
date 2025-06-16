#include "GraphicsAPI.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "../events/CallbackWrapper.h"
#include "../events/GLEvents.h"
#include "Globals.h"

using namespace gl;

std::thread::id glContextID;


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

void GraphicsAPI::init() {
    INIT_GL_THREAD

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); for apple

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    int w, h;
    glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &w, &h);


    w *= 0.6;
    h *= 0.6;

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    m_window = glfwCreateWindow(w, h, "LearnOpenGL", NULL, NULL);
    if (m_window == NULL) {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return exit(-1);
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);  // disable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD");
        return exit(-1);
    }

    glfwSetWindowPos(m_window, 500, 200);

    GLint max_layers, max_units;
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_units);
    printf("Max layers: %d\nMax units: %d\n", max_layers, max_units);

    registerCallbacks();
}

void GraphicsAPI::destroy() {
    glfwTerminate();
}


void GraphicsAPI::registerCallbacks() {
    if (!m_window)
        throw std::runtime_error("Calling registerCallbacks before calling init!");

    using MouseCB = void(GLFWwindow*, double, double);
    CallbackWrapper<MouseCB>::callback = [this](GLFWwindow* w, double x, double y) -> void {
        MouseEvent e{x, y};
        fireMouseMoveEvent(&e);
    };
    glfwSetCursorPosCallback(m_window, CallbackWrapper<MouseCB>::call);

    using ResizeCB = void(GLFWwindow*, int, int);
    CallbackWrapper<ResizeCB>::callback = [this](GLFWwindow* window, int width, int height) {
        ResizeEvent e{width, height};
        fireWindowResizeEvent(&e);
    };
    glfwSetFramebufferSizeCallback(m_window, CallbackWrapper<ResizeCB>::call);

    using KeyCB = void(GLFWwindow*, int, int, int, int);
    CallbackWrapper<KeyCB>::callback =
        [this](GLFWwindow* window, int key, int scancode, int action, int mods) {
            KeyboardEvent e{key, scancode, action, mods};
            fireKeyboardEvent(&e);
        };
    glfwSetKeyCallback(m_window, CallbackWrapper<KeyCB>::call);

    using DebugCB = void(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);
    CallbackWrapper<DebugCB>::callback = [this](
                                             GLenum source,
                                             GLenum type,
                                             GLuint id,
                                             GLenum severity,
                                             GLsizei length,
                                             const GLchar* message,
                                             const void* userParam
                                         ) -> void {
        printf(
            "GL DEBUG: source=%s, type=%s, id=%u, severity=%s\nMessage: %s\n",
            getDebugSource(source),
            getDebugType(type),
            id,
            getDebugSeverity(severity),
            message
        );
        throw std::runtime_error("GL Error");
    };

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(CallbackWrapper<DebugCB>::call, nullptr);
}

void GraphicsAPI::setRenderFlags() const {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
}

void GraphicsAPI::clearScreen() const {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsAPI::swapBuffers() const {
    glfwSwapBuffers(m_window);
}

void GraphicsAPI::mouseLock(bool state) {
    glfwSetInputMode(m_window, GLFW_CURSOR, state ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void GraphicsAPI::setWindowSize(int x, int y) const {
    glfwSetWindowSize(m_window, x, y);
}

glm::ivec2 GraphicsAPI::getWindowSize() const {
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return {w, h};
}
