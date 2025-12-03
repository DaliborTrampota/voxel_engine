#include "Game.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <UI/Renderer.h>
#include <UI/elements/Panel.h>

#include <audio/AudioManager.h>
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
    engine::AudioManager::Get().updatePositionalAudio();
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
            this, glm::ivec2(1024, 1024), &m_player->position(), glm::vec3(0.5f, -1.0f, 0.2f)
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
    engine::AudioManager& amgr = engine::AudioManager::Get();
    amgr.preload("resources/audio/test2.wav", "Test2", AudioType::Effect);
    amgr.preload("resources/audio/test.wav", "Test", AudioType::Music);

    amgr.playEffect("Test2");

    Audio music = amgr.playMusic("Test");
    music.playWithSettings({
        .directional = true,
        .directionalData = {
            .position = m_player->position(), .acceleration = glm::vec3(0.0f, 0.0f, 0.0f)
        },
    });

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


void Game::renderShadowMapDebug() {
    if (!m_directionalLightSource)
        return;

    // Save current OpenGL state
    GLboolean depthTestEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
    glDisable(GL_DEPTH_TEST);

    // CRITICAL: Unbind any FBO before reading the texture
    // You can't read from a texture that's attached to the currently bound framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    // Set viewport to full window size for debug quad (uses NDC coordinates)
    glm::ivec2 windowSize = m_window->windowSize();
    glViewport(0, 0, windowSize.x, windowSize.y);

    // Get texture ID and configure it for reading (not shadow comparison)
    unsigned int texID = m_directionalLightSource->shadowMapTexture();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Save current texture compare mode and disable it for reading raw depth
    GLint compareMode;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, &compareMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

    // Load a simple shader to display the depth map
    static Material* debugShader = nullptr;
    if (!debugShader) {
        debugShader = new Material(
            "resources/shaders/DebugDepthVert.glsl",
            "resources/shaders/DebugDepthFrag.glsl",
            "DebugDepthShader"
        );
    }

    debugShader->use();
    debugShader->setInt("depthMap", 0);
    debugShader->setFloat("nearPlane", 1.f);
    debugShader->setFloat("farPlane", 64.f);

    // Verify texture is bound
    GLint boundTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTexture);
    printf("Debug: Bound texture ID: %d, Expected: %u\n", boundTexture, texID);

    // Define a quad in the bottom-right corner of the screen
    // NDC coordinates: bottom-right corner, 1/4 screen size
    float quadVertices[] = {// positions   // texCoords
                            0.5f, 1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f, 0.0f,

                            0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f
    };

    // Create VAO and VBO
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO = 0;
    if (quadVAO == 0) {
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))
        );
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Restore texture comparison mode (if it was set)
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, compareMode);

    // Restore depth test
    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
}
