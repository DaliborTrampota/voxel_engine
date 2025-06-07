#pragma once


namespace engine {

    enum KeyState {  // : uint8_t
        None = 0,
        Pressed = 1,
        Released = 2,
        Held = 4,
        Down = Pressed | Held,
    };

    // When adding new key, add glfw conversion to InputSystem.cpp
    enum Key {
        Unknown,
        Esc,
        Space,
        LShift,
        W,
        A,
        S,
        D,
        P,
        L,
        Home,
        End,  // This should be last
    };
}  // namespace engine