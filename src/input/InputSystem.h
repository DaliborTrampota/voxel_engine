#pragma once

#include <array>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "InputTypes.h"
#include "events/GLFWEventSource.h"


struct GLFWwindow;

namespace engine {

    class InputSystem : public GLFWEventSource {
      public:
        InputSystem();
        InputSystem(GLFWwindow* window);

        void beginFrame();

        template <KeyState state>
        bool isKey(GLFWKey k) const {
            static_assert(false, "Invalid key state");
            return false;
        }
        KeyState getKeyState(GLFWKey k) const;
        float getAxis(InputAxis axis);

        template <KeyState state>
        bool isMouse(GLFWKey k) const {
            static_assert(false, "Invalid key state");
            return false;
        }

      private:
        GLFWwindow* m_window = nullptr;

        std::array<KeyState, GLFW_KEY_LAST + 1> m_keyStates{KeyState::None};
        std::array<KeyState, GLFW_MOUSE_BUTTON_LAST + 1> m_mouseButtonStates{KeyState::None};
        std::array<float, static_cast<int>(InputAxis::End)> m_axisStates{0.0f};

        float m_mouseX = 0.0f;
        float m_mouseY = 0.0f;

        void setAxis(InputAxis axis, float value);

        void registerCallbacks();
        int keyIndex(GLFWKey k) const;
    };


    template <>
    inline bool InputSystem::isKey<KeyState::Pressed>(GLFWKey k) const {
        return m_keyStates[keyIndex(k)] == KeyState::Pressed;
    }

    template <>
    inline bool InputSystem::isKey<KeyState::Released>(GLFWKey k) const {
        return m_keyStates[keyIndex(k)] == KeyState::Released;
    }

    template <>
    inline bool InputSystem::isKey<KeyState::Held>(GLFWKey k) const {
        return m_keyStates[keyIndex(k)] == KeyState::Held;
    }

    template <>
    inline bool InputSystem::isKey<KeyState::Down>(GLFWKey k) const {
        KeyState state = m_keyStates[keyIndex(k)];
        return state & KeyState::Pressed || state & KeyState::Held;
    }

    template <>
    inline bool InputSystem::isKey<KeyState::Up>(GLFWKey k) const {
        KeyState state = m_keyStates[keyIndex(k)];
        return state & KeyState::Released || state & KeyState::None;
    }


    template <>
    inline bool InputSystem::isMouse<KeyState::Pressed>(GLFWKey k) const {
        return m_mouseButtonStates[keyIndex(k)] == KeyState::Pressed;
    }

    template <>
    inline bool InputSystem::isMouse<KeyState::Released>(GLFWKey k) const {
        return m_mouseButtonStates[keyIndex(k)] == KeyState::Released;
    }

    template <>
    inline bool InputSystem::isMouse<KeyState::Held>(GLFWKey k) const {
        return m_mouseButtonStates[keyIndex(k)] == KeyState::Held;
    }

    template <>
    inline bool InputSystem::isMouse<KeyState::Down>(GLFWKey k) const {
        KeyState state = m_mouseButtonStates[keyIndex(k)];
        return state & KeyState::Pressed || state & KeyState::Held;
    }

    template <>
    inline bool InputSystem::isMouse<KeyState::Up>(GLFWKey k) const {
        KeyState state = m_mouseButtonStates[keyIndex(k)];
        return state & KeyState::Released || state & KeyState::None;
    }

}  // namespace engine