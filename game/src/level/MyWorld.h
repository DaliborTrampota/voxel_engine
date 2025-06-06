#pragma once

#include <level/World.h>
#include <render/RenderContext.h>


class Player;

class MyWorld : public engine::World {
    public:
        void render(engine::RenderContext& ctx, int pass = 0) override;
        void setPlayer(std::shared_ptr<Player> player);

    private:
        std::shared_ptr<Player> m_player;
};