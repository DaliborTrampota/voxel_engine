#pragma once

#include "utility/EventSource.h"

class MouseEvent;
class MouseButtonEvent;
class KeyboardEvent;

class GLFWEventSource : public engine::EventSource {
    friend class GLFWEventSite;

  protected:
    void fireMouseMoveEvent(MouseEvent* pEvent);
    void fireMouseButtonEvent(MouseButtonEvent* pEvent);
    void fireKeyboardEvent(KeyboardEvent* pEvent);
};