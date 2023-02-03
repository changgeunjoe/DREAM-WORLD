#pragma once
#include"ShaderComponent.h"
class ShadowMapShaderComponent:public ShaderComponent
{
public:
	ShadowMapShaderComponent(ShaderComponent* pObjectsShader);
	virtual ~ShadowMapShaderComponent();

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void ReleaseObjects();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState, ID3D12RootSignature* pd3dGraphicsRootSignature);

public:
	ShaderComponent* m_pObjectsShader = NULL;
	//CPlayer* m_pPlayer = NULL;
	
	//DepthRenderShader¿¡ ÀÖ´Â
	TextureComponent* m_pDepthTexture = NULL;
};

