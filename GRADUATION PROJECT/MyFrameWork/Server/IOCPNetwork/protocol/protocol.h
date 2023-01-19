
enum DIRECTION : char
{
	IDLE,
	FRONT,
	RIGHT,
	LEFT,
	BACK
};

enum ROTATE_AXIS :char
{
	X, Y, Z
};

#pragma pack (push, 1)
namespace CLIENT_PACKET {
	constexpr unsigned char MOVE = 1;
	constexpr unsigned char STOP = 2;
	constexpr unsigned char ROTATE = 3;

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
	};
}

#pragma pack (pop)