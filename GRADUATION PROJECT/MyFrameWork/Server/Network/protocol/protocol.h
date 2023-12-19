constexpr int NAME_SIZE = 20;

#pragma pack (push, 1)
namespace CLIENT_PACKET {
	constexpr unsigned char MOVE_KEY_DOWN = 1;
	constexpr unsigned char MOVE_KEY_UP = 2;
	constexpr unsigned char STOP = 3;
	constexpr unsigned char ROTATE = 4;
	constexpr unsigned char LOGIN = 5;
	constexpr unsigned char MATCH = 6;
	//constexpr unsigned char CREATE_ROOM = 7;
	//constexpr unsigned char REQUEST_ROOM_LIST = 8; // �� ����Ʈ ��û
	//constexpr unsigned char PLAYER_APPLY_ROOM = 9; // �� ��û
	//constexpr unsigned char CANCEL_APPLY_ROOM = 10; // ��û ���

	constexpr unsigned char SKILL_EXECUTE_Q = 7;
	constexpr unsigned char SKILL_EXECUTE_E = 8;
	constexpr unsigned char SKILL_INPUT_Q = 9;
	constexpr unsigned char SKILL_INPUT_E = 10;

	constexpr unsigned char MOUSE_INPUT = 11;
	constexpr unsigned char MATCH_REQUEST = 12;
	constexpr unsigned char SHOOTING_ARROW = 13;
	constexpr unsigned char SHOOTING_BALL = 14;
	constexpr unsigned char MELEE_ATTACK = 15;
	constexpr unsigned char GAME_END_OK = 16;
	constexpr unsigned char TEST_GAME_END = 17; //�ӽ÷� Ŭ�󿡼� �����Ͽ� ���� ���� �� �ְ�

	constexpr unsigned char TRIGGER_BOX_ON = 19;
	constexpr unsigned char TRIGGER_BOX_OUT = 20;

	constexpr unsigned char SKIP_NPC_COMMUNICATION = 23;
	constexpr unsigned char STAGE_CHANGE_BOSS = 24;

	constexpr unsigned char PLAYER_COMMON_ATTACK_EXECUTE = 25;

	constexpr unsigned char ARCHER_SKILL_ARROW = 26;

	constexpr unsigned char TIME_SYNC_REQUEST = 27;

	constexpr unsigned char PLAYER_COMMON_ATTACK = 29;//�ִ� ����
	constexpr unsigned char PLAYER_POSITION_STATE = 30;

	struct MovePacket
	{//�ð��� ���� ����, �̵� �Է�
		short size;
		char type;
		DIRECTION direction;
		std::chrono::utc_clock::time_point time;
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
		std::chrono::utc_clock::time_point time;
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

	/*struct CreateRoomPacket {
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
	};*/

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

	struct NotifyPacket {
		short size;
		char type;
	};

	struct PlayerCommonAttackPacket {
		short size;
		char type;
		DirectX::XMFLOAT3 dir;
		int power;// ��ó ����, ������ 3�ܰ�
	};

	struct SkillAttackPacket {
		short size;
		char type;
		XMFLOAT3 postionOrDirection;
	};

	struct PlayerPositionPacket {
		short size;
		char type;
		XMFLOAT3 position;
		std::chrono::utc_clock::time_point t;
	};

}

namespace SERVER_PACKET {
	constexpr unsigned char MOVE_KEY_DOWN = 65;
	constexpr unsigned char MOVE_KEY_UP = 66;
	constexpr unsigned char STOP = 67;
	constexpr unsigned char ROTATE = 68;
	constexpr unsigned char LOGIN_OK = 69;
	constexpr unsigned char ADD_PLAYER = 70;
	//constexpr unsigned char CREATE_ROOM_SUCCESS = 71; // �� ���� �� ���� ��Ŷ
	//constexpr unsigned char CREATE_ROOM_FAILURE = 72; // �� ���� �� ���� ��Ŷ
	//constexpr unsigned char REQUEST_ROOM_LIST = 73; // �� ����Ʈ �����
	//constexpr unsigned char REQUEST_ROOM_LIST_END = 74;// �� ����Ʈ�� ��
	//constexpr unsigned char REQUEST_ROOM_LIST_NONE = 75;//� �浵 ����
	//constexpr unsigned char ACCEPT_ENTER_ROOM = 76; // �� ���� Ȯ��
	//constexpr unsigned char REJECT_ENTER_ROOM = 77; // �� ���� �ź�
	//constexpr unsigned char NOT_FOUND_ROOM = 78; // ��û�� ���� �����
	//
	//constexpr unsigned char PLAYER_APPLY_ROOM = 79; // ��û�� ���� ����(��)���� ����
	//constexpr unsigned char PLAYER_CANCEL_ROOM = 80; // ��û ��� ���� ����(��)���� ����	

	constexpr unsigned char MOUSE_INPUT = 81;
	constexpr unsigned char INTO_GAME = 82;
	constexpr unsigned char BOSS_CHANGE_STATE_MOVE_DES = 83;
	constexpr unsigned char SHOOTING_ARROW = 84;
	constexpr unsigned char SHOOTING_BALL = 85;
	constexpr unsigned char EXECUTE_LIGHTNING = 80;
	constexpr unsigned char BOSS_ATTACK_EXECUTE = 87;
	constexpr unsigned char GAME_STATE_B = 86;
	constexpr unsigned char HIT_BOSS_MAGE = 88;
	constexpr unsigned char GAME_END = 89;
	constexpr unsigned char BOSS_MOVE_NODE = 90;
	constexpr unsigned char DUPLICATED_LOGIN = 91;//�ߺ��� �α����� ���Դ�
	constexpr unsigned char PRE_EXIST_LOGIN = 92; //�̹� �α��ε� ���̵��
	constexpr unsigned char GAME_STATE_S = 93;
	constexpr unsigned char STAGE_CHANGING_BOSS = 96;
	constexpr unsigned char STAGE_START_BOSS = 97;
	constexpr unsigned char SMALL_MONSTER_MOVE = 98;

	constexpr unsigned char HEAL_START = 99;
	constexpr unsigned char HEAL_END = 100;

	constexpr unsigned char SHIELD_START = 101;
	constexpr unsigned char SHIELD_END = 102;

	constexpr unsigned char NOTIFY_HEAL_HP = 103;
	constexpr unsigned char NOTIFY_SHIELD_APPLY = 104;

	constexpr unsigned char MONSTER_DAMAGED_ARROW = 105;
	constexpr unsigned char MONSTER_DAMAGED_ARROW_SKILL = 106;
	constexpr unsigned char MONSTER_DAMAGED_BALL = 107;

	constexpr unsigned char PLAYER_ATTACK_RESULT = 108;
	constexpr unsigned char PLAYER_ATTACK_RESULT_BOSS = 109;

	constexpr unsigned char SMALL_MONSTER_ATTACK = 110;

	constexpr unsigned char TIME_SYNC_RESPONSE = 111;

	constexpr unsigned char START_ANIMATION_Q = 114;
	constexpr unsigned char START_ANIMATION_E = 115;
	constexpr unsigned char COMMON_ATTACK_START = 116;

	constexpr unsigned char BOSS_ATTACK_PALYER = 117;
	constexpr unsigned char BOSS_CHANGE_DIRECION = 118;
	constexpr unsigned char METEO_PLAYER_ATTACK = 119;
	constexpr unsigned char METEO_DESTROY = 120;
	constexpr unsigned char METEO_CREATE = 121;



	struct MovePacket
	{
		short size;
		char type;
		ROLE role;
		DIRECTION direction;
		XMFLOAT3 position;
		XMFLOAT3 moveVec;
		MovePacket(const char& packetType, const ROLE& r, const DIRECTION& dir, const XMFLOAT3& pos, const XMFLOAT3& moveVector)
			: size(sizeof(MovePacket)), type(packetType), role(r), direction(dir), position(pos), moveVec(moveVector)
		{}
	};

	struct RotatePacket {
		short size;
		char type;
		ROLE role;
		ROTATE_AXIS axis;
		float angle;
		RotatePacket(const char& packetType, const ROLE& r, const ROTATE_AXIS& rotAxis, const float& rotAngle)
			: size(sizeof(RotatePacket)), type(packetType), role(r), axis(rotAxis), angle(rotAngle)
		{}
	};

	struct StopPacket {
		short size;
		char type;
		ROLE role;
		DirectX::XMFLOAT3 position;
		std::chrono::utc_clock::time_point t;
		StopPacket(const char& packetType, const ROLE& r, const XMFLOAT3& pos)
			: size(sizeof(StopPacket)), type(packetType), role(r), position(pos)
		{}
	};

	struct LoginPacket {
		short size;
		char type;
		int  userID;
		LoginPacket(const char& packetType, int id)
			: size(sizeof(LoginPacket)), type(packetType), userID(id)
		{}
	};

	struct AddPlayerPacket { // ROLE �� �ʿ��ϴٰ� ������ - �߰��ϴ°� ����?
		short size;
		char type;
		int userId;//�� ���� �ƴ����� �Ǵ��ؾ��ϴ� - �� �̻� �ʿ� ���� �����ϰŷ� �Ǵܵ�.
		ROLE role;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotate;
		AddPlayerPacket(const char& packetType, const ROLE& r, int id, const XMFLOAT3& pos, const XMFLOAT3& rot)
			: size(sizeof(AddPlayerPacket)), type(packetType), role(r), userId(id), position(pos), rotate(rot)
		{}
	};

	struct NotifyPacket {
		short size;
		char type;
		NotifyPacket(const char& packetType) : size(sizeof(NotifyPacket)), type(packetType)
		{}
	};

	struct MouseInputPacket {
		short size;
		char type;
		ROLE role;
		bool LClickedButton;
		bool RClickedButton;
		MouseInputPacket(const char& packetType, const ROLE& r, const bool& left, const bool& right)
			: size(sizeof(MouseInputPacket)), type(packetType), role(r), LClickedButton(left), RClickedButton(right)
		{}
	};

	struct BossChangeStateMovePacket {//�̰� �����ؾ��ҵ�?
		short size;
		char type;
		DirectX::XMFLOAT3 desPos;
		DirectX::XMFLOAT3 bossPos;
		std::chrono::utc_clock::time_point t;
		BossChangeStateMovePacket(const char& packetType, const XMFLOAT3& des, const XMFLOAT3& bossPosition, std::chrono::utc_clock::time_point& paramT)
			: size(sizeof(BossChangeStateMovePacket)), type(packetType), desPos(des), bossPos(bossPosition), t(paramT)
		{}
	};

	struct ShootingObject {
		short size;
		char type;
		XMFLOAT3 dir;
		ShootingObject(const char& packetType, const ROLE& r, const XMFLOAT3& d)
			: size(sizeof(ShootingObject)), type(packetType), dir(d)
		{}
	};

	struct InGamePlayerState {
		ROLE role;
		int hp;
		float shield;
		XMFLOAT3 pos;
		XMFLOAT3 rot;
		XMFLOAT3 moveVec;
	};

	struct ApplyHealForPlayer {
		char role;
		float hp;
	};

	struct ApplyShieldForPlayer {
		char role;
		float shield;
	};

	struct InGameSmallMonster {
		int hp;
		XMFLOAT3 pos;
		XMFLOAT3 rot;
		XMFLOAT3 moveVec;
		char idxSize;
		bool isAlive;
	};

	struct InGameBossState {
		int hp;
		XMFLOAT3 pos;
		XMFLOAT3 rot;
		XMFLOAT3 moveVec;
		XMFLOAT3 desVec;
	};

	struct MeteoInfo {
		XMFLOAT3 pos;
		float speed;
	};

	struct GameState_BOSS {//Player State-> pos rot...�߰��Ͽ� ����?
		short size;
		char type;
		InGamePlayerState userState[4];
		InGameBossState bossState;
		std::chrono::utc_clock::time_point time;
		GameState_BOSS(const char& packetType) :size(sizeof(GameState_BOSS)), type(packetType) {}
	};

	struct GameState_STAGE1 {//Player State-> pos rot...�߰��Ͽ� ����?
		short size;
		char type;
		InGamePlayerState userState[4];
		InGameSmallMonster smallMonster[15];
		int aliveMonsterCnt;//��� �ִ� ���� ���� �Դϴ�. �� 15��
		std::chrono::utc_clock::time_point time;
		GameState_STAGE1(const char& packetType) :size(sizeof(GameState_STAGE1)), type(packetType) {}
	};

	struct BossAttackPacket {
		short size;
		char type;
		BOSS_ATTACK bossAttackType;
		BossAttackPacket(const char& packetType, const BOSS_ATTACK& attackType)
			:size(sizeof(BossAttackPacket)), type(packetType), bossAttackType(attackType)
		{}
	};

	struct BossHitObject {
		short size;
		char type;
		XMFLOAT3 pos;
		BossHitObject(const char& packetType, const XMFLOAT3& position)
			:size(sizeof(BossHitObject)), type(packetType), pos(position)
		{}
	};

	struct BossMoveNodePacket {
		short size;
		char type;
		DirectX::XMFLOAT3 bossPos;
		//DirectX::XMFLOAT3 desPos;
		ROLE targetRole;
		int nodeCnt;
		int node[40];
		BossMoveNodePacket(const char& packetType, const XMFLOAT3& position, const ROLE& targetR)
			:size(sizeof(BossMoveNodePacket)), type(packetType), bossPos(position), targetRole(targetR)
		{}		
	};

	struct SmallMonsterMovePacket {
		short size;
		char type;
		XMFLOAT3 desPositions[15];
		SmallMonsterMovePacket(const char& packetType)
			:size(sizeof(SmallMonsterMovePacket)), type(packetType)
		{}
	};

	struct GameState_BOSS_INIT {//Player State-> pos rot...�߰��Ͽ� ����?
		short size;
		char type;
		XMFLOAT3 bossPosition;
		InGamePlayerState userState[4];
		GameState_BOSS_INIT(const char& packetType, const XMFLOAT3& bossPos)
			:size(sizeof(GameState_BOSS_INIT)), type(packetType), bossPosition(bossPos)
		{}
	};

	struct NotifyHealPacket {
		short size;
		char type;
		ApplyHealForPlayer applyHealPlayerInfo[4];
		NotifyHealPacket(const char& packetType)
			:size(sizeof(NotifyHealPacket)), type(packetType)
		{}
	};

	struct NotifyShieldPacket {
		short size;
		char type;
		ApplyShieldForPlayer applyShieldPlayerInfo[4];
		NotifyShieldPacket(const char& packetType)
			:size(sizeof(NotifyShieldPacket)), type(packetType)
		{}
	};

	struct ProjectileDamagePacket {//��ε� ĳ��Ʈ
		short size;
		char type;
		char projectileId;//�ε��� ��ȣ
		XMFLOAT3 position;//���� ��ġ
		float damage;// ������
		ProjectileDamagePacket(const char& packetType, const char& projectileIdx, const XMFLOAT3& pos, const float& dam)
			:size(sizeof(ProjectileDamagePacket)), type(packetType), projectileId(projectileIdx), position(pos), damage(dam)
		{}
	};

	struct PlayerAttackMonsterDamagePacket {//���θ� ������ �� �� �ְ�
		short size;
		char type;
		char role;
		char attackedMonsterCnt;//���� ���� ���� ����
		char monsterIdx[15];//�ش� ������ �ε������� �ִ� �迭
		float damage;// �÷��̾ ���� ������

	};

	struct PlayerAttackBossDamagePacket {
		short size;
		char type;
		char role;
		float damage;// �÷��̾ ���� ������
	};

	struct CommonAttackPacket {//��ü �÷��̾�鿡�� �˷��� �ִϸ��̼� ���
		short size;
		char type;
		char role;
	};

	struct SmallMonsterAttackPlayerPacket {
		short size;
		char type;
		char attackedRole;//�ڱ� �ڽ��� role���� �Ǵ��ϰ� �ǰ� ȭ�� ���
		char attackMonsterIdx;//�ִϸ��̼� ����� ���� �ε���
	};

	struct BossAttackPlayerPacket {
		short size;
		char type;
		float currentHp;
	};

	struct BossDirectionPacket {
		short size;
		char type;
		XMFLOAT3 directionVec;
	};

	struct DestroyedMeteoPacket {
		short size;
		char type;
		char idx;
	};

	struct MeteoStartPacket {
		short size;
		char type;
		MeteoInfo meteoInfo[10];
	};

	struct TimeSyncPacket {
		short size;
		char type;
		std::chrono::utc_clock::time_point t;
	};
}

#pragma pack (pop)