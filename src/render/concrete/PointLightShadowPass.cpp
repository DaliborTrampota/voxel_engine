#include "PointLightShadowPass.h"

#include "data/TextureManager.h"
#include "render/Material.h"


#include <glad/glad.h>
#include <format>
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
          "resources/shaders/light/Omni.vert",
          "resources/shaders/light/Omni.geom",
          "resources/shaders/light/Omni.frag",
          "OmniShadows"
      ) {
    m_material.mvpSupport(false);
    m_material.setTexture(0, TextureManager::Get().blockTextures(), "blockTextures");
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
    m_shadowFBO.attach(gl::FBOAttachment::Depth, &m_shadowMaps);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    assert(m_shadowFBO.checkCompleteness() == 0);
    m_shadowFBO.unbind();
}

void PointLightShadowPass::beforeRender(Engine& engine, uint8_t pass) {
    uint32_t lightCount = m_pointLightManager->lightCount();
    if (pass >= lightCount)
        return;

    const PointLight& light = m_pointLightManager->lights()[pass];
    if (light.shadowIndex == PointLight::NoShadow)
        return;

    glm::ivec2 res = viewportSize.value();
    glViewport(0, 0, res.x, res.y);

    m_shadowFBO.bind();

    if (pass == 0) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        m_shadowFBO.clearDepth(1.0f);
    }


    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, light.radius);

    for (size_t i = 0; i < 6; i++) {
        glm::vec3 faceDir = gl::CubeFaceDirections[i];
        glm::vec3 up = gl::CubeFaceUps[i];

        glm::mat4 view = glm::lookAt(light.position, light.position + faceDir, up);

        m_material.setMat4(std::format("faceMatrices[{}]", i), projection * view);
        // material->setMat4("projection", projection);
        // material->setMat4("view", view);
    }

    m_material.setInt("baseLayer", light.shadowIndex * 6);
    m_material.setVec3("lightPos", light.position);
    m_material.setFloat("farPlane", light.radius);
}

void PointLightShadowPass::afterRender(Engine& engine, uint8_t pass) {
    if (pass == passes() - 1) {
        glViewport(0, 0, m_resolution.x, m_resolution.y);
    }
}

uint8_t PointLightShadowPass::passes() const {
    return m_pointLightManager->shadowLightCount();
}

bool PointLightShadowPass::shouldRun() const {
    return m_pointLightManager->shadowLightCount() > 0;
}