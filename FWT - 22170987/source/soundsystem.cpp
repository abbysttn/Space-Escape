#include "soundsystem.h"
#include <iostream>

SoundSystem::SoundSystem() : m_pSystem(nullptr) {}

SoundSystem::~SoundSystem() {}

SoundSystem& SoundSystem::getInstance()
{
	static SoundSystem instance;
	return instance;
}

bool SoundSystem::initialise()
{
	FMOD::System_Create(&m_pSystem);
	m_pSystem->init(100, FMOD_INIT_NORMAL, 0);
	return true;
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
	if (m_pSystem->createSound(filePath, mode, 0, &sound) == FMOD_OK) {
		m_sounds[name] = sound;
		return true;
	}

	return false;
}

void SoundSystem::playSound(const string& name, float volume)
{
	auto it = m_sounds.find(name);

	if (it != m_sounds.end()) {
		FMOD::Channel* channel = nullptr;

		m_pSystem->playSound(it->second, 0, false, &channel);

		if (channel) {
			channel->setVolume(volume);
		}
	}

}

void SoundSystem::stopSound(const string& name)
{
	auto it = m_sounds.find(name);

	if (it != m_sounds.end()) {

		m_pSystem->playSound(it->second, 0, true, 0);
	}
}
