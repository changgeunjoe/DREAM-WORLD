#pragma once
#include "../../PCH/stdafx.h"
#include "../ExpOver/ExpOver.h"
#include "../IocpEvent/IocpListenEvent.h"

//static std::random_device rd;
//static std::default_random_engine dre(rd());
//static std::uniform_int_distribution<> aggroRandomPlayer(0, 3);//inclusive
//static std::uniform_int_distribution<> bossRandAttack(0, (int)BOSS_ATTACK::ATTACK_COUNT - 1);

class Iocp
{
public:
	Iocp();
	~Iocp();

	void StartWorkerThread();

	HANDLE GetIocpHandle()
	{
		return m_hIocp;
	}
	void RegistHandle(HANDLE registHandle, int key);

private:
	void Destroy();

	void WorkerThread();
private:
	HANDLE m_hIocp;
private:
	std::vector<std::thread> m_workerThread;
	IocpListenEvent m_listener;
};
