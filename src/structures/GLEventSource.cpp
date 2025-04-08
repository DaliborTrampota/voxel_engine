#include "GLEventSource.h"
#include "GLEventSite.h"

#include "GLEvents.h"

void GLEventSource::fireWindowResizeEvent(ResizeEvent* pEvent) {
    EventSource::fireEvent(&GLEventSite::windowResizeEvent, pEvent);
};

void GLEventSource::fireMouseMoveEvent(MouseEvent* pEvent) {
    EventSource::fireEvent(&GLEventSite::mouseMoveEvent, pEvent);
}