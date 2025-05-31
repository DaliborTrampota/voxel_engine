#include "Player.h"

#include <functional>
#include <memory>

#include <CoordUtils.h>
#include <InputSystem.h>
#include <level/Chunk.h>
#include <level/World.h>
#include <physics/AABB.h>
#include <scene/Camera.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "GameServices.h"
#include "Globals.h"

using namespace engine;

Player::Player()
    : m_camera(std::make_unique<Camera>(Camera::ProjectionType::Perspective)),
      m_currentChunk(nullptr),
      m_position(0, 0, 0),
      m_aabb(
          std::make_shared<AABB>(AABB({
              glm::vec3(-0.4f, 0.0f, -0.4f),
              glm::vec3(0.4f, 1.8f, 0.4f),
          }))
      ),
      m_collider(m_aabb, 0.5f) {}

Player::~Player() {
    if (m_viewDistThread.joinable())
        m_viewDistThread.join();
}

void Player::spawn(std::shared_ptr<World> world) {
    m_position = glm::vec3(Chunk::Dims.x / 2, 20, Chunk::Dims.z / 2);
    m_camera->position(m_position);
    m_camera->lookAt(glm::vec3(Chunk::Dims.x / 2, 0, Chunk::Dims.z / 2));

    m_aabb->transform(glm::translate(glm::mat4(1.0f), glm::vec3(m_position)));

    printf("Spawning player at %.2f %.2f %.2f\n", m_position.x, m_position.y, m_position.z);
    printf(
        "AABB: %.2f %.2f %.2f - %.2f %.2f %.2f\n",
        m_aabb->min.x,
        m_aabb->min.y,
        m_aabb->min.z,
        m_aabb->max.x,
        m_aabb->max.y,
        m_aabb->max.z
    );

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

    if (m_onGround && input->isKey(Pressed, Key::Space)) {
        m_velocity.y += 5.f;
    }
    float upDown = 0;
    //input->isKey(Down, Key::Space) ? 1 : input->isKey(Down, Key::LShift) ? -1 : 0;

    if (!(forward == 0 && sideways == 0 && upDown == 0))
        move(glm::normalize(glm::vec3(sideways, upDown, forward)), dt);
    else
        move(glm::vec3(0), dt);

    if (input->isKey(Down, Key::P)) {
        m_position = glm::vec3(Chunk::Dims.x / 2, 20, Chunk::Dims.z / 2);
        m_camera->position(m_position);
        m_camera->lookAt(glm::vec3(Chunk::Dims.x / 2, 0, Chunk::Dims.z / 2));

        m_aabb->position(m_position);
    }
}

// w prefixed variables are world space
// l prefixed variables are local space
void Player::move(glm::vec3 lDir, float dt) {
    glm::quat rotation = m_camera->rotation(true);
    glm::vec3 targetVelocity = rotation * lDir * m_speed;

    float acceleration = m_onGround ? 20.f : 1.f; // This is like friction.

    float y = m_velocity.y;
    m_velocity.y = 0;

    glm::vec3 blended = glm::mix(m_velocity, targetVelocity, glm::clamp(acceleration * dt, 0.0f, 1.0f));
    m_velocity.x = blended.x;
    m_velocity.z = blended.z;

    glm::vec2 horizontalVelocity{m_velocity.x, m_velocity.z};
    if (glm::length2(horizontalVelocity) > m_speed * m_speed) {
        horizontalVelocity = glm::normalize(horizontalVelocity) * m_speed;
        m_velocity.x = horizontalVelocity.x;
        m_velocity.z = horizontalVelocity.y;
    }

    m_velocity.y = y - 9.81f * dt;



    glm::vec3 displacement = m_velocity * dt;
    CollisionInfo col = m_collider.collide(displacement, m_position);
    m_velocity = displacement / dt;
    
    if (m_velocity.y < -20.f)
        m_velocity.y = -20.f;

    //printf("Speed: %.1f\n", glm::length(glm::vec2{m_velocity.x, m_velocity.z}));

    if (col.axis & 2) {
        m_onGround = true;
        m_velocity.y = .0f;
    } else {
        m_onGround = false;
    }

    m_position += displacement;
    m_aabb->position(m_position);
    m_camera->position(m_position);
}

void Player::rotate(float dx, float dy, bool constrainPitch) {
    m_camera->rotate(dx, dy, constrainPitch);
}