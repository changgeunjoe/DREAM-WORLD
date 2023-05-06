#pragma once
#include"stdafx.h"
#include"ShaderComponent.h"
class UiShaderComponent :public ShaderComponent
{
public:
	UiShaderComponent();
	virtual ~UiShaderComponent();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);
	
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState);
	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int nPipelineState);



};

class BlendingUiShaderComponent :public UiShaderComponent
{
public:
	BlendingUiShaderComponent() {};
	virtual ~BlendingUiShaderComponent() {};

	virtual D3D12_BLEND_DESC CreateBlendState(int nPipelineState);
};