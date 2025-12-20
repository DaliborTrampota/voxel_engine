#pragma once

#include "utility/EventSite.h"

namespace engine {
    struct MouseEvent;
    struct MouseButtonEvent;
    struct KeyboardEvent;

    class GLFWEventSite : public virtual EventSite {
        friend class GLFWEventSource;

      protected:
        virtual void mouseMoveEvent(MouseEvent* pEvent) {};
        virtual void mouseButtonEvent(MouseButtonEvent* pEvent) {};
        virtual void keyboardEvent(KeyboardEvent* pEvent) {};
    };
}  // namespace engine