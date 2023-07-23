#include "stdafx.h"
#include "GameSound.h"



GameSound::GameSound()
{
	FMOD_RESULT       result;
	void* extradriverdata = 0;
	result = FMOD::System_Create(&m_soundSystem);
	result = m_soundSystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
	MakeSound("testSound", "sound/sounds/sound_walk.mp3");
}

GameSound::~GameSound()
{
	FMOD_RESULT       result;
	for (auto& sound : m_sounds) {
		sound.second.first->release();
	}
	result = m_soundSystem->close();
	result = m_soundSystem->release();

}

void GameSound::MakeSound(string soundKey, string soundDirName)//soundKey: play, pause에 사용할 사운드 키 => map의 키, soundDirName: 사운드 실제 위치
{
	FMOD_RESULT       result;
	m_sounds.emplace(soundKey, make_pair(nullptr, nullptr));
	m_sounds[soundKey].first;
	m_sounds[soundKey].second;
	string aa = filesystem::current_path().string();
	result = m_soundSystem->createSound(soundDirName.c_str(), FMOD_DEFAULT, 0, &m_sounds[soundKey].first);
	if (FMOD_OK != result) {
		cout << soundKey << " load Fail, dir: " << soundDirName << endl;
	}
	result = m_sounds[soundKey].first->setMode(FMOD_LOOP_NORMAL);
}

void GameSound::Play(string soundName)
{
	if (!m_sounds.count(soundName))return;
	bool isPlay = false;
	m_sounds[soundName].second->isPlaying(&isPlay);
	if (isPlay) {
		m_sounds[soundName].second->setPaused(false);
		return;
	}
	m_soundSystem->playSound(m_sounds[soundName].first, 0, false, &m_sounds[soundName].second);
	m_sounds[soundName].second->setVolume(1.0f);
}

void GameSound::Pause(string soundName)
{
	if (!m_sounds.count(soundName))return;
	bool isPlay = false;
	m_sounds[soundName].second->isPlaying(&isPlay);
	if (!isPlay)return;
	m_sounds[soundName].second->setPaused(true);
	m_sounds[soundName].second->setPosition(0, FMOD_TIMEUNIT_MS);
}
