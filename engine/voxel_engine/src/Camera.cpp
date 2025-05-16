#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace engine;

Camera::Camera(ProjectionType type) : m_type(type)
{
	m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	m_front = glm::vec3(0.0f, 0.0f, -1.0f);

	m_projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10000.0f);

	m_yaw = -90.0f;
	m_pitch = 0.0f;

	updateVectors();
}

void Camera::lookAt(const glm::vec3& target)
{
	glm::vec3 dir = glm::normalize(target - m_position);
	m_pitch = glm::degrees(asin(dir.y));
	m_yaw = glm::degrees(atan2(dir.z, dir.x));
	updateVectors();
}

void Camera::move(Key key, float dt)
{
	switch (key) {
	case Key::W:
		m_position += m_front * m_speed * dt;
		break;

	case Key::S:
		m_position -= m_front * m_speed * dt;
		break;

	case Key::A:
		m_position -= m_right * m_speed * dt;
		break;

	case Key::D:
		m_position += m_right * m_speed * dt;
		break;
	}
}

void Camera::rotate(float dx, float dy, bool constrainPitch)
{
	float sens = 0.1f;
	m_yaw += dx * sens;
	m_pitch += dy * sens; 

	if (constrainPitch)
	{
		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		if (m_pitch < -89.0f)
			m_pitch = -89.0f;
	}

	updateVectors();
}

void Camera::resize(int width, int height)
{
	m_projection = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 10000.0f);
}

glm::mat4 Camera::getView() const
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::updateVectors()
{

	m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front.y = sin(glm::radians(m_pitch));
	m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(m_front);

	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}



