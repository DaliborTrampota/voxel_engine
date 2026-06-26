#include "PointLightManager.h"

#include <algorithm>
#include <cassert>
#include <cstdio>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>


#include "scene/Camera.h"

using namespace engine;

PointLightManager::PointLightManager(uint32_t maxLights) : m_maxLights(maxLights) {
    m_lights.create(m_maxLights);
}

void PointLightManager::bindLights(uint32_t binding) {
    m_lights.bind(binding);
}

uint32_t PointLightManager::lightCount() const {
    return m_lights.data().size();
}

uint32_t PointLightManager::shadowLightCount() const {
    return m_shadowLightCount;
}

PointLightManager::ID PointLightManager::addLight(PointLight& light) {
    if (m_lights.data().size() >= m_maxLights) {
        printf("Max lights reached\n");
        return PointLightManager::InvalidID;
    }
    light.id = m_nextID++;
    m_lights.add(light);
    return light.id;
}

void PointLightManager::removeLight(ID id) {
    auto it = std::remove_if(
        m_lights.data().begin(), m_lights.data().end(), [id](const PointLight& light) {
            return light.id == id;
        }
    );
    m_lights.data().erase(it, m_lights.data().end());
}

PointLight& PointLightManager::light(ID id) {
    return *std::find_if(
        m_lights.data().begin(), m_lights.data().end(), [id](const PointLight& light) {
            return light.id == id;
        }
    );
}

void PointLightManager::update(const Camera* camera) {
    assert(camera && "Camera is required for PointLightManager::update");

    std::vector<PointLight>& lights = m_lights.data();
    std::sort(lights.begin(), lights.end(), [camera](const PointLight& a, const PointLight& b) {
        return glm::distance2(a.position, camera->position()) <
               glm::distance2(b.position, camera->position());
    });

    m_shadowLightCount = 0;
    for (int i = 0; i < lights.size(); i++) {
        if (i < m_maxShadowLights) {
            lights[i].shadowIndex = i;
            m_shadowLightCount++;
        } else {
            lights[i].shadowIndex = PointLight::NoShadow;
        }
    }

    m_lights.upload();
}

const std::vector<PointLight>& PointLightManager::lights() const {
    return m_lights.data();
}

uint8_t PointLightManager::maxShadowLights() const {
    return m_maxShadowLights;
}

void PointLightManager::setMaxShadowLights(uint8_t count) {
    m_maxShadowLights = count;
}

void PointLightManager::setMaxLights(uint32_t maxLights) {
    m_maxLights = maxLights;
    m_lights.reserve(maxLights);
}

uint32_t PointLightManager::maxLights() const {
    return m_maxLights;
}