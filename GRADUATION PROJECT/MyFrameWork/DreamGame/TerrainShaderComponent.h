#pragma once
#include "ShaderComponent.h"
class TerrainShaderComponent :
    public ShaderComponent
{
public:
	TerrainShaderComponent();
	virtual ~TerrainShaderComponent();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int nPipelineState);
};

