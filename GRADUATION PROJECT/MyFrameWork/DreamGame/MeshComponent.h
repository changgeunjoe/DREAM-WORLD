#pragma once
#include "ComponentBase.h"
//#include"GameObject.h"
#define VERTEXT_POSITION				0x0001
#define VERTEXT_COLOR					0x0002
#define VERTEXT_NORMAL					0x0004
#define VERTEXT_TANGENT					0x0008
#define VERTEXT_TEXTURE_COORD0			0x0010
#define VERTEXT_TEXTURE_COORD1			0x0020

#define VERTEXT_BONE_INDEX_WEIGHT		0x1000

#define VERTEXT_TEXTURE					(VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_DETAIL					(VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
#define VERTEXT_NORMAL_TEXTURE			(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_NORMAL_TANGENT_TEXTURE	(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TANGENT | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_NORMAL_DETAIL			(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
#define VERTEXT_NORMAL_TANGENT__DETAIL	(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TANGENT | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)


class GameObject;

class CVertex
{
public:
	XMFLOAT3						m_xmf3Position;

public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};

class CDiffusedVertex : public CVertex
{
public:
	XMFLOAT4						m_xmf4Diffuse;

public:
	CDiffusedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); }
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse; }
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf4Diffuse = xmf4Diffuse; }
	~CDiffusedVertex() { }
};

class TextureComponentdVertex : public CVertex
{
public:
	XMFLOAT2						m_xmf2TexCoord;
	XMFLOAT3						m_xmf3Normal;

public:
	TextureComponentdVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); m_xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	TextureComponentdVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord, XMFLOAT3 xmx3Normal) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord; m_xmf3Normal = xmx3Normal; }
	TextureComponentdVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord, XMFLOAT3 xmx3Normal) { m_xmf3Position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord; m_xmf3Normal = xmx3Normal; }
	~TextureComponentdVertex() { }
};

class Textured2DUIVertex : public CVertex
{
public:
	XMFLOAT2						m_xmf2TexCoord;

public:
	Textured2DUIVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); }
	Textured2DUIVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord; }
	Textured2DUIVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord; }
	~Textured2DUIVertex() { }
};


class CDiffusedTexturedVertex : public CDiffusedVertex
{
public:
	XMFLOAT2						m_xmf2TexCoord;

public:
	CDiffusedTexturedVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); }
	CDiffusedTexturedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse, XMFLOAT2 xmf2TexCoord) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse; m_xmf2TexCoord = xmf2TexCoord; }
	CDiffusedTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) { m_xmf3Position = xmf3Position; m_xmf4Diffuse = xmf4Diffuse; m_xmf2TexCoord = xmf2TexCoord; }
	~CDiffusedTexturedVertex() { }
};

class MeshComponent : public ComponentBase
{
public:
	MeshComponent() {};
	~MeshComponent() {};

	virtual D3D12_VERTEX_BUFFER_VIEW	GetVertexBufferView();
	virtual D3D12_INDEX_BUFFER_VIEW		GetIndexBufferView();
	virtual D3D12_INDEX_BUFFER_VIEW		GetSubsetIndexBufferView(int nSubSet);
	virtual ID3D12Resource* GetIndexBufferResource();
	virtual UINT						GetSlot();
	virtual UINT						GetOffset();
	virtual UINT						GetVertices();
	virtual UINT						GetStride();
	virtual UINT						GetIndices();
	virtual UINT						GetSubsetIndices(int nSubSet);
	virtual D3D12_PRIMITIVE_TOPOLOGY	GetPrimitveTopology();
	virtual UINT						GetSubMeshes();
	virtual void						BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fWidth, float fHeight, float fDepth) {};
	virtual void						ReleaseUploadBuffers();
	virtual void						UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) {};
	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
	int CheckRayIntersection(XMFLOAT3& xmRayPosition, XMFLOAT3& xmRayDirection, float* pfNearHitDistance);
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	UINT GetType() { return(m_nType); }
	char							m_pstrMeshName[64] = { 0 };
protected:
	UINT							m_nType = 0x00;
	int								m_nReferences = 0;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;//최종출력에서 메쉬의 삼각형을 나타내는 프리미티브
	UINT							m_nSlot = 0;
	UINT							m_nOffset = 0;
	UINT							m_nVertices = 0;
	UINT							m_nStride = 0;

	UINT							m_nIndices = 0;
	UINT*							m_pnIndices = NULL;


	XMFLOAT3						m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3						m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMFLOAT3* m_pxmf3Positions = NULL;

	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;


	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

	//확인
	ID3D12Resource** m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource** m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW* m_pd3dSubSetIndexBufferViews = NULL;

	int								m_nSubMeshes = 0;
	int* m_pnSubSetIndices = NULL;
	UINT** m_ppnSubSetIndices = NULL;

	BoundingOrientedBox				m_xmBoundingBox;




};

class CubeMeshComponent :public MeshComponent
{
public:
	CubeMeshComponent();
	~CubeMeshComponent();
	void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fWidth, float fHeight, float fDepth);

};

class SphereMeshComponent :public MeshComponent
{
public:
	SphereMeshComponent();
	~SphereMeshComponent();
	void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fRadius, UINT nSlices, UINT nStacks);

};

class SkyBoxMeshComponent : public MeshComponent
{
public:
	SkyBoxMeshComponent();
	virtual ~SkyBoxMeshComponent();

	void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f);
};

class TrailMeshComponent : public MeshComponent
{
public:
	TrailMeshComponent();
	virtual ~TrailMeshComponent();

	void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int iMaxVertexCount);

	void		SetPosition(XMFLOAT3& xmf3Top1, XMFLOAT3& xmf3Bottom1, XMFLOAT3& xmf3Top2, XMFLOAT3& xmf3Bottom2);
	void		SetVertices(Textured2DUIVertex* pVertices, size_t iVertexCount);
private:
	UINT8* m_pBufferDataBegin = NULL;

};
class UIMeshComponent : public MeshComponent
{
public:
	UIMeshComponent();
	virtual ~UIMeshComponent();

	void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fxPosition, float fyPosition);
};
class StandardMeshComponent :public MeshComponent

{
public:
	StandardMeshComponent();
	virtual ~StandardMeshComponent();

protected:
	XMFLOAT4* m_pxmf4Colors = NULL;
	XMFLOAT3* m_pxmf3Normals = NULL;
	XMFLOAT3* m_pxmf3Tangents = NULL;
	XMFLOAT3* m_pxmf3BiTangents = NULL;

	XMFLOAT2* m_pxmf2TextureCoords0 = NULL;
	XMFLOAT2* m_pxmf2TextureCoords1 = NULL;

	ID3D12Resource* m_pd3dTextureCoord0Buffer = NULL;
	ID3D12Resource* m_pd3dTextureCoord0UploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord0BufferView;

	ID3D12Resource* m_pd3dTextureCoord1Buffer = NULL;
	ID3D12Resource* m_pd3dTextureCoord1UploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord1BufferView;

	ID3D12Resource* m_pd3dNormalBuffer = NULL;
	ID3D12Resource* m_pd3dNormalUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dNormalBufferView;

	ID3D12Resource* m_pd3dTangentBuffer = NULL;
	ID3D12Resource* m_pd3dTangentUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTangentBufferView;

	ID3D12Resource* m_pd3dBiTangentBuffer = NULL;
	ID3D12Resource* m_pd3dBiTangentUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBiTangentBufferView;

public:
	void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
};

#define VERTEXT_POSITION				0x0001
#define VERTEXT_COLOR					0x0002
#define VERTEXT_NORMAL					0x0004
#define VERTEXT_TANGENT					0x0008
#define VERTEXT_TEXTURE_COORD0			0x0010
#define VERTEXT_TEXTURE_COORD1			0x0020

#define VERTEXT_BONE_INDEX_WEIGHT		0x1000

#define VERTEXT_TEXTURE					(VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_DETAIL					(VERTEXT_POSITION | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
#define VERTEXT_NORMAL_TEXTURE			(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_NORMAL_TANGENT_TEXTURE	(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TANGENT | VERTEXT_TEXTURE_COORD0)
#define VERTEXT_NORMAL_DETAIL			(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)
#define VERTEXT_NORMAL_TANGENT__DETAIL	(VERTEXT_POSITION | VERTEXT_NORMAL | VERTEXT_TANGENT | VERTEXT_TEXTURE_COORD0 | VERTEXT_TEXTURE_COORD1)

class CharacterMeshComponent : public MeshComponent
{
public:
	CharacterMeshComponent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CharacterMeshComponent();

public:
	char		m_pstrMeshName[64] = { 0 };

protected:
	UINT		m_nType = 0x00;

	XMFLOAT4*	m_pxmf4Colors = NULL;
	XMFLOAT3*	m_pxmf3Normals = NULL;
	XMFLOAT3*	m_pxmf3Tangents = NULL;
	XMFLOAT3*	m_pxmf3BiTangents = NULL;

	XMFLOAT2*	m_pxmf2TextureCoords0 = NULL;
	XMFLOAT2*	m_pxmf2TextureCoords1 = NULL;

	ID3D12Resource* m_pd3dTextureCoord0Buffer = NULL;
	ID3D12Resource* m_pd3dTextureCoord0UploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord0BufferView;

	ID3D12Resource* m_pd3dTextureCoord1Buffer = NULL;
	ID3D12Resource* m_pd3dTextureCoord1UploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTextureCoord1BufferView;

	ID3D12Resource* m_pd3dNormalBuffer = NULL;
	ID3D12Resource* m_pd3dNormalUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dNormalBufferView;

	ID3D12Resource* m_pd3dTangentBuffer = NULL;
	ID3D12Resource* m_pd3dTangentUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dTangentBufferView;

	ID3D12Resource* m_pd3dBiTangentBuffer = NULL;
	ID3D12Resource* m_pd3dBiTangentUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBiTangentBufferView;


	int		m_nSubMeshes = 0;
	int*	m_pnSubSetIndices = NULL;
	UINT**	m_ppnSubSetIndices = NULL;

	ID3D12Resource** m_ppd3dSubSetIndexBuffers = NULL;
	ID3D12Resource** m_ppd3dSubSetIndexUploadBuffers = NULL;
	D3D12_INDEX_BUFFER_VIEW*	m_pd3dSubSetIndexBufferViews = NULL;


public:
	UINT GetType() { return(m_nType); }


	void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
};

#define SKINNED_ANIMATION_BONES		256

class CSkinnedMeshComponent : public CharacterMeshComponent
{
public:
	CSkinnedMeshComponent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CSkinnedMeshComponent();

protected:
	int								m_nBonesPerVertex = 4;

	XMINT4*			m_pxmn4BoneIndices = NULL;
	XMFLOAT4*		m_pxmf4BoneWeights = NULL;

	ID3D12Resource* m_pd3dBoneIndexBuffer = NULL;
	ID3D12Resource* m_pd3dBoneIndexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneIndexBufferView;

	ID3D12Resource* m_pd3dBoneWeightBuffer = NULL;
	ID3D12Resource* m_pd3dBoneWeightUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dBoneWeightBufferView;

public:
	int								m_nSkinningBones = 0;

	char(*m_ppstrSkinningBoneNames)[64];
	GameObject** m_ppSkinningBoneFrameCaches = NULL; //[m_nSkinningBones]

	XMFLOAT4X4* m_pxmf4x4BindPoseBoneOffsets = NULL; //Transposed

	ID3D12Resource* m_pd3dcbBindPoseBoneOffsets = NULL;
	XMFLOAT4X4* m_pcbxmf4x4MappedBindPoseBoneOffsets = NULL;

	ID3D12Resource* m_pd3dcbSkinningBoneTransforms = NULL; //Pointer Only
	XMFLOAT4X4* m_pcbxmf4x4MappedSkinningBoneTransforms = NULL;

public:
	void PrepareSkinning(GameObject* pModelRootObject);
	void LoadSkinInfoFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext);
};
