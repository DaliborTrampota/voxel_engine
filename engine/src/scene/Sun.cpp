#include "Sun.h"

#include <LWGL/texture/TextureBase.h>

#include <glad/glad.h>
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>

#include "../Globals.h"
#include "../render/Engine.h"

using namespace engine;

Sun::Sun(
    Engine* engine,
    glm::ivec2 resolution,
    const glm::vec3* targetPosition,
    const glm::vec3& direction
)
    : m_resolution(resolution),
      m_depthShader(
          "resources/shaders/SunVert.glsl", "resources/shaders/SunFrag.glsl", "SunDepthShader"
      ),
      m_engine(engine),
      m_directionalShadowPass(RenderPass::DirectionalShadow),
      m_direction(direction),
      m_targetPosition(targetPosition),
      m_view(glm::mat4(1)) {
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

    m_projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.f, 1000.f);
    m_depthShader.use();
    m_depthShader.setMat4("projection", m_projection);

    m_engine->registerRenderPass(
        {m_directionalShadowPass, &m_depthShader, &m_depthFBO, m_resolution}
    );
}

// todo inline lightPosition()?
void Sun::setTargetPosition(const glm::vec3* position) {
    m_targetPosition = position;
    m_view = glm::lookAt(lightPosition(), *m_targetPosition, UP);

    // m_depthShader.use();
    // m_depthShader.setMat4("view", m_view);
}

void Sun::setDirection(const glm::vec3& direction) {
    m_direction = glm::normalize(direction);
    m_view = glm::lookAt(lightPosition(), *m_targetPosition, UP);

    // m_depthShader.use();
    // m_depthShader.setMat4("view", m_view);
}

void Sun::setLightColor(const glm::vec3& color, float intensity) {
    m_lightColor = color;
    m_lightIntensity = intensity;

    // m_depthShader.use();
    // m_depthShader.setVec3("lightColor", m_lightColor);
    // m_depthShader.setFloat("lightIntensity", m_lightIntensity);
}

void Sun::update(float dt) {
    m_view = glm::lookAt(lightPosition(), *m_targetPosition, UP);
    m_depthShader.use();
    m_depthShader.setMat4("view", m_view);
}

glm::mat4 Sun::getLightSpaceTransform() const {
    return m_projection * m_view;
}

unsigned Sun::shadowMapTexture() const {
    return m_depthFBO.texture(gl::FBOAttachment::Depth);
}

// void Sun::render(Engine& engine, const Camera* camera, int pass) {
//     if (pass == 1) {
//         glViewport(0, 0, m_resolution.x, m_resolution.y);
//         m_depthFBO.bind();
//         m_depthFBO.clearDepth();

//     } else if (pass == 2) {
//         glm::ivec2 size = engine.window()->windowSize();
//         glViewport(0, 0, size.x, size.y);
//         m_depthFBO.unbind();
//     }
// }