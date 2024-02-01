#pragma once
#include "../../PCH/stdafx.h"

namespace PacketManager
{
	int ProccessPacket(const int& useId, const int& roomId, const unsigned long& ioByte, int remainSize, char* buffer);
};

