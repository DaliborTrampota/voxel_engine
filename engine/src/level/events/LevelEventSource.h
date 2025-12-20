#pragma once

#include "utility/EventSource.h"


namespace engine {
    struct ChunkUnloadEvent;
    struct ChunkBeforeLoadEvent;
    struct ViewDistanceUpdatedEvent;

    class LevelEventSource : public EventSource {
        friend class LevelEventSite;

      protected:
        void fireChunkUnloadEvent(ChunkUnloadEvent* pEvent);
        void fireChunkBeforeLoadEvent(ChunkBeforeLoadEvent* pEvent);
        // void fireViewDistanceUpdatedEvent(ViewDistanceUpdatedEvent* pEvent);
    };
}  // namespace engine