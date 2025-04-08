#pragma once

#include "events/EventSource.h"

class ResizeEvent;
class MouseEvent;

class GLEventSource : public EventSource
{
    friend class GLEventSite;
protected:

    void fireWindowResizeEvent(ResizeEvent* pEvent);
    void fireMouseMoveEvent(MouseEvent* pEvent);
};