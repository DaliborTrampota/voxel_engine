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

    double x, y;
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