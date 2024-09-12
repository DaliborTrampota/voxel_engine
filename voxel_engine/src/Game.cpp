#include "Game.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>



#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "structures/Shader.h"
#include "structures/ShaderPipeline.h"

#include "VertexData.h"


Game::Game(GLFWwindow* window, float w, float h) : m_window(window), m_mouseState(w, h)
{
	m_worlds[0] = lvl::World(new NoiseGenerator());
}

void Game::update(float dt)
{
}

void Game::processInput(GLFWwindow* window, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cam.move(GLFW_KEY_W, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cam.move(GLFW_KEY_S, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cam.move(GLFW_KEY_A, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cam.move(GLFW_KEY_D, dt);
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

    m_cam.rotate(xoffset, yoffset);
}

void Game::start()
{

    ShaderPipeline pipeline;
    {
        Shader vert("shaders/VertexShader.glsl", GL_VERTEX_SHADER);
        Shader frag("shaders/PixelShader.glsl", GL_FRAGMENT_SHADER);

        if (!pipeline.registerShader(GL_VERTEX_SHADER, vert)) std::cout << "Vert shader not registered" << std::endl;
        if (!pipeline.registerShader(GL_FRAGMENT_SHADER, frag)) std::cout << "Fragment shader not registered" << std::endl;
        if (!pipeline.link()) return;// throw error or something? 1;
    }


    glEnable(GL_DEPTH_TEST);




    /*float vertices[36 * 3];
    int idx = 0;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            for (int c = 0; c < 3; c++) {
                vertices[idx] = vert::vertices[vert::faces[i][j]][c] + 0.5f;
                idx++;
            }
        }
    }
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    pipeline.registerAttribute(0, 3, GL_FLOAT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind VBO
    glBindVertexArray(0); //unbind VAO


    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/


    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    pipeline.use();
    pipeline.setCamera(&m_cam);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(m_window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(m_window, deltaTime);
        update(deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // RENDERING HERE
        glm::mat4 view = m_cam.getView();
        pipeline.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        pipeline.setMat4("model", model);

        getCurrentWorld().render(&pipeline);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    /*glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);*/
    glDeleteProgram(pipeline.ID);
}
