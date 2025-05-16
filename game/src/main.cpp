#include <fstream>

#include "Globals.h"
#include "Game.h"
#include "Camera.h"
#include <GraphicsAPI.h>

int main() {
    

    gl::GraphicsAPI* gAPI = new gl::GraphicsAPI();
    gAPI->init();
    
    Game game(gAPI, gAPI->getWindowSize());
    game.start();
  
    return 0;
}