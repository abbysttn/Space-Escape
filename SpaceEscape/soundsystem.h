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

	static SoundSystem& getInstance();

	bool initialise();
	void update();
	void close();

	bool loadSound(const string& name, char* filePath, bool looping = false);
	void playSound(const string& name);

protected:
private:
	map<string, FMOD::Sound*> m_sounds;
	FMOD::System* m_pSystem;

};


#endif // !_SOUNDSYSTEM_H__
