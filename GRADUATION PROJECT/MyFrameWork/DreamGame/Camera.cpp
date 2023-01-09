#include "Camera.h"

CCamera::CCamera()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
}

CCamera::~CCamera()
{

}

void CCamera::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256�� ���
	ResourceCamera = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	ResourceCamera->Map(0, NULL, (void**)&MappedCamera);
}

void CCamera::ReleaseShaderVariables()
{
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMStoreFloat4x4(&MappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&MappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = ResourceCamera->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);



//---------------------------------constantbuffer---------------------------------------
	//XMFLOAT4X4 xmf4x4View;
	//XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	////��Ʈ �Ķ���� �ε��� 1��
	//pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4View, 0);
	//XMFLOAT4X4 xmf4x4Projection;
	//XMStoreFloat4x4(&xmf4x4Projection,
	//	XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	//pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4Projection, 16);
//---------------------------------------------------------------------------------------
}

void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf4x4View = Matrix4x4::LookAtLH(xmf3Position, xmf3LookAt, xmf3Up);
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle),
		fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}
