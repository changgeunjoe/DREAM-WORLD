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
	//constexpr unsigned char REQUEST_ROOM_LIST = 8; // 방 리스트 요청
	//constexpr unsigned char PLAYER_APPLY_ROOM = 9; // 방 신청
	//constexpr unsigned char CANCEL_APPLY_ROOM = 10; // 신청 취소

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
	constexpr unsigned char TEST_GAME_END = 17; //임시로 클라에서 전송하여 게임 끝낼 수 있게

	constexpr unsigned char TRIGGER_BOX_ON = 19;
	constexpr unsigned char TRIGGER_BOX_OUT = 20;

	constexpr unsigned char SKIP_NPC_COMMUNICATION = 23;
	constexpr unsigned char STAGE_CHANGE_BOSS = 24;

	constexpr unsigned char PLAYER_COMMON_ATTACK_EXECUTE = 25;

	constexpr unsigned char ARCHER_SKILL_ARROW = 26;

	constexpr unsigned char TIME_SYNC_REQUEST = 27;

	constexpr unsigned char PLAYER_COMMON_ATTACK = 29;//애니 실행
	constexpr unsigned char PLAYER_POSITION_STATE = 30;

	using NotifyPacket = PacketHeader;
	struct MovePacket : public PacketHeader
	{//시간과 진행 방향, 이동 입력
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
		int power;// 아처 줌인, 워리어 3단계
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
	constexpr unsigned char DUPLICATED_LOGIN = 91;//중복된 로그인이 들어왔다
	constexpr unsigned char PRE_EXIST_LOGIN = 92; //이미 로그인된 아이디다
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
	{ // ROLE 도 필요하다고 생각함 - 추가하는게 날듯?
		int userId;//나 인지 아닌지는 판단해야하니 - 더 이상 필요 없는 변수일거로 판단됨.
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
	{//이걸 수정해야할듯?

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
	{//Player State-> pos rot...추가하여 보정?		
		InGamePlayerState userState[4];
		InGameBossState bossState;
		std::chrono::utc_clock::time_point time;
	};

	struct GameState_STAGE1 : public PacketHeader
	{//Player State-> pos rot...추가하여 보정?		
		InGamePlayerState userState[4];
		InGameSmallMonster smallMonster[15];
		int aliveMonsterCnt;//살아 있는 몬스터 갯수 입니다. 총 15개
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
	{//Player State-> pos rot...추가하여 보정?		
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
	{//브로드 캐스트	
		char projectileId;//인덱스 번호
		XMFLOAT3 position;//맞은 위치
		float damage;// 데미지		
	};

	struct PlayerAttackMonsterDamagePacket : public PacketHeader
	{//본인만 데미지 볼 수 있게	
		char role;
		char attackedMonsterCnt;//뎀지 입은 몬스터 갯수
		char monsterIdx[15];//해당 몬스터의 인덱스값이 있는 배열
		float damage;// 플레이어가 입힌 데미지

	};

	struct PlayerAttackBossDamagePacket : public PacketHeader {
		char role;
		float damage;// 플레이어가 입힌 데미지
	};

	struct CommonAttackPacket : public PacketHeader
	{//전체 플레이어들에게 알려서 애니메이션 재생		
		char role;
	};

	struct SmallMonsterAttackPlayerPacket : public PacketHeader
	{
		char attackedRole;//자기 자신의 role인지 판단하고 피격 화면 출력
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
		std::chrono::utc_clock::time_point t;
	};
}

#pragma pack (pop)