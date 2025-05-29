#include "Window.h"

#include "core/gl/GLEvents.h"
#include "core/gl/GraphicsAPI.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace gl;


Window::Window(std::unique_ptr<GraphicsAPI> api) : m_api(std::move(api)) {
    m_api->init();
    m_api->subscribe(this);
    m_size = m_api->getWindowSize();
}

Window::~Window() {
    glfwSetWindowShouldClose(m_api->m_window, true);
}

void Window::windowResizeEvent(ResizeEvent* pEvent) {
    m_size.x = pEvent->width;
    m_size.y = pEvent->height;
    m_api->setWindowSize(m_size.x, m_size.y);
}

void Window::close() {
    m_close = true;
    //glfwSetWindowShouldClose(m_api->m_window, true);
}

bool Window::shouldClose() const {
    return m_close;  // glfwWindowShouldClose(m_api->m_window);
}

void Window::beginFrame() const {
    m_api->clearScreen();
}

void Window::endFrame() const {
    m_api->swapBuffers();

    MouseEvent mEvent;
    m_api->fireMouseMoveEvent(&mEvent);

    KeyboardEvent kbEvent;
    m_api->fireKeyboardEvent(&kbEvent);


    glfwPollEvents();
}

void Window::setRenderFlags() const {
    m_api->setRenderFlags();
}

float Window::time() const {
    return (float)glfwGetTime();
}

void Window::mouseLock(bool state) const {
    m_api->mouseLock(state);
}

const glm::ivec2& Window::size() const {
    return m_size;
}