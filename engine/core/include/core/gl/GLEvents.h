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
    MouseEvent() : x(-1), y(-1) {}

    double x, y;

    bool clearEvent() const { return x == -1 && y == -1; }
};

class KeyboardEvent : public engine::Event {
  public:
    KeyboardEvent(int key, int scancode, int action, int mods)
        : key(key),
          scancode(scancode),
          action(action),
          mods(mods) {}

    // Event to clear the state
    KeyboardEvent() : action(-1), key(-1), scancode(-1), mods(-1) {}

    int key;
    int scancode;
    int action;
    int mods;

    bool clearEvent() const { return action == -1; }
};