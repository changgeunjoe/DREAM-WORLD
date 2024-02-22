#include "stdafx.h"
#include "Room.h"
#include "../Network/ExpOver/ExpOver.h"


Room::Room()
{
}

Room::~Room()
{
}

void ::Room::Execute(ExpOver* over, const DWORD& ioByte, const ULONG_PTR& key)
{
	switch (over->GetOpCode())
	{
		//Room - update
	case OP_ROOM_UPDATE:
	{

	}
	break;
	case OP_GAME_STATE_SEND:
	{

	}
	break;

	//Room - Boss
	case OP_FIND_PLAYER:
	{

	}
	break;
	case OP_BOSS_ATTACK_SELECT:
	{

	}
	break;
	case OP_BOSS_ATTACK_EXECUTE:
	{

	}
	break;

	//Room - player skill
	case OP_PLAYER_HEAL:
	{

	}
	break;
	case OP_SKY_ARROW_ATTACK:
	{

	}
	break;
	default:
		break;
	}
}

