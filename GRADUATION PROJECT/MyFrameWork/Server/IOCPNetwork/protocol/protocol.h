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
	constexpr unsigned char REQUEST_ROOM_LIST = 8; // �� ����Ʈ ��û
	constexpr unsigned char PLAYER_APPLY_ROOM = 9; // �� ��û
	constexpr unsigned char CANCEL_APPLY_ROOM = 10; // ��û ���
	constexpr unsigned char MOUSE_INPUT = 11;
	constexpr unsigned char MATCH_REQUEST = 12;


	struct MovePacket
	{
		short size;
		char type;
		DIRECTION direction;
	};

	struct RotatePacket {
		short size;
		char type;
		ROTATE_AXIS axis;
		float angle;
	};

	struct StopPacket {
		short size;
		char type;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotate;
	};

	struct LoginPacket {
		short size;
		char type;
		char id[NAME_SIZE];
		char pw[NAME_SIZE];
	};

	struct MatchPacket {
		short size;
		char type;
		char Role;
	};

	struct CreateRoomPacket {
		short size;
		char type;
		char Role;
		wchar_t roomName[30];
	};

	struct RequestRoomListPacket {
		short size;
		char type;
	};

	struct PlayerApplyRoomPacket {
		short size;
		char type;
		char role;
		char roomId[40];
	};

	struct PlayerCancelRoomPacket {
		short size;
		char type;
		char roomId[40];
	};
	
	struct MouseInputPacket {
		short size;
		char type;
		char ClickedButton;
	};
}

namespace SERVER_PACKET {
	constexpr unsigned char MOVE_KEY_DOWN = 65;
	constexpr unsigned char MOVE_KEY_UP = 66;
	constexpr unsigned char STOP = 67;
	constexpr unsigned char ROTATE = 68;
	constexpr unsigned char LOGIN_OK = 69;
	constexpr unsigned char ADD_PLAYER = 70;
	constexpr unsigned char CREATE_ROOM_SUCCESS = 71; // �� ���� �� ���� ��Ŷ
	constexpr unsigned char CREATE_ROOM_FAILURE = 72; // �� ���� �� ���� ��Ŷ
	constexpr unsigned char REQUEST_ROOM_LIST = 73; // �� ����Ʈ �����
	constexpr unsigned char REQUEST_ROOM_LIST_END = 74;// �� ����Ʈ�� ��
	constexpr unsigned char REQUEST_ROOM_LIST_NONE = 75;//� �浵 ����
	constexpr unsigned char ACCEPT_ENTER_ROOM = 76; // �� ���� Ȯ��
	constexpr unsigned char REJECT_ENTER_ROOM = 77; // �� ���� �ź�
	constexpr unsigned char NOT_FOUND_ROOM = 78; // ��û�� ���� �����

	constexpr unsigned char PLAYER_APPLY_ROOM = 79; // ��û�� ���� ����(��)���� ����
	constexpr unsigned char PLAYER_CANCEL_ROOM = 80; // ��û ��� ���� ����(��)���� ����
	constexpr unsigned char MOUSE_INPUT = 81;
	

	struct MovePacket
	{
		short size;
		char type;
		int userId;
		DIRECTION direction;
	};

	struct RotatePacket {
		short size;
		char type;
		int userId;
		ROTATE_AXIS axis;
		float angle;
	};
	struct StopPacket {
		short size;
		char type;
		int userId;
		DirectX::XMFLOAT3 position;
		//DirectX::XMFLOAT3 rotate;
	};

	struct LoginPacket {
		short size;
		char type;
		int  userID;
		wchar_t name[NAME_SIZE];
	};

	struct AddPlayerPacket {
		short size;
		char type;
		int userId;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotate;
		wchar_t name[NAME_SIZE];
	};

	struct NotifyPacket {
		short size;
		char type;
	};

	struct RoomInfoPacket { // ��Ŷ ������ ������
		short size;
		char type;
		char roomId[40];
		wchar_t roomName[30];
		wchar_t playerName[4][20];//������ 0�� �ε����ϱ�?
		char role[4];
	};

	struct CreateRoomResultPacket {
		short size;
		char type;
		wchar_t roomName[30];
	};

	struct PlayerApplyRoomPacket {
		short size;
		char type;
		wchar_t name[NAME_SIZE];
		char role;
	};

	struct MouseInputPacket {
		short size;
		char type;
		int userId;
		char ClickedButton;
	};

}

#pragma pack (pop)