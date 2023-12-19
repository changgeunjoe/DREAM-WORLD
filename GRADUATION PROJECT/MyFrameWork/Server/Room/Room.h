//#pragma once
//#ifdef _DEBUG
//#include "../PCH/stdafx.h"
//#endif
//
//class Room
//{
//public:
//	Room();
//	Room(const Room& rhs);
//	~Room();
//public:
//	Room& operator=(Room& rhs);
//private:
//	std::atomic_bool m_isAlive;
//	int m_roomId = -1;
//	ROOM_STATE m_roomState = ROOM_STAGE1;
//private://play Character
//	std::mutex								m_playerMapLock;
//	std::unordered_map<int, ROLE>			m_playerMap;
//	std::map<ROLE, ChracterSessionObject*>	m_characterMap;
//	//Monster
//private:
//	MonsterSessionObject m_boss;
//	SmallMonsterSessionObject m_StageSmallMonster[15];
//private:
//	int m_arrowCount = 0;
//	int m_ballCount = 0;
//	std::array<ShootingSessionObject, 10> m_arrows;
//	std::array<ShootingSessionObject, 10> m_balls;
//	std::array< ShootingSessionObject, 3> m_skillarrow;
//	std::array< MeteoSessionObject, 10> m_meteos;
//
//	Concurrency::concurrent_queue<int> m_restArrow;
//	Concurrency::concurrent_queue<int> m_restBall;
//private:
//	XMFLOAT3 m_skyArrowAttack;
//	std::chrono::high_resolution_clock::time_point m_meteoTime = std::chrono::high_resolution_clock::now();
//	int GetPlayerIdFromRole(const ROLE& role);
//
//public:
//	void SetRoomId(int roomId);
//	ROOM_STATE GetStageState();
//public:
//	void ChracterAddDirection(const int userId, const ROLE& role, const DIRECTION& direction);
//	void ChracterRemoveDirection(const int userId, const ROLE& role, const DIRECTION& direction);
//	void ChracterStop(const ROLE& role);
//	void CharacterRotate(const ROLE& role, const ROTATE_AXIS& axis, const float& angle);
//
//	void CharacterMouseInput(const ROLE& role, const bool& left, const bool& right);
//
//	void CharacterSkill_Execute_Q(const ROLE& role, const XMFLOAT3& float3);
//	void CharacterSkill_Execute_E(const ROLE& role, const XMFLOAT3& float3);
//	void CharacterSkill_Input_Q(const ROLE& role);
//	void CharacterSkill_Input_E(const ROLE& role);
//	void CharacterCommonAttackExecute(const ROLE& role, const XMFLOAT3& dir, const int& power);
//
//	void SkipNPC_Communication();
//
//	void GameEnd();
//	void ChangeToBossStage();
//
//	///////////////////////////
//public:
//	std::unordered_map<int, ROLE> GetPlayerInfo();
//	std::map<ROLE, ChracterSessionObject*>& GetCharactersInfo();
//	void GameStart();
//	void Update();
//public:
//	void BroadCastPacket(const char* packet);
//	void BroadCastPacket(const char* packet, const std::unordered_map<int, ROLE>& playerMap);
//	void MultiCastPacket(const char* packet, int exclusionId);
//
//
//public:
//	void ExecuteMageThunder(const XMFLOAT3& position);
//	void ExecuteLongSwordAttack(const DirectX::XMFLOAT3& dir, const DirectX::XMFLOAT3& pos);
//	void ExecuteThreeArrow(const DirectX::XMFLOAT3& dir, const DirectX::XMFLOAT3& position);
//	void ExecuteHammerAttack(const DirectX::XMFLOAT3& dir, const XMFLOAT3& pos);
//	void ExecuteSkyArrow();
//	void MeleeAttack(ROLE r, DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 pos, int power);
//	void StartHealPlayerCharacter();
//	void UpdateShieldData();
//	void PutBarrierOnPlayer();
//	void RemoveBarrier();
//	void HealPlayerCharacter();
//	void StartSkyArrow(const XMFLOAT3& position);
//
//	void PushRestArrow(int id);
//	void PushRestBall(int id);
//	void ShootArrow(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed, float damage);
//	void ShootBall(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos);
//public:
//	const MonsterSessionObject& GetBoss();
//	SmallMonsterSessionObject* const GetStageMonsterArr();
//
//};
//
//
////
////void Room::ResetRoom()
////{
////	for (auto& character : m_characterMap) {
////		character.second->SetStage_1Position();
////	}
////	m_boss.SetBossStagePosition();
////	m_roomState = ROOM_STAGE1;
////	m_isAlive = false;
////	m_boss.isBossDie = false;
////}