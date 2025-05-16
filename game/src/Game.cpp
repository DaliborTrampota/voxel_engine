#include "Game.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gl/Shader.h>
#include <gl/ShaderPipeline.h>
#include <gl/Window.h>
#include <GraphicsAPI.h>
#include <gl/GLEvents.h>

#include <data/VertexData.h>

#include <Camera.h>
#include <TextureLoader.h>
#include <level/TerrainGenerator.h>

#include "registry/Blocks.h"
#include <level/World.h>

//#include <tracy/Tracy.hpp>
using namespace engine;


Game::Game(gl::GraphicsAPI* gAPI, glm::ivec2 dims) :
    gl::Window(gAPI),
    m_mouseState(dims)
{
    TextureLoader loader;
    loader.load("resources/textures/blocks/");
    //loader.bind();
    RegisterBlocks();
}

void Game::processInput(float dt)
{
    
    if (getKeyState(Key::Esc) == KeyState::Pressed)
        close();

    if (getKeyState(Key::W) == KeyState::Pressed)
        m_player.move(Key::W, dt);
    if (getKeyState(Key::S) == KeyState::Pressed)
        m_player.move(Key::S, dt);
    if (getKeyState(Key::A) == KeyState::Pressed)
        m_player.move(Key::A, dt);
    if (getKeyState(Key::D) == KeyState::Pressed)
        m_player.move(Key::D, dt);
}

void Game::processMouse(double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_mouseState.firstMouse)
    {
        m_mouseState.lastX = xpos;
        m_mouseState.lastY = ypos;
        m_mouseState.firstMouse = false;
    }

    float xoffset = xpos - m_mouseState.lastX;
    float yoffset = m_mouseState.lastY - ypos; // reversed since y-coordinates go from bottom to top

    m_mouseState.lastX = xpos;
    m_mouseState.lastY = ypos;

    m_player.rotate(xoffset, yoffset);
}

Game::~Game()
{
}

void Game::update(float dt)
{
}

void Game::render(double dt)
{
    processInput(dt);
    update(dt);

    Engine::render(&m_pipeline, m_plrCamera, activeWorld());
}

void Game::start()
{
	m_player.spawn(activeWorld());

    {
        gl::Shader vert("shaders/VertexShader.glsl", GL_VERTEX_SHADER);
        gl::Shader frag("shaders/PixelShader.glsl", GL_FRAGMENT_SHADER);

        if (!m_pipeline.registerShader(GL_VERTEX_SHADER, vert)) printf("Vert shader not registered");
        if (!m_pipeline.registerShader(GL_FRAGMENT_SHADER, frag)) printf("Fragment shader not registered");
        if (!m_pipeline.link()) return;// throw error or something? 1;
    }


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    m_plrCamera = m_player.getCamera();
    m_pipeline.use();

    m_plrCamera->lookAt(glm::vec3(0, 0, 0));
    //loader.bind(0);
    m_pipeline.setInt("texArray", 0);

    glm::mat4 model = glm::mat4(1.0f);
    m_pipeline.setMat4("model", model);
    m_pipeline.setMat4("projection", m_plrCamera->getProjection());


    
    mouseLock(true);
    gameloop();
}
