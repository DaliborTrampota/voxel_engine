#include "Player.h"

#include <future>
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
    : m_currentChunk(nullptr),
      m_world(nullptr),
      m_position(0, 0, 0),
      m_aabb(
          std::make_shared<AABB>(AABB({
              glm::vec3(-0.4f, 0.0f, -0.4f),
              glm::vec3(0.4f, 1.8f, 0.4f),
          }))
      ),
      m_collider(m_aabb, 0.51f) {
    CameraOptions opts;
    opts.fov = glm::radians(45.0f);
    opts.aspectRatio = 800.0f / 600.0f;
    m_camera = std::make_unique<Camera>(Camera::ProjectionType::Perspective, opts);
}

Player::~Player() {}

void Player::spawn(std::shared_ptr<World> world) {
    move(glm::vec3(Chunk::Dims.x / 2, 20, Chunk::Dims.z / 2));
    m_camera->lookAt(glm::vec3(Chunk::Dims.x / 2, 0, Chunk::Dims.z / 2));
    m_world = world;
    m_collider.setWorld(world);

    printf("Spawning player at %.2f %.2f %.2f\n", m_position.x, m_position.y, m_position.z);
    ChunkID id = getChunkID(m_position);
    updateViewDistance(id, true);
    m_currentChunk = world->getChunk(id);
}

void Player::updateViewDistance(ChunkID center, bool waitTillLoaded) {
    glm::ivec3 from = center - ViewDistance;
    glm::ivec3 to = center + ViewDistance;
    std::future<void> future = m_world->loadChunks(from, to, true);
    if (waitTillLoaded)
        future.wait();
}

void Player::update(float dt) {
    //if (!m_world)
    //printf("Player pos %.2f %.2f %.2f\n", m_position.x, m_position.y, m_position.z);
    ChunkID curChunkID = getChunkID(m_position);
    if (m_currentChunk->id() != curChunkID) {
        updateViewDistance(curChunkID, false);
        m_currentChunk = m_world->getChunk(curChunkID);
    }


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
        m_velocity = glm::vec3(0.f);
        m_camera->position(m_position);
        m_camera->lookAt(glm::vec3(Chunk::Dims.x / 2, 0, Chunk::Dims.z / 2));

        m_aabb->position(m_position);
    }

    if (input->isKey(Down, Key::L)) {
        auto dir = m_camera->lookDirection();
        printf("Look direction: %.1f, %.1f, %.1f\n", dir.x, dir.y, dir.z);
        printf("Position: %.1f, %.1f, %.1f\n", m_position.x, m_position.y, m_position.z);
        printf(
            "Camera: %.1f, %.1f, %.1f\n",
            m_camera->position().x,
            m_camera->position().y,
            m_camera->position().z
        );
    }
}

// w prefixed variables are world space
// l prefixed variables are local space
void Player::move(glm::vec3 lDir, float dt) {
    glm::quat rotation = m_camera->rotation(true);
    glm::vec3 targetVelocity = rotation * lDir * m_speed;

    float acceleration = m_onGround ? 20.f : 1.f;  // This is like friction.
    float blend = glm::clamp(acceleration * dt, 0.0f, 1.0f);
    m_velocity.x = glm::mix(m_velocity.x, targetVelocity.x, blend);
    m_velocity.z = glm::mix(m_velocity.z, targetVelocity.z, blend);

    float horSpeed = m_velocity.x * m_velocity.x + m_velocity.z * m_velocity.z;
    if (horSpeed > m_speed * m_speed) {
        float scale = m_speed / glm::sqrt(horSpeed);
        m_velocity.x *= scale;
        m_velocity.z *= scale;
    }

    m_velocity.y -= 9.81f * 2 * dt;
    m_velocity.y = glm::max(m_velocity.y, -TERMINAL_VELOCITY);

    glm::vec3 displacement = m_velocity * dt;
    CollisionInfo col = m_collider.collide(displacement, m_position);
    m_position += displacement;
    m_aabb->position(m_position);
    m_onGround = col.grounded;

    if (col.axis & CollisionInfo::AXIS_X_MASK)
        m_velocity.x = 0.0f;  // X collision
    if (col.axis & CollisionInfo::AXIS_Y_MASK)
        m_velocity.y = 0.0f;  // Y collision
    if (col.axis & CollisionInfo::AXIS_Z_MASK)
        m_velocity.z = 0.0f;  // Z collision


    // Detect step-up and start smooth animation
    if (col.stepHeight > 0.01f) {
        m_stepStartY = m_camera->position().y;
        m_stepTargetY = m_position.y;
        m_stepAnimProgress = 0.0f;  // Start animation
    }

    // Update step animation
    if (m_stepAnimProgress < 1.0f) {
        m_stepAnimProgress += dt * 8.0f;  // Animation speed (8 = 0.125 seconds)
        if (m_stepAnimProgress > 1.0f)
            m_stepAnimProgress = 1.0f;

        // Smooth interpolation using smoothstep
        float t = m_stepAnimProgress;
        t = t * t * (3.0f - 2.0f * t);  // smoothstep formula

        float interpolatedY = glm::mix(m_stepStartY, m_stepTargetY, t);

        // Set camera to interpolated position
        glm::vec3 cameraPos = m_position;
        cameraPos.y = interpolatedY;
        m_camera->position(cameraPos);
    } else {
        // No animation, camera follows physics exactly
        m_camera->position(m_position);
    }
}

void Player::rotate(float dx, float dy, bool constrainPitch) {
    m_camera->rotate(dx, dy, constrainPitch);
}

void Player::move(glm::vec3 position) {
    m_position = position;
    m_aabb->position(position);
    m_camera->position(position);
}
