#pragma once

#include "utility/EventSite.h"

class MouseEvent;
class MouseButtonEvent;
class KeyboardEvent;

class GLFWEventSite : public virtual engine::EventSite {
    friend class GLFWEventSource;

  protected:
    virtual void mouseMoveEvent(MouseEvent* pEvent) {};
    virtual void mouseButtonEvent(MouseButtonEvent* pEvent) {};
    virtual void keyboardEvent(KeyboardEvent* pEvent) {};
};