#include <DirectXMath.h>
enum DIRECTION : char
{
	IDLE,
	FRONT,
	RIGHT,
	LEFT,
	BACK,
	FRONT_L,
	FRONT_R,
	BACK_L,
	BACK_R
};

enum ROTATE_AXIS :char
{
	X, Y, Z
};

#pragma pack (push, 1)
namespace CLIENT_PACKET {
	constexpr unsigned char MOVE_DOWN = 1;
	constexpr unsigned char MOVE_UP = 2;
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
	constexpr unsigned char MOVE = 65;	
	constexpr unsigned char STOP = 66;
	constexpr unsigned char ROTATE = 67;

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