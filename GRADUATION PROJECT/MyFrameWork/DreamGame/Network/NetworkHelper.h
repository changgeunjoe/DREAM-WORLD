#pragma once
#include "../stdafx.h"
#include <thread>
#include <WS2tcpip.h>
#include "../../Server/IOCPNetwork/protocol/protocol.h"


#pragma comment(lib, "WS2_32.lib")

#define SERVER_IP "220.120.241.140"
#define SERVER_PORT 9000
#define MAX_BUF_SIZE 1024

class NetworkHelper {
private:
	SOCKET m_clientSocket;
private:
	char m_buffer[MAX_BUF_SIZE];
	int m_prevPacketSize = 0;
private:
	bool m_bIsRunnung = false;
	std::thread m_runThread;	
public:
	NetworkHelper();
	~NetworkHelper();

public:
	bool TryConnect();
	void Start();
	void RunThread();
public:
	void SendMovePacket(DIRECTION d, const XMFLOAT3& position);
	void SendStopPacket(const DirectX::XMFLOAT3& position); // , const DirectX::XMFLOAT3& rotate
	void SendRotatePacket(ROTATE_AXIS axis, float angle);
	void SendKeyUpPacket(DIRECTION d);
	void SendLoginData(char* loginId, char* pw);
	void SendMouseStatePacket(bool LClickedButton, bool RClickedButton);
	void Send_SkillExecute_Q();
	void Send_SkillExecute_E();	
	void SendMatchRequestPacket();
	void SendArrowAttackPacket(const XMFLOAT3& pos, const XMFLOAT3& dir, float speed);
	void SendBallAttackPacket(const XMFLOAT3& pos, const XMFLOAT3& dir, float speed);
	void SendMeleeAttackPacket(const XMFLOAT3& dir);
	void SendTestGameEndPacket();
	void SendTestGameEndOKPacket();
public:
	//Stage1
	void SendOnPositionTriggerBox1();//트리거 박스 내부에 도착해서 상태 변화할때
	void SendSkipNPCCommunicate();
	void SendChangeStage_BOSS();
	void SendCommonAttack(const XMFLOAT3& attackDirection, int power);
private:
	void ConstructPacket(int ioByte);
private:
	void Destroy();
};