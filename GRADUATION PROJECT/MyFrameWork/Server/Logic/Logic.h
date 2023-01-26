#pragma once

class Session;
class Logic
{
private:

public:
	Logic();
	~Logic();
private:
	volatile bool m_isRunningThread = false;
	std::thread m_PlayerMoveThread;
public:

	void AcceptPlayer(Session* session, int userId, SOCKET& sock);
	void ProcessPacket(int userId, char* p);
	void BroadCastPacket(void* p);
	void MultiCastOtherPlayer(int userId, void* p);
public:
	void AutoMoveServer();
};
