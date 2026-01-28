#include "TransparentPass.h"

#include <glad/glad.h>

#include "render/Engine.h"

using namespace engine;


TransparentPass::TransparentPass(glm::ivec2 resolution)
        2, RenderPassRegistry::Get().getPass<ScenePass>()->depthMap(), "opaqueDepthMap"
    );

void TransparentPass::beforeRender(Engine& engine, uint8_t pass) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (pass == 0) {
        glCullFace(GL_FRONT);
    } else {
        glCullFace(GL_BACK);
    }
}
void TransparentPass::afterRender(Engine& engine, uint8_t pass) {
    if (pass == 1)

};

