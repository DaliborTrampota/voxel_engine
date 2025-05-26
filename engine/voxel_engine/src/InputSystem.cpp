#include "InputSystem.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <core/gl/GLEvents.h>
#include <iostream>

using namespace engine;

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

KeyState InputSystem::getKeyState(Key k) {
    return m_keyStates[static_cast<size_t>(k)];
}

bool InputSystem::isKey(KeyState state, Key k) const {
    return m_keyStates[static_cast<size_t>(k)] & state;
}

float InputSystem::getAxis(Axis axis) {
    float value = m_axisStates[static_cast<size_t>(axis)];
    if (axis == Axis::MouseX || axis == Axis::MouseY) {
        // Reset mouse axis after reading
        m_axisStates[static_cast<size_t>(axis)] = 0.0f;
    }
    return value;
}

void InputSystem::setAxis(Axis axis, float value) {
    m_axisStates[static_cast<size_t>(axis)] = value;
}

void InputSystem::keyboardEvent(KeyboardEvent* pEvent) {
    KeyState state = KeyState::None;
    switch (pEvent->action) {
        case GLFW_PRESS: state = KeyState::Pressed; break;
        case GLFW_RELEASE: state = KeyState::Released; break;
        case GLFW_REPEAT: state = KeyState::Held; break;
    }
    Key key = fromGLFW(pEvent->key);
    m_keyStates[key] = state;

    float sideways = getAxis(Axis::Sideways);
    float forward = getAxis(Axis::Forward);

    int multiplier = state == Released ? -1 : state & Down ? 1 : 0;

    if (key == Key::A) {
        sideways -= 1.0f * multiplier;
    }
    if (key == Key::D) {
        sideways += 1.0f * multiplier;
    }
    if (key == Key::W) {
        forward += 1.0f * multiplier;
    }
    if (key == Key::S) {
        forward -= 1.0f * multiplier;
    }


    setAxis(Axis::Sideways, sideways);
    setAxis(Axis::Forward, forward);
}

void InputSystem::mouseMoveEvent(MouseEvent* pEvent) {
    float dx = !pEvent->still * (pEvent->x - m_mouseX);
    float dy = !pEvent->still *
               (m_mouseY - pEvent->y);  // reversed since y-coordinates go from bottom to top

    setAxis(Axis::MouseX, dx);
    setAxis(Axis::MouseY, dy);

    if (pEvent->still)
        return;

    m_mouseX = pEvent->x;
    m_mouseY = pEvent->y;
}