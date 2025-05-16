#include "Window.h"

#include "../GraphicsAPI.h"
#include "GLEvents.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

gl::Window::Window(GraphicsAPI* api) : m_api(api) {
    m_size = api->getWindowSize();
    m_api->subscribe(this);
}

gl::Window::~Window() {
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

KeyState gl::Window::getKeyState(Key k) const
{
    GLFWwindow* window = m_api->window();
    int state = glfwGetKey(window, static_cast<int>(k));
    switch(state) {
        case GLFW_RELEASE:
            return KeyState::Released;
        case GLFW_PRESS:
            return KeyState::Pressed;
        case GLFW_REPEAT:
            return KeyState::Held;
    }
    return KeyState::None;
}

void gl::Window::close() {
    glfwSetWindowShouldClose(m_api->window(), true);
}

bool gl::Window::shouldClose() const
{
    return glfwWindowShouldClose(m_api->window());
}

void gl::Window::gameloop()
{
    m_api->setRenderFlags();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while(!shouldClose())
    {
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

        glfwSwapBuffers(m_api->window());
        glfwPollEvents();
    }
}


void gl::Window::mouseLock(bool state) const
{
    m_api->mouseLock(state);
}