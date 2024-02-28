

//DB�� ����Ǵ� ���ڼ��� 19����, cpp������ null���� �����Ͽ� 20����
constexpr short NAME_SIZE = 20;

#pragma pack (push, 1)
//���������� ��Ŷ �ش��� ����
struct PacketHeader
{
	unsigned short size;
	unsigned char type;
};

namespace CLIENT_PACKET {
	enum class TYPE : unsigned char {
#pragma region Move
		MOVE_KEY_DOWN = 0,
		MOVE_KEY_UP,
		STOP,
		ROTATE,
#pragma endregion

#pragma region LOBBY
		LOGIN,
		MATCH,
#pragma endregion

#pragma region SKILL
		SKILL_EXECUTE_Q,
		SKILL_EXECUTE_E,
		SKILL_INPUT_Q,
		SKILL_INPUT_E,
#pragma endregion
		MATCH_REQUEST,

#pragma region ATTACK
		MOUSE_INPUT,

		SHOOTING_ARROW,
		SHOOTING_BALL,
		MELEE_ATTACK,
		PLAYER_COMMON_ATTACK_EXECUTE,
		ARCHER_SKILL_ARROW,
		PLAYER_COMMON_ATTACK, //�ִ� ����
#pragma endregion

#pragma region INGAME_PLAY
		TRIGGER_BOX_ON,
		TRIGGER_BOX_OUT,
		SKIP_NPC_COMMUNICATION,
		STAGE_CHANGE_BOSS,
		PLAYER_POSITION_STATE,
		GAME_END_OK,
		TEST_GAME_END, //�ӽ÷� Ŭ�󿡼� �����Ͽ� ���� ���� �� �ְ�
#pragma endregion
		TIME_SYNC_REQUEST,
	};

	//TYPE���� ���ߵǴ� ��Ŷ
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

#pragma region DISCARD
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
#pragma endregion
}

namespace SERVER_PACKET {
	enum class TYPE : unsigned char {
#pragma region MOVE
		MOVE_KEY_DOWN = 0,
		MOVE_KEY_UP,
		STOP,
		ROTATE,
#pragma endregion

#pragma region LOBBY
		LOGIN_SUCCESS,
		LOGIN_FAIL,
		DUPLICATED_LOGIN,//�ߺ��� �α����� ���Դ�
		PRE_EXIST_LOGIN, //�̹� �α��ε� ���̵��
#pragma endregion

		MOUSE_INPUT,

		ADD_PLAYER,
		INTO_GAME,

		BOSS_CHANGE_STATE_MOVE_DES,
		BOSS_ATTACK_EXECUTE,
		BOSS_MOVE_NODE,
		STAGE_CHANGING_BOSS,
		STAGE_START_BOSS,
		SMALL_MONSTER_MOVE,
		SMALL_MONSTER_ATTACK,
		BOSS_ATTACK_PALYER,
		BOSS_CHANGE_DIRECION,
		METEO_PLAYER_ATTACK,
		METEO_DESTROY,
		METEO_CREATE,

		GAME_STATE,
		GAME_END,

		COMMON_ATTACK_START,
		PLAYER_ATTACK_RESULT,
		PLAYER_ATTACK_RESULT_BOSS,

		SHOOTING_ARROW,
		SHOOTING_BALL,
		EXECUTE_LIGHTNING,
		HEAL_START,
		HEAL_END,
		SHIELD_START,
		SHIELD_END,
		NOTIFY_HEAL_HP,
		NOTIFY_SHIELD_APPLY,

		START_ANIMATION_Q,
		START_ANIMATION_E,

		TIME_SYNC_RESPONSE,
	};

#pragma region DISARD
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
#pragma endregion

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
		wchar_t nickName[NAME_SIZE];
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