#pragma once
enum PLAYER_STATE
{
	FREE,
	ALLOC,
	IN_GAME
};

enum SESSION_CATEGORY
{
	NONE,
	PLAYER,
	NPC
};

class SessionObject;
class Session
{
private:
	std::mutex			m_playerStateLock;
	int					m_prevPacket;
	int					m_id;
	PLAYER_STATE		m_playerState;

public:
	SESSION_CATEGORY	m_sessionCategory;
	SessionObject* m_sessionObject;
public:
	Session();
	~Session();

private:
	void Initialize();

public:
	void RegistPlayer(int id, SOCKET& socket);

	const int GetId() { return m_id; }
	const PLAYER_STATE GetPlayerState() { return m_playerState; };
};

