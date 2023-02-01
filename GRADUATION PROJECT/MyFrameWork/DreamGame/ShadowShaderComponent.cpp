#include "stdafx.h"
#include "ShadowShaderComponent.h"
CShadowShaderComponent::CShadowShaderComponent()
{
	m_ShdowInfo.m_xmf3LightPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_ShdowInfo.m_xmf4x4ShadowProjection = Matrix4x4::Identity();
	m_ShdowInfo.m_xmf4x4ShadowView = Matrix4x4::Identity();
	m_ShdowInfo.m_xmf4x4ShadowTransform = Matrix4x4::Identity();

	mSceneBounds.Center = XMFLOAT3((MAP_SIZE / 2), 0.0f, (MAP_SIZE / 2));
	mSceneBounds.Radius = sqrtf((MAP_SIZE / 2) * (MAP_SIZE / 2) + (MAP_SIZE / 2) * (MAP_SIZE / 2));
}
//void CShadowShader::SetPlayer(CPlayer* pPlayer)
//{
//	m_pPlayer = pPlayer;
//}
CShadowShaderComponent::~CShadowShaderComponent()
{
	Release();
	m_pd3dDSVDescriptorHeap->Release();
}
void CShadowShaderComponent::BuildShadow(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nWndClientWidth, UINT nWndClientHeight)
{
	CreateDsvDescriptorHeaps(pd3dDevice);
	CreateResource(pd3dDevice, nWndClientWidth, nWndClientHeight);
	CreateDepthStencilView(pd3dDevice);
}
void CShadowShaderComponent::Release()
{
	if (m_pd3dDepthStencilBuffer)
		m_pd3dDepthStencilBuffer->Release();
}
void CShadowShaderComponent::CreateDsvDescriptorHeaps(ID3D12Device* pd3dDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDsvDescriptorHeapDesc;
	::ZeroMemory(&d3dDsvDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	d3dDsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDsvDescriptorHeapDesc.NodeMask = 0;
	d3dDsvDescriptorHeapDesc.NumDescriptors = 1;
	HRESULT hResult = pd3dDevice->CreateDescriptorHeap(&d3dDsvDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dDSVDescriptorHeap);

	m_d3dDsvCPUDescriptorStartHandle = m_pd3dDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dDsvGPUDescriptorStartHandle = m_pd3dDSVDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}
void CShadowShaderComponent::CreateDepthStencilView(ID3D12Device* pd3dDevice)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;

	pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, m_d3dDsvCPUDescriptorStartHandle);
}

void CShadowShaderComponent::CreateResource(ID3D12Device* pd3dDevice, UINT nWndClientWidth, UINT nWndClientHeight)
{
	m_d3dViewport = { 0.0f, 0.0f, (float)nWndClientWidth, (float)nWndClientHeight, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, (int)nWndClientWidth, (int)nWndClientHeight };

	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nWndClientWidth;
	d3dResourceDesc.Height = nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	HRESULT hresult = pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc
		, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &d3dClearValue, __uuidof(ID3D12Resource), (void**)&m_pd3dDepthStencilBuffer);
}
void CShadowShaderComponent::UpdateTransform()
{
	XMMATRIX R = XMMatrixRotationY(m_fLightRotationAngle);

	XMVECTOR RotationlightDir = XMLoadFloat3(&m_xmf3Lightdir);;
	RotationlightDir = XMVector3TransformNormal(RotationlightDir, R);
	XMStoreFloat3(&m_xmf3Lightdir, RotationlightDir);

	XMVECTOR lightDir = XMLoadFloat3(&m_xmf3Lightdir);
	XMVECTOR lightPos = -2.0f * mSceneBounds.Radius * lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	XMStoreFloat3(&m_ShdowInfo.m_xmf3LightPosition, lightPos);

	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

	float l = sphereCenterLS.x - mSceneBounds.Radius;
	float b = sphereCenterLS.y - mSceneBounds.Radius;
	float n = sphereCenterLS.z - mSceneBounds.Radius;
	float r = sphereCenterLS.x + mSceneBounds.Radius;
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;

	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = lightView * lightProj * T;
	XMStoreFloat4x4(&m_ShdowInfo.m_xmf4x4ShadowView, lightView);
	XMStoreFloat4x4(&m_ShdowInfo.m_xmf4x4ShadowProjection, lightProj);
	XMStoreFloat4x4(&m_ShdowInfo.m_xmf4x4ShadowTransform, S);
}
SHADOW_INFO* CShadowShaderComponent::GetShadowInfo()
{
	return &m_ShdowInfo;
}
void CShadowShaderComponent::SwapResource(ID3D12GraphicsCommandList* pd3dCommandList, bool bType)
{
	if (bType)
	{
		pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dDepthStencilBuffer,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	}
	else
	{
		pd3dCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pd3dDepthStencilBuffer,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}
}
void CShadowShaderComponent::ClearDepthStencilView(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->ClearDepthStencilView(m_d3dDsvCPUDescriptorStartHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}
void CShadowShaderComponent::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->OMSetRenderTargets(0, NULL, false, &m_d3dDsvCPUDescriptorStartHandle);

	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}