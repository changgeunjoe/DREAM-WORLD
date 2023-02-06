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

namespace clientNet {
	class Logic
	{
	public:
		std::array<clientNet::Session, 4> m_inGamePlayerSession;
		int myId = 0;
	public:
		CKeyInput* m_KeyInput;
	public:
		Logic();
		~Logic() {}
	public:
		void ProcessPacket(char* p);
	};
}
