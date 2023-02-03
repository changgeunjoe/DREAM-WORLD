#include <DirectXMath.h>
enum DIRECTION : char
{
	IDLE =	0x00,
	FRONT = 0x01,
	RIGHT = 0x02,
	LEFT =	0x04,
	BACK =	0x08
};

enum ROTATE_AXIS :char
{
	X, Y, Z
};

#pragma pack (push, 1)
namespace CLIENT_PACKET {
	constexpr unsigned char MOVE_KEY_DOWN = 1;
	constexpr unsigned char MOVE_KEY_UP = 2;
	constexpr unsigned char STOP = 3;
	constexpr unsigned char ROTATE = 4;

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
}
namespace SERVER_PACKET {
	constexpr unsigned char MOVE_KEY_DOWN = 65;
	constexpr unsigned char MOVE_KEY_UP = 66;
	constexpr unsigned char STOP = 67;
	constexpr unsigned char ROTATE = 68;

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
}

#pragma pack (pop)