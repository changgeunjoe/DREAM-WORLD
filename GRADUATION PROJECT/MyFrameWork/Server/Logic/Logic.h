#pragma once

class Session;
class Logic
{
private:

public:
	Logic();
	~Logic();
public:

	void AcceptPlayer(Session* session, int userId, SOCKET& sock);	
	void ProcessPacket(int userId, char* p);
	void BroadCastPacket(void* p);

};
