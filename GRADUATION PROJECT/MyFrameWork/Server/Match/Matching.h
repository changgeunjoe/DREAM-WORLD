#pragma once
#include "../SingletonBase.h"
#include "MatchQueue.h"

class UserSession;
class Matching : public SingletonBase<Matching>
{
	friend SingletonBase;
private:
	Matching() = default;
	~Matching()
	{
		spdlog::info("Matching::~Matching()");
	}
public:
	void InserMatch(std::shared_ptr<UserSession>& userRef, const ROLE& role);
	void CancelMatch(std::shared_ptr<UserSession>& userRef, const ROLE& role);

	void StartMatching();
private:
	void MatchFunc();

private:
	MatchQueue m_warriorQueue;
	MatchQueue m_archerQueue;
	MatchQueue m_mageQueue;
	MatchQueue m_tankerQueue;

	std::weak_ptr<UserSession> m_lastWarriorUser;
	std::weak_ptr<UserSession> m_lastMageUser;
	std::weak_ptr<UserSession> m_lastTankerUser;
	std::weak_ptr<UserSession> m_lastArcherUser;

};

