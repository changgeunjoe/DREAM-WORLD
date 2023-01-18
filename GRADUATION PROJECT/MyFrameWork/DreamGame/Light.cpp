#include "Light.h"


CLight::CLight()
{
	m_xmf4x4viewMatrix = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
}


CLight::CLight(const CLight& other)
{

}


CLight::~CLight()
{
}


void CLight::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_xmf4ambientColor = XMFLOAT4(red, green, blue, alpha);
}


void CLight::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_xmf4diffuseColor = XMFLOAT4(red, green, blue, alpha);
}

void CLight::SetPosition(float x, float y, float z)
{
	m_xmf3position = XMFLOAT3(x, y, z);
}


void CLight::SetLookAt(float x, float y, float z)
{
	m_xmf3lookAt = XMFLOAT3(x, y, z);
}


XMFLOAT4 CLight::GetAmbientColor()
{
	return m_xmf4ambientColor;
}


XMFLOAT4 CLight::GetDiffuseColor()
{
	return m_xmf4diffuseColor;
}


XMFLOAT3 CLight::GetPosition()
{
	return m_xmf3position;
}


void CLight::GenerateViewMatrix()
{
	// 위쪽을 가리키는 벡터를 설정합니다.
	XMFLOAT3 upVector = XMFLOAT3(0.0f, 1.0f, 0.0f);


	// 세 벡터로부터 뷰 행렬을 만듭니다.
	m_xmf4x4viewMatrix = Matrix4x4::LookAtLH(m_xmf3position, m_xmf3lookAt, upVector);
}


void CLight::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	// 정사각형 광원에 대한 시야 및 화면 비율을 설정합니다.
	float fieldOfView = (float)XM_PI / 2.0f;
	float screenAspect = 1.0f;

	// 빛의 투영 행렬을 만듭니다.
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
}


XMFLOAT4X4 CLight::GetViewMatrix()
{
	return m_xmf4x4viewMatrix;
}


XMFLOAT4X4 CLight::GetProjectionMatrix()
{
	return m_xmf4x4Projection;
}

void CLight::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_LIGHT_INFO) + 255) & ~255); //256의 배수
	ResourceLight = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	ResourceLight->Map(0, NULL, (void**)&MappedLight);
}

void CLight::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	XMStoreFloat4x4(&MappedLight->m_xmf4x4LightView, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4viewMatrix)));
	XMStoreFloat4x4(&MappedLight->m_xmf4x4LightProjection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	::memcpy(&MappedLight->m_xmf4ambientColor, &m_xmf4ambientColor, sizeof(XMFLOAT4));
	::memcpy(&MappedLight->m_xmf4diffuseColor, &m_xmf4diffuseColor, sizeof(XMFLOAT4));
	::memcpy(&MappedLight->m_xmf3lightPosition, &m_xmf3position, sizeof(XMFLOAT3));
	::memcpy(&MappedLight->m_xmf3lightPosition, &m_xmf3lookAt, sizeof(XMFLOAT3));
	::memcpy(&MappedLight->m_xmfpadding, &m_xmfpadding, sizeof(float));
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = ResourceLight->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(1, d3dGpuVirtualAddress);
}
