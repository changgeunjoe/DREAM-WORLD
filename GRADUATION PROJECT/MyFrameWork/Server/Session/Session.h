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
	BOSS,
	MONSTER
};

class SessionObject;
class Session
{
private:
	std::mutex			m_playerStateLock;
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
	void SetInGameState() {
		{
			std::lock_guard<std::mutex> psLock(m_playerStateLock);
			m_playerState = PLAYER_STATE::IN_GAME;
		}
	}
	const int GetId() { return m_id; }
	const PLAYER_STATE GetPlayerState() { return m_playerState; };
	void RegistMonster(int id, std::string& roomId);
	friend class SessionObject;
};

