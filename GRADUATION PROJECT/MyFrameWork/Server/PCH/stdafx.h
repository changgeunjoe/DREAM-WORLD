#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define MAX_USER 40000
#define ALONE_TEST 1
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "lua54.lib")

#include <WS2tcpip.h>
#include <MSWSock.h>

#include <Windows.h>
#include <wchar.h>

#include <chrono>

#include <mutex>

#include <queue>
#include <vector>
#include <unordered_map>
#include <array>
#include <set>
#include <map>
#include <thread>
#include <utility>

#include <concurrent_queue.h>
#include <concurrent_priority_queue.h>
#include <concurrent_unordered_set.h>
#include <atomic>

#include <iostream>
#include <math.h>
#include <random>
#include <DirectXMath.h>
#include <DirectXCollision.h>

extern "C"
{
#include "../lua/include/lua.h"
#include "../lua/include/lauxlib.h"
#include "../lua/include/lualib.h"
}

enum PLAYER_STATE
{
	FREE,
	ALLOC,
	IN_GAME,
	IN_GAME_ROOM
};

enum IOCP_OP_CODE
{
	OP_NONE,
	OP_ACCEPT,
	OP_RECV,
	OP_SEND,
	OP_FIND_PLAYER,
	OP_BOSS_STATE,
	//OP_MOVE_BOSS,
	OP_GAME_STATE_SEND,
	OP_BOSS_ATTACK_SELECT,
	OP_BOSS_ATTACK_EXECUTE
};

enum DIRECTION : char
{
	IDLE = 0x00,
	FRONT = 0x01,
	RIGHT = 0x02,
	LEFT = 0x04,
	BACK = 0x08
};

enum ROTATE_AXIS :char
{
	X, Y, Z
};

enum ROLE :char {
	NONE_SELECT = 0x00,
	WARRIOR = 0x01,
	PRIEST = 0x02,
	TANKER = 0x04,
	ARCHER = 0x08,
	RAND = 0x10
};

enum BOSS_ATTACK : char {
	ATTACK_PUNCH,
	ATTACK_SPIN,
	ATTACK_KICK,
	ATTACK_COUNT//0~마지막 숫자 갯수
};

enum EVENT_TYPE {
	EV_NONE,
	EV_FIND_PLAYER,
	EV_BOSS_STATE,
	EV_GAME_STATE_SEND,
	EV_BOSS_ATTACK_ORDER,
	EV_BOSS_KICK,
	EV_BOSS_SPIN,
	EV_BOSS_PUNCH
};

struct TIMER_EVENT
{
	std::chrono::system_clock::time_point wakeupTime;
	int targetId;
	EVENT_TYPE eventId = EV_NONE;
	constexpr bool operator < (const TIMER_EVENT& L) const
	{
		return (wakeupTime > L.wakeupTime);
	}
};
constexpr int MAX_BUF_SIZE = 1024;
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

class TrinangleMesh
{
private:
	XMFLOAT3 m_vertex1;
	XMFLOAT3 m_vertex2;
	XMFLOAT3 m_vertex3;
private:
	XMFLOAT3 m_center;
	float m_areaSize;
private:
	XMFLOAT3 vec12;
	XMFLOAT3 vec13;
	XMFLOAT3 vec23;
public:
	std::map<int, float> m_relationMesh;

public:
	TrinangleMesh(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& v3) :m_vertex1(v1), m_vertex2(v2), m_vertex3(v3)
	{
		m_center = Vector3::ScalarProduct(Vector3::Add(m_vertex1, Vector3::Add(m_vertex2, m_vertex3)), 1.0f / 3.0f, false);
		vec12 = Vector3::Subtract(m_vertex2, m_vertex1);
		vec13 = Vector3::Subtract(m_vertex3, m_vertex1);
		vec23 = Vector3::Subtract(m_vertex3, m_vertex2);
		m_areaSize = Vector3::Length(Vector3::CrossProduct(vec12, vec13, false)) / 2.0f;
	}
	TrinangleMesh& operator=(const TrinangleMesh& other) {
		m_vertex1 = other.m_vertex1;
		m_vertex2 = other.m_vertex2;
		m_vertex3 = other.m_vertex3;
		m_center = other.m_center;
		vec12 = other.vec12;
		vec13 = other.vec13;
		vec23 = other.vec23;
		m_areaSize = other.m_areaSize;
	}
	float GetDistance(TrinangleMesh& other)
	{
		return Vector3::Length(Vector3::Subtract(m_center, other.m_center));
	}
	float GetDistance(float x, float y, float z)
	{
		return Vector3::Length(Vector3::Subtract(m_center, XMFLOAT3(x, y, z)));
	}
	bool IsOnTriangleMesh(float x, float y, float z)
	{
		XMFLOAT3 triVec1 = Vector3::Subtract(XMFLOAT3(x, 0, z), m_vertex1);
		XMFLOAT3 triVec2 = Vector3::Subtract(XMFLOAT3(x, 0, z), m_vertex2);
		XMFLOAT3 triVec3 = Vector3::Subtract(XMFLOAT3(x, 0, z), m_vertex3);

		float res = 0.0f;
		//1 - 2
		res = Vector3::Length(Vector3::CrossProduct(triVec1, triVec2, false));
		//3 - 1
		res += Vector3::Length(Vector3::CrossProduct(triVec3, triVec1, false));
		//2 - 3
		res += Vector3::Length(Vector3::CrossProduct(triVec2, triVec3, false));
		res /= 2.0f;
		return abs(m_areaSize - res) < 0.1f;
	}
	XMFLOAT3 const GetCenter() { return m_center; }
};

class AstarNode {
private:
	int m_nodeIdx = -1;
	float m_cost = 0.0f;
	float m_dis = 0.0f;
	float m_res = 0.0f;
	int m_parentNodeIdx = -1;

public:
	AstarNode() {}
	AstarNode(int nodeIdx, float cost, float dis, float res, int parentNodeIdx) : m_nodeIdx(nodeIdx), m_cost(cost), m_dis(dis), m_res(res), m_parentNodeIdx(parentNodeIdx) {}
	AstarNode(AstarNode& other)
	{
		m_nodeIdx = other.m_nodeIdx;
		m_cost = other.m_cost;
		m_dis = other.m_dis;
		m_res = other.m_res;
		m_parentNodeIdx = other.m_parentNodeIdx;
	}
	AstarNode(AstarNode&& other)
	{
		m_nodeIdx = other.m_nodeIdx;
		m_cost = other.m_cost;
		m_dis = other.m_dis;
		m_res = other.m_res;
		m_parentNodeIdx = other.m_parentNodeIdx;
	}
	~AstarNode() {}
public:
	void RefreshNodeData(int nodeIdx, float cost, float dis, float res, int parentNodeIdx)
	{
		m_nodeIdx = nodeIdx;
		m_cost = cost;
		m_dis = dis;
		m_res = res;
		m_parentNodeIdx = parentNodeIdx;
	}
	float GetResValue() { return m_res; }
	int GetIdx() { return m_nodeIdx; }
	int GetParentIdx() { return m_parentNodeIdx; }

public:
	constexpr bool operator< (const AstarNode& other)const {
		return m_res < other.m_res;
	}
	AstarNode& operator= (const AstarNode& other) {
		m_nodeIdx = other.m_nodeIdx;
		m_cost = other.m_cost;
		m_dis = other.m_dis;
		m_res = other.m_res;
		m_parentNodeIdx = other.m_parentNodeIdx;
		return *this;
	}

	AstarNode& operator= (const AstarNode&& other) noexcept {
		m_nodeIdx = other.m_nodeIdx;
		m_cost = other.m_cost;
		m_dis = other.m_dis;
		m_res = other.m_res;
		m_parentNodeIdx = other.m_parentNodeIdx;
		return *this;
	}
};


void PrintCurrentTime();
void DisplayWsaGetLastError(int Errcode);