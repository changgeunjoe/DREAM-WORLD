#pragma once
#ifdef _DEBUG
#include "../PCH/stdafx.h"
#endif
#include "../Session/SessionObject/ShootingSessionObject.h"
#include "../Session/SessionObject/MonsterSessionObject.h"
#include "../Session/SessionObject/SmallMonsterSessionObject.h"
#include "../Session/SessionObject/ChracterSessionObject.h"

class Room
{
public:
	Room();
	Room(const Room& rhs);
	~Room();
public:
	Room& operator=(Room& rhs);
private:
	bool m_isAlive = false;
	int m_roomId = -1;
	std::wstring m_roomName;
	int m_roomOwnerId = -1;// 룸 생성한 자의 ID
	ROOM_STATE m_roomState = ROOM_STAGE1;
private:
	int m_stage1TrigerCnt = 0;
public:
	void SetRoomId(int roomId);
	bool IsArriveState() { return m_isAlive; }
	//Player UserSession
private:
	//ingame Player
	std::mutex m_lockInGamePlayers;
	std::map<ROLE, int> m_inGamePlayers;

	//disconnect Player에 대해서 저장, logic클래스에서도 Player_State가 INGameRoom일때 nickName저장 후, 함
	std::mutex m_lockdisconnectedPlayer;
	std::map<std::wstring, ROLE> m_disconnectedPlayers;//key: nickName, value: Role
private:
	std::map<ROLE, ChracterSessionObject*> m_characterMap;

public:
	//ingame Player
	void SendAllPlayerInfo();
	void InsertInGamePlayer(std::map<ROLE, int>& matchPlayer);
	void InsertInGamePlayer(ROLE r, int playerId);
	void DeleteInGamePlayer(int playerId);
	std::map<ROLE, int> GetPlayerMap();
	int GetPlayerNum() { return m_inGamePlayers.size(); }
public:
	//disconnect Player
	void InsertDisconnectedPlayer(int id);
	bool CheckDisconnectedPlayer(std::wstring& name);
private:
	void DeleteDisconnectedPlayer(int playerId, std::wstring& name);
	//Monster UserSession
private:
	MonsterSessionObject m_boss;
	SmallMonsterSessionObject m_StageSmallMonster[15];
	SmallMonsterSessionObject m_BossSmallMonster[15];
public:
	void CreateBossMonster();
	MonsterSessionObject& GetBoss();
public:
	int m_arrowCount = 0;
	int m_ballCount = 0;
	std::array<ShootingSessionObject, 10> m_arrows;
	std::array<ShootingSessionObject, 10> m_balls;
	Concurrency::concurrent_queue<int> m_restArrow;
	Concurrency::concurrent_queue<int> m_restBall;
	void ShootArrow(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed);
	void ShootBall(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 srcPos, float speed);
public:
	Concurrency::concurrent_queue<short> m_bossDamagedQueue;
	bool MeleeAttack(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 pos);
public:
	void ChangeDirectionPlayCharacter(ROLE r, DIRECTION d);
	void StopMovePlayCharacter(ROLE r);
	DirectX::XMFLOAT3 GetPositionPlayCharacter(ROLE r);
	bool AdjustPlayCharacterInfo(ROLE r, DirectX::XMFLOAT3& postion);
	void RotatePlayCharacter(ROLE r, ROTATE_AXIS axis, float& angle);
	void StartMovePlayCharacter(ROLE r, DIRECTION d);
	void SetMouseInputPlayCharacter(ROLE r, bool left, bool right);
	bool GetLeftAttackPlayCharacter(ROLE r);
	short GetAttackDamagePlayCharacter(ROLE r);
public:
	void GameStart();
	void BossStageStart();
	void GameRunningLogic();
	void GameEnd();
	void BossFindPlayer();
	void ChangeBossState();
	void UpdateGameStateForPlayer_STAGE1();
	void UpdateSmallMonster();
	void UpdateGameStateForPlayer_BOSS();
	void BossAttackExecute();

};
