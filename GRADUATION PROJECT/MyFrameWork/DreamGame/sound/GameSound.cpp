#include "stdafx.h"
#include "GameSound.h"



GameSound::GameSound()
{
	FMOD_RESULT       result;
	void* extradriverdata = 0;
	result = FMOD::System_Create(&m_soundSystem);
	result = m_soundSystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
	MakeSound("testSound", "sound/sounds/Stage1background.mp3");
	MakeSound("ClickSound", "sound/sounds/Bubble.wav");
	MakeSound("UISound", "sound/sounds/Click.wav");
	MakeSound("BossRespawnSound", "sound/sounds/BossRespawnSound.mp3");
	MakeSound("LightningSound", "sound/sounds/LightningSound.wav");
	MakeSound("PriestAttackSound", "sound/sounds/PriestAttackSound.wav");
	MakeSound("AcherAttackSound", "sound/sounds/AcherAttackSound.wav");
	MakeSound("ArcherESkillSound", "sound/sounds/ArcherEskill.wav");
	MakeSound("ArcherQSkillSound", "sound/sounds/AcherQSkillSound.wav");
	//MakeSound("AcherBow", "sound/sounds/AcherAcherBow.mp3");
	MakeSound("WarriorAttackSound", "sound/sounds/WarriorAttackSound.mp3");
	MakeSound("WarriorQskillSound", "sound/sounds/WarriorQskill.mp3");
	MakeSound("TankerAttackSound", "sound/sounds/TankerAttackSound.mp3"); 
	MakeSound("WalkSound", "sound/sounds/WalkSound2.wav");
	MakeSound("ArrowBow", "sound/sounds/ArrowBow.mp3");
	MakeSound("NormalMonsterAttackSound", "sound/sounds/NormalMonsterAttack.mp3");
	MakeSoundLoop("LobbySound", "sound/sounds/LobbySound1.wav"); 
	MakeSoundLoop("Stage1Sound", "sound/sounds/Stage1Sound.wav"); 
	MakeSoundLoop("BossStage", "sound/sounds/BossStage1.mp3");
	MakeSoundLoop("HealSound", "sound/sounds/HealSound.wav");
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
	result = m_sounds[soundKey].first->setMode(FMOD_LOOP_OFF);
}

void GameSound::MakeSoundLoop(string soundKey, string soundDirName)//soundKey: play, pause에 사용할 사운드 키 => map의 키, soundDirName: 사운드 실제 위치
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

void GameSound::Play(string soundName, float volume)
{
    if (!m_sounds.count(soundName)) return;
    bool isPlay = false;
    m_sounds[soundName].second->isPlaying(&isPlay);
    if (isPlay) { 
        m_sounds[soundName].second->setPaused(false);
        m_sounds[soundName].second->setVolume(volume); // 볼륨 조절
        return;
    }
    m_soundSystem->playSound(m_sounds[soundName].first, 0, false, &m_sounds[soundName].second);
    m_sounds[soundName].second->setVolume(volume); // 볼륨 조절
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

void GameSound::NoLoopPlay(string soundName, float volume)
{
	Pause(soundName);
	Play(soundName, volume);
}
