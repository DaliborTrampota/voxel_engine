#include "Camera.h"

#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "render/events/EngineEvents.h"

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
      m_fov(opts.fov),
      m_aspectRatio(opts.aspectRatio) {
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

void Camera::lookDirection(const glm::vec3& direction) {
    lookAt(m_position + direction);
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
        m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        m_projection = glm::perspective(m_fov, m_aspectRatio, m_zNear, m_zFar);
    }
}

glm::mat4 Camera::getView() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

std::array<Plane, 6> Camera::getFrustum() const {
    glm::mat4 pv = m_projection * getView();
    glm::vec4 left = glm::row(pv, 3) - glm::row(pv, 0);
    glm::vec4 right = glm::row(pv, 3) + glm::row(pv, 0);
    glm::vec4 top = glm::row(pv, 3) - glm::row(pv, 1);
    glm::vec4 bottom = glm::row(pv, 3) + glm::row(pv, 1);
    glm::vec4 near = glm::row(pv, 3) + glm::row(pv, 2);
    glm::vec4 far = glm::row(pv, 3) - glm::row(pv, 2);

    return {
        Plane(glm::vec3(left.x, left.y, left.z), -left.w),
        Plane(glm::vec3(right.x, right.y, right.z), -right.w),
        Plane(glm::vec3(top.x, top.y, top.z), -top.w),
        Plane(glm::vec3(bottom.x, bottom.y, bottom.z), -bottom.w),
        Plane(glm::vec3(near.x, near.y, near.z), -near.w),
        Plane(glm::vec3(far.x, far.y, far.z), -far.w)
    };
}

std::array<glm::vec3, 8> Camera::getFrustumCorners() const {
    return getFrustumCorners(m_projection, getView());
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

std::array<glm::vec3, 8> Camera::getFrustumCorners(
    const glm::mat4& projection, const glm::mat4& view
) {
    glm::mat4 invPV = glm::inverse(projection * view);
    // NDC space corners: (X, Y, Z, W) where X: -1=left,+1=right | Y: -1=bottom,+1=top | Z: -1=near,+1=far
    glm::vec4 nbr{1.0f, -1.0f, -1.0f, 1.0f};   // near bottom right
    glm::vec4 nbl{-1.0f, -1.0f, -1.0f, 1.0f};  // near bottom left
    glm::vec4 ntr{1.0f, 1.0f, -1.0f, 1.0f};    // near top right
    glm::vec4 ntl{-1.0f, 1.0f, -1.0f, 1.0f};   // near top left
    glm::vec4 fbr{1.0f, -1.0f, 1.0f, 1.0f};    // far bottom right
    glm::vec4 fbl{-1.0f, -1.0f, 1.0f, 1.0f};   // far bottom left
    glm::vec4 ftr{1.0f, 1.0f, 1.0f, 1.0f};     // far top right
    glm::vec4 ftl{-1.0f, 1.0f, 1.0f, 1.0f};    // far top left

    // Helper lambda to perform perspective divide (w-division) after transformation
    auto toWorldSpace = [&](const glm::vec4& ndc) {
        glm::vec4 worldPos = invPV * ndc;
        return glm::vec3(worldPos) / worldPos.w;
    };

    return {
        toWorldSpace(nbr),
        toWorldSpace(nbl),
        toWorldSpace(ntr),
        toWorldSpace(ntl),
        toWorldSpace(fbr),
        toWorldSpace(fbl),
        toWorldSpace(ftr),
        toWorldSpace(ftl)
    };
}