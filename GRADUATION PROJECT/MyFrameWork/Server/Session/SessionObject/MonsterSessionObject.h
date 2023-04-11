#pragma once
#include "SessionObject.h"
#include "../../Room/Room.h"
class MonsterSessionObject : public SessionObject
{
public:
	MonsterSessionObject(Session* session);
	MonsterSessionObject(Session* session, std::string& roomId);
	~MonsterSessionObject();

private:
	std::string m_roomId;
public:
	virtual void Recv() override;
public:
	void BossRandMove();
};
