#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <unordered_map>

#include "level/World.h"
#include "Camera.h"

struct MouseState
{
	MouseState() = delete;
	MouseState(float width, float height) : lastX(width / 2.0f), lastY(height / 2.0f), firstMouse(true) {}
	float lastX, lastY;
	bool firstMouse;
};

class Game
{

public:
	Game() = delete;
	Game(GLFWwindow* window, float w, float h);

	void start();

	void update(float dt);
	void processInput(GLFWwindow* window, float dt);
    void processMouse(GLFWwindow* window, double xposIn, double yposIn);

	Camera m_cam{ ProjectionType::Perspective };

private:

	GLFWwindow* m_window;
	MouseState m_mouseState;
	std::unordered_map<int, lvl::World> m_worlds;

	int m_currentWorld = 0;

	friend struct GameWrapper;
};