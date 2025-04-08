#pragma once

#include <unordered_map>

#include <structures/GLEventSite.h>

#include "level/World.h"
#include "Player.h"

struct GLFWwindow;
class GraphicsAPI;
class Camera;
class Event;

struct MouseState
{
	MouseState() = delete;
	MouseState(glm::ivec2 dims) : lastX(dims.x / 2.0f), lastY(dims.y / 2.0f), firstMouse(true) {}
	float lastX, lastY;
	bool firstMouse;
};

class Game : public GLEventSite
{

public:
	Game() = delete;
	Game(std::unique_ptr<GraphicsAPI> gAPI, glm::ivec2 dims);
	~Game();

	void start();

	lvl::World& getCurrentWorld() { return m_worlds[m_currentWorld]; }
	const Player* getPlayer() const { return &m_player; }

	void update(float dt);
	void processInput(GLFWwindow* window, float dt);
	void processMouse(GLFWwindow* window, double xposIn, double yposIn);
	void mouseLock(GLFWwindow* window, bool state) const;

protected:    
    void windowResizeEvent(ResizeEvent* pEvent) override;
    void mouseMoveEvent(MouseEvent* pEvent) override;;
	//data::TextureManager* getTextureManager() { return &m_textureManager; 

private:
	std::unique_ptr<GraphicsAPI> m_api;
	GLFWwindow* m_window;
	MouseState m_mouseState;

	//data::TextureManager m_textureManager;


	std::unordered_map<int, lvl::World> m_worlds;

	Player m_player;
	int m_currentWorld = 0;


	friend struct GameWrapper;
};