#pragma once

#include <unordered_map>

#include "level/World.h"
#include "Player.h"

struct GLFWwindow;

class Camera;

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
	~Game();

	void start();

	lvl::World& getCurrentWorld() { return m_worlds[m_currentWorld]; }
	const Player* getPlayer() const { return &m_player; }

	void update(float dt);
	void processInput(GLFWwindow* window, float dt);
	void processMouse(GLFWwindow* window, double xposIn, double yposIn);
	void mouseLock(GLFWwindow* window, bool state) const;

	//data::TextureManager* getTextureManager() { return &m_textureManager; 

private:

	GLFWwindow* m_window;
	MouseState m_mouseState;

	//data::TextureManager m_textureManager;


	std::unordered_map<int, lvl::World> m_worlds;

	Player m_player;
	int m_currentWorld = 0;


	friend struct GameWrapper;
};