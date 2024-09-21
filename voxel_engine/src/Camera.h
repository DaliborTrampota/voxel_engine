#pragma once

#include <glm/glm.hpp>



struct GLFWwindow;


enum class ProjectionType
{
	Perspective,
	Orthographic
};

class Camera
{

public:
	Camera(ProjectionType type);
	Camera() : Camera(ProjectionType::Perspective) {};

	void setPosition(const glm::vec3& pos) { m_position = pos; };
	void lookAt(const glm::vec3& target);

	void move(int key, float dt);
	void rotate(float dx, float dy, bool constrainPitch = true);
	void resize(int width, int height);

	void mouseLock(GLFWwindow* window, bool state);

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

};

