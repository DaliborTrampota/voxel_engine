#include <core/GraphicsAPI.h>

#include "Globals.h"
#include "Game.h"

int gameEntry() {
    gl::GraphicsAPI* gAPI = new gl::GraphicsAPI();
    gAPI->init();

    Game game(gAPI, gAPI->getWindowSize());
    game.start();

    return 0;
}

#if defined(_WIN32)
#include "Windows.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return gameEntry();
}

#endif

int main(int argc, char* argv[]) {
    return gameEntry();
}
