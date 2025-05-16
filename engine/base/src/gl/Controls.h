#pragma once


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace engine {

    enum KeyState {
        None = 0,
        Pressed = 1,
        Released = 2,
        Held = 3,
    };

    enum Key {
        Esc = GLFW_KEY_ESCAPE,
        Space = GLFW_KEY_SPACE,
        W = GLFW_KEY_W,
        A = GLFW_KEY_A,
        S = GLFW_KEY_S,
        D = GLFW_KEY_D,
    };
}