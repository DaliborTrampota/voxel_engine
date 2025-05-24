#include "Player.h"

#include <functional>

#include <Camera.h>
#include <CoordUtils.h>
#include <level/Chunk.h>
#include <level/World.h>´
#include <InputSystem.h>

#include "GameServices.h"

using namespace engine;

Player::Player()
    : m_camera(std::make_unique<Camera>(Camera::ProjectionType::Perspective)),
      m_currentChunk(nullptr){}

Player::~Player() {
    if (m_viewDistThread.joinable())
        m_viewDistThread.join();
}

void Player::spawn(std::shared_ptr<World> world) {
    m_position = glm::vec3(Chunk::Dims.x / 2, 20, Chunk::Dims.z / 2);
    m_camera->setPosition(m_position);
    m_camera->lookAt(glm::vec3(Chunk::Dims.x / 2, 0, Chunk::Dims.z / 2));


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
    m_camera->move(key, dt);
}

void Player::rotate(float dx, float dy, bool constrainPitch) {
    m_camera->rotate(dx, dy, constrainPitch);
}
