#include "DirectionalLight.h"

#include <LWGL/GLTypes.h>
#include <LWGL/texture/TextureArray.h>
#include <LWGL/texture/TextureBase.h>

#include <glad/glad.h>
#include <cassert>
#include <glm/gtc/matrix_transform.hpp>


using namespace engine;

DirectionalLight::DirectionalLight(
    const glm::vec3& direction, const glm::vec3& color, float intensity
)
    : m_direction(glm::normalize(direction)),
      m_lightColor(color),
      m_lightIntensity(intensity) {}

void DirectionalLight::setDirection(const glm::vec3& direction) {
    m_direction = glm::normalize(direction);
}

void DirectionalLight::setLightColor(const glm::vec3& color, float intensity) {
    m_lightColor = color;
    m_lightIntensity = intensity;
}
