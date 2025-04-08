#pragma once

#include <glm/glm.hpp>
#include <thread>

class Camera;
namespace lvl {
	class World;
	class Chunk;
}

class Player
{

public:
	Player();
	~Player();

	void spawn(lvl::World* world);

	void move(int key, float dt);
	void rotate(float dx, float dy, bool constrainPitch = true);

	Camera* getCamera() { return m_camera; }

private:
	std::thread m_viewDistThread;

	Camera *m_camera;
	glm::vec3 m_position;
	lvl::Chunk* m_currentChunk;
};

