#include "Window.h"

#include "core/gl/GraphicsAPI.h"
#include "core/gl/GLEvents.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>




gl::Window::Window(std::unique_ptr<GraphicsAPI> api) : m_api(std::move(api)) {
    m_size = api->getWindowSize();
    m_api->subscribe(this);
}

gl::Window::~Window() {
    glfwSetWindowShouldClose(m_api->m_window, true);
}

void gl::Window::windowResizeEvent(ResizeEvent* pEvent) {
    m_size.x = pEvent->width;
    m_size.y = pEvent->height;
    m_api->setWindowSize(m_size.x, m_size.y);
}

void gl::Window::close() {
    m_close = true;
    //glfwSetWindowShouldClose(m_api->m_window, true);
}

bool gl::Window::shouldClose() const {
    return m_close;  // glfwWindowShouldClose(m_api->m_window);
}

void gl::Window::beginFrame() const {
    m_api->clearScreen();
}

void gl::Window::endFrame() const {
    m_api->swapBuffers();
    glfwPollEvents();
}

void gl::Window::setRenderFlags() const {
    m_api->setRenderFlags();
}

float gl::Window::time() const {
    return (float)glfwGetTime();
}

void gl::Window::mouseLock(bool state) const {
    m_api->mouseLock(state);
}

const glm::ivec2& gl::Window::size() const {
    return m_size;
}