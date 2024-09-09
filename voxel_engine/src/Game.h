#pragma once

#include <GLFW/glfw3.h>

#include <unordered_map>

#include "level/World.h"
#include "Camera.h"


class Game
{

public:
	Game();

	void update(float dt);
	void processInput(GLFWwindow* window, float dt);

	Camera cam{ ProjectionType::Perspective };

private:


	std::unordered_map<int, lvl::World> worlds;
};

