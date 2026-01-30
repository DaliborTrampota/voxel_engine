#pragma once

#include "utility/EventSite.h"

namespace engine {
    struct ResizeEvent;

    class EngineEventSite : public virtual EventSite {
        friend class EngineEventSource;

      protected:
        virtual void windowResizeEvent(ResizeEvent* pEvent) {};
    };
}  // namespace engine