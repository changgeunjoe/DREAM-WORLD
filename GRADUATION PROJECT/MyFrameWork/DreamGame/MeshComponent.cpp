#include"MeshComponent.h"



CubeMeshComponent::CubeMeshComponent()
{
}
CubeMeshComponent::~CubeMeshComponent()
{
}

void CubeMeshComponent::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature,float fWidth, float fHeight, float fDepth)
{
	m_nVertices = 36;
	m_nStride = sizeof(TextureComponentdVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	TextureComponentdVertex pVertices[36];
	int i = 0;

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

D3D12_VERTEX_BUFFER_VIEW MeshComponent::GetVertexBufferView()
{
	return m_d3dVertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW MeshComponent::GetIndexBufferView()
{
	return m_d3dIndexBufferView;
}

ID3D12Resource* MeshComponent::GetIndexBufferResource()
{
	return m_pd3dIndexBuffer;
}

UINT MeshComponent::GetSlot()
{
	return m_nSlot;
}

UINT MeshComponent::GetOffset()
{
	return m_nOffset;
}

UINT MeshComponent::GetVertices()
{
	return m_nVertices;
}

UINT MeshComponent::GetStride()
{
	return m_nStride;
}

UINT MeshComponent::GetIndices()
{
	return m_nIndices;
}

D3D12_PRIMITIVE_TOPOLOGY MeshComponent::GetPrimitveTopology()
{
	return m_d3dPrimitiveTopology;
}
