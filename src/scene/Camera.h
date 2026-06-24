#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../Globals.h"
#include "physics/Shapes.h"
#include "render/events/EngineEventSite.h"


namespace engine {

    struct CameraOptions {
        float zNear = 0.1f;
        float zFar = 1200.0f;
    };

    struct OrthoOptions : public CameraOptions {
        float width;
        float height;
    };

    struct PerspectiveOptions : public CameraOptions {
        float fov;
        float aspectRatio;
    };

    class Camera : public EngineEventSite {
      public:
        enum class ProjectionType {
            Perspective,
            Orthographic
        };

        Camera(OrthoOptions opts);
        Camera(PerspectiveOptions opts);

        void windowResizeEvent(ResizeEvent* ev) override;

        void position(const glm::vec3& pos) { m_position = pos; }
        const glm::vec3& position() const { return m_position; }
        const glm::vec3& lookDirection() const { return m_front; }

        /// @brief Returns the camera's rotation as a quaternion.
        /// @param ignorePitch If true, the pitch (up/down rotation) is ignored. Good if you want to move on XZ plane only.
        /// @return Quaternion representing the camera's rotation.
        glm::quat rotation(bool ignorePitch) const;

        void lookAt(const glm::vec3& target);
        void lookDirection(const glm::vec3& direction);

        void rotate(float dx, float dy, bool constrainPitch = true);
        void resize(int width, int height);

        glm::mat4 getView() const;
        glm::mat4 getProjection() const { return m_projection; };

        float farPlane() const { return m_zFar; }
        float nearPlane() const { return m_zNear; }
        float fov() const { return m_fov; }
        float aspectRatio() const { return m_aspectRatio; }

        /// @brief Returns the camera's frustum in world space.
        /// @note Order of the planes is: left, right, top, bottom, near, far.
        std::array<Plane, 6> getFrustum() const;
        /// @brief Returns the camera's frustum corners in world space.
        /// @note Order of the corners is:
        ///     [0] near bottom right, [1] near bottom left,
        ///     [2] near top right,    [3] near top left,
        ///     [4] far bottom right,  [5] far bottom left,
        ///     [6] far top right,     [7] far top left.
        std::array<glm::vec3, 8> getFrustumCorners() const;

        static std::array<glm::vec3, 8> getFrustumCorners(
            const glm::mat4& projection, const glm::mat4& view
        );

      protected:
        glm::vec3 m_worldUp = UP;

        float m_yaw, m_pitch;
        glm::vec3 m_position;
        glm::vec3 m_front;
        glm::vec3 m_up;
        glm::vec3 m_right;

        glm::mat4 m_projection;
        ProjectionType m_type;

        float m_fov = 0.0f;
        float m_zNear = 0.1f;
        float m_zFar = 1200.0f;
        float m_aspectRatio = 1.0f;

        void updateVectors();
    };
}  // namespace engine
