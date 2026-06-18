#include "LayerBuffer.h"

#include <LWGL/indirect/VertexPool.h>
#include <block/Vertex.h>
#include <glad/glad.h>

using namespace engine;

void LayerData::add(Vertex v) {
    vertices.push_back(v);
    dirty = true;
}

void LayerData::clear() {
    if (vertices.size() != 0)
        dirty = true;
    vertices.clear();
}

void LayerData::reserve(size_t n) {
    vertices.reserve(n);
}

size_t LayerData::size() const {
    return vertices.size();
}

void LayerBuffer::clear() {
    back.clear();
}

void LayerBuffer::moveToFront() {
    front = std::move(back);
    back.clear();
    back.dirty = false;
    front.dirty = true;
}

void LayerBuffer::uploadToPool(gl::VertexPool<Vertex>& pool) {
    if (!front.dirty)
        return;
    if (allocation.isValid()) {
        pool.free(allocation);
    }
    if (front.vertices.empty()) {
        allocation = {};
        front.dirty = false;
        return;
    }
    allocation = pool.allocate(front.vertices);
    if (allocation.isValid())
        front.dirty = false;
}

void LayerBuffer::releaseFromPool(gl::VertexPool<Vertex>& pool) {
    if (allocation.isValid()) {
        pool.free(allocation);
        allocation = {};
    }
}
