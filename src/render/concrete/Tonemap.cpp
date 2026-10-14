#include "Tonemap.h"

#include <glad/glad.h>

#include "../RenderPassRegistry.h"
#include "CompositePass.h"
#include "render/utility/UtilityShaders.h"


using namespace engine;


void TonemapPass::init() {
    m_tonemapShader.setTexture(
        0, RenderPassRegistry::Get().getPass<CompositePass>()->color(), "hdrColor"
    );
}


void TonemapPass::beforeRender(Engine& engine, uint8_t pass) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_tonemapShader.use();
    m_tonemapShader.bindTextures();
    m_tonemapShader.setFloat("exposure", m_exposure);
    m_tonemapShader.setInt("tonemapper", static_cast<int>(m_mode));

    drawFullscreenTriangle();
}