#pragma once

class Game;

namespace engine {
    class InputSystem;
    class AudioManager;
}  // namespace engine

class GameServices {
  public:
    static Game* getGame() { return m_game; }
    static void setGame(Game* game) { m_game = game; }

    static engine::InputSystem* getInputSystem() { return m_inputSystem; }
    static void setInputSystem(engine::InputSystem* inputSystem) { m_inputSystem = inputSystem; }

    static engine::AudioManager* getAudioManager() { return m_audioManager; }
    static void setAudioManager(engine::AudioManager* audioManager) {
        m_audioManager = audioManager;
    }

  private:
    static inline Game* m_game = nullptr;
    static inline engine::InputSystem* m_inputSystem = nullptr;
    static inline engine::AudioManager* m_audioManager = nullptr;
};