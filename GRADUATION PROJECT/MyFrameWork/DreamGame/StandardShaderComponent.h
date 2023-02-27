#pragma once
#include"ShaderComponent.h"
class StandardShaderComponent :public ShaderComponent
{
public:
	StandardShaderComponent();
	virtual ~StandardShaderComponent();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);
};

class BoundingBoxShaderComponent : public StandardShaderComponent
{
	bool m_bEnable = true;
public:
	BoundingBoxShaderComponent() {};
	virtual ~BoundingBoxShaderComponent() {};

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int nPipelineState);
};