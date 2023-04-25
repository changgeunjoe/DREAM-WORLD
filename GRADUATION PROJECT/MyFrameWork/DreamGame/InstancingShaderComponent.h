#pragma once
#include "ShaderComponent.h"
class InstancingShaderComponent : public ShaderComponent
{
	public:
		InstancingShaderComponent();
		virtual ~InstancingShaderComponent();

		virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);

		virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
		virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);
		
		
		virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
		virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

		virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, vector<GameObject*>& pObjects);
		virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nPipelineState, ID3D12RootSignature* pd3dGraphicsRootSignature);

protected:
	vector<GameObject*> m_ppObjects;


};

