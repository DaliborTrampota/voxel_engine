#pragma once

#include <soloud.h>

#include "Audio.h"
#include "AudioSettings.h"

#include <string>
#include <unordered_map>


namespace engine {

    class AudioManager {
      public:
        AudioManager();
        ~AudioManager();

        static AudioManager& Get() {
            static AudioManager instance;
            return instance;
        }

        /// @brief Preloads an audio file for future use.
        /// @param path The path to the audio file.
        /// @param key The identifier for this audio for future use.
        /// @param type The type of audio file.
        /// @return True if the audio file was preloaded successfully, false if the key already exists.
        /// @throws std::runtime_error if the audio file cannot be loaded.
        bool preload(const std::string& path, const std::string& key, AudioType type);

        /// @brief Plays a sound effect. Sound effects are short, one-off sounds.
        /// @param key The identifier of the audio to play (specified in preload).
        /// @param settings The settings for the sound effect.
        /// @return The audio object for the sound effect.
        /// @throws std::runtime_error if the audio file is not preloaded.
        Audio playEffect(const std::string& key, AudioSettings settings = AudioSettings());

        /// @brief Plays a music track. Music tracks are long, and in case of background repeating sounds.
        /// @param key The identifier of the audio to play (specified in preload).
        /// @param settings The settings for the music track.
        /// @return The audio object for the music track.
        /// @throws std::runtime_error if the audio file is not preloaded.
        /// @note If the audio is already playing, it will start from the beginning.
        [[nodiscard]] Audio playMusic(
            const std::string& key, AudioSettings settings = AudioSettings()
        );

        void destroyAll();
        void updatePositionalAudio(
            glm::vec3 listenerPos, glm::vec3 listenerDir, glm::vec3 listenerVel
        );

      private:
        friend class Audio;
        SoLoud::Soloud m_soloud;

        // The SoLoud handle index wraps around 1 million (0xfffff).
        // TODO check if its possible to wrap when playing for long time.
        // std::unordered_map<std::string, Audio> m_audio;
        std::unordered_map<std::string, SoLoud::WavStream> m_music;
        std::unordered_map<std::string, SoLoud::Wav> m_effects;
    };
}  // namespace engine