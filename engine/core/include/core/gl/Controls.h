#pragma once


namespace engine {

    enum KeyState {
        None = 0,
        Pressed = 1,
        Released = 2,
        Held = 3,
    };

    enum Key {
        Esc,
        Space,
        W,
        A,
        S,
        D,
    };
}  // namespace engine