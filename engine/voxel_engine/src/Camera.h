#pragma once

#include <glm/glm.hpp>

#include <core/gl/Controls.h>


namespace engine {

	class Camera
	{
	public:

		enum class ProjectionType
		{
			Perspective,
			Orthographic
		};

		Camera(ProjectionType type);
		Camera() : Camera(ProjectionType::Perspective) {};

		void setPosition(const glm::vec3& pos) { m_position = pos; };
		void lookAt(const glm::vec3& target);

		void move(Key key, float dt);
		void rotate(float dx, float dy, bool constrainPitch = true);
		void resize(int width, int height);

		glm::mat4 getView() const;
		glm::mat4 getProjection() const { return m_projection; };


	private:

		glm::vec3 m_worldUp;

		float m_yaw, m_pitch;
		glm::vec3 m_position;
		glm::vec3 m_front;
		glm::vec3 m_up;
		glm::vec3 m_right;

		glm::mat4 m_projection;

		float m_speed = 10.0f;

		ProjectionType m_type;

		void updateVectors();

		friend class Player;
	};
}

