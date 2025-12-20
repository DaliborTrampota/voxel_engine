#pragma once

#include "utility/EventSource.h"

namespace engine {
    struct MouseEvent;
    struct MouseButtonEvent;
    struct KeyboardEvent;

    class GLFWEventSource : public EventSource {
        friend class GLFWEventSite;

      protected:
        void fireMouseMoveEvent(MouseEvent* pEvent);
        void fireMouseButtonEvent(MouseButtonEvent* pEvent);
        void fireKeyboardEvent(KeyboardEvent* pEvent);
    };
}  // namespace engine