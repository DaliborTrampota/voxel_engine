#include "AudioManager.h"

#include "soloud_wav.h"
#include "soloud_wavstream.h"

#include <stdexcept>


using namespace engine;
using namespace SoLoud;

AudioManager::AudioManager() {
    m_soloud.init();
}

AudioManager::~AudioManager() {
    destroyAll();
    m_soloud.deinit();
}

bool AudioManager::preload(const std::string& path, const std::string& key, AudioType type) {
    switch (type) {
        case AudioType::Effect: {
            auto [it, inserted] = m_effects.try_emplace(key);
            if (inserted) {
                result res = it->second.load(path.c_str());
                if (res != SO_NO_ERROR) {
                    throw std::runtime_error("Failed to load audio file: " + path);
                }
            }
            return inserted;
        }
        case AudioType::Music:
        case AudioType::Background: {
            auto [it, inserted] = m_music.try_emplace(key);
            if (inserted) {
                result res = it->second.load(path.c_str());
                if (res != SO_NO_ERROR) {
                    throw std::runtime_error("Failed to load audio file: " + path);
                }
            }
            return inserted;
        }
        default:
            throw std::runtime_error(
                "Invalid audio type: " + std::to_string(static_cast<int>(type))
            );
    }
}

Audio AudioManager::playEffect(const std::string& key, AudioSettings settings) {
    auto it = m_effects.find(key);
    if (it == m_effects.end())
        throw std::runtime_error("Audio file not preloaded " + key);

    return Audio(&m_soloud, &it->second, AudioType::Effect, settings);
    // Wav& effect = it->second;
    // if (settings.position.has_value()) {
    //     glm::vec3 position = settings.position.value();
    //     m_soloud.play3d(effect, position.x, position.y, position.z);
    // } else {
    //     m_soloud.play(effect, settings.volume);
    // }
    // return Audio(&m_soloud, &effect, AudioType::Effect, settings);
}

Audio AudioManager::playMusic(const std::string& key, AudioSettings settings) {
    AudioType audioType = settings.background ? AudioType::Background : AudioType::Music;

    auto it = m_music.find(key);
    if (it == m_music.end()) {
        throw std::runtime_error("Audio file not preloaded " + key);
    }
    return Audio(&m_soloud, &it->second, audioType, settings);

    // WavStream& music = it->second;
    // if (settings.background) {
    //     m_soloud.playBackground(music, settings.volume);
    // } else {
    //     if (settings.position.has_value()) {
    //         glm::vec3 position = settings.position.value();
    //         m_soloud.play3d(music, position.x, position.y, position.z);
    //     } else {
    //         m_soloud.play(music, settings.volume);
    //     }
    // }
    // return Audio(&m_soloud, &music, audioType);
}

void AudioManager::destroyAll() {
    m_soloud.stopAll();
    m_music.clear();
    m_effects.clear();
}

void AudioManager::updatePositionalAudio() {
    m_soloud.update3dAudio();
}