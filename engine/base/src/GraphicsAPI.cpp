#include "GraphicsAPI.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Globals.h"
#include "gl/CallbackWrapper.h"
#include "gl/GLEvents.h"

using namespace gl;

std::thread::id glContextID;

void GraphicsAPI::init() {

    INIT_GL_THREAD

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    if (m_window == NULL)
    {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return exit(-1);
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0); // disable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
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




void GraphicsAPI::registerCallbacks()
{
    using MouseCB = void(GLFWwindow*, double, double);
    CallbackWrapper<MouseCB>::callback = [this](GLFWwindow* w, double x, double y) -> void {
        MouseEvent e{ x, y };
        fireMouseMoveEvent(&e);
    };
    glfwSetCursorPosCallback(m_window, CallbackWrapper<MouseCB>::call);

    using ResizeCB = void(GLFWwindow*, int, int);
    CallbackWrapper<ResizeCB>::callback = [this](GLFWwindow* window, int width, int height) {
        ResizeEvent e{ width, height };
        fireWindowResizeEvent(&e);
    };

    glfwSetFramebufferSizeCallback(m_window, CallbackWrapper<ResizeCB>::call);
}

void GraphicsAPI::setRenderFlags()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
}

void GraphicsAPI::clearScreen()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsAPI::mouseLock(bool state)
{
    glfwSetInputMode(m_window, GLFW_CURSOR, state ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void GraphicsAPI::setWindowSize(int x, int y) const
{
    glfwSetWindowSize(m_window, x, y);
}

glm::ivec2 GraphicsAPI::getWindowSize() const
{
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return { w, h };
}
