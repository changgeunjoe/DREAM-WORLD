constexpr int NAME_SIZE = 20;

#pragma pack (push, 1)
struct PacketHeader
{
	short size;
	char type;
};

namespace CLIENT_PACKET {
#pragma region Move
	constexpr unsigned char MOVE_KEY_DOWN = 1;
	constexpr unsigned char MOVE_KEY_UP = 2;
	constexpr unsigned char STOP = 3;
	constexpr unsigned char ROTATE = 4;
#pragma endregion
	constexpr unsigned char LOGIN = 5;
	constexpr unsigned char MATCH = 6;
	//constexpr unsigned char CREATE_ROOM = 7;
	//constexpr unsigned char REQUEST_ROOM_LIST = 8; // �� ����Ʈ ��û
	//constexpr unsigned char PLAYER_APPLY_ROOM = 9; // �� ��û
	//constexpr unsigned char CANCEL_APPLY_ROOM = 10; // ��û ���

#pragma region Skill
	constexpr unsigned char SKILL_EXECUTE_Q = 7;
	constexpr unsigned char SKILL_EXECUTE_E = 8;
	constexpr unsigned char SKILL_INPUT_Q = 9;
	constexpr unsigned char SKILL_INPUT_E = 10;
#pragma endregion

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

	using NotifyPacket = PacketHeader;
	struct MovePacket : public PacketHeader
	{//�ð��� ���� ����, �̵� �Է�
		DIRECTION direction;
		std::chrono::utc_clock::time_point time;
	};

	struct RotatePacket : public PacketHeader
	{
		ROTATE_AXIS axis;
		float angle;
	};

	struct StopPacket : public PacketHeader
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotate;
		std::chrono::utc_clock::time_point time;
	};

	struct LoginPacket : public PacketHeader
	{
		char id[NAME_SIZE];
		char pw[NAME_SIZE];
	};

	struct MatchPacket : public PacketHeader
	{
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

	struct MouseInputPacket : public PacketHeader
	{
		bool LClickedButton;
		bool RClickedButton;
	};

	struct ShootingObject : public PacketHeader
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 dir;
		float speed;
	};

	struct PlayerCommonAttackPacket : public PacketHeader
	{
		DirectX::XMFLOAT3 dir;
		int power;// ��ó ����, ������ 3�ܰ�
	};

	struct SkillAttackPacket : public PacketHeader
	{
		XMFLOAT3 postionOrDirection;
	};

	struct PlayerPositionPacket : public PacketHeader
	{

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

	using NotifyPacket = PacketHeader;

	struct MovePacket : public PacketHeader
	{
		ROLE role;
		DIRECTION direction;
		XMFLOAT3 position;
		XMFLOAT3 moveVec;
	};

	struct RotatePacket : public PacketHeader
	{
		ROLE role;
		ROTATE_AXIS axis;
		float angle;
	};

	struct StopPacket : public PacketHeader
	{
		ROLE role;
		DirectX::XMFLOAT3 position;
		std::chrono::utc_clock::time_point t;
	};

	struct LoginPacket : public PacketHeader
	{
		int  userID;

	};

	struct AddPlayerPacket : public PacketHeader
	{ // ROLE �� �ʿ��ϴٰ� ������ - �߰��ϴ°� ����?
		int userId;//�� ���� �ƴ����� �Ǵ��ؾ��ϴ� - �� �̻� �ʿ� ���� �����ϰŷ� �Ǵܵ�.
		ROLE role;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotate;
	};

	struct MouseInputPacket : public PacketHeader
	{
		ROLE role;
		bool LClickedButton;
		bool RClickedButton;
	};

	struct BossChangeStateMovePacket : public PacketHeader
	{//�̰� �����ؾ��ҵ�?

		DirectX::XMFLOAT3 desPos;
		DirectX::XMFLOAT3 bossPos;
		std::chrono::utc_clock::time_point t;
	};

	struct ShootingObject : public PacketHeader
	{
		XMFLOAT3 dir;
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

	struct GameState_BOSS : public PacketHeader
	{//Player State-> pos rot...�߰��Ͽ� ����?		
		InGamePlayerState userState[4];
		InGameBossState bossState;
		std::chrono::utc_clock::time_point time;
	};

	struct GameState_STAGE1 : public PacketHeader
	{//Player State-> pos rot...�߰��Ͽ� ����?		
		InGamePlayerState userState[4];
		InGameSmallMonster smallMonster[15];
		int aliveMonsterCnt;//��� �ִ� ���� ���� �Դϴ�. �� 15��
		std::chrono::utc_clock::time_point time;
	};

	struct BossAttackPacket : public PacketHeader
	{
		BOSS_ATTACK bossAttackType;
	};

	struct BossHitObject : public PacketHeader
	{
		XMFLOAT3 pos;
	};

	struct BossMoveNodePacket : public PacketHeader
	{
		DirectX::XMFLOAT3 bossPos;
		//DirectX::XMFLOAT3 desPos;
		ROLE targetRole;
		int nodeCnt;
		int node[40];
	};

	struct SmallMonsterMovePacket : public PacketHeader
	{
		XMFLOAT3 desPositions[15];
	};

	struct GameState_BOSS_INIT : public PacketHeader
	{//Player State-> pos rot...�߰��Ͽ� ����?		
		XMFLOAT3 bossPosition;
		InGamePlayerState userState[4];
	};

	struct NotifyHealPacket : public PacketHeader
	{
		ApplyHealForPlayer applyHealPlayerInfo[4];
	};

	struct NotifyShieldPacket : public PacketHeader
	{
		ApplyShieldForPlayer applyShieldPlayerInfo[4];
	};

	struct ProjectileDamagePacket : public PacketHeader
	{//��ε� ĳ��Ʈ	
		char projectileId;//�ε��� ��ȣ
		XMFLOAT3 position;//���� ��ġ
		float damage;// ������		
	};

	struct PlayerAttackMonsterDamagePacket : public PacketHeader
	{//���θ� ������ �� �� �ְ�	
		char role;
		char attackedMonsterCnt;//���� ���� ���� ����
		char monsterIdx[15];//�ش� ������ �ε������� �ִ� �迭
		float damage;// �÷��̾ ���� ������

	};

	struct PlayerAttackBossDamagePacket : public PacketHeader {
		char role;
		float damage;// �÷��̾ ���� ������
	};

	struct CommonAttackPacket : public PacketHeader
	{//��ü �÷��̾�鿡�� �˷��� �ִϸ��̼� ���		
		char role;
	};

	struct SmallMonsterAttackPlayerPacket : public PacketHeader
	{
		char attackedRole;//�ڱ� �ڽ��� role���� �Ǵ��ϰ� �ǰ� ȭ�� ���
		char attackMonsterIdx;//�ִϸ��̼� ����� ���� �ε���
	};

	struct BossAttackPlayerPacket : public PacketHeader
	{
		float currentHp;
	};

	struct BossDirectionPacket : public PacketHeader
	{
		XMFLOAT3 directionVec;
	};

	struct DestroyedMeteoPacket : public PacketHeader
	{
		char idx;
	};

	struct MeteoStartPacket : public PacketHeader
	{
		MeteoInfo meteoInfo[10];
	};

	struct TimeSyncPacket : public PacketHeader
	{
		std::chrono::utc_clock::time_point t;
	};
}

#pragma pack (pop)