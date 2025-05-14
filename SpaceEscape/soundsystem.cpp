#include "soundsystem.h"
#include <iostream>

SoundSystem::SoundSystem() : m_pSystem(nullptr) {}

SoundSystem::~SoundSystem() {}

bool SoundSystem::initialise()
{
	FMOD_RESULT result;

	result = FMOD::System_Create(&m_pSystem);

	if (result != FMOD_OK) {
		return false;
	}

	m_pSystem->setDSPBufferSize(2048, 8);

	result = m_pSystem->setSoftwareFormat(48000, FMOD_SPEAKERMODE_STEREO, 0);

	result = m_pSystem->init(100, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, nullptr);

	m_pSystem->createChannelGroup("Music", &m_musicGroup);
	m_pSystem->createChannelGroup("SFX", &m_sfxGroup);

	m_musicGroup->setVolume(0.3f);
	m_sfxGroup->setVolume(1.0f);

	return (result == FMOD_OK);
}

void SoundSystem::update()
{
	m_pSystem->update();
}

void SoundSystem::close()
{
	for (auto& pair : m_sounds) {
		pair.second->release();
	}

	if (m_pSystem) {
		m_pSystem->close();
		m_pSystem->release();
		m_pSystem = 0;
	}
}

bool SoundSystem::loadSound(const string& name, const char* filePath, bool looping)
{
	FMOD_MODE mode = FMOD_DEFAULT;
	if (looping) {
		mode |= FMOD_LOOP_NORMAL;
	}

	FMOD::Sound* sound = nullptr;
	FMOD_RESULT result = m_pSystem->createSound(filePath, mode, nullptr, &sound);
	if (result == FMOD_OK) {
		m_sounds[name] = sound;
		return true;
	}

	return false;
}

void SoundSystem::playSound(const string& name, float volume, bool isMusic)
{
	auto it = m_sounds.find(name);
	if (it == m_sounds.end()) return;

	FMOD::Channel* channel;
	FMOD::ChannelGroup* channelG = isMusic ? m_musicGroup : m_sfxGroup;

	FMOD_RESULT result = m_pSystem->playSound(it->second, channelG, false, &channel);

	if (result == FMOD_OK && channel) {
		channel->setVolume(volume);

		channel->setMode(FMOD_3D_WORLDRELATIVE);
		channel->setPriority(128);
	}

}

void SoundSystem::stopSound(const string& name)
{
	auto it = m_sounds.find(name);

	if (it != m_sounds.end()) {

		m_pSystem->playSound(it->second, 0, true, 0);
	}
}
