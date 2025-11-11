#include "InputSystem.h"

#include "GLFWEvents.h"

using namespace engine;


InputSystem::InputSystem() {
    m_window = glfwGetCurrentContext();
    registerCallbacks();
}

InputSystem::InputSystem(GLFWwindow* window) : m_window(window) {
    registerCallbacks();
}

void InputSystem::beginFrame() {
    for (int i = 0; i < m_keyStates.size(); i++) {
        if (m_keyStates[i] == Pressed) {
            m_keyStates[i] = Held;
        } else if (m_keyStates[i] == Released) {
            m_keyStates[i] = None;
        }
    }

    setAxis(Axis::MouseX, 0.0f);
    setAxis(Axis::MouseY, 0.0f);
}

float InputSystem::getAxis(Axis axis) {
    float value = m_axisStates[static_cast<int>(axis)];
    if (axis == Axis::MouseX || axis == Axis::MouseY) {
        // Reset mouse axis after reading
        m_axisStates[static_cast<int>(axis)] = 0.0f;
    }
    return value;
}

void InputSystem::setAxis(Axis axis, float value) {
    m_axisStates[static_cast<int>(axis)] = value;
}

void InputSystem::registerCallbacks() {
    glfwSetCursorPosCallback(m_window, [](GLFWwindow* w, double x, double y) -> void {
        InputSystem* input = static_cast<InputSystem*>(glfwGetWindowUserPointer(w));
        MouseEvent ev{x, y};

        float dx = x - input->m_mouseX;
        float dy = input->m_mouseY - ev.y;  // reversed since y-coordinates go from bottom to top

        input->setAxis(Axis::MouseX, dx);
        input->setAxis(Axis::MouseY, dy);

        input->m_mouseX = x;
        input->m_mouseY = y;

        input->fireMouseMoveEvent(&ev);
    });

    glfwSetMouseButtonCallback(
        m_window, [](GLFWwindow* w, int button, int action, int mods) -> void {
            InputSystem* api = static_cast<InputSystem*>(glfwGetWindowUserPointer(w));
            MouseButtonEvent ev{button, action, mods};
            api->fireMouseButtonEvent(&ev);
        }
    );

    glfwSetKeyCallback(m_window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        InputSystem* input = static_cast<InputSystem*>(glfwGetWindowUserPointer(w));
        KeyboardEvent ev{key, scancode, action, mods};


        KeyState state = ev.action == GLFW_PRESS     ? KeyState::Pressed
                         : ev.action == GLFW_RELEASE ? KeyState::Released
                                                     : KeyState::Held;
        input->m_keyStates[input->keyIndex(key)] = state;


        float sideways = 0.f;
        float forward = 0.f;

        if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS)
            sideways -= 1.0f;
        if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS)
            sideways += 1.0f;
        if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS)
            forward -= 1.0f;
        if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS)
            forward += 1.0f;

        input->setAxis(Axis::Sideways, sideways);
        input->setAxis(Axis::Forward, forward);

        input->fireKeyboardEvent(&ev);
    });
}

int InputSystem::keyIndex(GLFWKey k) const {
    return k;  //static_cast<int>(k % GLFW_KEY_LAST);
}