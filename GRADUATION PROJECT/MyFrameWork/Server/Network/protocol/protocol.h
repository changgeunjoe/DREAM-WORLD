

//DB�� ����Ǵ� ���ڼ��� 19����, cpp������ null���� �����Ͽ� 20����
constexpr short NAME_SIZE = 20;

#pragma pack (push, 1)
//���������� ��Ŷ �ش��� ����
struct PacketHeader
{
	unsigned short size;
	unsigned char type;
	PacketHeader(const unsigned char& type) : type(type), size(sizeof(PacketHeader)) {}
	PacketHeader(const unsigned char& type, const unsigned short& sisz) : type(type), size(size) {}
};
using PacketTime = std::chrono::high_resolution_clock::time_point;

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
		SKILL_FLOAT3_EXECUTE_Q,
		SKILL_EXECUTE_E,
		SKILL_FLOAT3_EXECUTE_E,
		SKILL_INPUT_Q,
		SKILL_INPUT_E,
#pragma endregion
		MATCH_REQUEST,

#pragma region ATTACK
		MOUSE_INPUT,

		PLAYER_COMMON_ATTACK_EXECUTE,
		PLAYER_POWER_ATTACK_EXECUTE,

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
		PacketTime time;
		MovePacket(const DIRECTION& direction, const PacketTime& time, const char& type)
			: PacketHeader(type, sizeof(MovePacket)), direction(direction), time(time)
		{}
	};

	struct RotatePacket : public PacketHeader
	{
		ROTATE_AXIS axis;
		float angle;
		RotatePacket(const char& type = static_cast<char>(TYPE::ROTATE)) : PacketHeader(type, sizeof(RotatePacket)) {}
		RotatePacket(const ROTATE_AXIS& axis, const float& angle, const char& type = static_cast<char>(TYPE::ROTATE))
			: PacketHeader(type, sizeof(RotatePacket)), axis(axis), angle(angle)
		{}

	};

	struct StopPacket : public PacketHeader
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotate;
		PacketTime time;

		StopPacket(const char& type = static_cast<char>(TYPE::STOP)) : PacketHeader(type, sizeof(StopPacket)) {}
		StopPacket(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotate, const PacketTime& time, const char& type = static_cast<char>(TYPE::STOP))
			: PacketHeader(type, sizeof(StopPacket)), position(position), rotate(rotate), time(time)
		{}
	};

	struct LoginPacket : public PacketHeader
	{
		char id[NAME_SIZE];
		char pw[NAME_SIZE];
		LoginPacket(const char& type = static_cast<char>(TYPE::LOGIN)) : PacketHeader(type, sizeof(LoginPacket)) {}
	};

	struct MatchPacket : public PacketHeader
	{
		ROLE role;
		MatchPacket(const ROLE& role, const char& type = static_cast<char>(TYPE::MATCH))
			: PacketHeader(type, sizeof(MatchPacket)), role(role)
		{}
	};

	struct MouseInputPacket : public PacketHeader
	{
		bool leftClickedButton;
		bool rightClickedButton;

		MouseInputPacket(const char& type = static_cast<char>(TYPE::MOUSE_INPUT)) : PacketHeader(type, sizeof(MouseInputPacket)) {}
		MouseInputPacket(const bool& left, const bool& right, const char& type = static_cast<char>(TYPE::MOUSE_INPUT))
			: PacketHeader(type, sizeof(MouseInputPacket)), leftClickedButton(left), rightClickedButton(right)
		{}
	};

	struct PlayerCommonAttackPacket : public PacketHeader
	{
		DirectX::XMFLOAT3 direction;
		PlayerCommonAttackPacket(const DirectX::XMFLOAT3& direction, const char& type = static_cast<char>(TYPE::PLAYER_COMMON_ATTACK_EXECUTE))
			: PacketHeader(type, sizeof(PlayerCommonAttackPacket)), direction(direction)
		{}
	};

	struct PlayerPowerAttackPacket : public PacketHeader
	{
		DirectX::XMFLOAT3 direction;
		int power;// ��ó ����, ������ 3�ܰ�
		PlayerPowerAttackPacket(const DirectX::XMFLOAT3& direction, const int& power, const char& type = static_cast<char>(TYPE::PLAYER_POWER_ATTACK_EXECUTE))
			: PacketHeader(type, sizeof(PlayerPowerAttackPacket)), direction(direction), power(power)
		{}
	};

	struct CommonSkillPacket : public PacketHeader
	{
		CommonSkillPacket(const char& type, const short& size = sizeof(CommonSkillPacket)) : PacketHeader(type, size) {}
	};

	struct FloatDataSkillPacket : public CommonSkillPacket
	{
		FloatDataSkillPacket(const char& type, const XMFLOAT3& floatData)
			: CommonSkillPacket(type, sizeof(FloatDataSkillPacket)), floatData(floatData) {}
		XMFLOAT3 floatData;
	};


	/*struct PlayerPositionPacket : public PacketHeader
	{

		XMFLOAT3 position;
		PacketTime time;
		PlayerPositionPacket(const char& type) : PacketHeader(type, sizeof(PlayerPositionPacket)) {}
	};*/

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

#pragma region SMALL_MONSTER
		//SMALL_MONSTER_MOVE_SET_LOOK,
		//SMALL_MONSTER_STOP_SET_LOOK,
		SMALL_MONSTER_STOP,
		SMALL_MONSTER_MOVE,
		SMALL_MONSTER_ATTACK,
		SMALL_MONSTER_SET_DESTINATION,
		//SMALL_MONSTER_MOVE,
		//SMALL_MONSTER_ATTACK,
#pragma endregion

		BOSS_CHANGE_STATE_MOVE_DES,
		BOSS_ATTACK_EXECUTE,
		BOSS_MOVE_NODE,
		STAGE_CHANGING_BOSS,
		STAGE_START_BOSS,
		BOSS_ATTACK_PALYER,
		BOSS_CHANGE_DIRECION,
		METEO_PLAYER_ATTACK,
		METEO_DESTROY,
		METEO_CREATE,

		GAME_STATE_STAGE,
		GAME_STATE_BOSS,
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

#pragma region DISCARD
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

	struct LoginPacket : public PacketHeader
	{
		wchar_t nickName[NAME_SIZE];
		LoginPacket(const char& type = static_cast<char>(TYPE::LOGIN_SUCCESS)) : PacketHeader(type, sizeof(LoginPacket)) {}
		void SetData(const wchar_t* name)
		{

		}
	};

	struct InitMonsterData
	{
		int id;
		XMFLOAT3 position;
		XMFLOAT3 lookVector;
		float maxHp;
	};

	struct IntoGamePacket : public PacketHeader
	{
		ROLE role;
		InitMonsterData monsterData[15];
		IntoGamePacket(const char& type = static_cast<char>(TYPE::INTO_GAME))
			:PacketHeader(type, sizeof(IntoGamePacket))
		{}
		IntoGamePacket(const ROLE& role, const char& type = static_cast<char>(TYPE::INTO_GAME))
			:PacketHeader(type, sizeof(IntoGamePacket)), role(role)
		{
		}
	};

	struct MovePacket : public PacketHeader
	{
		ROLE role;
		DIRECTION direction;
		PacketTime time;
		MovePacket(const char& type) : PacketHeader(type, sizeof(MovePacket)) {}
		MovePacket(const ROLE& role, const DIRECTION& direction, const PacketTime& time, const char& type = static_cast<char>(TYPE::MOVE_KEY_DOWN))
			: PacketHeader(type, sizeof(MovePacket)), role(role), direction(direction), time(time)
		{}
	};

	struct RotatePacket : public PacketHeader
	{
		ROLE role;
		ROTATE_AXIS axis;
		float angle;
		RotatePacket(const char& type = static_cast<char>(TYPE::ROTATE)) : PacketHeader(type, sizeof(RotatePacket)) {}
		RotatePacket(const ROLE& role, const ROTATE_AXIS& axis, const float& angle, const char& type = static_cast<char>(TYPE::ROTATE))
			: PacketHeader(type, sizeof(RotatePacket)), role(role), angle(angle)
		{}
	};

	struct StopPacket : public PacketHeader
	{
		ROLE role;
		PacketTime time;
		StopPacket(const char& type = static_cast<char>(TYPE::STOP)) : PacketHeader(type, sizeof(StopPacket)) {}
		StopPacket(const ROLE& role, const PacketTime& time, const char& type = static_cast<char>(TYPE::STOP))
			: PacketHeader(type, sizeof(StopPacket)), role(role), time(time)
		{}
	};

	struct MouseInputPacket : public PacketHeader
	{
		ROLE role;
		bool leftClickedButton;
		bool rightClickedButton;
		MouseInputPacket(const ROLE& role, const bool& left, const bool& right, const char& type = static_cast<char>(TYPE::MOUSE_INPUT))
			: PacketHeader(type, sizeof(MouseInputPacket)), role(role), leftClickedButton(left), rightClickedButton(right)
		{}
	};

	struct SmallMonsterBase : public PacketHeader
	{
		int id;
		SmallMonsterBase(const int& id, const char& type, const unsigned short& size = sizeof(SmallMonsterBase))
			: PacketHeader(type, size), id(id) {}
	};

	using SmallMonsterPacket = SmallMonsterBase;

	struct SmallMonsterAttackPacket : public SmallMonsterPacket
	{
		ROLE role;
		float hp;
		float shield;
		SmallMonsterAttackPacket(const int& id, const ROLE& role, const float& hp, const float& shield, const char& type = static_cast<char>(TYPE::SMALL_MONSTER_ATTACK))
			: SmallMonsterPacket(id, type, sizeof(SmallMonsterAttackPacket)), role(role), hp(hp), shield(shield)
		{}
	};
	struct SmallMonsterDestinationPacket : public SmallMonsterPacket
	{
		XMFLOAT3 destinationPosition;
		SmallMonsterDestinationPacket(const int& id, const XMFLOAT3& destinationPosition, const char& type = static_cast<char>(TYPE::SMALL_MONSTER_SET_DESTINATION))
			: SmallMonsterPacket(id, type, sizeof(SmallMonsterDestinationPacket)), destinationPosition(destinationPosition)
		{}
	};

	struct GameObjectState {
		XMFLOAT3 position;
		PacketTime time;
	};

	struct LiveObjectState : public GameObjectState {
		float hp;
	};

	struct PlayerState : public LiveObjectState {
		ROLE role;
		float resetShield;
	};

	struct SmallMonsterState : public LiveObjectState {
		char idx;
		bool isAlive;
	};

	struct MeteoInfo {
		XMFLOAT3 position;
		float speed;
	};

	using BossState = LiveObjectState;

	struct GameState_Base : public PacketHeader {
		PlayerState userState[4];
		GameState_Base(const char& type, const short& size) : PacketHeader(type, size) {}
	};

	struct GameState_BOSS : public GameState_Base
	{
		BossState bossState;
		GameState_BOSS() : GameState_Base(static_cast<char>(SERVER_PACKET::TYPE::GAME_STATE_BOSS), sizeof(GameState_BOSS)) {}
	};

	struct GameState_STAGE : public GameState_Base
	{
		SmallMonsterState smallMonster[15];
		GameState_STAGE() : GameState_Base(static_cast<char>(SERVER_PACKET::TYPE::GAME_STATE_STAGE), sizeof(GameState_STAGE)) {}
	};

	struct BossChangeStateMovePacket : public PacketHeader
	{//�̰� �����ؾ��ҵ�?

		DirectX::XMFLOAT3 desPos;
		DirectX::XMFLOAT3 bossPos;
		PacketTime time;
		BossChangeStateMovePacket(const char& type) : PacketHeader(type, sizeof(BossChangeStateMovePacket)) {}
	};

	struct NotifyPlayerAnimationPacket : public PacketHeader
	{//��ü �÷��̾�鿡�� �˷��� �ִϸ��̼� ���
		ROLE role;
		NotifyPlayerAnimationPacket(const char& type, const ROLE& role)
			:PacketHeader(type, sizeof(NotifyPlayerAnimationPacket)), role(role) {}
	};




	struct ShootingObject : public PacketHeader
	{
		XMFLOAT3 dir;
		ShootingObject(const char& type) : PacketHeader(type, sizeof(ShootingObject)) {}
	};

	struct ApplyHealForPlayer {
		char role;
		float hp;
	};

	struct ApplyShieldForPlayer {
		char role;
		float shield;
	};

	struct BossAttackPacket : public PacketHeader
	{
		BOSS_ATTACK bossAttackType;
		BossAttackPacket(const char& type) : PacketHeader(type, sizeof(BossAttackPacket)) {}
	};

	struct BossHitObject : public PacketHeader
	{
		XMFLOAT3 pos;
		BossHitObject(const char& type) : PacketHeader(type, sizeof(BossHitObject)) {}
	};

	struct BossMoveNodePacket : public PacketHeader
	{
		DirectX::XMFLOAT3 bossPos;
		//DirectX::XMFLOAT3 desPos;
		ROLE targetRole;
		int nodeCnt;
		int node[40];
		BossMoveNodePacket(const char& type) : PacketHeader(type, sizeof(BossMoveNodePacket)) {}
	};

	/*struct SmallMonsterMovePacket : public PacketHeader
	{
		XMFLOAT3 desPositions[15];
	};*/

	struct GameState_BOSS_INIT : public PacketHeader
	{//Player State-> pos rot...�߰��Ͽ� ����?		
		XMFLOAT3 bossPosition;
		PlayerState userState[4];
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
		ROLE role;
		char attackedMonsterCnt;//���� ���� ���� ����
		char monsterIdx[15];//�ش� ������ �ε������� �ִ� �迭
		float damage;// �÷��̾ ���� ������

	};

	struct PlayerAttackBossDamagePacket : public PacketHeader {
		ROLE role;
		float damage;// �÷��̾ ���� ������
	};

	struct SmallMonsterAttackPlayerPacket : public PacketHeader
	{
		ROLE attackedRole;//�ڱ� �ڽ��� role���� �Ǵ��ϰ� �ǰ� ȭ�� ���
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
		PacketTime time;
		TimeSyncPacket()
			: PacketHeader(static_cast<char>(TYPE::TIME_SYNC_RESPONSE), sizeof(TimeSyncPacket)), time(std::chrono::high_resolution_clock::now())
		{}
	};
}
#pragma pack (pop)