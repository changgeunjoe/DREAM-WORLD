#include <DirectXMath.h>

constexpr int NAME_SIZE = 20;

#pragma pack (push, 1)
namespace CLIENT_PACKET {
	constexpr unsigned char MOVE_KEY_DOWN = 1;
	constexpr unsigned char MOVE_KEY_UP = 2;
	constexpr unsigned char STOP = 3;
	constexpr unsigned char ROTATE = 4;
	constexpr unsigned char LOGIN = 5;
	constexpr unsigned char MATCH = 6;
	constexpr unsigned char CREATE_ROOM = 7;
	constexpr unsigned char REQUEST_ROOM_LIST = 8;
	constexpr unsigned char PLAYER_APPLY_ROOM = 9;
	constexpr unsigned char CANCEL_ROOM = 10;


	struct MovePacket
	{
		char size;
		char type;
		DIRECTION direction;
	};

	struct RotatePacket {
		char size;
		char type;
		ROTATE_AXIS axis;
		float angle;
	};

	struct StopPacket {
		char size;
		char type;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotate;
	};

	struct LoginPacket {
		char size;
		char type;
		char id[NAME_SIZE];
		char pw[NAME_SIZE];
	};

	struct MatchPacket {
		char size;
		char type;
		char Role;
	};

	struct CreateRoomPacket {
		char size;
		char type;
		char Role;
		char roomName[30];
	};

	struct RequestRoomListPacket {
		char size;
		char type;
	};

	struct PlayerApplyRoomPacket {
		char size;
		char type;
		char role;
		char roomId[40];
	};

}

namespace SERVER_PACKET {
	constexpr unsigned char MOVE_KEY_DOWN = 65;
	constexpr unsigned char MOVE_KEY_UP = 66;
	constexpr unsigned char STOP = 67;
	constexpr unsigned char ROTATE = 68;
	constexpr unsigned char LOGIN_OK = 69;
	constexpr unsigned char ADD_PLAYER = 70;
	constexpr unsigned char CREATE_ROOM_SUCCESS = 71;
	constexpr unsigned char CREATE_ROOM_FAILURE = 72;
	constexpr unsigned char REQUEST_ROOM_LIST = 73;
	constexpr unsigned char REQUEST_ROOM_LIST_END = 74;
	constexpr unsigned char REQUEST_ROOM_LIST_NONE = 75;


	struct MovePacket
	{
		char size;
		char type;
		int userId;
		DIRECTION direction;
	};

	struct RotatePacket {
		char size;
		char type;
		int userId;
		ROTATE_AXIS axis;
		float angle;
	};
	struct StopPacket {
		char size;
		char type;
		int userId;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotate;
	};

	struct LoginPacket {
		char size;
		char type;
		int  userID;
		wchar_t name[NAME_SIZE];
	};

	struct AddPlayerPacket {
		char size;
		char type;
		int userId;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotate;
		wchar_t name[NAME_SIZE];
	};

	struct NoneRoomInfoPacket {
		char size;
		char type;
	};

	struct RoomInfoPacket {
		char size;
		char type;
		char roomId[40];
		char roomName[30];
		char playerName[4][20];//방장은 0번 인덱스일까?
		char role[4];
	};

	struct CreateRoomResultPacket {
		char size;
		char type;
		char roomName[30];
	};
}

#pragma pack (pop)