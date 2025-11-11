#pragma once

#include <array>
#include <cstdint>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "GLFWEventSource.h"

struct GLFWwindow;

namespace engine {

    using GLFWKey = int;

    enum Axis {
        Sideways,
        Forward,
        Vertical,
        MouseX,
        MouseY,
    };

    enum KeyState : uint8_t {
        None = 1,
        Released = 2,
        Pressed = 4,
        Held = 8,
        Down = Pressed | Held,
        Up = Released | None,
    };
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
        float getAxis(Axis axis);

      private:
        GLFWwindow* m_window = nullptr;

        std::array<KeyState, GLFW_KEY_LAST + 1> m_keyStates{KeyState::None};
        std::array<float, static_cast<int>(Axis::MouseY) + 1> m_axisStates{0.0f};

        float m_mouseX = 0.0f;
        float m_mouseY = 0.0f;

        void setAxis(Axis axis, float value);

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
}  // namespace engine