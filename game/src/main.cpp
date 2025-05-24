#include <core/gl/GraphicsAPI.h>

#include "Game.h"
#include "Globals.h"

int gameEntry() {

    std::unique_ptr<gl::Window> window = std::make_unique<gl::Window>(std::make_unique<gl::GraphicsAPI>());
    Game game(std::move(window), window->size());
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
