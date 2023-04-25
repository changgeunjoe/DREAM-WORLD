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

};

