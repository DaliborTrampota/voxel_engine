#include <iostream>

#include <render/Window.h>
#include "Directory.h"
#include "Game.h"


// Force dedicated GPU (win only) (fixed bindless textures [probably laptop only issue?])
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

    std::cout << "Helo Zem!" << std ::endl;  // Vzkaz od mojí Niky <3

    Game game(std::move(window), window->windowSize());
    game.start();

    return 0;
}

#if defined(_WIN32)
#include "Windows.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    initDirectories(exePath);
    return gameEntry();
}

#endif

int main(int argc, char* argv[]) {
    initDirectories(argv[0]);
    return gameEntry();
}
