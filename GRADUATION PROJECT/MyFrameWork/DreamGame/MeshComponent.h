#pragma once
#include "ComponentBase.h"
#include "Vertex.h"
class MeshComponent : public ComponentBase
{
public: 
	MeshComponent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
	~MeshComponent() {};

	virtual D3D12_VERTEX_BUFFER_VIEW	GetVertexBufferView();
	virtual D3D12_INDEX_BUFFER_VIEW		GetIndexBufferView();
	virtual UINT						GetSlot();
	virtual UINT						GetOffset();
	virtual UINT						GetVertices();
	virtual UINT						GetStride();
	virtual UINT						GetIndices();

	virtual void						BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) {};
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
	CubeMeshComponent(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth);
	~CubeMeshComponent();

};




