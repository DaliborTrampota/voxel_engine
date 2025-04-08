#include <fstream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
//#include <glad/glad.h>

#include "Globals.h"
#include "Game.h"
#include "Camera.h"
#include "structures/GraphicsAPI.h"
//void printMat4(glm::mat4 mat) {
//	for (int i = 0; i < 4; i++) {
//		for (int j = 0; j < 4; j++) {
//			std::cout << mat[j][i] << " ";
//		}
//		std::cout << std::endl;
//	}
//
//}


//struct GameWrapper {
//    GameWrapper() = default;
//    GameWrapper(Game* game) {
//        GameWrapper::game = game;
//
//        glfwSetErrorCallback(GameWrapper::errorCallback);
//    }
//
//    static inline Game* game;
//
//    static void errorCallback(int errCode, const char* desc) {
//        printf("code = %d, message = %s\n", errCode, desc);
//    }
//};


int main() {
    

    std::unique_ptr<GraphicsAPI> gAPI = std::make_unique<GraphicsAPI>();

    gAPI.get()->init();

    //gAPI.get()->mouseLock(true);
    Game game(std::move(gAPI), gAPI.get()->getWindowSize());

    game.start();
  
    return 0;
}