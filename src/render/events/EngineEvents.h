#pragma once

#include "utility/Event.h"

namespace engine {
    struct ResizeEvent : public Event {
        ResizeEvent(int w, int h) : width(w), height(h) {}

        int width, height;
    };
}  // namespace engine
