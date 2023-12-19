#pragma once
#include "../PCH/stdafx.h"

using namespace std;
using namespace chrono;


namespace Logic
{
	void AcceptPlayer(int userId, SOCKET& sock);

	//room	
	void CharacterAddDirection(int userId, const DIRECTION& applyDirection);
	void CharacterRemoveDirection(int userId, const DIRECTION& applyDirection);
	void CharacterStop(int userId);
	void CharacterRotate(int userId, const ROTATE_AXIS& axis, const float& angle);//degree
	void CharacterInput(int userId, bool leftClick, bool rightClick);
	void CharacterInputSkiil_Q(int userId, const XMFLOAT3& float3);
	void CharacterInputSkiil_E(int userId, const XMFLOAT3& float3);
	void CharacterSkillExecute_Q(int userId);
	void CharacterSkillExecute_E(int userId);
	void CharacterCommonAttack(int userId);
	void CharacterCommonAttackExecute(int userId, const XMFLOAT3& direction, int power);

	void ExitPlayerInRoom(int userId);
	void SkipNPC_Communication(int userId);

	void ForceGameEnd(int userId);
	void ForceChangeToBossState(int userId);

	//Time sync
	void TimeSyncRequest(int userId);
};

