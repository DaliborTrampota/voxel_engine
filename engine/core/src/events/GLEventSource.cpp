#include "GLEventSource.h"
#include "GLEventSite.h"

#include "GLEvents.h"

void GLEventSource::fireWindowResizeEvent(ResizeEvent* pEvent) {
    engine::EventSource::fireEvent(&GLEventSite::windowResizeEvent, pEvent);
};

void GLEventSource::fireMouseMoveEvent(MouseEvent* pEvent) {
    engine::EventSource::fireEvent(&GLEventSite::mouseMoveEvent, pEvent);
}

void GLEventSource::fireKeyboardEvent(KeyboardEvent* pEvent) {
    engine::EventSource::fireEvent(&GLEventSite::keyboardEvent, pEvent);
}