#pragma once

#include <cstddef>
#include <vector>

#include <LWGL/buffer/Attributes.h>
#include <LWGL/indirect/VertexPool.h>
#include "block/Vertex.h"


namespace engine {

    struct LayerData {
        std::vector<Vertex> vertices;
        bool dirty = false;

        void reserve(size_t n);
        void clear();
        void add(Vertex v);
        // const std::vector<Vertex>& data() const;
        size_t size() const;
    };

    struct LayerBuffer {
        LayerData front, back;
        gl::PoolAllocation allocation;

        LayerData& read() { return front; }
        LayerData& write() { return back; }
        void clear();
        void moveToFront();
        void uploadToPool(gl::VertexPool<Vertex>& pool);
        void releaseFromPool(gl::VertexPool<Vertex>& pool);
    };
}  // namespace engine