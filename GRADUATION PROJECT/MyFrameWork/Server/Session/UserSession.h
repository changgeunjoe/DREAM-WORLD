#pragma once
#include "../PCH/stdafx.h"
#include "ExpOver.h"

class ChracterSessionObject;
class UserSession
{
private:
	std::mutex			m_playerStateLock;
	int					m_id;
	PLAYER_STATE		m_playerState;

	ExpOver				m_exOver;
	SOCKET				m_socket;
	int					m_prevBufferSize;
	std::wstring		m_playerName;
	std::wstring		m_loginId;

	ROLE				m_role;
private:
	std::string		m_ip;
	short		m_port;
public:
	std::string GetUserAddrIn();	
private:
	ChracterSessionObject* m_sessionObject;
	int m_roomId;
public:
	UserSession();
	~UserSession();
public:
	void SetInfoIpAndPort(char* ip, short port);
private:
	void Initialize();
public:
public:
	void Recv();
	void Send(void* p);
	void ConstructPacket(int ioByte);
public:
	void SetInGameState() {
		std::lock_guard<std::mutex> psLock(m_playerStateLock);
		m_playerState = PLAYER_STATE::IN_GAME;
	}
	const int GetId() { return m_id; }
	const PLAYER_STATE GetPlayerState() { return m_playerState; };
public:
	void SetRoomId(int roomId) {
		std::lock_guard<std::mutex> psLock(m_playerStateLock);
		m_playerState = PLAYER_STATE::IN_GAME_ROOM;
		m_roomId = roomId;
	}
	int GetRoomId() { return m_roomId; }
	void RegistPlayer(SOCKET& sock, int id);

public:
	void SetName(std::wstring& inputWst)
	{
		m_playerName = inputWst;
	}
	std::wstring& GetName() { return m_playerName; }
	void SetLoginId(std::wstring& lgId)
	{
		m_loginId = lgId;
	}
	std::wstring& GetLoginId() { return m_loginId; }
public:
	ChracterSessionObject* SetPlaySessionObject(ChracterSessionObject* pSession);
	void PlayCharacterMove();
	std::tuple<int, int, DirectX::XMFLOAT3, DirectX::XMFLOAT3> GetPlayCharacterState();
public:
	void ResetPlayerToLobbyState();
public:
	void SetRole(ROLE r) { m_role = r; }
	ROLE GetRole() { return m_role; }
public:
	void ResetSession();
	/*void ChangeDirectionPlayCharacter(DIRECTION d);
	void StopMovePlayCharacter();
	DirectX::XMFLOAT3 GetPositionPlayCharacter();
	bool AdjustPlayCharacterInfo(DirectX::XMFLOAT3& postion);
	void RotatePlayCharacter(ROTATE_AXIS axis, float& angle);
	void StartMovePlayCharacter(DIRECTION d);
	void SetMouseInputPlayCharacter(bool left, bool right);
	bool GetLeftAttackPlayCharacter();
	short GetAttackDamagePlayCharacter();*/
	friend class SessionObject;
};
