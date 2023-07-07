#pragma once

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "windowscodecs.lib")


// C�� ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <dwrite_3.h>
#include <string>
#include <wrl.h>
#include <shellapi.h>
#include<list>
#include <fstream>
#include <array>
#include <vector>
#include <queue>
#include<iostream>
#include <chrono>
#include <filesystem>
using namespace std;
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include <d2d1_3.h>
#include <dwrite_1.h>
#include <d3d11on12.h>
#include <d2d1_1helper.h>

#include <Mmsystem.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif
#define PLAYER_MAX_RANGE 288.0f
//#ifndef LOCAL_TASK
#define LOCAL_TASK 1
//#endif // !LOCAL_TASK

///////////////////////////////////////
/*** ���⼭���� �� å�� ��� �������� �������� �����Ͽ� ����ϴ� �ڵ��̴�. ***/

#define _CRT_SECURE_NO_WARNINGS // ���� C �Լ� ��� �� ��� ����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // ���� ���� API ��� �� ��� ����

#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���

#include <tchar.h> // _T(), ...
#include <stdio.h> // printf(), ...
#include <stdlib.h> // exit(), ...
#include <string.h> // strncpy(), ...
#include <string>
#include <iostream>
#include<random>
#include <unordered_map>
#include <map>
#include <utility>

#include "d3dx12.h"

#include <mutex>

#pragma comment(lib, "ws2_32") // ws2_32.lib ��ũ
///////////////////////////////////////////

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

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE
#define PIXELCOUNT				257
#define FRAME_BUFFER_WIDTH		1280
#define FRAME_BUFFER_HEIGHT		720


#define _PLANE_WIDTH			300
#define _PLANE_HEIGHT			300

#define RP_DEPTH_BUFFER	16
#define RP_TO_LIGHT	15


#define _DEPTH_BUFFER_WIDTH		(FRAME_BUFFER_WIDTH * 8)
#define _DEPTH_BUFFER_HEIGHT	(FRAME_BUFFER_HEIGHT * 8)

#define ANIMATION_TYPE_ONCE				0
#define ANIMATION_TYPE_LOOP				1
#define ANIMATION_TYPE_HALF				2

#define DEFAULT_MODE			0
#define CARTOON_MODE			1
#define OUTLINE_MODE			2
#define CELLSHADING_MODE		3

#define ANIMATION_CALLBACK_EPSILON		0.0165f
//#define _WITH_CB_GAMEOBJECT_32BIT_CONSTANTS
//#define _WITH_CB_GAMEOBJECT_ROOT_DESCRIPTOR
#define _WITH_CB_WORLD_MATRIX_DESCRIPTOR_TABLE

#define _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS

//#define _WITH_DIRECT2D

#define PARAMETER_STANDARD_TEXTURE		3
#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS
#define PARAMETER_SKYBOX_CUBE_TEXTURE	10
#else
#define PARAMETER_SKYBOX_CUBE_TEXTURE	4
#endif

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

#define MAX_ARROW 10

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

extern UINT gnCbvSrvDescriptorIncrementSize;
extern UINT	gnRtvDescriptorIncrementSize;
extern UINT gnDsvDescriptorIncrementSize;

extern void SynchronizeResourceTransition(ID3D12GraphicsCommandList* pd3dCommandList, ID3D12Resource* pd3dResource, D3D12_RESOURCE_STATES d3dStateBefore, D3D12_RESOURCE_STATES d3dStateAfter);
extern void WaitForGpuComplete(ID3D12CommandQueue* pd3dCommandQueue, ID3D12Fence* pd3dFence, UINT64 nFenceValue, HANDLE hFenceEvent);

extern ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource** ppd3dUploadBuffer = NULL);
extern ID3D12Resource* CreateTextureResourceFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
extern ID3D12Resource* CreateTextureResourceFromWICFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
extern ID3D12Resource* CreateTexture2DResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue);

extern BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken);
extern int ReadIntegerFromFile(FILE* pInFile);
extern float ReadFloatFromFile(FILE* pInFile);
extern XMFLOAT3 ReadVectorFromFile(FILE* pInFile, int n);
#define RANDOM_COLOR			XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

#define EPSILON					1.0e-10f

inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsZero(float fValue, float fEpsilon) { return((fabsf(fValue) < fEpsilon)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline bool IsEqual(float fA, float fB, float fEpsilon) { return(::IsZero(fA - fB, fEpsilon)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float* pfS, float* pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }
inline float RandomValue(float a, float b) { return(a + (((float)rand() / (float)RAND_MAX) * (b - a))); }
inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

struct MATERIAL
{
	XMFLOAT4						m_xmf4Ambient;
	XMFLOAT4						m_xmf4Diffuse;
	XMFLOAT4						m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4						m_xmf4Emissive;
};

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
	MATERIAL						m_material;
	UINT							m_nType;
	bool							m_bAnimateshader;
	
};

struct CB_GAMEOBJECTWORLD_INFO
{
	XMFLOAT4X4						m_xmf4x4World;
};
struct CB_GAMEOBJECTCOLOR_INFO
{
	XMFLOAT4						m_xmf4Color;
};

struct CB_GAMEOBJECT_STAT
{
	float							m_xmfHP;
	bool							m_bRimLight;
};
struct CB_UIOBJECT_INFO
{
	bool							m_bUIActive;
};
struct CB_GAMEOBJECT_MULTISPRITE
{
	XMFLOAT4X4						m_xmf4x4Texture;
	bool							m_bMultiSprite;
};
struct SHADOW_INFO
{
	XMFLOAT4X4						m_xmf4x4View;
	XMFLOAT4X4						m_xmf4x4Projection;
	XMFLOAT4X4						m_xmf4x4ShadowView;
	XMFLOAT4X4						m_xmf4x4ShadowProjection;
	XMFLOAT4X4						m_xmf4x4ShadowTransform;
	XMFLOAT3						m_xmf3CameraPosition;
	XMFLOAT3						m_xmf3LightPosition;
};

struct CB_GAMEFRAMEWORK_INFO
{
	float							m_xmfTime;
	float							m_xmfMode;
};
enum entity_id
{
	SQUARE_ENTITY,
	PlANE_ENTITY,
	UNDEF_ENTITY,
	UI_ENTITY,
	MultiSPRITE_ENTITY
};
enum component_id
{
	UNDEF_COMPONENT,
	RENDER_COMPONENT,
	INSRENDER_COMPONENT,
	PHYSICS_COMPONENT,
	MESH_COMPONENT,
	CUBEMESH_COMPONENT,
	SKYBOXMESH_COMPONENT,
	UIMESH_COMPONENT,
	SPHEREMESH_COMPONENT,
	SHADER_COMPONENT,
	SKYSHADER_COMPONENT,
	SHADOWSHADER_COMPONENT,
	LOADEDMODEL_COMPONET,
	TEXTURE_COMPONENT,
	DEPTHSHADER_COMPONENT,
	UISHADER_COMPONENT,
	BLENDINGUISHADER_COMPONENT,
	SPRITESHADER_COMPONENT,
	BOUNDINGBOX_COMPONENT,
	SPHERE_COMPONENT,
	TRAILMESH_COMPONENT,
	TRAILSHADER_COMPONENT,
	HEIGHTMESH_COMPONENT,
	TERRAINSHADER_COMPONENT,	
	NAVIMESHSHADER_COMPONENT,
	EFFECTSHADER_COMPONENT,
	BLENDSHADER_COMPONENT
};
//mean about component_id;
enum CharacterType
{
	CT_NONE = 0,
	CT_WARRIOR,
	CT_ARCHER,
	CT_TANKER,
	CT_PRIEST,
};

enum CharacterAnimation
{
	CA_IDLE = 0,
	CA_MOVE,
	CA_ATTACK,
	CA_DIE,
	CA_VICTORY,
	CA_SKILL,
	CA_BLENDING = 99
};

//NPCText
enum TEXT
{
	NPC_TEXT=0
};


enum BOSS_ATTACK : char {
	ATTACK_PUNCH,
	ATTACK_SPIN,
	ATTACK_KICK,
	ATTACK_COUNT//0~������ ���� ����
};

struct VS_VB_INSTANCE
{
	XMFLOAT4X4 m_xmf4x4Transform;
};

enum Message {
	ITEM_TYPE,
};
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

	inline XMFLOAT3 Subtract(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
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

	inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
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
	inline XMFLOAT3 CatmullRom(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, XMFLOAT3& xmf3Vector3, XMFLOAT3& xmf3Vector4, float t)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVectorCatmullRom(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2),
			XMLoadFloat3(&xmf3Vector3), XMLoadFloat3(&xmf3Vector4), t));
		return(xmf3Result);
	}

	inline XMFLOAT3 CatmullRom(XMFLOAT3* xmf3Vector, float t)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVectorCatmullRom(XMLoadFloat3(&xmf3Vector[0]), XMLoadFloat3(&xmf3Vector[1]),
			XMLoadFloat3(&xmf3Vector[2]), XMLoadFloat3(&xmf3Vector[3]), t));
		return(xmf3Result);
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

	inline XMFLOAT4X4 Zero()
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixSet(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
		return(xmf4x4Result);
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

	inline XMFLOAT4X4 Scale(XMFLOAT4X4& xmf4x4Matrix, float fScale)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMLoadFloat4x4(&xmf4x4Matrix) * fScale);
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Add(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMStoreFloat4x4(&xmf4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) + XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmf4x4Result);
	}

	inline XMFLOAT4X4 Interpolate(XMFLOAT4X4& xmf4x4Matrix1, XMFLOAT4X4& xmf4x4Matrix2, float t)
	{
		XMFLOAT4X4 xmf4x4Result;
		XMVECTOR S0, R0, T0, S1, R1, T1;
		XMMatrixDecompose(&S0, &R0, &T0, XMLoadFloat4x4(&xmf4x4Matrix1));
		XMMatrixDecompose(&S1, &R1, &T1, XMLoadFloat4x4(&xmf4x4Matrix2));
		XMVECTOR S = XMVectorLerp(S0, S1, t);
		XMVECTOR T = XMVectorLerp(T0, T1, t);
		XMVECTOR R = XMQuaternionSlerp(R0, R1, t);
		XMStoreFloat4x4(&xmf4x4Result, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
		return(xmf4x4Result);
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

	inline XMFLOAT4X4 OrthographicsFovLH(float ScreenWidth, float ScreenHeight, float NearZ, float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixOrthographicLH(ScreenWidth, ScreenHeight, NearZ, FarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 LookAtLH(const XMFLOAT3& xmf3EyePosition, const XMFLOAT3& xmf3LookAtPosition, const XMFLOAT3& xmf3UpDirection)
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

namespace Triangle
{
	inline bool Intersect(XMFLOAT3& xmf3RayPosition, XMFLOAT3& xmf3RayDirection, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2, float& fHitDistance)
	{
		return(TriangleTests::Intersects(XMLoadFloat3(&xmf3RayPosition), XMLoadFloat3(&xmf3RayDirection), XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), fHitDistance));
	}
}

namespace Plane
{
	inline XMFLOAT4 Normalize(XMFLOAT4& xmf4Plane)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMPlaneNormalize(XMLoadFloat4(&xmf4Plane)));
		return(xmf4Result);
	}
}
/////////////////////////////

#define RESOURCE_TEXTURE1D			0x01
#define RESOURCE_TEXTURE2D			0x02
#define RESOURCE_TEXTURE2D_ARRAY	0x03	//[]
#define RESOURCE_TEXTURE2DARRAY		0x04
#define RESOURCE_TEXTURE_CUBE		0x05
#define RESOURCE_BUFFER				0x06


class TrinangleMesh
{
private:
	XMFLOAT3 m_vertex1;
	XMFLOAT3 m_vertex2;
	XMFLOAT3 m_vertex3;
	XMFLOAT3 m_center;
public:
	std::map<int, float> m_relationMesh;

public:
	TrinangleMesh(XMFLOAT3& v1, XMFLOAT3& v2, XMFLOAT3& v3) :m_vertex1(v1), m_vertex2(v2), m_vertex3(v3)
	{
		m_center = Vector3::ScalarProduct(Vector3::Add(m_vertex1, Vector3::Add(m_vertex2, m_vertex3)), 1.0f / 3.0f, false);
	}
	TrinangleMesh& operator=(const TrinangleMesh& rhs) {
		m_vertex1 = rhs.m_vertex1;
		m_vertex2 = rhs.m_vertex2;
		m_vertex3 = rhs.m_vertex3;
		m_center = rhs.m_center;
	}
	float GetDistance(TrinangleMesh& other)
	{
		return Vector3::Length(Vector3::Subtract(m_center, other.m_center));
	}
	float GetDistance(float x, float y, float z)
	{
		return Vector3::Length(Vector3::Subtract(m_center, XMFLOAT3(x, y, z)));
	}
	XMFLOAT3 const GetCenter() { return m_center; }
	XMFLOAT3 const GetVertex1() { return m_vertex1; }
	XMFLOAT3 const GetVertex2() { return m_vertex2; }
	XMFLOAT3 const GetVertex3() { return m_vertex3; }
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