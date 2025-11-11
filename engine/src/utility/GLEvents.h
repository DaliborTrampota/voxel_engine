#pragma once

#include "Event.h"


class ResizeEvent : public engine::Event {
  public:
    ResizeEvent(int w, int h) : width(w), height(h) {}

    int width, height;
};
