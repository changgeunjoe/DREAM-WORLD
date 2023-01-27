#pragma once
#include "ComponentBase.h"

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
	TextureComponentdVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); m_xmf3Normal= XMFLOAT3(0.0f, 0.0f,0.0f);}
	TextureComponentdVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord, XMFLOAT3 xmx3Normal) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord; m_xmf3Normal = xmx3Normal; }
	TextureComponentdVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord, XMFLOAT3 xmx3Normal) { m_xmf3Position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord; m_xmf3Normal = xmx3Normal;}
	~TextureComponentdVertex() { }
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
	virtual ID3D12Resource*				GetIndexBufferResource();
	virtual UINT						GetSlot();
	virtual UINT						GetOffset();
	virtual UINT						GetVertices();
	virtual UINT						GetStride();
	virtual UINT						GetIndices();
	virtual D3D12_PRIMITIVE_TOPOLOGY	GetPrimitveTopology();
	virtual void						BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fWidth, float fHeight, float fDepth) {};
	virtual void sec() {};
protected:

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;//최종출력에서 메쉬의 삼각형을 나타내는 프리미티브
	UINT							m_nSlot = 0;
	UINT							m_nOffset = 0;
	UINT							m_nVertices = 0;
	UINT							m_nStride = 0;
	UINT							m_nIndices = 0;

	BoundingOrientedBox			    m_xmOOBB;

	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;

	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;
};
class CubeMeshComponent :public MeshComponent 
{
public:
	CubeMeshComponent();
	~CubeMeshComponent();

	void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fWidth, float fHeight, float fDepth);
	void sec() {};
};






