#pragma once
#include "ComponentBase.h"
#include "MeshComponent.h"
class MeshComponent;
class RenderComponent :public ComponentBase
{
public:
	RenderComponent();
	~RenderComponent();
protected:
	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;//최종출력에서 메쉬의 삼각형을 나타내는 프리미티브
	UINT							m_nSlot = 0;
	UINT							m_nOffset = 0;
	UINT							m_nVertices = 0;
	UINT							m_nStride = 0;
	UINT							m_nIndices = 0;

	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;

	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;
public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, MeshComponent* meshcomponent);
	virtual void HandleMessage(Message message);
};

