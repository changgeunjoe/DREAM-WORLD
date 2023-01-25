#pragma once
#include"stdafx.h"
#include"ShaderComponent.h"

#define MAP_SIZE 2056.0f//昏力 
#define PLAYER_MAP_RANGE 200.0f//昏力
#define MINI_MAP_SIZE 0.25f;//昏力
#define ARROW_CENTER -0.75f//昏力

class CShadowShaderComponent:public ShaderComponent
{
public:
	CShadowShaderComponent();
	virtual ~CShadowShaderComponent();

	void BuildShadow(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nWndClientWidth, UINT nWndClientHeight);
	void Release();
	void CreateDsvDescriptorHeaps(ID3D12Device* pd3dDevice);
	void CreateResource(ID3D12Device* pd3dDevice, UINT nWndClientWidth, UINT nWndClientHeight);
	void CreateDepthStencilView(ID3D12Device* pd3dDevice);

	void UpdateTransform();
	void ClearDepthStencilView(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void SwapResource(ID3D12GraphicsCommandList* pd3dCommandList, bool bType);
	//void SetPlayer(CPlayer* pPlayer);
	SHADOW_INFO* GetShadowInfo();
	ID3D12Resource* GetShadowMap()
	{
		return m_pd3dDepthStencilBuffer;
	}
private:
	ID3D12DescriptorHeap* m_pd3dDSVDescriptorHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dDsvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dDsvGPUDescriptorStartHandle;

	ID3D12Resource* m_pd3dDepthStencilBuffer = NULL;
	BoundingSphere mSceneBounds;

	SHADOW_INFO m_ShdowInfo;

//	CPlayer* m_pPlayer = NULL;

	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	XMFLOAT3 m_xmf3Lightdir = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	float m_fLightRotationAngle = 0.0f;
};

