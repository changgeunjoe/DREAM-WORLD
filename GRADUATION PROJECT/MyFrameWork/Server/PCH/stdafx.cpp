#include "stdafx.h"

#pragma warning (disable:4996)

void PrintCurrentTime()
{
	auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::cout << std::ctime(&currentTime) << ": ";
}

void DisplayWsaGetLastError(int Errcode)
{
	if (Errcode == 997)return;
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, Errcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf, 0, NULL);
	std::wcout << "ErrorCode: " << Errcode << " - " << (WCHAR*)lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
}
