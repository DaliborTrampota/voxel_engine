#include "PointLightManager.h"

#include <algorithm>
#include <cassert>

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

void PointLightManager::addLight(const PointLight& light) {
    if (m_lights.data().size() >= m_maxLights) {
        printf("Max lights reached\n");
        return;
    }
    m_lights.add(light);
}

void PointLightManager::removeLight(const PointLight& light) {}

void PointLightManager::update(const Camera* camera) {
    assert(camera && "Camera is required for PointLightManager::update");

    std::vector<PointLight> lights = m_lights.data();
    std::sort(lights.begin(), lights.end(), [camera](const PointLight& a, const PointLight& b) {
        return glm::distance(a.position, camera->position()) <
               glm::distance(b.position, camera->position());
    });

    for (int i = 0; i < lights.size(); i++) {
        if (i < m_shadowLights) {
            lights[i].shadowIndex = i;
        } else {
            lights[i].shadowIndex = PointLight::NoShadow;
        }
    }

    m_lights.setData(std::move(lights));
    m_lights.upload();
}

const std::vector<PointLight>& PointLightManager::lights() const {
    return m_lights.data();
}

uint8_t PointLightManager::shadowLights() const {
    return m_shadowLights;
}

void PointLightManager::setShadowLights(uint8_t count) {
    m_shadowLights = count;
}

void PointLightManager::setMaxLights(uint32_t maxLights) {
    m_maxLights = maxLights;
    m_lights.reserve(maxLights);
}

uint32_t PointLightManager::maxLights() const {
    return m_maxLights;
}