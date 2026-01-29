#include "ScenePass.h"

#include <LWGL/GLTypes.h>
#include <LWGL/texture/TextureBase.h>
#include <LWGL/texture/TextureRef.h>

#include <glad/glad.h>

#include "render/Engine.h"


using namespace engine;

void ScenePass::beforeRender(Engine& engine, uint8_t pass) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    m_sceneFBO.clearActive(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 1.0f);
}

void ScenePass::init() {
    m_opaque.create(gl::TextureParams(gl::TextureParams::ClampToEdge, gl::TextureParams::Linear));
    m_opaque.allocate(
        {.width = m_resolution.x,
         .height = m_resolution.y,
         .format = gl::ImageFormat::RGBA,
         .dataType = gl::ImageDataType::HalfFloat}
    );

    m_depthMap.create(gl::TextureParams::Depth());
    m_depthMap.allocate(gl::TextureStorage::FBODepth(m_resolution.x, m_resolution.y));

    m_sceneFBO.bind();
    m_sceneFBO.bindTexture(gl::FBOAttachment::Color, &m_opaque);
    m_sceneFBO.bindTexture(gl::FBOAttachment::Depth, &m_depthMap);

    assert(m_sceneFBO.checkCompleteness() == 0);
    m_sceneFBO.unbind();

    fbo = &m_sceneFBO;
}
