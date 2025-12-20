#pragma once

#include "utility/Event.h"

namespace engine {

    struct MouseEvent : public Event {
        MouseEvent(double x, double y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}

        float x, y;
    };

    struct MouseButtonEvent : public Event {
        MouseButtonEvent(int button, int action, int mods)
            : button(button),
              action(action),
              mods(mods) {}

        int button;
        int action;
        int mods;
    };

    struct KeyboardEvent : public Event {
        KeyboardEvent(int key, int scancode, int action, int mods)
            : key(key),
              scancode(scancode),
              action(action),
              mods(mods) {}

        int key;
        int scancode;
        int action;
        int mods;
    };
}  // namespace engine