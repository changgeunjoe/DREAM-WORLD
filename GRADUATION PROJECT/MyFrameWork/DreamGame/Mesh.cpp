#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer)m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer)m_pd3dVertexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{
	//���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	//�Ž��� ������Ƽ�� ������ �����Ѵ�.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	//�޽��� ���� ���� �並 �����Ѵ�.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	//�޽��� ���� ���� �並 �������Ѵ�.(������ ����(�Է� ������)�� �۵��ϰ� �Ѵ�).
	pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

CTriangleMesh::CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) :CMesh(pd3dDevice, pd3dCommandList)
{
	//�ﰢ�� �޽��� �����Ѵ�.
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*����(�ﰢ���� ������)�� ������ �ð���� ������� ������, ���, �Ķ������� �����Ѵ�. RGBA(Red, Green, Blue,
Alpha) 4���� �Ķ���͸� ����Ͽ� ������ ǥ���Ѵ�. �� �Ķ���ʹ� 0.0~1.0 ������ �Ǽ����� ������.*/

	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f,
		1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f,
		1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4((Colors::Blue)));

	//�ﰢ�� �޽��� ���ҽ�(���� ����)�� �����Ѵ�.

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	//���� ���� �並 �����Ѵ�.
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	//������ü�� 6���� �� ����(x-��) ����
	m_nVertices = 36;
	m_nStride = sizeof(CDiffusedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//fWidth: ������ü ����(x-��) ����, fHeight: ������ü ����(y-��) ����, fDepth: ������ü ����(z-��) ����
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	CDiffusedVertex pVertices[36];
	int i = 0;
	//���� ���� �����ʹ� �ﰢ�� ����Ʈ�̹Ƿ� 36���� ���� �����͸� �غ��Ѵ�. 
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	//�� ����(Top) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	//�� ����(Left) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	//�� ����(Right) �簢���� ���� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);
	//���� ���� �並 �����Ѵ�. 
	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshDiffused::~CCubeMeshDiffused()
{
}
