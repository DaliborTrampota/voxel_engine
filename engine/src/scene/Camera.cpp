#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "render/EngineEvents.h"

using namespace engine;

Camera::Camera(OrthoOptions opts)
    : m_type(ProjectionType::Orthographic),
      m_front(NORTH),
      m_position(0),
      m_yaw(90.0f),  // 90 degrees is north
      m_pitch(0.0f),
      m_zNear(opts.zNear),
      m_zFar(opts.zFar),
      m_fov(0.0f) {
    m_projection = glm::ortho(
        -opts.width / 2, opts.width / 2, -opts.height / 2, opts.height / 2, opts.zNear, opts.zFar
    );
    updateVectors();
}

Camera::Camera(PerspectiveOptions opts)
    : m_type(ProjectionType::Perspective),
      m_front(NORTH),
      m_position(0),
      m_yaw(90.0f),  // 90 degrees is north
      m_pitch(0.0f),
      m_zNear(opts.zNear),
      m_zFar(opts.zFar),
      m_fov(opts.fov) {
    m_projection = glm::perspective(opts.fov, opts.aspectRatio, opts.zNear, opts.zFar);
    updateVectors();
}


void Camera::windowResizeEvent(ResizeEvent* ev) {
    resize(ev->width, ev->height);
}

void Camera::lookAt(const glm::vec3& target) {
    glm::vec3 dir = glm::normalize(target - m_position);
    m_pitch = glm::degrees(asin(dir.y));
    m_yaw = glm::degrees(atan2(dir.z, dir.x));
    updateVectors();
}

void Camera::rotate(float dx, float dy, bool constrainPitch) {
    float sens = 0.1f;
    m_yaw += dx * sens;
    m_pitch += dy * sens;

    if (constrainPitch) {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    updateVectors();
}

void Camera::resize(int width, int height) {
    if (m_type == ProjectionType::Perspective) {
        m_projection = glm::perspective(
            m_fov, static_cast<float>(width) / static_cast<float>(height), m_zNear, m_zFar
        );
    }
}

glm::mat4 Camera::getView() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::quat Camera::rotation(bool ignorePitch) const {
    glm::vec3 front = ignorePitch ? glm::normalize(glm::vec3(m_front.x, 0.0f, m_front.z)) : m_front;
    return glm::quatLookAt(front, m_worldUp);
}

void Camera::updateVectors() {
    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.y = sin(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(m_front);

    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
