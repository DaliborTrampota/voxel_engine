#pragma once

#include "../events/EventSource.h"

class ResizeEvent;
class MouseEvent;
class KeyboardEvent;

class GLEventSource : public engine::EventSource {
    friend class GLEventSite;

  protected:
    void fireWindowResizeEvent(ResizeEvent* pEvent);
    void fireMouseMoveEvent(MouseEvent* pEvent);
    void fireKeyboardEvent(KeyboardEvent* pEvent);
};