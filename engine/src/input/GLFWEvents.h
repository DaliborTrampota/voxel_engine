#pragma once

#include <utility/Event.h>

class MouseEvent : public engine::Event {
  public:
    MouseEvent(double x, double y) : x(x), y(y) {}

    double x, y;
};

class MouseButtonEvent : public engine::Event {
  public:
    MouseButtonEvent(int button, int action, int mods)
        : button(button),
          action(action),
          mods(mods) {}

    int button;
    int action;
    int mods;
};

class KeyboardEvent : public engine::Event {
  public:
    KeyboardEvent(int key, int scancode, int action, int mods)
        : key(key),
          scancode(scancode),
          action(action),
          mods(mods) {}

    int key;
    int scancode;
    int action;
    int mods;
};