#include "Sun.h"

#include <LWGL/GLTypes.h>
#include <LWGL/texture/TextureArray.h>
#include <LWGL/texture/TextureBase.h>

#include <glad/glad.h>
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>
#include <numeric>

#include "../Globals.h"
#include "data/TextureManager.h"
#include "physics/AABB.h"
#include "scene/Camera.h"


using namespace engine;

Sun::Sun(glm::ivec2 resolution, const Camera* target, const glm::vec3& direction)
    : m_resolution(resolution),
      m_depthShader(
          "resources/shaders/SunVert.glsl",
          "resources/shaders/SunGeom.glsl",
          "resources/shaders/SunFrag.glsl",
          "SunDepthShader"
      ),
      m_direction(glm::normalize(direction)) {
    setTarget(target);

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

    // gl::FrameBufferSettings vsmSettings = {
    //     gl::Settings(gl::Settings::ClampToEdge, gl::Settings::Linear),
    //     resolution.x,
    //     resolution.y,
    //     gl::ImageFormat::GrayAlpha,
    //     gl::ImageDataType::Float
    // };

    // m_depthFBO.createTexture(gl::FBOAttachment::Color, vsmSettings);
    // m_depthFBO.createTexture(gl::FBOAttachment::Color + 1, vsmSettings);  // for blur


    gl::ArraySettings cmsSettings = {
        gl::Settings(gl::Settings::ClampToEdge, gl::Settings::Linear),
        static_cast<unsigned>(m_cascadeSplits.size()),
        resolution.x,
        resolution.y,
        gl::ImageFormat::Depth
    };
    TextureManager::Get().cascadeShadowMaps()->create(cmsSettings);
    m_depthFBO.bind();
    m_depthFBO.bindTexture(gl::FBOAttachment::Depth, TextureManager::Get().cascadeShadowMaps());


    m_depthFBO.bind();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    assert(m_depthFBO.checkCompleteness() == 0);
    m_depthFBO.unbind();

    m_depthShader.use();
    m_depthShader.setTexture(0, TextureManager::Get().blockTextures(), "blockTextures");
}

void Sun::setTarget(const Camera* target) {
    m_target = target;
}

void Sun::setDirection(const glm::vec3& direction) {
    m_direction = glm::normalize(direction);
}

void Sun::setLightColor(const glm::vec3& color, float intensity) {
    m_lightColor = color;
    m_lightIntensity = intensity;

    // m_depthShader.use();
    // m_depthShader.setVec3("lightColor", m_lightColor);
    // m_depthShader.setFloat("lightIntensity", m_lightIntensity);
}


gl::TextureRef Sun::cascadeShadowMaps() const {
    return m_depthFBO.texture(gl::FBOAttachment::Depth);
    // return m_depthFBO.texture(gl::FBOAttachment::Color);
}

void Sun::update(float dt) {
    calculateLightSpaceMatrices();

    for (int i = 0; i < m_cascadeSplits.size(); i++) {
        m_lightSpaceUBO.setSubData(i, &m_cascadeSplits[i].lightSpace);
    }
}

void Sun::calculateLightSpaceMatrices() {
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
            float distance = glm::dot(corner - center, -m_direction);
            maxDistance = std::max(maxDistance, distance);
        }
        maxDistance += 50.0f;  // extra padding

        glm::mat4 lightView = glm::lookAt(center - m_direction * maxDistance, center, UP);

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