#include "stdafx.h"
#include "EffectObject.h"
#include"GameObject.h"



EffectObject::EffectObject()
{
}

EffectObject::~EffectObject()
{
}

void EffectObject::BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	for (int i = 0;i<m_pSmokeObject.size(); i++) {
		m_pSmokeObject[i] = new GameObject(UNDEF_ENTITY);
		m_pSmokeObject[i]->InsertComponent<RenderComponent>();
		m_pSmokeObject[i]->InsertComponent<UIMeshComponent>();
		m_pSmokeObject[i]->InsertComponent<EffectShaderComponent>();
		m_pSmokeObject[i]->InsertComponent<TextureComponent>();
		m_pSmokeObject[i]->SetTexture(L"MagicEffect/Smoke.dds", RESOURCE_TEXTURE2D, 3);
		m_pSmokeObject[i]->SetAddPosition( XMFLOAT3(RandF(-5, 5),RandF(-0, 10), RandF(-5, 5)));
		m_pSmokeObject[i]->SetColor(XMFLOAT4(0.0f, 1.0f, 0.2568f , 0));
		m_pSmokeObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pSmokeObject[i]->m_fTime = RandF(0, 10);
		m_pEffectObjects.push_back(m_pSmokeObject[i]);
	}
	for (int i = 0; i < m_pPointObject.size(); i++) {
		m_pPointObject[i] = new GameObject(UNDEF_ENTITY);
		m_pPointObject[i]->InsertComponent<RenderComponent>();
		m_pPointObject[i]->InsertComponent<UIMeshComponent>();
		m_pPointObject[i]->InsertComponent<EffectShaderComponent>();
		m_pPointObject[i]->InsertComponent<TextureComponent>();
		m_pPointObject[i]->SetTexture(L"MagicEffect/Point1.dds", RESOURCE_TEXTURE2D, 3);
		m_pPointObject[i]->SetAddPosition(XMFLOAT3(RandF(-5, 5), RandF(-5, 5), RandF(-5, 5)));
		m_pPointObject[i]->SetPosition(XMFLOAT3(0,0,0));
		m_pPointObject[i]->SetColor(XMFLOAT4(0.0f, 1.0f, 0.2568f, 0));
		m_pPointObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pPointObject[i]->m_fTime = RandF(0, 10);
		m_pEffectObjects.push_back(m_pPointObject[i]);
	}
	for (int i = 0; i < m_pArrowObject.size(); i++) {
		m_pArrowObject[i] = new GameObject(UNDEF_ENTITY);
		m_pArrowObject[i]->InsertComponent<RenderComponent>();
		m_pArrowObject[i]->InsertComponent<UIMeshComponent>();
		m_pArrowObject[i]->InsertComponent<EffectShaderComponent>();
		m_pArrowObject[i]->InsertComponent<TextureComponent>();
		m_pArrowObject[i]->SetTexture(L"MagicEffect/Arrow.dds", RESOURCE_TEXTURE2D, 3);
		m_pArrowObject[i]->SetPosition(XMFLOAT3(0, 40, 50));
		m_pArrowObject[i]->SetAddPosition(XMFLOAT3(RandF(-5, 5), RandF(-5, 5), RandF(-5, 5)));
		m_pArrowObject[i]->SetRowColumn(16, 8, 0.05);
		m_pArrowObject[i]->SetColor(XMFLOAT4(0.0f, 1.0f, 0.2568f, 0));
		m_pArrowObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pArrowObject[i]->m_fTime = RandF(0, 10);
		m_pEffectObjects.push_back(m_pArrowObject[i]);
	}
	for (int i = 0; i < m_pFlareObject.size(); i++) {
		m_pFlareObject[i] = new GameObject(UNDEF_ENTITY);
		m_pFlareObject[i]->InsertComponent<RenderComponent>();
		m_pFlareObject[i]->InsertComponent<UIMeshComponent>();
		m_pFlareObject[i]->InsertComponent<EffectShaderComponent>();
		m_pFlareObject[i]->InsertComponent<TextureComponent>();
		m_pFlareObject[i]->SetTexture(L"MagicEffect/Flare.dds", RESOURCE_TEXTURE2D, 3);
		m_pFlareObject[i]->SetPosition(XMFLOAT3(0, 40, 50));
		m_pFlareObject[i]->SetAddPosition(XMFLOAT3(RandF(-5, 5), RandF(-5, 5), RandF(-5, 5)));
		m_pFlareObject[i]->SetColor(XMFLOAT4(0.0f, 1.0f, 0.2568f, 0));
		m_pFlareObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pFlareObject[i]->m_fTime = RandF(0, 10);
		m_pEffectObjects.push_back(m_pFlareObject[i]);
	}
	m_pAttackObject = new GameObject(UNDEF_ENTITY);
	m_pAttackObject->InsertComponent<RenderComponent>();
	m_pAttackObject->InsertComponent<UIMeshComponent>();
	m_pAttackObject->InsertComponent<EffectShaderComponent>();
	m_pAttackObject->InsertComponent<TextureComponent>();
	m_pAttackObject->SetTexture(L"MagicEffect/Attack.dds", RESOURCE_TEXTURE2D, 3);
	m_pAttackObject->SetPosition(XMFLOAT3(0, 0, 0));
	m_pAttackObject->SetAddPosition(XMFLOAT3(RandF(-5, 5), RandF(-5, 5), RandF(-5, 5)));
	m_pAttackObject->SetColor(XMFLOAT4(0.0f, 0.0f, 0.0f, 0));
	m_pAttackObject->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pAttackObject->m_fTime = RandF(0, 10);
	m_pEffectObjects.push_back(m_pAttackObject);

	for (int i = 0; i < m_ppParticleObjects.size(); i++) {
		m_ppParticleObjects[i] = new GameObject(UNDEF_ENTITY);
		m_ppParticleObjects[i]->InsertComponent<RenderComponent>();
		m_ppParticleObjects[i]->InsertComponent<UIMeshComponent>();
		m_ppParticleObjects[i]->InsertComponent<EffectShaderComponent>();
		m_ppParticleObjects[i]->InsertComponent<TextureComponent>();
		m_ppParticleObjects[i]->SetTexture(L"MagicEffect/Point1.dds", RESOURCE_TEXTURE2D, 3);
		m_ppParticleObjects[i]->SetAddPosition(XMFLOAT3(RandF(-5, 5), RandF(-5, 5), RandF(-5, 5)));
		m_ppParticleObjects[i]->SetPosition(XMFLOAT3(0, 0, 0));
		m_ppParticleObjects[i]->SetColor(XMFLOAT4(0.0f, 1.0f, 0.2568f, 0));
		m_ppParticleObjects[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppParticleObjects[i]->m_fTime = RandF(0, 10);
		m_ppParticleObjects[i]->m_xmf3RamdomDirection = XMFLOAT3(RandF(-1, 1), RandF(-1, 1), RandF(-1, 1));
		m_pEffectObjects.push_back(m_ppParticleObjects[i]);
	}
}

void EffectObject::RenderEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	SortEffect();//카메라 거리별로 Sort후 렌더

	for (int i = 0; i < m_pEffectObjects.size(); i++) {
		m_pEffectObjects[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	}

}

void EffectObject::AnimateEffect(CCamera* pCamera,XMFLOAT3 xm3position,float ftimeelapsed, float fTime)
{
	for (int i = 0; i < m_pSmokeObject.size(); i++) {
		if (m_pSmokeObject[i]->m_fTime < 10) {
			m_pSmokeObject[i]->m_fTime = m_pSmokeObject[i]->m_fTime + ftimeelapsed * 6;
		}
		else {
			m_pSmokeObject[i]->m_fTime = 0;
			m_pSmokeObject[i]->SetAddPosition(XMFLOAT3(RandF(-8, 8), RandF(4, 20), RandF(-8, 8)));
		}
		
		m_pSmokeObject[i]->SetLookAt(pCamera->GetPosition());
		m_pSmokeObject[i]->SetScale(2);
		m_pSmokeObject[i]->Rotate(0, 180, fTime*5);
		m_pSmokeObject[i]->SetPosition(XMFLOAT3(
			xm3position.x + m_pSmokeObject[i]->GetAddPosition().x,
			xm3position.y + m_pSmokeObject[i]->GetAddPosition().y ,
			xm3position.z + m_pSmokeObject[i]->GetAddPosition().z));
	}
	
	for (int i = 0; i < m_pPointObject.size(); i++) {
		if (m_pPointObject[i]->m_fTime < 10) {
			m_pPointObject[i]->m_fTime = m_pPointObject[i]->m_fTime + ftimeelapsed*6;
		}
		else {
			m_pPointObject[i]->m_fTime = 0;
			m_pPointObject[i]->SetAddPosition(XMFLOAT3(RandF(-6, 6), RandF(-2, 6), RandF(-6, 6)));
		}
		m_pPointObject[i]->SetLookAt(pCamera->GetPosition());
		m_pPointObject[i]->SetScale(float((10-m_pPointObject[i]->m_fTime)/100), 2.5, sin(fTime / 10 + i));
		m_pPointObject[i]->Rotate(0, 180, 0);
		m_pPointObject[i]->SetPosition(XMFLOAT3(
			xm3position.x + m_pPointObject[i]->GetAddPosition().x,
			xm3position.y + m_pPointObject[i]->GetAddPosition().y+ m_pPointObject[i]->m_fTime+i,
			xm3position.z + m_pPointObject[i]->GetAddPosition().z));
	}
	for (int i = 0; i < m_pArrowObject.size(); i++) {
		if (m_pArrowObject[i]->m_fTime < 10) {
			m_pArrowObject[i]->m_fTime = m_pArrowObject[i]->m_fTime + ftimeelapsed * 6;
		}
		else {
			m_pArrowObject[i]->m_fTime = 0;
			m_pArrowObject[i]->SetAddPosition(XMFLOAT3(RandF(-6, 6), RandF(-2, 6), RandF(-6, 6)));
		}
		m_pArrowObject[i]->SetLookAt(pCamera->GetPosition());
		if (sin(fTime / 5 + i) > 0) {
			m_pArrowObject[i]->SetScale(sin(fTime / 5 + i));
		}
		m_pArrowObject[i]->Rotate(0, 180, -90);
		m_pArrowObject[i]->SetColor(XMFLOAT4(0.0f, 1.0f, 0.2568f, sin(fTime /5 + i) ));
		m_pArrowObject[i]->SetPosition(XMFLOAT3(
			xm3position.x + m_pArrowObject[i]->GetAddPosition().x,
			xm3position.y + m_pArrowObject[i]->GetAddPosition().y + m_pArrowObject[i]->m_fTime+i,
			xm3position.z + m_pArrowObject[i]->GetAddPosition().z));
	}
	for (int i = 0; i < m_pFlareObject.size(); i++) {
		if (m_pFlareObject[i]->m_fTime < 10) {
			m_pFlareObject[i]->m_fTime = m_pFlareObject[i]->m_fTime + ftimeelapsed * 6;
		}
		else {
			m_pFlareObject[i]->m_fTime = 0;
			m_pFlareObject[i]->SetAddPosition(XMFLOAT3(RandF(-6, 6), RandF(-2, 6), RandF(-6, 6)));
		}
		m_pFlareObject[i]->SetLookAt(pCamera->GetPosition());
		m_pFlareObject[i]->SetScale(sin(fTime/5+i));
		m_pFlareObject[i]->Rotate(0, 180, 0);
		m_pFlareObject[i]->SetPosition(XMFLOAT3(
			xm3position.x + m_pFlareObject[i]->GetAddPosition().x,
			xm3position.y + m_pFlareObject[i]->GetAddPosition().y + m_pFlareObject[i]->m_fTime + i,
			xm3position.z + m_pFlareObject[i]->GetAddPosition().z));
	}
	m_pAttackObject->SetLookAt(pCamera->GetPosition());
	m_pAttackObject->SetScale(1);
	m_pAttackObject->Rotate(0, 180, 0);
	m_pAttackObject->SetPosition(XMFLOAT3(xm3position.x,xm3position.y+10,xm3position.z ));

	for (int i = 0; i < m_pEffectObjects.size(); i++) {
		m_pEffectObjects[i]->CalculateDistance(pCamera->GetPosition());
	}
	Particle();
}

void EffectObject::SortEffect()
{
	std::sort(m_pEffectObjects.begin(), m_pEffectObjects.end(), CompareGameObjects);
}

void EffectObject::Particle()
{
	for (int i = 0; i < m_ppParticleObjects.size(); i++) {
	
		
		//빌드 오브젝트
		m_ppParticleObjects[i]->MoveVelocity(m_ppParticleObjects[i]->m_xmf3RamdomDirection, 0.01);
	}
	
	


}


bool CompareGameObjects(const GameObject* obj1, const GameObject* obj2)
{
	return obj1->GetDistance() > obj2->GetDistance();
}

LightningEffectObject::LightningEffectObject()
{
}

LightningEffectObject::~LightningEffectObject()
{
}

void LightningEffectObject::BuildEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	for (int i = 0; i < m_pLightningSpriteObject.size(); i++) {
		m_pLightningSpriteObject[i] = new GameObject(UNDEF_ENTITY);
		m_pLightningSpriteObject[i]->InsertComponent<RenderComponent>();
		m_pLightningSpriteObject[i]->InsertComponent<UIMeshComponent>();
		m_pLightningSpriteObject[i]->InsertComponent<MultiSpriteShaderComponent>();
		m_pLightningSpriteObject[i]->InsertComponent<TextureComponent>();
		m_pLightningSpriteObject[i]->SetTexture(L"MagicEffect/Lightning_2x2.dds", RESOURCE_TEXTURE2D, 3);
		m_pLightningSpriteObject[i]->SetPosition(XMFLOAT3(0, 40, 100+10*i));
		m_pLightningSpriteObject[i]->SetScale(7.0f, 12.0f, 7.0f);
		m_pLightningSpriteObject[i]->SetRowColumn(2.0f, 2.0f, 0.06f+0.5*i);
		m_pLightningSpriteObject[i]->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_pLightningSpriteObject[i]->m_fTime = RandF(0, 10);
		m_pEffectObjects.push_back(m_pLightningSpriteObject[i]);
	}
}


void LightningEffectObject::AnimateEffect(CCamera* pCamera, XMFLOAT3 xm3position, float ftimeelapsed, float fTime)
{
	for (int i = 0; i < m_pLightningSpriteObject.size(); i++) {

		m_pLightningSpriteObject[i]->SetLookAt(pCamera->GetPosition());
		
		m_pLightningSpriteObject[i]->SetScale(7.0f, 20.0f, 7.0f);
		m_pLightningSpriteObject[i]->Rotate(0, 180, 0);
		m_pLightningSpriteObject[i]->SetPosition(XMFLOAT3(
			xm3position.x ,
			xm3position.y +42,
			xm3position.z ));
		m_pLightningSpriteObject[i]->AnimateRowColumn(ftimeelapsed);
	}
	for (int i = 0; i < m_pEffectObjects.size(); i++) {
		m_pEffectObjects[i]->CalculateDistance(pCamera->GetPosition());
	}
}
