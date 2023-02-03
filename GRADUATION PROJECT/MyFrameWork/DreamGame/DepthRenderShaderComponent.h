#pragma once
#include"stdafx.h"
#include"ShaderComponent.h"
#include"TextureComponent.h"
#include "Light.h"
#include "Camera.h"

#define MAP_SIZE 2056.0f//삭제 
#define PLAYER_MAP_RANGE 200.0f//삭제
#define MINI_MAP_SIZE 0.25f;//삭제
#define ARROW_CENTER -0.75f//삭제

#define _WITH_RASTERIZER_DEPTH_BIAS
class DepthRenderShaderComponent : public ShaderComponent
{
public:
	DepthRenderShaderComponent();
	virtual ~DepthRenderShaderComponent();

	//virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(int nPipelineState) { return(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE); }
	//virtual UINT GetNumRenderTargets(int nPipelineState) { return(1); }
	virtual DXGI_FORMAT GetRTVFormat(int nPipelineState, int nRenderTarget) { return(DXGI_FORMAT_R32_FLOAT); }
	virtual DXGI_FORMAT GetDSVFormat(int nPipelineState) { return(DXGI_FORMAT_D32_FLOAT); }

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob, int nPipelineState);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
	virtual void ReleaseObjects();

	void PrepareShadowMap(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature, int nPipelineState = 0);

protected:
	//깊이 저장 텍스쳐
	TextureComponent* m_pDepthTexture = NULL;

	//조명 위치에서 깊이정보 저장하기위한 카메라 4개
	CCamera* m_ppDepthRenderCameras[MAX_DEPTH_TEXTURES];

	ID3D12DescriptorHeap* m_pd3dRtvDescriptorHeap = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_pd3dRtvCPUDescriptorHandles[MAX_DEPTH_TEXTURES];

	ID3D12DescriptorHeap* m_pd3dDsvDescriptorHeap = NULL;
	ID3D12Resource* m_pd3dDepthBuffer = NULL;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_d3dDsvDescriptorCPUHandle;

	//프로젝션을 텍스쳐로 바꾸는 행렬
	XMMATRIX						m_xmProjectionToTexture;

public:
	TextureComponent* GetDepthTexture() { return(m_pDepthTexture); }
	ID3D12Resource* GetDepthTextureResource(UINT nIndex) { return(m_pDepthTexture->GetTexture(nIndex)); }

public:
	//CStandardObjectsShader* m_pObjectsShader = NULL;
	//CPlayer* m_pPlayer = NULL;

protected:
	LIGHT* m_pLights = NULL;

	TOLIGHTSPACES* m_pToLightSpaces;

	ID3D12Resource* m_pd3dcbToLightSpaces = NULL;
	TOLIGHTSPACES* m_pcbMappedToLightSpaces = NULL;
};
