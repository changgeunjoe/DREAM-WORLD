#pragma once
#include "../stdafx.h"
#include <thread>
#include <WS2tcpip.h>



#pragma comment(lib, "WS2_32.lib")

//#define SERVER_IP "183.101.110.217"
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000


class NetworkHelper {
private:
	SOCKET m_clientSocket;
private:
	char m_buffer[MAX_RECV_BUF_SIZE];
	int m_prevPacketSize = 0;
private:
	bool m_bIsRunnung = false;
	std::thread m_runThread;
private:
	std::chrono::utc_clock::time_point m_positionSendTime = std::chrono::utc_clock::now();
public:
	NetworkHelper();
	~NetworkHelper();

public:
	bool TryConnect();
	void Start();
	void RunThread();
public:
	void SendMovePacket(DIRECTION d);
	void SendStopPacket(const DirectX::XMFLOAT3& position); // , const DirectX::XMFLOAT3& rotate
	void SendRotatePacket(ROTATE_AXIS axis, float angle);
	void SendKeyUpPacket(DIRECTION d);
	void SendLoginData(char* loginId, char* pw);
	void SendMouseStatePacket(bool LClickedButton, bool RClickedButton);

	void Send_SkillInput_Q();
	void Send_SkillInput_E();
	void Send_SkillExecute_Q();//스킬 공격 서버에 활성화
	void Send_SkillExecute_Q(XMFLOAT3& floatData);//스킬 공격 서버에 활성화
	void Send_SkillExecute_E();//스킬 공격 서버에 활성화
	void Send_SkillExecute_E(const XMFLOAT3& floatData);//스킬 공격 서버에 활성화

	void SendMatchRequestPacket();
	void SendTestGameEndPacket();
	void SendTestGameEndOKPacket();
public:
	//Stage1
	void SendOnPositionTriggerBox1();//트리거 박스 내부에 도착해서 상태 변화할때
	void SendSkipNPCCommunicate();
	void SendChangeStage_BOSS();
	void SendCommonAttackExecute(const XMFLOAT3& attackDirection);//기본 공격 실제 공격 시행
	void SendPowerAttackExecute(const XMFLOAT3& attackDirection, int power);//파워 공격 실제 공격 시행
	void SendCommonAttackStart();//기본 공격 애니메이션
	void SendPlayerPosition(XMFLOAT3& position);
public:
	void SendTimeSyncPacket();

private:
	void ConstructPacket(const int& ioByte);
private:
	void Destroy();
};