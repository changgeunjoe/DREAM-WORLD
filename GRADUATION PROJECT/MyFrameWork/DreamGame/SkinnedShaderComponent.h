#pragma once
#include"ShaderComponent.h"
class SkinnedShaderComponent :public ShaderComponent
{

	public:
		SkinnedShaderComponent();
	virtual ~SkinnedShaderComponent();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(int nPipelineState);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int nPipelineState);
};

