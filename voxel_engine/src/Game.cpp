#include "Game.h"



Game::Game()
{
	worlds[0] = lvl::World(new NoiseGenerator());
}

void Game::update(float dt)
{
}

void Game::processInput(GLFWwindow* window, float dt)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.move(GLFW_KEY_W, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.move(GLFW_KEY_S, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.move(GLFW_KEY_A, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.move(GLFW_KEY_D, dt);
}