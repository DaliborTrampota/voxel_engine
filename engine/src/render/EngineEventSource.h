#pragma once

#include "utility/EventSource.h"

class ResizeEvent;

class EngineEventSource : public engine::EventSource {
    friend class EngineEventSite;

  protected:
    void fireWindowResizeEvent(ResizeEvent* pEvent);
};