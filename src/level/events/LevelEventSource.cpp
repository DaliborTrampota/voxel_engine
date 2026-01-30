#include "LevelEventSource.h"
#include "LevelEventSite.h"
#include "LevelEvents.h"

using namespace engine;

void LevelEventSource::fireChunkUnloadEvent(ChunkUnloadEvent* pEvent) {
    EventSource::fireEvent(&LevelEventSite::chunkUnloadEvent, pEvent);
};

void LevelEventSource::fireChunkBeforeLoadEvent(ChunkBeforeLoadEvent* pEvent) {
    EventSource::fireEvent(&LevelEventSite::chunkBeforeLoadEvent, pEvent);
};

// void LevelEventSource::fireViewDistanceUpdatedEvent(ViewDistanceUpdatedEvent* pEvent) {
//     EventSource::fireEvent(&LevelEventSite::viewDistanceUpdatedEvent, pEvent);
// };