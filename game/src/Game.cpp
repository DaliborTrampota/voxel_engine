#include "Game.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <UI/Renderer.h>
#include <UI/elements/Panel.h>

#include <GLFWUserPointer.h>
#include <audio/AudioManager.h>
#include <data/TextureLoader.h>
#include <input/InputSystem.h>
#include <level/World.h>
#include <render/Window.h>
#include <scene/Camera.h>
#include <scene/Sun.h>
#include <scene/Updateable.h>

#include "Directory.h"
#include "GameServices.h"
#include "registry/Blocks.h"
#include "registry/Geometries.h"

#include <render/concrete/ScenePass.h>

using namespace engine;


Game::Game(std::unique_ptr<Window> window, glm::ivec2 dims)
    : Engine(std::move(window)),
      m_plrCamera(nullptr) {
    m_window->makeCurrent();
    m_inputSystem = std::make_unique<engine::InputSystem>();
    m_uiManager = std::make_unique<UIManager>(dims);

    m_pointer = GLFWUserPointer{m_inputSystem.get(), m_window.get()};
    m_window->setUserPointer(&m_pointer);

    m_inputSystem->subscribe(m_uiManager.get());
    m_window->subscribe(m_uiManager.get());

    GameServices::setGame(this);
    GameServices::setInputSystem(m_inputSystem.get());
    GameServices::setWorldManager(&m_worldManager);
}

Game::~Game() {}


void Game::processInput() {
    if (m_inputSystem->isKey<Down>(GLFW_KEY_ESCAPE))
        window()->setShouldClose();
}

void Game::render(double dt) {
    processInput();
    fireUpdate(dt);

    auto world = m_worldManager.activeWorld();
    for (int pass = 1; pass <= 3; pass++) {
        world->render(*this, m_plrCamera, pass);
    }
    // world->render(*this, m_plrCamera, 0);
    // m_commonUBO.setSubData(1, glm::value_ptr(m_plrCamera->getView()));
    // world->getMaterial().use();
    // world->getMaterial().setMat4("projection", m_player->getCamera()->getProjection());
    // world->getMaterial().setMat4("view", m_player->getCamera()->getView());
}

void Game::afterRender() {
    // render UI after flush() so it draws on top of everything
    m_uiManager->render();
    m_inputSystem->beginFrame();  // TODO proper name or placement
    engine::AudioManager::Get().updatePositionalAudio(
        m_player->position(), m_player->getCamera()->lookDirection(), m_player->velocity()
    );
}

void Game::start() {
    uint32_t texSlot = 0;
    TextureLoader loader(texSlot);
    loader.load(getAssetsDirectory() / "textures/blocks/");  // bind in load

    RegisterGeometries();
    RegisterBlocks();
    RegisterMultiBlocks();

    m_uiManager->createUI();

    m_worldManager.setup();
    m_player = std::make_shared<Player>();
    m_player->spawn(activeWorld());


    registerDefaultRenderPasses();
    setDirectionalLightSource(
        std::make_shared<engine::Sun>(
            glm::ivec2(1024, 1024), &m_player->position(), glm::vec3(0.5f, -1.0f, 0.2f)
        )
    );


    subscribeUpdate(m_player);
    subscribeUpdate(m_uiManager);
    subscribeUpdate(m_directionalLightSource);
    subscribeUpdate(activeWorld());

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    m_plrCamera = m_player->getCamera();
    m_plrCamera->lookAt(glm::vec3(0, 0, 0));
    m_window->subscribe(m_plrCamera);
    engine::AudioManager& amgr = engine::AudioManager::Get();
    amgr.preload("resources/audio/test2.wav", "Test2", AudioType::Effect);
    amgr.preload("resources/audio/test.wav", "Test", AudioType::Music);

    amgr.playEffect("Test2");

    Audio music = amgr.playMusic(
        "Test",
        {
            .volume = 0.5f,
            .directional = true,
            .directionalData = {
                .position = m_player->position(), .velocity = glm::vec3(0.0f, 0.0f, 0.0f)
            },
        }
    );

    // glm::mat4 commonData[2] = {
    //     m_player->getCamera()->getProjection(),
    //     m_player->getCamera()->getView(),
    // };

    // m_commonUBO = gl::UBO(0, {gl::Type::Mat4, gl::Type::Mat4}, "Common");
    // m_commonUBO.create();
    // m_commonUBO.setData(static_cast<void*>(commonData));
    // m_worldManager.activeWorld()->getMaterial().bindUBO(m_commonUBO);
    // m_worldManager.activeWorld()->getMaterial().setInt("texArray", texSlot);

    window()->mouseLock(true);
    gameloop();
}