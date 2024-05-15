#pragma once
#include "../../PCH/stdafx.h"
#include "../ExpOver/ExpOver.h"

//static std::random_device rd;
//static std::default_random_engine dre(rd());
//static std::uniform_int_distribution<> aggroRandomPlayer(0, 3);//inclusive
//static std::uniform_int_distribution<> bossRandAttack(0, (int)BOSS_ATTACK::ATTACK_COUNT - 1);

namespace IOCP
{
	class ListenEvent;
	class Iocp : public std::enable_shared_from_this<Iocp>
	{
	public:
		Iocp();
		~Iocp();

		void Start();

		HANDLE GetIocpHandle()
		{
			return m_hIocp;
		}
		void RegistHandle(HANDLE registHandle, int key);

		std::shared_ptr<Iocp> GetSharedPtr()
		{
			return shared_from_this();
		}

	private:
		void WorkerThread();
	private:
		HANDLE m_hIocp;
	private:
		std::shared_ptr<IOCP::ListenEvent> m_listener;
	};
}
