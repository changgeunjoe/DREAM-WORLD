#include "StandardShaderComponent.h"

StandardShaderComponent::StandardShaderComponent()
{
}

StandardShaderComponent::~StandardShaderComponent()
{
}

D3D12_INPUT_LAYOUT_DESC StandardShaderComponent::CreateInputLayout()
{
	return D3D12_INPUT_LAYOUT_DESC();
}

D3D12_SHADER_BYTECODE StandardShaderComponent::CreateVertexShader()
{
	return D3D12_SHADER_BYTECODE();
}

D3D12_SHADER_BYTECODE StandardShaderComponent::CreatePixelShader()
{
	return D3D12_SHADER_BYTECODE();
}
