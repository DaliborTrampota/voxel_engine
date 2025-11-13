#pragma once

namespace engine {
    class InputSystem;
    class Window;

    struct GLFWUserPointer {
        InputSystem* input = nullptr;
        Window* window = nullptr;
    };

}  // namespace engine