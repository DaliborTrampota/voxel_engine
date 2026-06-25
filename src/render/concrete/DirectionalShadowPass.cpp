#include "DirectionalShadowPass.h"

#include <LWGL/buffer/FBO.h>
#include <glad/glad.h>
#include <numeric>

#include "physics/AABB.h"
#include "render/Engine.h"
#include "render/utility/UtilityShaders.h"
#include "scene/Camera.h"
#include "scene/DirectionalLight.h"


#include "data/TextureManager.h"

using namespace engine;

DirectionalShadowPass::DirectionalShadowPass(
    glm::ivec2 resolution,
    const Camera* camera,
    const DirectionalLight* light,
    const glm::ivec2& shadowRes
)
    : RenderPass(resolution, RenderPass::DirectionalShadow),
      m_depthShader(
          "resources/shaders/light/SunVert.glsl",
          "resources/shaders/light/SunGeom.glsl",
          "resources/shaders/light/SunFrag.glsl",
          "SunDepthShader"
      ),
      m_light(light),
      m_target(camera) {
    Material::setGlobalConstant("CascadeCount", static_cast<int>(m_cascadeSplits.size()));
    this->material = &m_depthShader;
    this->fbo = &m_depthFBO;
    viewportSize = shadowRes;


    m_lightSpaceUBO = gl::UBO(
        0,
        {
            gl::Type::Mat4,
            gl::Type::Mat4,
            gl::Type::Mat4,
            gl::Type::Mat4,
        },
        "LightSpaceMatrices"
    );
    m_lightSpaceUBO.create();
    m_depthShader.bindUBO(m_lightSpaceUBO);


    m_cascadeShadowMaps.create(
        gl::TextureParams(gl::TextureParams::ClampToEdge, gl::TextureParams::Linear)
    );
    m_cascadeShadowMaps.allocate(
        {.width = shadowRes.x,
         .height = shadowRes.y,
         .depth = static_cast<int>(m_cascadeSplits.size()),
         .format = gl::ImageFormat::Depth,
         .dataType = gl::ImageDataType::Float}
    );
    m_depthFBO.bind();
    m_depthFBO.attach(gl::FBOAttachment::Depth, &m_cascadeShadowMaps);


    m_depthFBO.bind();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    assert(m_depthFBO.checkCompleteness() == 0);
    m_depthFBO.unbind();

    m_depthShader.use();
    m_depthShader.setTexture(0, TextureManager::Get().blockTextures(), "blockTextures");
}

void DirectionalShadowPass::beforeRender(Engine& engine, uint8_t pass) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    fbo->bind();
    fbo->clearDepth(1.0f);

    glm::ivec2 res = viewportSize.value();
    glViewport(0, 0, res.x, res.y);

    calculateLightSpaceMatrices();
    for (int i = 0; i < m_cascadeSplits.size(); i++) {
        m_lightSpaceUBO.setSubData(i, &m_cascadeSplits[i].lightSpace);
    }
}

void DirectionalShadowPass::afterRender(Engine& engine, uint8_t pass) {
    glViewport(0, 0, m_resolution.x, m_resolution.y);

    // glm::ivec2 resolution = engine.directionalLightSource()->resolution();
    // applyLinearSamplingBlur(
    //     *fbo,
    //     gl::FBOAttachment::Color,
    //     gl::FBOAttachment::Color + 1,
    //     resolution.x,
    //     resolution.y,
    //     nullptr
    // );
}

bool DirectionalShadowPass::shouldRun() const {
    return m_light->castShadows();
}


void DirectionalShadowPass::calculateLightSpaceMatrices() {
    for (int i = 0; i < m_cascadeSplits.size(); i++) {
        glm::mat4 cameraPerspective = glm::perspective(
            m_target->fov(),
            m_target->aspectRatio(),
            i == 0 ? m_target->nearPlane() : m_cascadeSplits[i - 1].farPlane,
            m_cascadeSplits[i].farPlane
        );

        std::array<glm::vec3, 8> cascadeCorners =
            Camera::getFrustumCorners(cameraPerspective, m_target->getView());

        glm::vec3 center =
            std::accumulate(cascadeCorners.begin(), cascadeCorners.end(), glm::vec3(0.0f)) / 8.0f;

        float maxDistance = 0.0f;
        for (auto& corner : cascadeCorners) {
            // this returns the distance to a corner in the direction of the light
            float distance = glm::dot(corner - center, -m_light->direction());
            maxDistance = std::max(maxDistance, distance);
        }
        maxDistance += 50.0f;  // extra padding

        glm::mat4 lightView = glm::lookAt(center - m_light->direction() * maxDistance, center, UP);

        for (auto& corner : cascadeCorners) {
            corner = lightView * glm::vec4(corner, 1.0f);
        }

        AABB frustumAABB =
            AABB::fromPoints(std::vector<glm::vec3>{cascadeCorners.begin(), cascadeCorners.end()});

        // include blocks that are not visible by the camera but can cast shadows to the area visible by the camera
        const float shiftFactor = 10.0f;
        frustumAABB.min.z -= shiftFactor;
        frustumAABB.max.z += shiftFactor;

        // Shadow stabilization: snap to texel increments to prevent swimming
        // float worldUnitsPerTexel = (frustumAABB.max.x - frustumAABB.min.x) / m_resolution.x;

        // frustumAABB.min.x = glm::floor(frustumAABB.min.x / worldUnitsPerTexel) * worldUnitsPerTexel;
        // frustumAABB.max.x = glm::floor(frustumAABB.max.x / worldUnitsPerTexel) * worldUnitsPerTexel;
        // frustumAABB.min.y = glm::floor(frustumAABB.min.y / worldUnitsPerTexel) * worldUnitsPerTexel;
        // frustumAABB.max.y = glm::floor(frustumAABB.max.y / worldUnitsPerTexel) * worldUnitsPerTexel;

        m_cascadeSplits[i].lightSpace = glm::ortho(
                                            frustumAABB.min.x,
                                            frustumAABB.max.x,
                                            frustumAABB.min.y,
                                            frustumAABB.max.y,
                                            -frustumAABB.max.z,
                                            -frustumAABB.min.z
                                        ) *
                                        lightView;
    }
}