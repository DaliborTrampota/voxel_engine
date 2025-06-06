#pragma once

#include <level/World.h>
#include <render/RenderContext.h>

namespace engine
{
    class Engine;
}


class Player;

class MyWorld : public engine::World {
    public:
        void render(engine::Engine& engine, int pass = 0) override;
        void setPlayer(std::shared_ptr<Player> player);

    private:
        std::shared_ptr<Player> m_player;
};