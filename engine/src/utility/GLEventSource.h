#pragma once

#include "EventSource.h"

class ResizeEvent;
class MouseEvent;
class MouseButtonEvent;
class KeyboardEvent;

class GLEventSource : public engine::EventSource {
    friend class GLEventSite;

  protected:
    void fireWindowResizeEvent(ResizeEvent* pEvent);
};