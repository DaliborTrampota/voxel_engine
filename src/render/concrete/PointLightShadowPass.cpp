#include "PointLightShadowPass.h"

#include "render/Material.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

using namespace engine;

PointLightShadowPass::PointLightShadowPass(
    glm::ivec2 resolution, PointLightManager* pointLightManager, const glm::ivec2& shadowRes
)
    : RenderPass(
          resolution, RenderPass::PointLightShadow, pointLightManager->maxShadowLights() * 6
      ),
      m_pointLightManager(pointLightManager),
      m_material(
          "resources/shaders/light/Omni.vert", "resources/shaders/light/Omni.frag", "OmniShadows"
      ) {
    m_material.mvpSupport(false);
    material = &m_material;
    fbo = &m_shadowFBO;
    viewportSize = shadowRes;

    m_shadowMaps.create(gl::TextureParams::Depth());
    m_shadowMaps.allocate(
        gl::TextureStorage::FBODepth(
            shadowRes.x, shadowRes.y, m_pointLightManager->maxShadowLights()
        )
    );
    // m_shadowFBO.setDrawBuffers({});
    m_shadowFBO.bind();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    // assert(m_shadowFBO.checkCompleteness() == 0);
    m_shadowFBO.unbind();
}

void PointLightShadowPass::beforeRender(Engine& engine, uint8_t pass) {
    uint32_t lightIndex = pass / 6;
    uint32_t faceIndex = pass % 6;
    uint32_t lightCount = m_pointLightManager->lightCount();
    if (lightIndex >= lightCount)
        return;
    const PointLight& light = m_pointLightManager->lights()[lightIndex];
    if (light.shadowIndex == PointLight::NoShadow) {
        return;
    }
    glm::ivec2 res = viewportSize.value();
    glViewport(0, 0, res.x, res.y);

    if (pass == 0) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    m_shadowFBO.bind();
    m_shadowFBO.attachLayer(
        gl::FBOAttachment::Depth,
        m_shadowMaps,
        light.shadowIndex,
        static_cast<gl::CubeFace>(faceIndex)
    );
    m_shadowFBO.clearDepth(1.0f);

    assert(m_shadowFBO.checkCompleteness() == 0);

    glm::vec3 faceDir = gl::CubeFaceDirections[faceIndex];
    glm::vec3 up = gl::CubeFaceUps[faceIndex];

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, light.radius);
    glm::mat4 view = glm::lookAt(light.position, light.position + faceDir, up);

    material->setMat4("projection", projection);
    material->setMat4("view", view);
    material->setVec3("lightPos", light.position);
    material->setFloat("farPlane", light.radius);

    printf("Shadows for light %d, face %d\n", lightIndex, faceIndex);
}

void PointLightShadowPass::afterRender(Engine& engine, uint8_t pass) {
    if (pass == passes() - 1) {
        glViewport(0, 0, m_resolution.x, m_resolution.y);
    }
}

uint8_t PointLightShadowPass::passes() const {
    return m_pointLightManager->shadowLightCount() * 6;
}