#pragma once

#include <cstdint>

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
};  // namespace engine