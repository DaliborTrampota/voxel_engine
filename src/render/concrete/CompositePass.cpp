#include "CompositePass.h"

#include <LWGL/GLTypes.h>
#include <LWGL/texture/TextureBase.h>
#include <LWGL/texture/TextureRef.h>

#include <glad/glad.h>

#include "render/Engine.h"
#include "render/RenderPassRegistry.h"
#include "render/concrete/ScenePass.h"
#include "render/concrete/TransparentPass.h"
#include "render/utility/UtilityShaders.h"


using namespace engine;

void CompositePass::beforeRender(Engine& engine, uint8_t pass) {
    // composite pass renders a full-screen triangle to screen and relies
    // on previously composited textures from earlier passes.
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    m_hdrFBO.bind();

    m_compositeShader.use();
    m_compositeShader.bindTextures();

    drawFullscreenTriangle();
    m_hdrFBO.unbind();
}

void CompositePass::init() {
    m_compositeShader.use();


    m_hdrTexture.create(
        gl::TextureParams(gl::TextureParams::ClampToEdge, gl::TextureParams::Linear)
    );
    m_hdrTexture.allocate(
        {.width = m_resolution.x,
         .height = m_resolution.y,
         .format = gl::ImageFormat::RGBPackedFloat,
         .dataType = gl::ImageDataType::Float}  // dataType choise doesnt matter
    );
    m_hdrFBO.bind();
    m_hdrFBO.attach(gl::FBOAttachment::Color, &m_hdrTexture);

    assert(m_hdrFBO.checkCompleteness() == 0);
    m_hdrFBO.unbind();

    fbo = &m_hdrFBO;

    TransparentPass* tPass = RenderPassRegistry::Get().getPass<TransparentPass>();
    ScenePass* sPass = RenderPassRegistry::Get().getPass<ScenePass>();
    m_compositeShader.setTexture(0, tPass->accumulation(), "accum");
    m_compositeShader.setTexture(1, tPass->revealage(), "reveal");
    m_compositeShader.setTexture(2, sPass->color(), "opaque");
}

void CompositePass::resize(glm::ivec2 resolution) {
    if (resolution.x == 0 || resolution.y == 0)
        return;

    m_resolution = resolution;

    // TODO duplication
    m_hdrTexture.allocate(
        {.width = m_resolution.x,
         .height = m_resolution.y,
         .format = gl::ImageFormat::RGBPackedFloat,
         .dataType = gl::ImageDataType::Float}
    );

    assert(m_hdrFBO.checkCompleteness() == 0);
}