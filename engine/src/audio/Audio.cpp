#include "Audio.h"

#include <soloud.h>

using namespace engine;
using namespace SoLoud;


Audio::Audio(
    SoLoud::Soloud* soloud,
    SoLoud::WavStream* audio,
    AudioType audioType,
    const AudioSettings& settings
)
    : m_soloud(soloud),
      m_audioType(audioType),
      m_source(audio),
      m_settings(settings) {
    playWithSettings(settings);
}

Audio::Audio(
    SoLoud::Soloud* soloud, SoLoud::Wav* audio, AudioType audioType, const AudioSettings& settings
)
    : m_soloud(soloud),
      m_audioType(audioType),
      m_source(audio),
      m_settings(settings) {
    playWithSettings(settings);
}

// Audio::Audio() : m_soloud(nullptr), m_audioType(AudioType::Invalid) {
//     m_handle = -1;
// }

void Audio::play() {
    if (m_audioType == AudioType::Effect)
        playWithSettings(m_settings);
}

void Audio::stop() {
    m_soloud->stop(m_handle);
}

void Audio::restart() {
    if (finished()) {
        playWithSettings(m_settings);
    } else {
        m_soloud->seek(m_handle, 0.0f);
    }
}

bool Audio::finished() const {
    return !m_soloud->isValidVoiceHandle(m_handle);
}

void Audio::pause() {
    m_soloud->setPause(m_handle, true);
}

void Audio::resume() {
    m_soloud->setPause(m_handle, false);
}

bool Audio::isPaused() const {
    return m_soloud->getPause(m_handle);
}

bool Audio::isPlaying() const {
    return m_soloud->isValidVoiceHandle(m_handle) && !m_soloud->getPause(m_handle);
}

void Audio::setLooping(bool looping) {
    m_settings.loop = looping;
    m_soloud->setLooping(m_handle, looping);
}

void Audio::setVolume(float volume) {
    m_settings.volume = volume;
    m_soloud->setVolume(m_handle, volume);
}

void Audio::setPosition(const glm::vec3& position) {
    m_settings.directionalData.position = position;
    m_soloud->set3dSourcePosition(m_handle, position.x, position.y, position.z);
}

void Audio::setAcceleration(const glm::vec3& acceleration) {
    m_settings.directionalData.acceleration = acceleration;
    m_soloud->set3dSourceVelocity(m_handle, acceleration.x, acceleration.y, acceleration.z);
}

void Audio::applySettings(const AudioSettings& settings) {
    m_settings = settings;
    m_soloud->setVolume(m_handle, settings.volume);
    m_soloud->setLooping(m_handle, settings.loop);
    if (settings.directional) {
        glm::vec3 position = settings.directionalData.position;
        glm::vec3 acceleration = settings.directionalData.acceleration;
        m_soloud->set3dSourcePosition(m_handle, position.x, position.y, position.z);
        m_soloud->set3dSourceVelocity(m_handle, acceleration.x, acceleration.y, acceleration.z);
    }
}

// TODO, check if already playing and restart
void Audio::playWithSettings(const AudioSettings& settings, bool saveSettings) {
    if (saveSettings) {
        m_settings = settings;
    }
    if (m_audioType == AudioType::Background) {
        m_handle = m_soloud->playBackground(*m_source, settings.volume);

    } else {
        if (settings.directional) {
            glm::vec3 position = settings.directionalData.position;
            glm::vec3 acceleration = settings.directionalData.acceleration;
            m_handle = m_soloud->play3d(
                *m_source,
                position.x,
                position.y,
                position.z,
                acceleration.x,
                acceleration.y,
                acceleration.z,
                settings.volume
            );
        } else {
            m_handle = m_soloud->play(*m_source, settings.volume);
        }
    }
    if (settings.loop)
        setLooping(true);
}

const AudioSettings& Audio::getSettings() const {
    return m_settings;
}