#include "GLEventSource.h"
#include "GLEventSite.h"
#include "GLEvents.h"

void GLEventSource::fireWindowResizeEvent(ResizeEvent* pEvent) {
    engine::EventSource::fireEvent(&GLEventSite::windowResizeEvent, pEvent);
};