#pragma once

#include "utility/EventSite.h"

namespace engine {
    struct ChunkUnloadEvent;
    struct ChunkBeforeLoadEvent;
    // struct ViewDistanceUpdatedEvent;

    class LevelEventSite : public virtual EventSite {
        friend class LevelEventSource;

      protected:
        /// @brief Event fired when a chunk is unloaded.
        virtual void chunkUnloadEvent(ChunkUnloadEvent* pEvent) {};

        /// @brief Event fired when a chunk is about to be loaded.
        virtual void chunkBeforeLoadEvent(ChunkBeforeLoadEvent* pEvent) {};

        /// @brief Event fired when the view distance is updated.
        // virtual void viewDistanceUpdatedEvent(ViewDistanceUpdatedEvent* pEvent) {};
    };
}  // namespace engine