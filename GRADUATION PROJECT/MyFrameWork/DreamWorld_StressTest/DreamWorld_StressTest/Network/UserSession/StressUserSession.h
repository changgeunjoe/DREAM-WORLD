#pragma once
#include "UserSession.h"

class EventController;
class UserState;
enum class USER_STATE;
namespace DreamWorld
{
	class StressUserSession : public Network::UserSession
	{
		using TIME = std::chrono::high_resolution_clock;
		using SEC = std::chrono::seconds;
		using MSEC = std::chrono::milliseconds;

		static constexpr std::string_view MOVE = "MOVE";
		static constexpr std::string_view DELAY_CHECK = "DELAY_CHECK";

		static constexpr MSEC MOVE_COOL_TIME = MSEC(5'000);
		static constexpr MSEC DELAY_CHECK_COOL_TIME = MSEC(3'000);


	public:
		StressUserSession();
		void Initialize();
		const ROLE& GetRole()const
		{
			return m_currentRole;
		}

		const bool& IsActive() const
		{
			return m_isActive;
		}

		float GetX() const
		{
			return m_x;
		}
		
		float GetZ() const
		{
			return m_z;
		}

		void SendPacketByState();

		void SendLoginPacket();

		void SendMatchPacket();

		void SendIngamePacket();
		void SendMovePacket(const DIRECTION& direction, const bool& isApply);
		void SendStopPacket();
		void SendRotatePacket();

		void SendStageChangeToBoss();
		void SendForceGameEnd();
		void SendGameEndOkayPacket();

		void SendDelayCheckPacket();

		void ChangeUserState(const USER_STATE& state);

		virtual void Disconnect() override;
	protected:
		virtual void ExecutePacket(const PacketHeader* executePacketHeader) override;
		virtual void Connect(SOCKET connectSocket);
	private:
		static DIRECTION GetRandomDirection();
		static ROLE GetRandomRole();
	private:
		std::atomic_bool m_isActive;
		ROLE m_currentRole;
		std::shared_ptr<UserState> m_currentUserState;
		std::unordered_map<USER_STATE, std::shared_ptr<UserState>> m_userStates;

		bool m_isAbleCheckDelay;
		long long m_delayTime;
		double m_dDelayTime;
		TIME::time_point m_lastDelaySendTime;
		TIME::time_point m_loginSendTime;

		float m_x;
		float m_z;
		std::shared_ptr<EventController> m_coolTimeCtrl;
	};
}

