#include "stdafx.h"
#include "Player.h"
#include "Projectile.h"
#include "Character.h"
#include "EffectObject.h"
#include "GameFramework.h"
#include "GameobjectManager.h"

extern CGameFramework gGameFramework;

Projectile::Projectile(entity_id eid) : GameObject(eid)
{
	m_xmf3startPosition = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	m_xmf3direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_fSpeed = 0.0f;
	m_bActive = false;
	m_Angle = 0.0f;
}

Projectile::~Projectile()
{
}

void Projectile::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{

	m_pd3dGraphicsRootSignature = pd3dGraphicsRootSignature;
	ComponentBase* pComponent = GetComponent(component_id::RENDER_COMPONENT);
	if (pComponent != NULL)
	{
		m_pRenderComponent = static_cast<RenderComponent*>(pComponent);
	}
	ComponentBase* pSphereMeshComponent = GetComponent(component_id::SPHEREMESH_COMPONENT);
	if (pSphereMeshComponent != NULL)
	{
		m_pSphereComponent = static_cast<SphereMeshComponent*>(pSphereMeshComponent);
		m_pSphereComponent->BuildObject(pd3dDevice, pd3dCommandList, m_fBoundingSize, 20, 20);
		m_pMeshComponent = m_pSphereComponent;
	}
	ComponentBase* pSphereShaderComponent = GetComponent(component_id::SPHERE_COMPONENT);
	if (pSphereShaderComponent != NULL)
	{
		m_pShaderComponent = static_cast<SphereShaderComponent*>(pSphereShaderComponent);
		m_pShaderComponent->CreateGraphicsPipelineState(pd3dDevice, pd3dGraphicsRootSignature, 0);
		m_pShaderComponent->CreateCbvSrvDescriptorHeaps(pd3dDevice, 1, 0);
		m_pShaderComponent->CreateShaderVariables(pd3dDevice, pd3dCommandList);
		m_pShaderComponent->CreateConstantBufferViews(pd3dDevice, 1, m_pd3dcbGameObjects, ncbElementBytes);
		m_pShaderComponent->SetCbvGPUDescriptorHandlePtr(m_pShaderComponent->GetGPUCbvDescriptorStartHandle().ptr + (::gnCbvSrvDescriptorIncrementSize * nObjects));
	}
	ComponentBase* pLoadedmodelComponent = GetComponent(component_id::LOADEDMODEL_COMPONET);
	if (pLoadedmodelComponent != NULL)
	{
		CLoadedModelInfoCompnent* pModel = nullptr;
		//MaterialComponent::PrepareShaders(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, m_pd3dcbGameObjects);
		if (m_pLoadedModelComponent == nullptr)
		{
			pModel = static_cast<CLoadedModelInfoCompnent*>(pLoadedmodelComponent);
			pModel = LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList,
				pd3dGraphicsRootSignature, pszModelNames, NULL, true);//NULL ->Shader
		}
		else
		{
			pModel = m_pLoadedModelComponent;
		}
		SetChild(pModel->m_pModelRootObject, true);
	}

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void Projectile::Move(XMFLOAT3 dir, float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	float moveDist = Vector3::Length(Vector3::Subtract(xmf3Position, m_xmf3startPosition));
	if (moveDist > 250.0f)
	{
		SetActive(false);
		return;
	}

	xmf3Position = Vector3::Add(xmf3Position, dir, fDistance);
	GameObject::SetPosition(xmf3Position);

}

void Projectile::Reset()
{
	m_xmf3startPosition = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	m_xmf3direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_fSpeed = 0.0f;
	m_bActive = false;
	m_Angle = 0.0f;
}

Arrow::Arrow() : Projectile()
{
	m_fSpeed = 150.0f;
	m_iArrowType = 0;
	m_fArrowPos = 1.0f;
	m_HostRole = ROLE::ARCHER;
	//m_xmf3TargetPos = XMFLOAT3(0.0f, -1.0f, 0.0f);
	//m_ArrowPos = 0;
	// Y값이 마우스 회전 범위 안쪽이면 일반 화살 아니면 꺾이는 화살
}

void Arrow::Reset()
{
	m_fArrowPos = 1.0f;
}

Arrow::~Arrow()
{
}

void Arrow::Animate(float fTimeElapsed)
{
	XMFLOAT3 xmf3CurrentPos = GetPosition();
	if (m_iArrowType == 1)
	{
		if (m_fArrowPos > 1.0f)
		{
			m_bActive = false;
			return;
		}
		if (m_fArrowPos < FLT_EPSILON)
		{
			m_fArrowPos += fTimeElapsed;
			SetPosition(m_xmf3startPosition);
			SetLook(m_xmf3direction);
			return;
		}

		XMFLOAT3 controlPoint = Vector3::Add(m_xmf3startPosition, m_xmf3direction, 100.0f);
		XMFLOAT3 firstVec = Vector3::Subtract(controlPoint, m_xmf3startPosition);
		XMFLOAT3 secondVec = Vector3::Subtract(m_xmf3TargetPos, controlPoint);
		XMFLOAT3 FirstLinePoint = Vector3::Add(Vector3::ScalarProduct(m_xmf3startPosition, (1 - m_fArrowPos), false), Vector3::ScalarProduct(controlPoint, m_fArrowPos, false));
		XMFLOAT3 SecondLinePoint = Vector3::Add(Vector3::ScalarProduct(controlPoint, (1 - m_fArrowPos), false), Vector3::ScalarProduct(m_xmf3TargetPos, m_fArrowPos, false));
		XMFLOAT3 curPos = Vector3::Add(Vector3::ScalarProduct(FirstLinePoint, (1 - m_fArrowPos), false), Vector3::ScalarProduct(SecondLinePoint, m_fArrowPos, false));
		SetPosition(curPos);
		SetLook(Vector3::Subtract(SecondLinePoint, FirstLinePoint));
		m_fArrowPos += fTimeElapsed * m_fSpeed;
	}
	else
	{
		if (m_bActive == false) return;
		SetLook(m_xmf3direction);
		Move(m_xmf3direction, fTimeElapsed * m_fSpeed);
	}
	if (m_VisualizeSPBB) m_VisualizeSPBB->SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y, GetPosition().z));
	if (GetPosition().y < -1.0f) SetActive(false);
}

void Arrow::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void Arrow::SetActive(bool bActive)
{
	m_bActive = bActive;
	Character* archerCharacter = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::ARCHER);
	GameObject::SetPosition(archerCharacter->GetPosition());
}

EnergyBall::EnergyBall() : Projectile(SQUARE_ENTITY)
{
	m_fSpeed = 75.0f;
	m_fProgress = 0.0f;
	m_HostRole = ROLE::TANKER;
}

EnergyBall::~EnergyBall()
{
}

void EnergyBall::Animate(float fTimeElapsed)
{
	if (m_bActive == false) return;

	if (m_fProgress > 1.0f)
	{
		m_bActive = false;
		m_fProgress = 0.0f;
		EffectObject** shieldEffects = gGameFramework.GetScene()->GetObjectManager()->GetShieldEffectArr();
		for (int i = 0; i < 4; ++i)
			shieldEffects[i]->SetActive(true);
		return;
	}

	Player* Target = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(m_Target);
	XMFLOAT3 TargetPos = Target->GetPosition();
	TargetPos.y += 8.0f;
	m_xmf3direction = Vector3::Normalize(Vector3::Subtract(TargetPos, m_xmf3startPosition));
	m_xmf3direction.y = 0.6f;

	XMFLOAT3 controlPoint = Vector3::Add(m_xmf3startPosition, m_xmf3direction, 100.0f);
	XMFLOAT3 firstVec = Vector3::Subtract(controlPoint, m_xmf3startPosition);
	XMFLOAT3 secondVec = Vector3::Subtract(TargetPos, controlPoint);

	XMFLOAT3 FirstLinePoint = Vector3::Add(Vector3::ScalarProduct(m_xmf3startPosition, (1 - m_fProgress), false), Vector3::ScalarProduct(controlPoint, m_fProgress, false));
	XMFLOAT3 SecondLinePoint = Vector3::Add(Vector3::ScalarProduct(controlPoint, (1 - m_fProgress), false), Vector3::ScalarProduct(TargetPos, m_fProgress, false));
	XMFLOAT3 curPos = Vector3::Add(Vector3::ScalarProduct(FirstLinePoint, (1 - m_fProgress), false), Vector3::ScalarProduct(SecondLinePoint, m_fProgress, false));

	SetPosition(curPos);
	// SetLook(Vector3::Subtract(SecondLinePoint, FirstLinePoint));
	m_fProgress += fTimeElapsed * 0.8f;
}

void EnergyBall::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	if (m_bActive)
	{
		GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
	}
}

void EnergyBall::Reset()
{
	m_fProgress = 0.0f;
}


IceLance::IceLance() : Projectile()
{
	m_HostRole = ROLE::PRIEST;
	m_fSpeed = 100.0f;
}

IceLance::~IceLance()
{
}

void IceLance::Animate(float fTimeElapsed)
{
	if (m_bActive == false) return;
	SetLook(m_xmf3direction);
	XMFLOAT3 xmf3Direction = Vector3::Normalize(m_xmf3direction);
	Move(xmf3Direction, fTimeElapsed * m_fSpeed);
}

RockSpike::RockSpike()
{
	m_xmf3direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_fSpeed = 50.0f;

}

RockSpike::~RockSpike()
{
}

void RockSpike::Move(XMFLOAT3 dir, float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, dir, fDistance);
	if (xmf3Position.y < 0.0f)
	{
		m_bActive = false;
		return;
	}
	GameObject::SetPosition(xmf3Position);
}

void RockSpike::Animate(float fTimeElapsed)
{
	if (m_bActive == false) return;
	Move(m_xmf3direction, fTimeElapsed * m_fSpeed);
}

void RockSpike::Reset()
{
	m_xmf3direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
}
