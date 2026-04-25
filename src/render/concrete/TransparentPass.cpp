#include "TransparentPass.h"

#include <LWGL/GLTypes.h>
#include <LWGL/texture/TextureRef.h>

#include <glad/glad.h>

#include "data/TextureManager.h"
#include "render/Engine.h"
#include "render/concrete/ScenePass.h"


using namespace engine;


TransparentPass::TransparentPass(glm::ivec2 resolution)
    : RenderPass(resolution, engine::RenderPass::SceneTransparent, 1),
      m_transparentMat(
          "resources/shaders/ChunkVert.glsl",
          "resources/shaders/TransparentFrag.glsl",
          "TransparentChunk"
      ) {
    m_transparentMat.setShadowSupport(true);
    m_transparentMat.use();
    m_transparentMat.setTexture(0, TextureManager::Get().blockTextures(), "blockTextures");
    m_transparentMat.setTexture(1, TextureManager::Get().cascadeShadowMaps(), "shadowMap");
    m_transparentMat.setTexture(
        2, RenderPassRegistry::Get().getPass<ScenePass>()->depthMap(), "opaqueDepthMap"
    );

    init();
}

void TransparentPass::beforeRender(Engine& engine, uint8_t pass) {
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);
    glBlendFunci(0, GL_ONE, GL_ONE);                   // accumulation
    glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);  // revealage
    glBlendEquation(GL_FUNC_ADD);

    m_OIT.bind();
    m_OIT.clearColor(gl::FBOAttachment::Color, glm::vec4(0.0f));
    m_OIT.clearColor(gl::FBOAttachment::Color + 1, glm::vec4(1.0f));
}

void TransparentPass::init() {
    m_accumulation.create(
        gl::TextureParams(gl::TextureParams::ClampToEdge, gl::TextureParams::Linear)
    );
    m_revealage.create(
        gl::TextureParams(gl::TextureParams::ClampToEdge, gl::TextureParams::Linear)
    );
    allocateTextures();

    ScenePass* sPass = RenderPassRegistry::Get().getPass<ScenePass>();

    m_OIT.bind();
    m_OIT.bindTexture(gl::FBOAttachment::Color, &m_accumulation);
    m_OIT.bindTexture(gl::FBOAttachment::Color + 1, &m_revealage);
    m_OIT.bindTexture(gl::FBOAttachment::Depth, sPass->depthMap());

    m_OIT.setDrawBuffers({gl::FBOAttachment::Color, gl::FBOAttachment::Color + 1});

    assert(m_OIT.checkCompleteness() == 0);
    m_OIT.unbind();

    material = &m_transparentMat;
    fbo = &m_OIT;
}

void TransparentPass::resize(glm::ivec2 resolution) {
    RenderPass::resize(resolution);
    allocateTextures();
}

void TransparentPass::allocateTextures() {
    m_accumulation.allocate(
        {.width = m_resolution.x,
         .height = m_resolution.y,
         .format = gl::ImageFormat::RGBA,
         .dataType = gl::ImageDataType::HalfFloat}
    );
    m_revealage.allocate(
        {.width = m_resolution.x,
         .height = m_resolution.y,
         .format = gl::ImageFormat::Gray,
         .dataType = gl::ImageDataType::HalfFloat}
    );
}
