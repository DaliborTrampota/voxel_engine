#pragma once


namespace engine {

    enum KeyState {  // : uint8_t
        None = 0,
        Pressed = 1,
        Released = 2,
        Held = 4,
        Down = Pressed | Held,
    };

    enum Key {
        Esc,
        Space,
        W,
        A,
        S,
        D,
        Home,
        End,  // This should be last
    };
}  // namespace engine