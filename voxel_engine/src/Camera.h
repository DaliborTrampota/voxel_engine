#pragma once


#include <glm/glm.hpp>

enum class ProjectionType
{
	Perspective,
	Orthographic
};



class Camera
{

public:
	Camera() : m_type(ProjectionType::Perspective) {};
	Camera(ProjectionType type);

	void move(int key, float dt);
	void rotate(float dx, float dy, bool constrainPitch = true);
	void resize(int width, int height);

	glm::mat4 getView() const;
	glm::mat4 getProjection() const { return m_projection; };

private:

	glm::vec3 m_worldUp{ .0f, 1.0f, .0f };

	float m_yaw, m_pitch;
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;

	glm::mat4 m_projection;

	float m_speed = 3.0f;


	ProjectionType m_type;

	void updateVectors();

};

