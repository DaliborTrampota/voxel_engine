#pragma once

class Game;

class GameServices {
    public:
        static Game* getGame() { return m_game; }
        static void setGame(Game* game) { m_game = game; }

    private:
        static inline Game* m_game;
};