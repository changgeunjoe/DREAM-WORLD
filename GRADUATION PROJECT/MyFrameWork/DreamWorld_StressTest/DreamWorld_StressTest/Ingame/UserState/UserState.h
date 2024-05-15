#pragma once
#include "../../PCH/stdafx.h"
#include "../StateBase/StateBase.h"

namespace DreamWorld {
	class StressUserSession;
}

class CharacterObject;

enum class USER_STATE
{
	LOBBY,
	MATCH,
	INGAME,
};

class UserState : public StateBase
{
public:
public:
	UserState(std::shared_ptr<DreamWorld::StressUserSession> userSession) : m_userSession(userSession) {}
	virtual void EnterState() = 0;
	virtual void UpdateState() = 0;
	virtual void ExitState() = 0;

protected:
	std::shared_ptr<DreamWorld::StressUserSession> m_userSession;
};

class LobbyState : public UserState
{
public:
	LobbyState(std::shared_ptr<DreamWorld::StressUserSession> userSession) : UserState(userSession) {}
	virtual void EnterState() override;
	virtual void UpdateState() override {}
	virtual void ExitState() override {}
};

class MatchState : public UserState
{
public:
	MatchState(std::shared_ptr<DreamWorld::StressUserSession> userSession) : UserState(userSession) {}
	virtual void EnterState() override {}
	virtual void UpdateState() override;
	virtual void ExitState() override {}
};

class InGameState : public UserState
{
public:
	InGameState(std::shared_ptr<DreamWorld::StressUserSession> userSession);
	virtual void EnterState() override;
	virtual void UpdateState() override;
	virtual void ExitState() override;
private:
	//플레이 하는 캐릭터마다 스킬 공격 쿨타임이 다르기 때문에 따로 생성
	std::shared_ptr<CharacterObject> m_characterRef;
	std::atomic<ROLE> m_currentRole;

	bool m_isChangeBoss = false;
	std::chrono::high_resolution_clock::time_point m_changeBossTime;
	bool m_isGameEnd = false;
	std::chrono::high_resolution_clock::time_point m_gameEndTime;


};
