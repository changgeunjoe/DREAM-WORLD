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
