#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <iostream>
#include <fstream>


#include "Game.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



void printMat4(glm::mat4 mat) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << mat[j][i] << " ";
		}
		std::cout << std::endl;
	}

}


struct GameWrapper {
    GameWrapper() = default;
    GameWrapper(Game* game) {
        GameWrapper::game = game;

        glfwSetCursorPosCallback(game->m_window, GameWrapper::mouseCallback);
        glfwSetFramebufferSizeCallback(game->m_window, GameWrapper::framebufferSizeCallback);
        glfwSetErrorCallback(GameWrapper::errorCallback);
    }

    static inline Game* game;

    static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
        game->processMouse(window, xpos, ypos);
    }
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        game->m_cam.resize(width, height);
    }
    static void errorCallback(int errCode, const char* desc) {
        printf("code = %d, message = %s\n", errCode, desc);
    }
};


int main() {
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); for apple

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }




    Game game(window, SCR_WIDTH, SCR_HEIGHT);
    GameWrapper gameWrapper(&game);
    game.m_cam.mouseLock(window, true);

    game.start();
  
    glfwTerminate();
    return 0;
}