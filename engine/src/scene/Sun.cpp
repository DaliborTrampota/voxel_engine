#include "Sun.h"

#include <LWGL/texture/TextureBase.h>

#include <glad/glad.h>
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>

#include "../Globals.h"
#include "../render/Engine.h"

using namespace engine;

Sun::Sun(glm::ivec2 resolution)
    : m_resolution(resolution),
      m_depthShader("shaders/SunVert.glsl", "shaders/SunFrag.glsl", "SunDepthShader") {
    m_depthFBO.createTexture(
        gl::FBOAttachment::Depth, gl::FrameBufferSettings::Depth(resolution.x, resolution.y)
    );
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    // m_depthFBO.clearDepth();
    // m_depthFBO.clearDrawBuffers();
    // m_depthFBO.clearReadBuffer();
    assert(m_depthFBO.checkCompleteness() == 0);
    m_depthFBO.unbind();

    m_view = glm::lookAt(m_position, m_position + m_direction, UP);
    m_projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 7.5f);
}

void Sun::render(Engine& engine, const Camera* camera, int pass) {
    if (pass == 1) {
        glViewport(0, 0, m_resolution.x, m_resolution.y);
        m_depthFBO.bind();
        m_depthFBO.clearDepth();

        // render
        engine.setRenderOverride(&m_depthShader, &m_depthFBO);

    } else if (pass == 2) {
        glm::ivec2 size = engine.window()->windowSize();
        glViewport(0, 0, size.x, size.y);
        engine.clearRenderOverride();
        m_depthFBO.unbind();
    }
}