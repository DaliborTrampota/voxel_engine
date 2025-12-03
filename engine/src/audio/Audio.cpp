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

void Audio::setVelocity(const glm::vec3& velocity) {
    m_settings.directionalData.velocity = velocity;
    m_soloud->set3dSourceVelocity(m_handle, velocity.x, velocity.y, velocity.z);
}

void Audio::applySettings(const AudioSettings& settings) {
    m_settings = settings;
    m_soloud->setVolume(m_handle, settings.volume);
    m_soloud->setLooping(m_handle, settings.loop);
    if (settings.directional) {
        m_soloud->set3dSourcePosition(
            m_handle,
            settings.directionalData.position.x,
            settings.directionalData.position.y,
            settings.directionalData.position.z
        );
        m_soloud->set3dSourceVelocity(
            m_handle,
            settings.directionalData.velocity.x,
            settings.directionalData.velocity.y,
            settings.directionalData.velocity.z
        );
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
            // Configure 3D attenuation on the source before playing
            m_source->set3dMinMaxDistance(
                settings.directionalData.minDistance, settings.directionalData.maxDistance
            );
            m_source->set3dAttenuation(
                settings.directionalData.attenuationModel, settings.directionalData.rolloff
            );


            m_handle = m_soloud->play3d(
                *m_source,
                settings.directionalData.position.x,
                settings.directionalData.position.y,
                settings.directionalData.position.z,
                settings.directionalData.velocity.x,
                settings.directionalData.velocity.y,
                settings.directionalData.velocity.z,
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