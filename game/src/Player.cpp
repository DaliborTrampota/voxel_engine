#include "Player.h"

#include <functional>
#include <memory>

#include <CoordUtils.h>
#include <InputSystem.h>
#include <level/Chunk.h>
#include <level/World.h>
#include <scene/Camera.h>
#include <physics/AABB.h>

#include "GameServices.h"

using namespace engine;

Player::Player()
    : m_camera(std::make_unique<Camera>(Camera::ProjectionType::Perspective)),
      m_currentChunk(nullptr),
      m_position(0, 0, 0),
      m_aabb(std::make_shared<AABB>(AABB({
        glm::vec3(-0.5f, 0.0f, -0.5f),
        glm::vec3(0.5f, 1.8f, 0.5f),
      }))),
      m_collider(m_aabb, 0.5f) {}

Player::~Player() {
    if (m_viewDistThread.joinable())
        m_viewDistThread.join();
}

void Player::spawn(std::shared_ptr<World> world) {
    m_position = glm::vec3(Chunk::Dims.x / 2, 20, Chunk::Dims.z / 2);
    m_camera->setPosition(m_position);
    m_camera->lookAt(glm::vec3(Chunk::Dims.x / 2, 0, Chunk::Dims.z / 2));


    m_collider.setWorld(world);
    glm::vec3 pos = m_position;
    ChunkID coords = extractChunkCoords(pos);
    glm::ivec3 from = coords - ViewDistance;
    glm::ivec3 to = coords + ViewDistance;
    world->loadChunks(from, to);
    //m_viewDistThread = std::thread(&World::updateViewDistance, world, m_position);
}

void Player::update(float dt) {
    auto input = GameServices::getInputSystem();
    float mouseX = input->getAxis(Axis::MouseX);
    float mouseY = input->getAxis(Axis::MouseY);

    rotate(mouseX, mouseY, true);

    if (input->isKey(KeyState::Down, Key::W))
        move(Key::W, dt);
    if (input->isKey(KeyState::Down, Key::S))
        move(Key::S, dt);
    if (input->isKey(KeyState::Down, Key::A))
        move(Key::A, dt);
    if (input->isKey(KeyState::Down, Key::D))
        move(Key::D, dt);
}

void Player::move(Key key, float dt) {
    glm::vec3 velocity{1, 1, 1};
    m_collider.collide(velocity, m_camera->m_position);
    if (!glm::any(glm::notEqual(velocity, glm::vec3(1.0f))))
        m_camera->move(key, dt);
}

void Player::rotate(float dx, float dy, bool constrainPitch) {
    m_camera->rotate(dx, dy, constrainPitch);
}