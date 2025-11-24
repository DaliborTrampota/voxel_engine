#pragma once

#include "utility/EventSite.h"

class ResizeEvent;

class EngineEventSite : public virtual engine::EventSite {
    friend class EngineEventSource;

  protected:
    virtual void windowResizeEvent(ResizeEvent* pEvent) {};
};