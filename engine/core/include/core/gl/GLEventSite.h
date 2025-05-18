#pragma once

#include "../events/EventSite.h"

class MouseEvent;
class ResizeEvent;

class GLEventSite : public engine::EventSite {
    friend class GLEventSource;

  protected:
    virtual void windowResizeEvent(ResizeEvent* pEvent) {};
    virtual void mouseMoveEvent(MouseEvent* pEvent) {};
};