#include "GLFWEventSource.h"

#include "GLFWEventSite.h"

void GLFWEventSource::fireMouseMoveEvent(MouseEvent* pEvent) {
    engine::EventSource::fireEvent(&GLFWEventSite::mouseMoveEvent, pEvent);
}

void GLFWEventSource::fireMouseButtonEvent(MouseButtonEvent* pEvent) {
    engine::EventSource::fireEvent(&GLFWEventSite::mouseButtonEvent, pEvent);
}

void GLFWEventSource::fireKeyboardEvent(KeyboardEvent* pEvent) {
    engine::EventSource::fireEvent(&GLFWEventSite::keyboardEvent, pEvent);
}