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
	constexpr unsigned char REQUEST_ROOM_LIST = 8; // 방 리스트 요청
	constexpr unsigned char PLAYER_APPLY_ROOM = 9; // 방 신청
	constexpr unsigned char CANCEL_APPLY_ROOM = 10; // 신청 취소
	constexpr unsigned char MOUSE_INPUT = 11;
	constexpr unsigned char MATCH_REQUEST = 12;	
	constexpr unsigned char SHOOTING_ARROW = 13;
	constexpr unsigned char SHOOTING_BALL = 14;
	constexpr unsigned char MELEE_ATTACK = 15;
	constexpr unsigned char GAME_END_OK = 16;
	constexpr unsigned char TEST_GAME_END = 17; //임시로 클라에서 전송하여 게임 끝낼 수 있게



	struct MovePacket
	{
		short size;
		char type;
		DIRECTION direction;
		std::chrono::utc_clock::time_point t;
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
		std::chrono::utc_clock::time_point t;
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
		bool LClickedButton;
		bool RClickedButton;
	};

	struct ShootingObject {
		short size;
		char type;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 dir;
		float speed;
	};

	struct MeleeAttackPacket {
		short size;
		char type;
		DirectX::XMFLOAT3 dir;
	};

	struct GameEndPacket {
		short size;
		char type;
	};
}

namespace SERVER_PACKET {
	constexpr unsigned char MOVE_KEY_DOWN = 65;
	constexpr unsigned char MOVE_KEY_UP = 66;
	constexpr unsigned char STOP = 67;
	constexpr unsigned char ROTATE = 68;
	constexpr unsigned char LOGIN_OK = 69;
	constexpr unsigned char ADD_PLAYER = 70;
	constexpr unsigned char CREATE_ROOM_SUCCESS = 71; // 룸 생성 시 성공 패킷
	constexpr unsigned char CREATE_ROOM_FAILURE = 72; // 룸 생성 시 실패 패킷
	constexpr unsigned char REQUEST_ROOM_LIST = 73; // 방 리스트 출력중
	constexpr unsigned char REQUEST_ROOM_LIST_END = 74;// 방 리스트의 끝
	constexpr unsigned char REQUEST_ROOM_LIST_NONE = 75;//어떤 방도 없다
	constexpr unsigned char ACCEPT_ENTER_ROOM = 76; // 방 입장 확인
	constexpr unsigned char REJECT_ENTER_ROOM = 77; // 방 입장 거부
	constexpr unsigned char NOT_FOUND_ROOM = 78; // 신청한 방이 사라짐

	constexpr unsigned char PLAYER_APPLY_ROOM = 79; // 신청자 정보 방장(방)한테 전송
	constexpr unsigned char PLAYER_CANCEL_ROOM = 80; // 신청 취소 정보 방장(방)한테 전송
	constexpr unsigned char MOUSE_INPUT = 81;
	constexpr unsigned char INTO_GAME = 82;
	constexpr unsigned char BOSS_CHANGE_STATE_MOVE_DES = 83;
	constexpr unsigned char SHOOTING_ARROW = 84;
	constexpr unsigned char SHOOTING_BALL = 85;
	constexpr unsigned char GAME_STATE = 86;
	constexpr unsigned char BOSS_ATTACK = 87;
	constexpr unsigned char HIT_BOSS_MAGE = 88;
	constexpr unsigned char GAME_END = 89;
	constexpr unsigned char BOSS_MOVE_NODE = 90;
	constexpr unsigned char DUPLICATED_LOGIN = 91;//중복된 로그인이 들어왔다
	constexpr unsigned char PRE_EXIST_LOGIN = 92; //이미 로그인된 아이디다


	struct MovePacket
	{
		short size;
		char type;
		int userId;
		DIRECTION direction;
		std::chrono::utc_clock::time_point t;
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
		std::chrono::utc_clock::time_point t;
	};

	struct LoginPacket {
		short size;
		char type;
		int  userID;
		wchar_t name[NAME_SIZE];
	};

	struct AddPlayerPacket { // ROLE 도 필요하다고 생각함 - 추가하는게 날듯?
		short size;
		char type;
		int userId;
		char role;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotate;
		wchar_t name[NAME_SIZE];
	};

	struct NotifyPacket {
		short size;
		char type;
	};

	struct RoomInfoPacket { // 패킷 사이즈 오버됨
		short size;
		char type;
		char roomId[40];
		wchar_t roomName[30];
		wchar_t playerName[4][20];//방장은 0번 인덱스일까?
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
		bool LClickedButton;
		bool RClickedButton;
	};

	struct BossChangeStateMovePacket {//이걸 수정해야할듯?
		short size;
		char type;
		DirectX::XMFLOAT3 desPos;
		DirectX::XMFLOAT3 bossPos;
		std::chrono::utc_clock::time_point t;
	};

	struct ShootingObject {
		short size;
		char type;
		XMFLOAT3 dir;
		XMFLOAT3 srcPos;
		float speed;
	};

	struct InGamePlayerState {
		int userId = -1;
		int hp;
		XMFLOAT3 pos;
		XMFLOAT3 rot;
		
	};

	struct InGameBossState {
		int hp;
		XMFLOAT3 pos;
		XMFLOAT3 rot;
		XMFLOAT3 directionVector;
	};

	struct GameState {//Player State-> pos rot...추가하여 보정?
		short size;
		char type;
		InGamePlayerState userState[4];
		InGameBossState bossState;
		std::chrono::utc_clock::time_point time;
	};

	struct BossAttackPacket {
		short size;
		char type;
		char bossAttackType;
	};

	struct BossHitObject {
		short size;
		char type;
		XMFLOAT3 pos;
	};

	struct BossMoveNodePacket {
		short size;
		char type;
		DirectX::XMFLOAT3 bossPos;
		//DirectX::XMFLOAT3 desPos;
		int desPlayerId;
		int nodeCnt;
		int node[40];
	};

}

#pragma pack (pop)