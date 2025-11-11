#include <iostream>

#include <render/Window.h>
#include "Game.h"

// Force dedicated GPU (Windows only)
#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;  // for nvidia
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;    // for amd
}
#endif

int gameEntry() {
    std::unique_ptr<engine::Window> window = std::make_unique<engine::Window>();
    if (!window->init(2304, 1440, "Super Survival")) {
        std::cerr << "Failed to initialize window" << std::endl;
        return -1;
    }
    Game game(std::move(window), window->windowSize());
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
