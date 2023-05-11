#pragma once
#include "../PCH/stdafx.h"
#include "ExpOver.h"

class PlayerSessionObject;
class Session
{
private:
	std::mutex			m_playerStateLock;
	int					m_id;
	PLAYER_STATE		m_playerState;

	ExpOver				m_exOver;
	SOCKET				m_socket;
	int					m_prevBufferSize;
	std::wstring m_playerName;

public:
	PlayerSessionObject* m_sessionObject;
	int m_roomId;
public:
	Session();
	~Session();

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
public:
	PlayerSessionObject* SetPlaySessionObject(ROLE r);
	void ResetPlayerToLobbyState();
	friend class SessionObject;
};
