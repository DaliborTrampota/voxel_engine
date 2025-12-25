#include "TransparentPass.h"

#include <glad/glad.h>

#include "render/Engine.h"

using namespace engine;

void TransparentPass::beforeRender(Engine& engine, uint8_t pass) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Two-pass rendering for correct transparency on convex objects:
    if (pass == 0) {
        // Pass 1: Render back faces first (cull front)
        glCullFace(GL_FRONT);
    } else {
        // Pass 2: Render front faces on top (cull back)
        glCullFace(GL_BACK);
    }
}
void TransparentPass::afterRender(Engine& engine, uint8_t pass) {
    if (pass == 1)
        glDisable(GL_BLEND);
};
