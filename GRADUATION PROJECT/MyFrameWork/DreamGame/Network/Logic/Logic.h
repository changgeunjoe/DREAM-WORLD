#pragma once
#include "../Session/Session.h"

namespace clientNet {
	class Logic
	{
	private:
		std::array<Session, 4> m_inGamePlayerSession;
		int myId = 0;
	public:
		Logic() {}
		~Logic() {}
	public:
		void ProcessPacket(char* p);
	};
}
