#include "Player.h"

#include <thread>

#include "Camera.h"
#include "level/World.h"
#include "level/Chunk.h"

Player::Player() :
	m_camera(new Camera(Camera::ProjectionType::Perspective)),
	m_currentChunk(nullptr),
	m_position(0, 0, 0)
{
}

Player::~Player()
{
	delete m_camera;
}

void Player::spawn(lvl::World* world)
{
	m_position = glm::vec3(lvl::ChunkDim.x / 2, 50, lvl::ChunkDim.z / 2);
	m_camera->setPosition(m_position);
	m_camera->lookAt(glm::vec3(lvl::ChunkDim.x / 2, 0, lvl::ChunkDim.z / 2));

	world->updateViewDistance(m_position);
	//m_viewDistThread = std::thread(&lvl::World::updateViewDistance, world, m_position);
}

void Player::move(int key, float dt)
{
	m_camera->move(key, dt);
}

void Player::rotate(float dx, float dy, bool constrainPitch)
{
	m_camera->rotate(dx, dy, constrainPitch);
}
