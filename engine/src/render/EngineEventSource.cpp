#include "EngineEventSource.h"
#include "EngineEventSite.h"
#include "EngineEvents.h"

void EngineEventSource::fireWindowResizeEvent(ResizeEvent* pEvent) {
    engine::EventSource::fireEvent(&EngineEventSite::windowResizeEvent, pEvent);
};