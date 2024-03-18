

//DB에 저장되는 글자수는 19글자, cpp에서는 null문자 포함하여 20글자
constexpr short NAME_SIZE = 20;

#pragma pack (push, 1)
//공통적으로 패킷 해더를 가짐
struct PacketHeader
{
	unsigned short size;
	unsigned char type;
	PacketHeader(const unsigned char& t) : type(t), size(sizeof(PacketHeader)) {}
	PacketHeader(const unsigned char& t, const unsigned short& s) : type(t), size(s) {}
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
		PLAYER_COMMON_ATTACK, //애니 실행
#pragma endregion

#pragma region INGAME_PLAY
		TRIGGER_BOX_ON,
		TRIGGER_BOX_OUT,
		SKIP_NPC_COMMUNICATION,
		STAGE_CHANGE_BOSS,
		PLAYER_POSITION_STATE,
		GAME_END_OK,
		TEST_GAME_END, //임시로 클라에서 전송하여 게임 끝낼 수 있게
#pragma endregion
		TIME_SYNC_REQUEST,
	};

	//TYPE으로 유추되는 패킷
	using NotifyPacket = PacketHeader;

	struct MovePacket : public PacketHeader
	{//시간과 진행 방향, 이동 입력
		DIRECTION direction;
		PacketTime time;
		MovePacket(const char& type) : PacketHeader(type, sizeof(MovePacket)) {}
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

	struct ShootingObject : public PacketHeader
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 direction;
		float speed;
		ShootingObject(const char& type) : PacketHeader(type, sizeof(ShootingObject)) {}
	};

	struct PlayerCommonAttackPacket : public PacketHeader
	{
		DirectX::XMFLOAT3 direction;
		int power;// 아처 줌인, 워리어 3단계
		PlayerCommonAttackPacket(const char& type = static_cast<char>(TYPE::PLAYER_COMMON_ATTACK_EXECUTE)) : PacketHeader(type, sizeof(PlayerCommonAttackPacket)) {}
		PlayerCommonAttackPacket(const DirectX::XMFLOAT3& direction, const int& power, const char& type = static_cast<char>(TYPE::PLAYER_COMMON_ATTACK_EXECUTE))
			: PacketHeader(type, sizeof(PlayerCommonAttackPacket)), direction(direction), power(power)
		{}
	};

	struct SkillAttackPacket : public PacketHeader
	{
		XMFLOAT3 float3Data;
		SkillAttackPacket(const char& type) : PacketHeader(type, sizeof(SkillAttackPacket)) {}
		SkillAttackPacket(const XMFLOAT3& float3Data, const char& type)
			: PacketHeader(type, sizeof(SkillAttackPacket)), float3Data(float3Data)
		{}
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
		DUPLICATED_LOGIN,//중복된 로그인이 들어왔다
		PRE_EXIST_LOGIN, //이미 로그인된 아이디다
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

#pragma region DISARD
	//constexpr unsigned char CREATE_ROOM_SUCCESS = 71; // 룸 생성 시 성공 패킷
	//constexpr unsigned char CREATE_ROOM_FAILURE = 72; // 룸 생성 시 실패 패킷
	//constexpr unsigned char REQUEST_ROOM_LIST = 73; // 방 리스트 출력중
	//constexpr unsigned char REQUEST_ROOM_LIST_END = 74;// 방 리스트의 끝
	//constexpr unsigned char REQUEST_ROOM_LIST_NONE = 75;//어떤 방도 없다
	//constexpr unsigned char ACCEPT_ENTER_ROOM = 76; // 방 입장 확인
	//constexpr unsigned char REJECT_ENTER_ROOM = 77; // 방 입장 거부
	//constexpr unsigned char NOT_FOUND_ROOM = 78; // 신청한 방이 사라짐
	//
	//constexpr unsigned char PLAYER_APPLY_ROOM = 79; // 신청자 정보 방장(방)한테 전송
	//constexpr unsigned char PLAYER_CANCEL_ROOM = 80; // 신청 취소 정보 방장(방)한테 전송
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

	struct IntoGamePacket : public PacketHeader
	{
		ROLE role;
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
		RotatePacket(const ROLE& role, const ROTATE_AXIS& axis, const char& type = static_cast<char>(TYPE::ROTATE))
			: PacketHeader(type, sizeof(RotatePacket)), role(role)
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
		MouseInputPacket(const char& type = static_cast<char>(TYPE::MOUSE_INPUT)) : PacketHeader(type, sizeof(MouseInputPacket)) {}
		MouseInputPacket(const ROLE& role, const bool& left, const bool& right, const char& type = static_cast<char>(TYPE::MOUSE_INPUT))
			: PacketHeader(type, sizeof(MouseInputPacket)), role(role), leftClickedButton(left), rightClickedButton(right)
		{}
	};

	struct BossChangeStateMovePacket : public PacketHeader
	{//이걸 수정해야할듯?

		DirectX::XMFLOAT3 desPos;
		DirectX::XMFLOAT3 bossPos;
		PacketTime time;
		BossChangeStateMovePacket(const char& type) : PacketHeader(type, sizeof(BossChangeStateMovePacket)) {}
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
		int aliveMonsterCnt;//살아 있는 몬스터 갯수 입니다. 최대 15개
		GameState_STAGE() : GameState_Base(static_cast<char>(SERVER_PACKET::TYPE::GAME_STATE_STAGE), sizeof(GameState_STAGE)) {}
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
	{//Player State-> pos rot...추가하여 보정?		
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
	{//브로드 캐스트	
		char projectileId;//인덱스 번호
		XMFLOAT3 position;//맞은 위치
		float damage;// 데미지		
	};

	struct PlayerAttackMonsterDamagePacket : public PacketHeader
	{//본인만 데미지 볼 수 있게	
		ROLE role;
		char attackedMonsterCnt;//뎀지 입은 몬스터 갯수
		char monsterIdx[15];//해당 몬스터의 인덱스값이 있는 배열
		float damage;// 플레이어가 입힌 데미지

	};

	struct PlayerAttackBossDamagePacket : public PacketHeader {
		ROLE role;
		float damage;// 플레이어가 입힌 데미지
	};

	struct CommonAttackPacket : public PacketHeader
	{//전체 플레이어들에게 알려서 애니메이션 재생		
		ROLE role;
	};

	struct SmallMonsterAttackPlayerPacket : public PacketHeader
	{
		ROLE attackedRole;//자기 자신의 role인지 판단하고 피격 화면 출력
		char attackMonsterIdx;//애니메이션 재생할 몬스터 인덱스
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