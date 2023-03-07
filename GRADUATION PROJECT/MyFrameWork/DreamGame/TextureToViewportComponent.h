#pragma once
#include"stdafx.h"
#include"ShaderComponent.h"
#include"TextureComponent.h"

#include "Light.h"
class TextureToViewportComponent : public ShaderComponent
{	
public:
	TextureToViewportComponent();
	virtual ~TextureToViewportComponent();

	//virtual DXGI_FORMAT GetRTVFormat(int nPipelineState, int nRenderTarget) { return DXGI_FORMAT_UNKNOWN; }

	virtual D3D12_DEPTH_STENCIL_DESC  CreateDepthStencilState(int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature, int nPipelineState = 0);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
	virtual void ReleaseObjects();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState , ID3D12RootSignature* pd3dGraphicsRootSignature);

	bool		m_bRender = true;
protected:
	TextureComponent* m_pDepthTexture = NULL;

};

