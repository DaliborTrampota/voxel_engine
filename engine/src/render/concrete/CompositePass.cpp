#include "CompositePass.h"

#include <LWGL/GLTypes.h>
#include <LWGL/texture/TextureBase.h>
#include <LWGL/texture/TextureRef.h>

#include <glad/glad.h>

#include "render/Engine.h"
#include "render/RenderPassRegistry.h"
#include "render/concrete/ScenePass.h"
#include "render/concrete/TransparentPass.h"


using namespace engine;

void CompositePass::beforeRender(Engine& engine, uint8_t pass) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    // glDepthFunc(GL_ALWAYS);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_compositeShader.use();
    m_compositeShader.bindTextures();

    static unsigned dummyVAO = 0;
    if (dummyVAO == 0) {
        glGenVertexArrays(1, &dummyVAO);
    }
    glBindVertexArray(dummyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
void CompositePass::afterRender(Engine& engine, uint8_t pass) {}


void CompositePass::init() {
    m_compositeShader.use();

    TransparentPass* tPass = RenderPassRegistry::Get().getPass<TransparentPass>();
    ScenePass* sPass = RenderPassRegistry::Get().getPass<ScenePass>();
    m_compositeShader.setTexture(0, tPass->accumulation(), "accum");
    m_compositeShader.setTexture(1, tPass->revealage(), "reveal");
    m_compositeShader.setTexture(2, sPass->color(), "opaque");
}