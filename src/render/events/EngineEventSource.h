#pragma once

#include "utility/EventSource.h"

namespace engine {
    struct ResizeEvent;

    class EngineEventSource : public EventSource {
        friend class EngineEventSite;

      protected:
        void fireWindowResizeEvent(ResizeEvent* pEvent);
    };
}  // namespace engine