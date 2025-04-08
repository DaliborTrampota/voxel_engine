#include "Game.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "structures/Shader.h"
#include "structures/ShaderPipeline.h"

#include "data/VertexData.h"

#include "Camera.h"
#include "TextureLoader.h"
#include "level/TerrainGenerator.h"

#include "data/registry/Registry.h"
#include "data/TextureManager.h"

namespace data {
    extern TextureManager textureManager;
}



Game::Game(GLFWwindow* window, float w, float h) : 
    m_window(window), 
    m_mouseState(w, h)
{



    asset::TextureLoader loader;
    loader.load("resources/textures/blocks/", &data::textureManager);

    registry::Init();



	m_worlds[0] = lvl::World(new NoiseGenerator());
	m_player.spawn(&m_worlds[0]);
}

void Game::processInput(GLFWwindow* window, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_player.move(GLFW_KEY_W, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_player.move(GLFW_KEY_S, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_player.move(GLFW_KEY_A, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_player.move(GLFW_KEY_D, dt);
}

void Game::processMouse(GLFWwindow* window, double xposIn, double yposIn)
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

void Game::start()
{



    ShaderPipeline pipeline;
    {
        Shader vert("shaders/VertexShader.glsl", GL_VERTEX_SHADER);
        Shader frag("shaders/PixelShader.glsl", GL_FRAGMENT_SHADER);

        if (!pipeline.registerShader(GL_VERTEX_SHADER, vert)) printf("Vert shader not registered");
        if (!pipeline.registerShader(GL_FRAGMENT_SHADER, frag)) printf("Fragment shader not registered");
        if (!pipeline.link()) return;// throw error or something? 1;
    }


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);



    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);




    pipeline.use();
    pipeline.setCamera(m_player.getCamera());

    //loader.bind(0);
    pipeline.setInt("texArray", 0);

    glm::mat4 model = glm::mat4(1.0f);
    pipeline.setMat4("model", model);

	float fpsCounter = 5.0f;
    int samples = 0;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(m_window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(m_window, deltaTime);
        update(deltaTime);

		samples++;
		fpsCounter -= deltaTime;
        if (fpsCounter <= 0) {
            printf("FPS: %f\n", samples / 5.0f);
			fpsCounter = 5.0f;
            samples = 0;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // RENDERING HERE
        pipeline.setViewMatrix();
        getCurrentWorld().render(&pipeline);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
			printf("OpenGL error: %d\n", err);
        }

        glfwSwapBuffers(m_window);
        glfwPollEvents();

    }

    /*glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);*/
    glDeleteProgram(pipeline.ID);
}

void Game::mouseLock(GLFWwindow* window, bool state) const
{
    glfwSetInputMode(window, GLFW_CURSOR, state ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
