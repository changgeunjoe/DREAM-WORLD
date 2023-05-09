#include "stdafx.h"
#include"MeshComponent.h"



CubeMeshComponent::CubeMeshComponent()
{
}
CubeMeshComponent::~CubeMeshComponent()
{
}

void CubeMeshComponent::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, float fWidth, float fHeight, float fDepth)
{
	m_nVertices = 36;
	m_nStride = sizeof(TextureComponentdVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	TextureComponentdVertex pVertices[36];
	int i = 0;

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, +fy, +fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, -fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(-fx, -fy, +fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, +fz), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, +fy, -fz), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, +fz), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	pVertices[i++] = TextureComponentdVertex(XMFLOAT3(+fx, -fy, -fz), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f));

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

D3D12_VERTEX_BUFFER_VIEW MeshComponent::GetVertexBufferView()
{
	return m_d3dVertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW MeshComponent::GetIndexBufferView()
{
	return m_d3dIndexBufferView;
}

D3D12_INDEX_BUFFER_VIEW MeshComponent::GetSubsetIndexBufferView(int nSubSet)
{
	return m_pd3dSubSetIndexBufferViews[nSubSet];
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

UINT MeshComponent::GetSubsetIndices(int nSubSet)
{
	return m_pnSubSetIndices[nSubSet];
}

D3D12_PRIMITIVE_TOPOLOGY MeshComponent::GetPrimitveTopology()
{
	return m_d3dPrimitiveTopology;
}

UINT MeshComponent::GetSubMeshes()
{
	return m_nSubMeshes;
}

void MeshComponent::ReleaseUploadBuffers()
{

	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;

	if ((m_nSubMeshes > 0) && m_ppd3dSubSetIndexUploadBuffers)
	{
		for (int i = 0; i < m_nSubMeshes; i++)
		{
			if (m_ppd3dSubSetIndexUploadBuffers[i]) m_ppd3dSubSetIndexUploadBuffers[i]->Release();
		}
		if (m_ppd3dSubSetIndexUploadBuffers) delete[] m_ppd3dSubSetIndexUploadBuffers;
		m_ppd3dSubSetIndexUploadBuffers = NULL;
	}
}

void MeshComponent::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
}

int MeshComponent::CheckRayIntersection(XMFLOAT3& xmf3RayOrigin, XMFLOAT3& xmf3RayDirection, float* pfNearHitDistance)
{
	int nIntersections = 0;

	int nOffset = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	int nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	XMVECTOR xmRayOrigin = XMLoadFloat3(&xmf3RayOrigin);
	XMVECTOR xmRayDirection = XMLoadFloat3(&xmf3RayDirection);

	bool bIntersected = m_xmBoundingBox.Intersects(xmRayOrigin, xmRayDirection, *pfNearHitDistance);
	if (bIntersected)
	{
		float fNearHitDistance = FLT_MAX;
		for (int i = 0; i < nPrimitives; i++)
		{
			XMVECTOR v0 = XMLoadFloat3(&m_pxmf3Positions[(m_pnIndices) ? m_pnIndices[(i * nOffset) + 0] : ((i * nOffset) + 0)]);
			XMVECTOR v1 = XMLoadFloat3(&m_pxmf3Positions[(m_pnIndices) ? m_pnIndices[(i * nOffset) + 1] : ((i * nOffset) + 1)]);
			XMVECTOR v2 = XMLoadFloat3(&m_pxmf3Positions[(m_pnIndices) ? m_pnIndices[(i * nOffset) + 2] : ((i * nOffset) + 2)]);

			float fHitDistance;
			BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
			if (bIntersected)
			{
				if (fHitDistance < fNearHitDistance) *pfNearHitDistance = fNearHitDistance = fHitDistance;
				nIntersections++;
			}
		}
	}
	return(nIntersections);
}



StandardMeshComponent::StandardMeshComponent()
{
}

StandardMeshComponent::~StandardMeshComponent()
{
	if (m_pd3dTextureCoord0Buffer) m_pd3dTextureCoord0Buffer->Release();
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
	if (m_pd3dTangentBuffer) m_pd3dTangentBuffer->Release();
	if (m_pd3dBiTangentBuffer) m_pd3dBiTangentBuffer->Release();

	if (m_pxmf4Colors) delete[] m_pxmf4Colors;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;
	if (m_pxmf3Tangents) delete[] m_pxmf3Tangents;
	if (m_pxmf3BiTangents) delete[] m_pxmf3BiTangents;
	if (m_pxmf2TextureCoords0) delete[] m_pxmf2TextureCoords0;
	if (m_pxmf2TextureCoords1) delete[] m_pxmf2TextureCoords1;
}

void StandardMeshComponent::LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, FILE* pInFile)
{
	char pstrToken[64] = { '\0' };
	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	UINT nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);

	::ReadStringFromFile(pInFile, m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, pInFile);
			if (nPositions > 0)
			{
				m_nType |= VERTEXT_POSITION;
				m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile);

				m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

				m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
				m_d3dVertexBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dVertexBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{
				m_nType |= VERTEXT_COLOR;
				m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD0;
				m_pxmf2TextureCoords0 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords0, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord0Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords0, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord0UploadBuffer);

				m_d3dTextureCoord0BufferView.BufferLocation = m_pd3dTextureCoord0Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord0BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord0BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				m_nType |= VERTEXT_TEXTURE_COORD1;
				m_pxmf2TextureCoords1 = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(m_pxmf2TextureCoords1, sizeof(XMFLOAT2), nTextureCoords, pInFile);

				m_pd3dTextureCoord1Buffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords1, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoord1UploadBuffer);

				m_d3dTextureCoord1BufferView.BufferLocation = m_pd3dTextureCoord1Buffer->GetGPUVirtualAddress();
				m_d3dTextureCoord1BufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_d3dTextureCoord1BufferView.SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);
			if (nNormals > 0)
			{
				m_nType |= VERTEXT_NORMAL;
				m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);

				m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);

				m_d3dNormalBufferView.BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
				m_d3dNormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dNormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
				m_nType |= VERTEXT_TANGENT;
				m_pxmf3Tangents = new XMFLOAT3[nTangents];
				nReads = (UINT)::fread(m_pxmf3Tangents, sizeof(XMFLOAT3), nTangents, pInFile);

				m_pd3dTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Tangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTangentUploadBuffer);

				m_d3dTangentBufferView.BufferLocation = m_pd3dTangentBuffer->GetGPUVirtualAddress();
				m_d3dTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				m_pxmf3BiTangents = new XMFLOAT3[nBiTangents];
				nReads = (UINT)::fread(m_pxmf3BiTangents, sizeof(XMFLOAT3), nBiTangents, pInFile);

				m_pd3dBiTangentBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3BiTangents, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dBiTangentUploadBuffer);

				m_d3dBiTangentBufferView.BufferLocation = m_pd3dBiTangentBuffer->GetGPUVirtualAddress();
				m_d3dBiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3dBiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			nReads = (UINT)::fread(&(m_nSubMeshes), sizeof(int), 1, pInFile);
			if (m_nSubMeshes > 0)
			{
				m_pnSubSetIndices = new int[m_nSubMeshes];
				m_ppnSubSetIndices = new UINT * [m_nSubMeshes];

				m_ppd3dSubSetIndexBuffers = new ID3D12Resource * [m_nSubMeshes];
				m_ppd3dSubSetIndexUploadBuffers = new ID3D12Resource * [m_nSubMeshes];
				m_pd3dSubSetIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[m_nSubMeshes];

				for (int i = 0; i < m_nSubMeshes; i++)
				{
					::ReadStringFromFile(pInFile, pstrToken);
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile); //i
						nReads = (UINT)::fread(&(m_pnSubSetIndices[i]), sizeof(int), 1, pInFile);
						if (m_pnSubSetIndices[i] > 0)
						{
							m_ppnSubSetIndices[i] = new UINT[m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(m_ppnSubSetIndices[i], sizeof(UINT), m_pnSubSetIndices[i], pInFile);

							m_ppd3dSubSetIndexBuffers[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_ppnSubSetIndices[i], sizeof(UINT) * m_pnSubSetIndices[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_ppd3dSubSetIndexUploadBuffers[i]);

							m_pd3dSubSetIndexBufferViews[i].BufferLocation = m_ppd3dSubSetIndexBuffers[i]->GetGPUVirtualAddress();
							m_pd3dSubSetIndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
							m_pd3dSubSetIndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_pnSubSetIndices[i];
						}
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
}

void StandardMeshComponent::ReleaseUploadBuffers()
{
	MeshComponent::ReleaseUploadBuffers();

	if (m_pd3dTextureCoord0UploadBuffer) m_pd3dTextureCoord0UploadBuffer->Release();
	m_pd3dTextureCoord0UploadBuffer = NULL;

	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();
	m_pd3dNormalUploadBuffer = NULL;

	if (m_pd3dTangentUploadBuffer) m_pd3dTangentUploadBuffer->Release();
	m_pd3dTangentUploadBuffer = NULL;

	if (m_pd3dBiTangentUploadBuffer) m_pd3dBiTangentUploadBuffer->Release();
	m_pd3dBiTangentUploadBuffer = NULL;
}

void StandardMeshComponent::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	D3D12_VERTEX_BUFFER_VIEW pVertexBufferViews[5] = { m_d3dVertexBufferView, m_d3dTextureCoord0BufferView, m_d3dNormalBufferView, m_d3dTangentBufferView, m_d3dBiTangentBufferView };
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 5, pVertexBufferViews);
}

SkyBoxMeshComponent::SkyBoxMeshComponent()
{

}

SkyBoxMeshComponent::~SkyBoxMeshComponent()
{
}

void SkyBoxMeshComponent::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pxmf3Positions = new XMFLOAT3[m_nVertices];

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	// Front Quad (quads point inward)
	m_pxmf3Positions[0] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[1] = XMFLOAT3(+fx, +fx, +fx);
	m_pxmf3Positions[2] = XMFLOAT3(-fx, -fx, +fx);
	m_pxmf3Positions[3] = XMFLOAT3(-fx, -fx, +fx);
	m_pxmf3Positions[4] = XMFLOAT3(+fx, +fx, +fx);
	m_pxmf3Positions[5] = XMFLOAT3(+fx, -fx, +fx);
	// Back Quad										
	m_pxmf3Positions[6] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[7] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[8] = XMFLOAT3(+fx, -fx, -fx);
	m_pxmf3Positions[9] = XMFLOAT3(+fx, -fx, -fx);
	m_pxmf3Positions[10] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[11] = XMFLOAT3(-fx, -fx, -fx);
	// Left Quad										
	m_pxmf3Positions[12] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[13] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[14] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[15] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[16] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[17] = XMFLOAT3(-fx, -fx, +fx);
	// Right Quad										
	m_pxmf3Positions[18] = XMFLOAT3(+fx, +fx, +fx);
	m_pxmf3Positions[19] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[20] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[21] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[22] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[23] = XMFLOAT3(+fx, -fx, -fx);
	// Top Quad											
	m_pxmf3Positions[24] = XMFLOAT3(-fx, +fx, -fx);
	m_pxmf3Positions[25] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[26] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[27] = XMFLOAT3(-fx, +fx, +fx);
	m_pxmf3Positions[28] = XMFLOAT3(+fx, +fx, -fx);
	m_pxmf3Positions[29] = XMFLOAT3(+fx, +fx, +fx);
	// Bottom Quad										
	m_pxmf3Positions[30] = XMFLOAT3(-fx, -fx, +fx);
	m_pxmf3Positions[31] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[32] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[33] = XMFLOAT3(-fx, -fx, -fx);
	m_pxmf3Positions[34] = XMFLOAT3(+fx, -fx, +fx);
	m_pxmf3Positions[35] = XMFLOAT3(+fx, -fx, -fx);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dVertexBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;
}

UIMeshComponent::UIMeshComponent()
{
}

UIMeshComponent::~UIMeshComponent()
{
}

void UIMeshComponent::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fxPosition, float fyPosition)
{
	m_nVertices = 6;
	m_nStride = sizeof(Textured2DUIVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Textured2DUIVertex pVertices[6];

	float fx = (fWidth * 0.5f) + fxPosition, fy = (fHeight * 0.5f) + fyPosition, fz = 0.0f;

	pVertices[0] = Textured2DUIVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
	pVertices[1] = Textured2DUIVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
	pVertices[2] = Textured2DUIVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[3] = Textured2DUIVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
	pVertices[4] = Textured2DUIVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
	pVertices[5] = Textured2DUIVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));

	m_pxmf3Positions = new XMFLOAT3[m_nVertices];

	m_pxmf3Positions[0] = XMFLOAT3(+fx, +fy, fz);
	m_pxmf3Positions[1] = XMFLOAT3(+fx, -fy, fz);
	m_pxmf3Positions[2] = XMFLOAT3(-fx, -fy, fz);
	m_pxmf3Positions[3] = XMFLOAT3(-fx, -fy, fz);
	m_pxmf3Positions[4] = XMFLOAT3(-fx, +fy, fz);
	m_pxmf3Positions[5] = XMFLOAT3(+fx, +fy, fz);
	/*
		pVertices[0] = Textured2DUIVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));
		pVertices[1] = Textured2DUIVertex(XMFLOAT3(-fx, -fy, fz), XMFLOAT2(1.0f, 1.0f));
		pVertices[2] = Textured2DUIVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
		pVertices[3] = Textured2DUIVertex(XMFLOAT3(+fx, -fy, fz), XMFLOAT2(0.0f, 1.0f));
		pVertices[4] = Textured2DUIVertex(XMFLOAT3(+fx, +fy, fz), XMFLOAT2(0.0f, 0.0f));
		pVertices[5] = Textured2DUIVertex(XMFLOAT3(-fx, +fy, fz), XMFLOAT2(1.0f, 0.0f));

		*/


	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	m_xmBoundingBox= BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	
	m_nIndices = 12;
	m_pnIndices = new UINT[m_nIndices];
	//m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	//m_pnIndices[3] = 2; m_pnIndices[4] = 1; m_pnIndices[5] = 3;
	//m_pnIndices[6] = 0; m_pnIndices[7] = 5; m_pnIndices[8] = 4;
	//m_pnIndices[9] = 1; m_pnIndices[10] = 5; m_pnIndices[11] = 0;

	m_pnIndices[0] = 3; m_pnIndices[1] = 1; m_pnIndices[2] = 0;
	m_pnIndices[3] = 2; m_pnIndices[4] = 4; m_pnIndices[5] = 5;
	m_pnIndices[6] = 0; m_pnIndices[7] = 2; m_pnIndices[8] = 4;
	m_pnIndices[9] = 1; m_pnIndices[10] = 2; m_pnIndices[11] = 0;
}

SphereMeshComponent::SphereMeshComponent()
{
}

SphereMeshComponent::~SphereMeshComponent()
{
}

void SphereMeshComponent::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fRadius, UINT nSlices, UINT nStacks)
{
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fDeltaPhi = float(XM_PI / nStacks);
	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);
	int k = 0;

	m_nVertices = 2 + (nSlices * (nStacks - 1));

	m_pxmf3Positions = new XMFLOAT3[m_nVertices];

	m_pxmf3Positions[k] = XMFLOAT3(0.0f, +fRadius, 0.0f); k++;

	float theta_i, phi_j;
	for (UINT j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (UINT i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			m_pxmf3Positions[k] = XMFLOAT3(fRadius * sinf(phi_j) * cosf(theta_i), fRadius * cosf(phi_j), fRadius * sinf(phi_j) * sinf(theta_i));
			k++;
		}
	}
	m_pxmf3Positions[k] = XMFLOAT3(0.0f, -fRadius, 0.0f); k++;

	m_pd3dVertexBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(XMFLOAT3);
	m_d3dVertexBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	k = 0;
	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];
	for (UINT i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}
	for (UINT j = 0; j < nStacks - 2; j++)
	{
		for (UINT i = 0; i < nSlices; i++)
		{
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	for (UINT i = 0; i < nSlices; i++)
	{
		m_pnIndices[k++] = (m_nVertices - 1);
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

TrailMeshComponent::TrailMeshComponent()
{
}

TrailMeshComponent::~TrailMeshComponent()
{
}

void TrailMeshComponent::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int iMaxVertexCount)
{
	//m_nVertices = 1000;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Textured2DUIVertex pVertices[6];

	//Á¤Á¡ ÃÖ´ë°¹¼ö °öÇØÁà¾ßÇÔ
	UINT ncbElementBytes = ((sizeof(Textured2DUIVertex) * iMaxVertexCount + 255) & ~255); //256ÀÇ ¹è¼ö
	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
	m_pd3dVertexBuffer->Map(0, NULL, (void**)&m_pBufferDataBegin);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = sizeof(Textured2DUIVertex);
	m_d3dVertexBufferView.SizeInBytes = sizeof(Textured2DUIVertex) * m_nVertices;

}

void TrailMeshComponent::SetPosition(XMFLOAT3& xmf3Top1, XMFLOAT3& xmf3Bottom1, XMFLOAT3& xmf3Top2, XMFLOAT3& xmf3Bottom2)
{
}

void TrailMeshComponent::SetVertices(Textured2DUIVertex* pVertices, size_t iVertexCount)
{
	m_nVertices = (int)iVertexCount;

	m_d3dVertexBufferView.SizeInBytes = sizeof(Textured2DUIVertex) * m_nVertices;
	memcpy(m_pBufferDataBegin, pVertices, sizeof(Textured2DUIVertex) * iVertexCount);
}
