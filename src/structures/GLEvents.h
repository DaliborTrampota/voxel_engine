#pragma once

#include "events/Event.h"


class ResizeEvent : public Event {
public:
    ResizeEvent(int w, int h) : width(w), height(h) {}

    int width, height;
};

class MouseEvent : public Event {
public:
    MouseEvent(double x, double y) : x(x), y(y) {}
    
    double x, y;
};