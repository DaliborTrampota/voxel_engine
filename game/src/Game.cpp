#include "Game.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <UI/Renderer.h>
#include <UI/elements/Panel.h>

#include <data/TextureLoader.h>
#include <level/World.h>
#include <render/Window.h>
#include <scene/Camera.h>
#include <scene/Sun.h>
#include <scene/Updateable.h>


#include <GLFWUserPointer.h>
#include <input/InputSystem.h>


#include "GameServices.h"
#include "registry/Blocks.h"


//#include <tracy/Tracy.hpp>
using namespace engine;


Game::Game(std::unique_ptr<Window> window, glm::ivec2 dims)
    : Engine(std::move(window)),
      m_worldManager(this),
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
    // Render UI AFTER flush() so it draws on top of everything
    m_uiManager->render();
    m_inputSystem->beginFrame();  // TODO proper name or placement
}

void Game::start() {
    uint32_t texSlot = 0;
    TextureLoader loader(texSlot);
    loader.load("resources/textures/blocks/");  // bind in load

    RegisterBlocks();

    m_player = std::make_shared<Player>();
    m_player->spawn(activeWorld());

    setDirectionalLightSource(
        std::make_shared<engine::Sun>(
            this, glm::ivec2(2048, 2048), &m_player->position(), glm::vec3(0.2f, -1.0f, 0.2f)
        )
    );


    subscribeUpdate(m_player);
    subscribeUpdate(m_uiManager);
    subscribeUpdate(m_directionalLightSource);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    m_plrCamera = m_player->getCamera();
    m_plrCamera->lookAt(glm::vec3(0, 0, 0));
    m_window->subscribe(m_plrCamera);


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
