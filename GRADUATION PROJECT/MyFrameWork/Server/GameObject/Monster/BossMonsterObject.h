#pragma once
#include "../../PCH/stdafx.h"
#include "MonsterObject.h"
class BossMonsterObject : public MonsterObject
{
public:
	BossMonsterObject() = delete;
	BossMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);
	~BossMonsterObject() = default;

	virtual void Update() override;

	void UpdateRoad();//Astar�� �̿��ؼ� ��ã�� ��� ������ �ֽ�ȭ

protected:
	std::mutex m_roadLock;
	//std::queue<AstarNode> m_road;
private:
	XMFLOAT3 m_nextNode;
};

