#include "Player.h"

#include <functional>
#include <memory>

#include <CoordUtils.h>
#include <InputSystem.h>
#include <level/Chunk.h>
#include <level/World.h>
#include <scene/Camera.h>
#include <physics/AABB.h>

#include <glm/gtc/matrix_transform.hpp>

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

    m_aabb->transform(glm::translate(glm::mat4(1.0f), glm::vec3(m_position)));


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

    float forward = input->getAxis(Axis::Forward);
    float sideways = input->getAxis(Axis::Sideways);
    float upDown = input->isKey(Down, Key::Space) ? 1 : input->isKey(Down, Key::LShift) ? -1 : 0;

    if (!(forward == 0 && sideways == 0 && upDown == 0))
        move(glm::normalize(glm::vec3(forward, upDown, sideways)), dt);
    else
        move(glm::vec3(0), dt);
}

void Player::move(glm::vec3 dir, float dt) {
    const float friction = 10.f;

    glm::vec3 acceleration = dir * m_speed;
    m_velocity += acceleration * dt;

    for (int i = 0; i < 3; ++i) {
        if (acceleration[i] == 0.0f) {
            if (m_velocity[i] > 0.0f) {
                m_velocity[i] = std::max(0.0f, m_velocity[i] - friction * dt);
            } else if (m_velocity[i] < 0.0f) {
                m_velocity[i] = std::min(0.0f, m_velocity[i] + friction * dt);
            }
        }
    }


    m_collider.collide(m_velocity, m_position);

    m_position += m_velocity * dt;
    m_aabb->move(m_velocity * dt);
    m_camera->setPosition(m_position);

    //printf("%.2f %.2f %.2f\n", m_velocity.x, m_velocity.y, m_velocity.z);
}

void Player::rotate(float dx, float dy, bool constrainPitch) {
    m_camera->rotate(dx, dy, constrainPitch);
}