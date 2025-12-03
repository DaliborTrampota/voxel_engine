#pragma once

#include <glm/glm.hpp>

#include <soloud_wav.h>
#include <soloud_wavstream.h>
#include <variant>

#include "AudioSettings.h"

namespace SoLoud {
    class Soloud;
}

namespace engine {

    enum class AudioType {
        // Invalid,
        Effect,
        Music,
        Background,
    };

    class Audio {
      public:
        //~Audio();

        // void isValid();

        /// @brief Plays the audio with the current settings.
        /// @note For effects, it will spawn a new instance of the audio and it is not possible to control the settings of previous instances. If you need to control settings of all instances, use AudioManager::playEffect and store the Audio object.
        void play();

        /// @brief Stops the audio.
        void stop();

        /// @brief Restarts the audio.
        void restart();

        /// @brief Checks if the audio has finished playing.
        /// @return True if the audio has finished playing, false otherwise.
        bool finished() const;

        void pause();
        void resume();
        bool isPaused() const;
        bool isPlaying() const;

        void setVolume(float volume);
        void setLooping(bool looping);
        void setPosition(const glm::vec3& position);
        void setAcceleration(const glm::vec3& acceleration);

        /// @brief Applies the given settings to the audio.
        /// @param settings The settings to apply to the audio.
        /// @note It will be used rightaway for currently playing audio and for future plays.
        void applySettings(const AudioSettings& settings);

        /// @brief Plays the audio with the given settings.
        /// @param settings The settings to apply to the audio.
        /// @param saveSettings If true, the settings will be saved and used for future plays.
        /// @note If the audio is already playing, it will restart.
        /// @note If the audio is later restarted and saveSettings is false, this settings will be lost.
        void playWithSettings(const AudioSettings& settings, bool saveSettings = false);

        const AudioSettings& getSettings() const;

      private:
        friend class AudioManager;

        Audio(
            SoLoud::Soloud* soloud,
            SoLoud::Wav* audio,
            AudioType audioType,
            const AudioSettings& settings
        );
        Audio(
            SoLoud::Soloud* soloud,
            SoLoud::WavStream* audio,
            AudioType audioType,
            const AudioSettings& settings
        );
        // Audio(); // invalid ctor

        SoLoud::Soloud* m_soloud;
        AudioType m_audioType;
        SoLoud::AudioSource* m_source;
        SoLoud::handle m_handle;

        AudioSettings m_settings;
    };
}  // namespace engine