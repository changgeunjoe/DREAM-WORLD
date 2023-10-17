#pragma once
#include "../Session/Session.h"

class CKeyInput {
public:
	CKeyInput() {};
	~CKeyInput() {};
	bool m_bWKey = false;
	bool m_bAKey = false;
	bool m_bSKey = false;
	bool m_bDKey = false;
	bool m_bEKey = false;
	bool m_bQKey = false;
	bool m_bLeftMouse_Button = false;
public:
	bool IsAllMovekeyUp() {
		return!(m_bWKey || m_bAKey || m_bSKey || m_bDKey);
	}
};

class Logic
{
public:
	//std::array<Session, 4> m_inGamePlayerSession;
	//Session m_MonsterSession;
	//Session m_SmallMonsterSession[15];
private:
	long long C2S_DiffTime = 0;//C2S_DiffTime -> micro second
	double m_RTT = 0.0f;//milliseconds
private:
	int myId = -1;
	ROLE myRole = ROLE::NONE_SELECT;
public:
	CKeyInput* m_KeyInput;
	char		m_MouseInput;
public:
	std::queue<PlayerCharacterOperation> m_inputOperation;
public:
	Logic();
	~Logic();
public:
	void ProcessPacket(char* p);
	XMFLOAT3 GetPostion(ROLE r);
	friend class CGameFramework;
private:
	chrono::high_resolution_clock::time_point attckPacketRecvTime;

public:
	ROLE GetMyRole() {
		return myRole;
	}
	void ResetMyRole() { myRole = ROLE::NONE_SELECT; }
	void SetMyRole(ROLE r) { myRole = r; }
	const long long GetDiffTime() {
		return C2S_DiffTime
			;
	}
	HWND loginWnd;
private:
	std::chrono::utc_clock::time_point m_requestTime;
	std::chrono::utc_clock::time_point m_responseTime;
public:
	void SetrequestTime();
	std::chrono::utc_clock::time_point& GetResponseTime() {
		return m_responseTime;
	}
	std::chrono::utc_clock::time_point& GetReQuestTime() {
		return m_requestTime;
	}
	double GetRTT() { return  m_RTT; }
};
