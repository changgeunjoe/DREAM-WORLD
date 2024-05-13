#pragma once
#include "../../PCH/stdafx.h"
#include "../../SingletonBase.h"

namespace Network
{
	class NetworkModule
	{
		static const int MAX_THREAD_NUM = 4;
	public:
		virtual void InitializeNetwork();
		/*HANDLE GetIocpHandle()
		{
			return m_iocpHandle;
		}*/
		void RegistHandle(HANDLE& registHandle);
	private:
		void WorkerThread();
	private:
		std::vector<std::thread> m_workerThread;
		HANDLE m_iocpHandle;
	};
}

namespace DreamWorld
{
	static const std::string_view SERVER_IP = "127.0.0.1";
	static const short SERVER_PORT = 9000;

	static constexpr int MAX_TEST = 2500;
	static constexpr int MAX_CLIENTS = MAX_TEST * 2;

	using Time = std::chrono::high_resolution_clock;
	using SEC = std::chrono::seconds;
	using MS = std::chrono::milliseconds;

	class StressTestNetwork : public Network::NetworkModule, public SingletonBase<StressTestNetwork>
	{
		friend SingletonBase;
	public:
		virtual void InitializeNetwork() override;
		void IncreaseActiveClient();
		void DisconnectClient();
		int GetActiveNum()
		{
			return m_activeClientNum;
		}
	private:
		void ConnectThread();

		//클라이언트 연결 관리
		void AdjustClientNumber();
	private:
		std::thread m_connectThread;

		//강제로 연결 끊을 때 사용하는 idx번호
		std::atomic_int m_disconnectClientNum;

		//마지막으로 connect와 관련된 일을 한 시간
		Time::time_point m_lastConnectTime;

		//connect까지만 성공한 클라이언트 수
		std::atomic_int m_connections;

		//현재 로그인까지 성공한 클라이언트 수
		std::atomic_int m_activeClientNum;
	public:
		//연결된 클라이언트의 딜레이 총합
		std::atomic_ullong globalDelay;
		std::atomic<double> dGlobalDelay;
		std::atomic_int globalMaxDelay;
	};

}

