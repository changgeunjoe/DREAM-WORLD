#pragma once
#include "../stdafx.h"
#include "include/fmod.hpp"
#include "include/fmod_errors.h"

using namespace std;
class GameSound
{
private:
	FMOD::System* m_soundSystem;

	map<string, pair<FMOD::Sound*, FMOD::Channel*> > m_sounds;
public:
	GameSound();
	~GameSound();
public:
	void MakeSound(string soundKey, string soundDirName);
	void MakeSoundLoop(string soundKey, string soundDirName);
	void Play(string soundName, float volume);
	void Pause(string soundName);
	void ALLPause();
	void NoLoopPlay(string soundName, float volume);
};