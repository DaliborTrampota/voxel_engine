#pragma once

#include <array>
#include <cstddef>
#include "Controls.h"

#include <core/gl/GLEventSite.h>

namespace engine {

    enum Axis {
        Sideways,
        Forward,
        Vertical,
        MouseX,
        MouseY,
    };

    class InputSystem : public GLEventSite {
      public:
        InputSystem() = default;

        KeyState getKeyState(Key k);
        bool isKey(KeyState state, Key k) const;
        float getAxis(Axis axis);

        void keyboardEvent(KeyboardEvent* pEvent) override;
        void mouseMoveEvent(MouseEvent* pEvent) override;

      private:
        std::array<KeyState, static_cast<size_t>(Key::End) + 1> m_keyStates{KeyState::None};
        std::array<float, static_cast<size_t>(Axis::MouseY) + 1> m_axisStates{0.0f};

        float m_mouseX = 0.0f;
        float m_mouseY = 0.0f;

        void setAxis(Axis axis, float value);
    };
}  // namespace engine