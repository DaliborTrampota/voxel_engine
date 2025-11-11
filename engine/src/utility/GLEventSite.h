#pragma once

#include "EventSite.h"

class MouseEvent;
class MouseButtonEvent;
class ResizeEvent;
class KeyboardEvent;

class GLEventSite : public engine::EventSite {
    friend class GLEventSource;

  protected:
    virtual void windowResizeEvent(ResizeEvent* pEvent) {};
};