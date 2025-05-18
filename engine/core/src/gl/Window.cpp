#include "Window.h"

#include "core/GraphicsAPI.h"
#include "core/gl/GLEvents.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace {
    constexpr Key fromGLFW(int key) {
        switch (key) {
            case GLFW_KEY_ESCAPE: return Key::Esc;
            case GLFW_KEY_SPACE: return Key::Space;
            case GLFW_KEY_W: return Key::W;
            case GLFW_KEY_A: return Key::A;
            case GLFW_KEY_S: return Key::S;
            case GLFW_KEY_D: return Key::D;
        }
        return Key::Esc;
    }

    constexpr int toGLFW(Key key) {
        switch (key) {
            case Key::Esc: return GLFW_KEY_ESCAPE;
            case Key::Space: return GLFW_KEY_SPACE;
            case Key::W: return GLFW_KEY_W;
            case Key::A: return GLFW_KEY_A;
            case Key::S: return GLFW_KEY_S;
            case Key::D: return GLFW_KEY_D;
        }
        return GLFW_KEY_ESCAPE;
    }
}  // namespace


gl::Window::Window(GraphicsAPI* api) : m_api(api) {
    m_size = api->getWindowSize();
    m_api->subscribe(this);
}

gl::Window::~Window() {
    glfwSetWindowShouldClose(m_api->m_window, true);
    delete m_api;
}

void gl::Window::windowResizeEvent(ResizeEvent* pEvent) {
    m_size.x = pEvent->width;
    m_size.y = pEvent->height;
    m_api->setWindowSize(m_size.x, m_size.y);
}

void gl::Window::mouseMoveEvent(MouseEvent* pEvent) {
    processMouse(pEvent->x, pEvent->y);
}

KeyState gl::Window::getKeyState(Key k) const {
    GLFWwindow* window = m_api->m_window;
    int state = glfwGetKey(window, static_cast<int>(toGLFW(k)));
    switch (state) {
        case GLFW_RELEASE: return KeyState::Released;
        case GLFW_PRESS: return KeyState::Pressed;
        case GLFW_REPEAT: return KeyState::Held;
    }
    return KeyState::None;
}

void gl::Window::close() {
    m_close = true;
    //glfwSetWindowShouldClose(m_api->m_window, true);
}

bool gl::Window::shouldClose() const {
    return m_close;  // glfwWindowShouldClose(m_api->m_window);
}

void gl::Window::gameloop() {
    m_api->setRenderFlags();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!m_close) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        m_api->clearScreen();

        beforeRender();
        render(deltaTime);
        afterRender();

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
            printf("OpenGL error: %d\n", err);

        glfwSwapBuffers(m_api->m_window);
        glfwPollEvents();
    }
}


void gl::Window::mouseLock(bool state) const {
    m_api->mouseLock(state);
}