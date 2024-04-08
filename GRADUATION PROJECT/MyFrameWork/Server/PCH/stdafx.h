#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define MAX_USER 4000
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "WS2_32.lib")

#ifdef _DEBUG
#pragma comment(lib, "lib/x64/Debug/spdlogd.lib")
#else  //Release
#pragma comment(lib, "lib/x64/Release/spdlog.lib")
#endif // _DEBUG

#include <spdlog/spdlog.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <codecvt>
#include <atlconv.h>
#include <strsafe.h>

#include <WS2tcpip.h>
#include <MSWSock.h>

#include <Windows.h>
#include <wchar.h>

#include <chrono>

#include <mutex>
#include <shared_mutex>

#include <queue>
#include <stack>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <utility>

#include <span>
#include <ranges>
#include <functional>
#include <optional>

#include <thread>
#include <atomic>
#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_priority_queue.h>
#include <tbb/concurrent_queue.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>

#include<sqlext.h>

#include<filesystem>
#include <iostream>
#include <sstream>
#include <fstream>

#include <math.h>
#include <random>
#include <DirectXMath.h>
#include <DirectXCollision.h>


const float MONSTER_ABLE_ATTACK_COS_VALUE = std::cos(25.0f * 3.14f / 180.0f);//30도 - 0.96정도
const float PLAYER_ABLE_ATTACK_COS_VALUE = std::cos(25.0f * 3.14f / 180.0f);//30도 - 0.96정도
const float BOSS_ABLE_ATTACK_COS_VALUE = std::cos(15.0f * 3.14f / 180.0f);//30도 - 0.96정도

enum class PLAYER_STATE : char
{
	LOBBY,
	MATCH,
	IN_GAME
};

enum class IOCP_OP_CODE : char
{
	OP_NONE,
	//통신
	OP_ACCEPT,
	OP_RECV,
	OP_SEND,

	//DB
	//로그인, 플레이어 로그인 성공
	OP_SUCCESS_GET_PLAYER_INFO,
	//로그인, 플레이어 로그인 실패
	OP_FAIL_GET_PLAYER_INFO,
	//클라이언트의 DB작업이 실패 했을 때
	OP_DB_ERROR,

	//Room - update, player pos send
	OP_ROOM_UPDATE,
	OP_GAME_STATE_SEND,
	OP_SEND_NPC_MOVE,
	//Room - Boss
	OP_FIND_PLAYER,
	OP_BOSS_ATTACK_SELECT,
	OP_BOSS_ATTACK_EXECUTE,

	//Room - player skill
	OP_PLAYER_HEAL, // 지속 힐 - 틱마다
	OP_SKY_ARROW_ATTACK, //n초 이후 그 위치 공격

	//전제 플레이어 RTT계산을 위한
	OP_SYNC_TIME,
};

enum class DIRECTION : char
{
	IDLE = 0x00,
	FRONT = 0x01,
	RIGHT = 0x02,
	LEFT = 0x04,
	BACK = 0x08
};

enum class ROTATE_AXIS :char
{
	X, Y, Z
};

enum class ROLE :char {
	NONE_SELECT = 0x00,
	WARRIOR = 0x01,
	MAGE = 0x02,
	TANKER = 0x04,
	ARCHER = 0x08,
};

enum class BOSS_ATTACK : char {
	ATTACK_PUNCH,
	ATTACK_SPIN,
	ATTACK_KICK,
	ATTACK_FLOOR_BOOM,
	ATTACK_FLOOR_BOOM_SECOND,
	ATTACK_METEO,
	ATTACK_COUNT //0~마지막 숫자 갯수
};

enum class TIMER_EVENT_TYPE : char {
	EV_NONE,
	//게임 상태 관련
	EV_ROOM_UPDATE,
	EV_SEND_GAME_STATE,
	EV_SEND_NPC_MOVE,
	//보스 관련
	EV_BOSS_FIND_PLAYER,
	EV_BOSS_ATTACK,
	//플레이어 관련
	EV_HEAL,
	EV_TANKER_SHIELD_END,
	EV_SKY_ARROW_ATTACK,
	//전체 플레이어 RTT체크를 위한 타이머
	EV_SYNC_TIME
};

enum class ROOM_STATE : char {
	ROOM_COMMON,
	ROOM_BOSS,
	ROOM_END
};

constexpr short PORT = 9000;

constexpr int MAX_SEND_BUF_SIZE = 514;
constexpr int MAX_RECV_BUF_SIZE = 1024;

using namespace DirectX;

namespace Vector3
{
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}

	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}

	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}

	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	inline float DotProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}

	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result);
	}

	inline XMFLOAT3 Normalize(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}

	inline float Length(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}

	inline float Angle(XMVECTOR& xmvVector1, XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return(XMConvertToDegrees(XMVectorGetX(xmvAngle)));
	}

	inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
	}

	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}

	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformNormal(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}
}

namespace Vector4
{
	inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}

	inline XMFLOAT4 Multiply(float fScalar, XMFLOAT4& xmf4Vector)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));
		return(xmf4Result);
	}
}

namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 RotationAxis(XMFLOAT3& xmf3Axis, float fAngle)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixRotationAxis(XMLoadFloat3(&xmf3Axis), XMConvertToRadians(fAngle)));
		return(xmmtx4x4Result);
	}
}

void PrintCurrentTime();
void DisplayWsaGetLastError(const int& wsaErrcode);
void StartLogger();
//wchar->char
std::string ConvertWideStringToString(const wchar_t* wstr);
//char->wchar
std::wstring ConvertStringToWideString(const char* str);
