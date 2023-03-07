#include "stdafx.h"

#pragma warning (disable:4996)

void PrintCurrentTime()
{
	auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::cout << std::ctime(&currentTime);
}
