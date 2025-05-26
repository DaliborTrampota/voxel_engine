#include "Game.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <core/gl/GLEvents.h>
#include <core/gl/GraphicsAPI.h>
#include <core/gl/Shader.h>
#include <core/gl/ShaderPipeline.h>

#include <Camera.h>
#include <TextureLoader.h>
#include <Updateable.h>
#include <level/World.h>

#include "GameServices.h"
#include "registry/Blocks.h"

//#include <tracy/Tracy.hpp>
using namespace engine;


Game::Game(std::unique_ptr<gl::Window> window, glm::ivec2 dims)
    : Engine(std::move(window)),
      m_mouseState(dims) {
    m_inputSystem = std::make_unique<engine::InputSystem>();

    this->window()->graphicsAPI()->subscribe(m_inputSystem.get());

    GameServices::setGame(this);
    GameServices::setInputSystem(m_inputSystem.get());
}

void Game::processInput() {
    if (m_inputSystem->getKeyState(Key::Esc) == KeyState::Pressed)
        window()->close();
}

Game::~Game() {}


void Game::render(double dt) {
    processInput();
    fireUpdate(dt);

    Engine::render(&m_pipeline, m_plrCamera, activeWorld());
}

void Game::start() {
    uint32_t texSlot = 0;
    TextureLoader loader(texSlot);
    loader.load("resources/textures/blocks/");
    //loader.bind();
    RegisterBlocks();

    m_player = std::make_shared<Player>();
    m_player->spawn(activeWorld());

    subscribeUpdate(m_player);

    {
        gl::Shader vert("../../../game/shaders/VertexShader.glsl", GL_VERTEX_SHADER);
        gl::Shader frag("../../../game/shaders/PixelShader.glsl", GL_FRAGMENT_SHADER);

        if (!m_pipeline.registerShader(GL_VERTEX_SHADER, vert))
            printf("Vert shader not registered");
        if (!m_pipeline.registerShader(GL_FRAGMENT_SHADER, frag))
            printf("Fragment shader not registered");
        if (!m_pipeline.link())
            return;  // throw error or something? 1;
    }


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    m_plrCamera = m_player->getCamera();
    m_pipeline.use();

    m_plrCamera->lookAt(glm::vec3(0, 0, 0));
    m_pipeline.setInt("texArray", texSlot);

    glm::mat4 model = glm::mat4(1.0f);
    m_pipeline.setMat4("model", model);
    m_pipeline.setMat4("projection", m_plrCamera->getProjection());


    window()->mouseLock(true);
    gameloop();
}
