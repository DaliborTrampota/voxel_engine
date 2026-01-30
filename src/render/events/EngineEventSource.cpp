#include "EngineEventSource.h"
#include "EngineEventSite.h"
#include "EngineEvents.h"

using namespace engine;

void EngineEventSource::fireWindowResizeEvent(ResizeEvent* pEvent) {
    EventSource::fireEvent(&EngineEventSite::windowResizeEvent, pEvent);
};