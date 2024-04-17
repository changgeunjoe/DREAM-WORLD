#pragma once
#include "../../PCH/stdafx.h"
#include "MonsterObject.h"

class CharacterObject;
class BossMonsterObject : public MonsterObject
{
private:
	static constexpr std::string_view RESEARCH_ROAD = "RESEARCH_ROAD";
public:
	BossMonsterObject() = delete;
	BossMonsterObject(const float& maxHp, const float& moveSpeed, const float& boundingSize, std::shared_ptr<Room>& roomRef);
	~BossMonsterObject() = default;

	virtual void Update() override;

	void UpdateAggro();
	void UpdateRoad();//Astar�� �̿��ؼ� ��ã�� ��� ������ �ֽ�ȭ
protected:
	virtual std::shared_ptr<CharacterObject> FindAggroCharacter() override;
	//virtual void Attack() override;

private:
	std::mutex m_roadLock;
	std::list<XMFLOAT3> m_road;

	std::shared_ptr<CharacterObject> m_aggroCharacter;
};

