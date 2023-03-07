#pragma once
#include"ShaderComponent.h"
class ShadowMapShaderComponent:public ShaderComponent
{
public:
	ShadowMapShaderComponent();
	virtual ~ShadowMapShaderComponent();

	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);
	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void BuildShadow(vector<GameObject*>& pObjects);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext = NULL);
	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void ReleaseObjects();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);
	virtual void ReleaseUploadBuffers();
	virtual void Animate(float fTimeElapsed);

	virtual void Render(ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState, ID3D12RootSignature* pd3dGraphicsRootSignature);

public:
	vector<GameObject*> m_ppObjects;
	//CPlayer* m_pPlayer = NULL;
	
	//DepthRenderShader¿¡ ÀÖ´Â
	TextureComponent* m_pDepthTexture = NULL;
};

