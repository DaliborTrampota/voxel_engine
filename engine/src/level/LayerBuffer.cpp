#include "LayerBuffer.h"

#include <glad/glad.h>

using namespace engine;

LayerBuffer::LayerBuffer() : front(GL_DYNAMIC_DRAW), back(GL_DYNAMIC_DRAW) {
    front.create();
    back.create();
}
