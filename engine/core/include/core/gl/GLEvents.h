#pragma once

#include "../events/Event.h"


class ResizeEvent : public engine::Event {
  public:
    ResizeEvent(int w, int h) : width(w), height(h) {}

    int width, height;
};

class MouseEvent : public engine::Event {
  public:
    MouseEvent(double x, double y) : x(x), y(y) {}
    MouseEvent(bool still) : x(0), y(0), still(still) {}

    double x, y;
    bool still = false;
};

class KeyboardEvent : public engine::Event {
    public:
    KeyboardEvent(int key, int scancode, int action, int mods)
        : key(key), scancode(scancode), action(action), mods(mods) {}

    int key;
    int scancode;
    int action;
    int mods;
};