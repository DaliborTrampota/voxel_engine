#pragma once

#include <LWGL/buffer/Attributes.h>
#include "block/Vertex.h"

namespace engine {
    struct LayerBuffer {
        gl::Attributes<Vertex> front;
        gl::Attributes<Vertex> back;

        LayerBuffer();

        gl::Attributes<Vertex>& read() { return front; }
        gl::Attributes<Vertex>& write() { return back; }
        void clear() { back.clear(); }
        void moveToFront() { front.swapData(back); }
    };
}  // namespace engine