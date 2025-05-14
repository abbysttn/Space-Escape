#ifndef _SOUNDSYSTEM_H__
#define _SOUNDSYSTEM_H__

using namespace std;

#include "fmod.hpp"
#include <map>
#include <string>

class SoundSystem {
public:
	SoundSystem();
	~SoundSystem();

	bool initialise();
	void update();
	void close();

	bool loadSound(const string& name, const char* filePath, bool looping = false);
	void playSound(const string& name, float volume, bool isMusic);

	void stopSound(const string& name);

protected:
private:
	map<string, FMOD::Sound*> m_sounds;
	FMOD::System* m_pSystem;
	FMOD::ChannelGroup* m_musicGroup;
	FMOD::ChannelGroup* m_sfxGroup;

};


#endif // !_SOUNDSYSTEM_H__
