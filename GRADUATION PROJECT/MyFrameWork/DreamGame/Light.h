#pragma once
#include "stdafx.h"


#define MAX_LIGHTS			50

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
};
struct CB_LIGHT_INFO
{
	XMFLOAT4X4 m_xmf4x4LightView;
	XMFLOAT4X4 m_xmf4x4LightProjection;
	XMFLOAT4 m_xmf4ambientColor ;
	XMFLOAT4 m_xmf4diffuseColor ;
	XMFLOAT3 m_xmf3lightPosition;
	XMFLOAT3 m_xmf3lightLookat;
	float m_xmfpadding ;
};

class CLight 
{
public:
	CLight();
	CLight(const CLight&);
	~CLight();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetPosition();

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);


private:
	XMFLOAT4X4 m_xmf4x4viewMatrix;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT4 m_xmf4ambientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 m_xmf4diffuseColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float m_xmfpadding = 0;
	
	ID3D12Resource* ResourceLight = NULL;
	CB_LIGHT_INFO* MappedLight = NULL;
};