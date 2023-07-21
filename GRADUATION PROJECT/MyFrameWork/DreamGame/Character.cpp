#include "stdafx.h"
#include "Character.h"
#include "Animation.h"
#include "GameFramework.h"
#include "GameobjectManager.h"
#include "EffectObject.h"
#include "Network/NetworkHelper.h"
#include "Network/Logic/Logic.h"
#include "Network/MapData/MapData.h"

extern Logic g_Logic;
extern NetworkHelper g_NetworkHelper;
extern bool GameEnd;
extern MapData g_bossMapData;
extern CGameFramework gGameFramework;

Character::Character() : GameObject(UNDEF_ENTITY)
{
	//m_xmf3RotateAxis = XMFLOAT3(0.0f, -90.0f, 0.0f);
	m_skillDuration = { std::chrono::seconds(0), std::chrono::seconds(0) };
	m_skillCoolTime = { std::chrono::seconds(0), std::chrono::seconds(0) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now(), std::chrono::high_resolution_clock::now() };
}

Character::~Character()
{
}

void Character::RbuttonClicked(float fTimeElapsed)
{
}

void Character::RbuttonUp(const XMFLOAT3& CameraAxis)
{
	if (m_pCamera)
	{
		m_pCamera->ReInitCamrea();
		SetCamera(m_pCamera);
		m_pCamera->Rotate(CameraAxis.x, CameraAxis.y, CameraAxis.z);
	}
	m_iRButtionCount = 0;
}

void Character::Reset()
{
	m_fHp = m_fMaxHp;
	SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pSkinnedAnimationController->ResetTrack();
	m_bMoveState = false;
	SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_bLButtonClicked = false;
	m_bRButtonClicked = false;
	m_projectilesLookY = 0;
	for (int i = 0; i < m_pProjectiles.size(); ++i)
		if (m_pProjectiles[i]) m_pProjectiles[i]->m_bActive = false;

	if (m_pSkinnedAnimationController->m_CurrentAnimation != CharacterAnimation::CA_IDLE)
	{
		m_pSkinnedAnimationController->m_CurrentAnimation = CharacterAnimation::CA_IDLE;
		m_pSkinnedAnimationController->SetTrackEnable(CharacterAnimation::CA_IDLE, 2);
	}
	if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_IDLE)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_IDLE;
		m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_IDLE;
		m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
	}
	if (m_pCamera)
	{
		m_pCamera = nullptr;
	}
}

void Character::MoveObject()
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(m_xmf3RotateAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);

	XMFLOAT3 xmf3Rev = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float fRotateAngle = -1.0f;

	DIRECTION tempDir = m_currentDirection;

	if (m_bRButtonClicked)
	{
		SetLook(xmf3Look);
		return;
	}

	if (tempDir != DIRECTION::IDLE)
	{
		if (((tempDir & DIRECTION::LEFT) == DIRECTION::LEFT) &&
			((tempDir & DIRECTION::RIGHT) == DIRECTION::RIGHT))
		{
			tempDir = (DIRECTION)(tempDir ^ DIRECTION::LEFT);
			tempDir = (DIRECTION)(tempDir ^ DIRECTION::RIGHT);
		}
		if (((tempDir & DIRECTION::FRONT) == DIRECTION::FRONT) &&
			((tempDir & DIRECTION::BACK) == DIRECTION::BACK))
		{
			tempDir = (DIRECTION)(tempDir ^ DIRECTION::FRONT);
			tempDir = (DIRECTION)(tempDir ^ DIRECTION::BACK);
		}
		switch (tempDir)
		{
		case DIRECTION::FRONT:						fRotateAngle = 0.0f;	break;
		case DIRECTION::LEFT | DIRECTION::FRONT:	fRotateAngle = 45.0f;	break;
		case DIRECTION::LEFT:						fRotateAngle = 90.0f;	break;
		case DIRECTION::BACK | DIRECTION::LEFT:		fRotateAngle = 135.0f;	break;
		case DIRECTION::BACK:						fRotateAngle = 180.0f;	break;
		case DIRECTION::RIGHT | DIRECTION::BACK:	fRotateAngle = 225.0f;	break;
		case DIRECTION::RIGHT:						fRotateAngle = 270.0f;	break;
		case DIRECTION::FRONT | DIRECTION::RIGHT:	fRotateAngle = 315.0f;	break;
		default:
			return;
		}

		fRotateAngle = fRotateAngle * (3.14159265359 / 180.0f);
		xmf3Rev.x = xmf3Look.x * cos(fRotateAngle) - xmf3Look.z * sin(fRotateAngle);
		xmf3Rev.z = xmf3Look.x * sin(fRotateAngle) + xmf3Look.z * cos(fRotateAngle);
		xmf3Rev = Vector3::Normalize(xmf3Rev);
	}

	if ((xmf3Rev.x || xmf3Rev.y || xmf3Rev.z))
	{
		SetLook(xmf3Rev);
	}
}

bool Character::CheckAnimationEnd(int nAnimation)
{
	return m_pSkinnedAnimationController->m_pAnimationTracks[nAnimation].m_bAnimationEnd;
}

void Character::InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos)
{
	XMFLOAT3 playerInterpolationVector = Vector3::Subtract(recvPos, GetPosition());
	float playerPosDistance = Vector3::Length(playerInterpolationVector);
	chrono::utc_clock::time_point playerCurrentUTC_Time = chrono::utc_clock::now();
	double durationTime = chrono::duration_cast<chrono::microseconds>(playerCurrentUTC_Time - recvTime).count();
	durationTime = (double)durationTime / 1000.0f;//microseconds to mill
	durationTime = (double)durationTime / 1000.0f;//milliseconds to sec
	float playerInterpolationDistance = playerPosDistance - (float)durationTime * 50.0f;//length - v*t

	if (playerPosDistance < DBL_EPSILON) {
		m_interpolationDistance = 0.0f;
	}
	else if (abs(playerInterpolationDistance) > 5.0f) {
		//cout << "client playerPos: " << playerPos.x << ", " << playerPos.z << endl;
		//cout << "server playerPos: " << recvPacket->userState[i].pos.x << ", " << recvPacket->userState[i].pos.z << endl;
		SetPosition(recvPos);
	}
	else if (abs(playerInterpolationDistance) < 3.0f) {
		m_interpolationDistance = 0.0f;
	}
	else {
		m_interpolationDistance = abs(playerInterpolationDistance);
		m_interpolationVector = Vector3::Normalize(playerInterpolationVector);
	}
}

Warrior::Warrior() : Character()
{
	m_fHp = 400.0f;
	m_fMaxHp = 400.0f;
	m_fSpeed = 100.0f;
	m_fDamage = 100.0f;
}

Warrior::~Warrior()
{
}

void Warrior::Attack(float fSpeed)
{
	if (m_bQSkillClicked)
	{
		m_bQSkillClicked = false;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_bAnimationEnd = false;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_FIRSTSKILL].m_fSpeed = 1.0f;
	}
	else if (m_bLButtonClicked)
	{
		if (m_pCamera)
			g_NetworkHelper.SendMeleeAttackPacket(GetLook());
	}
}

void Warrior::RbuttonClicked(float fTimeElapsed)
{

}

void Warrior::Move(float fDistance)
{
	DIRECTION tempDir = m_currentDirection;
	if (((tempDir & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tempDir & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::LEFT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::RIGHT);
	}
	if (((tempDir & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tempDir & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::FRONT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::BACK);
	}

	if (!m_bRButtonClicked)
	{
		switch (tempDir)
		{
		case DIRECTION::FRONT:
		case DIRECTION::FRONT | DIRECTION::RIGHT:
		case DIRECTION::RIGHT:
		case DIRECTION::BACK | DIRECTION::RIGHT:
		case DIRECTION::BACK:
		case DIRECTION::BACK | DIRECTION::LEFT:
		case DIRECTION::LEFT:
		case DIRECTION::FRONT | DIRECTION::LEFT:
			MoveForward(fDistance);
		default: break;
		}
	}
	else
	{
		//fDistance /= 3;
		switch (tempDir)
		{
		case DIRECTION::IDLE: break;
		case DIRECTION::FRONT: MoveForward(fDistance); break;
		case DIRECTION::FRONT | DIRECTION::RIGHT: MoveDiagonal(1, 1, fDistance); break;
		case DIRECTION::RIGHT: MoveStrafe(fDistance); break;
		case DIRECTION::BACK | DIRECTION::RIGHT: MoveDiagonal(-1, 1, fDistance);  break;
		case DIRECTION::BACK: MoveForward(-fDistance); break;
		case DIRECTION::BACK | DIRECTION::LEFT: MoveDiagonal(-1, -1, fDistance); break;
		case DIRECTION::LEFT: MoveStrafe(-fDistance); break;
		case DIRECTION::FRONT | DIRECTION::LEFT: MoveDiagonal(1, -1, fDistance); break;
		default: break;
		}
	}
}

void Warrior::Animate(float fTimeElapsed)
{
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	if (GameEnd)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_VICTORY)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	bool RButtonAnimation = false;
	bool UpperLock = false;
	switch (AfterAnimation.first)
	{
	case CharacterAnimation::CA_FIRSTSKILL:
	{
		if (CheckAnimationEnd(CA_FIRSTSKILL) == false)
		{
			UpperLock = true;
		}
		break;
	}
	case CharacterAnimation::CA_ATTACK:
	{
		if (CheckAnimationEnd(CA_ATTACK) == false)
		{
			UpperLock = true;
		}
		break;
	}
	}
	if (CheckAnimationEnd(CA_ATTACK) || CheckAnimationEnd(CA_FIRSTSKILL))
	{
		Attack();
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
	}

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bQSkillClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_FIRSTSKILL;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else
	{
		if (m_bQSkillClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_FIRSTSKILL;
			AfterAnimation.second = CharacterAnimation::CA_FIRSTSKILL;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
		}
		else if (!UpperLock)					// IDLE
		{
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}

	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimations)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}

	MoveObject();
	Move(50 * fTimeElapsed);
	GameObject::Animate(fTimeElapsed);
}

Archer::Archer() : Character()
{
	m_fHp = 250.0f;
	m_fMaxHp = 250.0f;
	m_fSpeed = 150.0f;
	m_fDamage = 80.0f;
	m_CameraLook = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Archer::~Archer()
{
	for (auto& i : m_pProjectiles)
	{
		if (i != nullptr)
			delete i;
	}
}

void Archer::Attack(float fSpeed)
{
}

void Archer::SetArrow(Projectile* pArrow)
{
	if (m_nProjectiles < MAX_ARROW)
	{
		m_pProjectiles[m_nProjectiles] = static_cast<Projectile*>(pArrow);
		m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 7.5f, 0.0f)));
		m_pProjectiles[m_nProjectiles]->SetLook(GetObjectLook());
		m_pProjectiles[m_nProjectiles]->m_bActive = false;
		m_nProjectiles++;
	}
}

void Archer::RbuttonClicked(float fTimeElapsed)
{
}

void Archer::RbuttonUp(const XMFLOAT3& CameraAxis)
{
}

void Archer::Move(float fDistance)
{
	//fDistance *= m_fSpeed;
	DIRECTION tempDir = m_currentDirection;
	if (((tempDir & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tempDir & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::LEFT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::RIGHT);
	}
	if (((tempDir & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tempDir & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::FRONT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::BACK);
	}

	//if (!m_bRButtonClicked)
	//{
	//	switch (tempDir)
	//	{
	//	case DIRECTION::FRONT:
	//	case DIRECTION::FRONT | DIRECTION::RIGHT:
	//	case DIRECTION::RIGHT:
	//	case DIRECTION::BACK | DIRECTION::RIGHT:
	//	case DIRECTION::BACK:
	//	case DIRECTION::BACK | DIRECTION::LEFT:
	//	case DIRECTION::LEFT:
	//	case DIRECTION::FRONT | DIRECTION::LEFT:
	//		MoveForward(fDistance);
	//	default: break;
	//	}
	//}
	//else
	//{
		//fDistance /= 3;
	switch (tempDir)
	{
	case DIRECTION::IDLE: break;
	case DIRECTION::FRONT: MoveForward(fDistance); break;
	case DIRECTION::FRONT | DIRECTION::RIGHT: MoveDiagonal(1, 1, fDistance); break;
	case DIRECTION::RIGHT: MoveStrafe(fDistance); break;
	case DIRECTION::BACK | DIRECTION::RIGHT: MoveDiagonal(-1, 1, fDistance);  break;
	case DIRECTION::BACK: MoveForward(-fDistance); break;
	case DIRECTION::BACK | DIRECTION::LEFT: MoveDiagonal(-1, -1, fDistance); break;
	case DIRECTION::LEFT: MoveStrafe(-fDistance); break;
	case DIRECTION::FRONT | DIRECTION::LEFT: MoveDiagonal(1, -1, fDistance); break;
	default: break;
	}
	//}
}

void Archer::Animate(float fTimeElapsed)
{
	m_fTimeElapsed = fTimeElapsed;
	if (m_bESkillClicked)
		SecondSkillDown();

	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	if (GameEnd)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_VICTORY)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}

	if (m_pSkinnedAnimationController->m_CurrentAnimations.first == CharacterAnimation::CA_ATTACK)
	{
		if (CheckAnimationEnd(CA_ATTACK))
		{
			if (m_pCamera)
			{
				ShootArrow();
			}
			m_bOnAttack = false;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
		}
		else
		{
			m_bOnAttack = true;
		}
	}

	// 현재 재생될 애니메이션 선택
	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bRButtonClicked || m_bLButtonClicked || m_bQSkillClicked || m_bESkillClicked)
		{
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else
		{// 그냥 움직이기
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else
	{
		if (m_bRButtonClicked || m_bLButtonClicked || m_bQSkillClicked || m_bESkillClicked)
		{
			if (!m_bOnAttack) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
		}
		else if (!m_bOnAttack)
		{// IDLE
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}

	// 재생될 애니메이션이 기존 애니메이션과 다르면 변경
	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimations)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}

	MoveObject();
	Move(50 * fTimeElapsed);
	GameObject::Animate(fTimeElapsed);
}

void Archer::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void Archer::MoveObject()
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(m_xmf3RotateAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	SetLook(xmf3Look);
}

void Archer::FirstSkillDown()
{
	m_bQSkillClicked = true;
	// 실제 화살 발사는 애니메이션이 끝난 후 ShootArrow에서 설정
}

void Archer::FirstSkillUp()
{
}

void Archer::SecondSkillDown()
{
	if (!(m_pSkinnedAnimationController->m_CurrentAnimations.first == CharacterAnimation::CA_ATTACK &&
		CheckAnimationEnd(CA_ATTACK) == false)
		|| m_bESkillClicked == true)
	{
		m_bESkillClicked = true;

		if (m_iRButtionCount == 0)
		{
			// 줌인을 위해서 애니메이션 타입 및 속도 변경
			m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType = ANIMATION_TYPE_HALF;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
			m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fSpeed = 0.3f;
		}
		if (m_iRButtionCount < 50)
		{
			// 카메라 줌인 효과
			if (m_pCamera)
			{
				XMFLOAT3 LookVector = XMFLOAT3(m_pCamera->GetLookVector().x, 0.0f, m_pCamera->GetLookVector().z);
				XMFLOAT3 CameraOffset = m_pCamera->GetOffset();
				LookVector = Vector3::ScalarProduct(LookVector, m_fTimeElapsed * 10.0f, false);
				CameraOffset = Vector3::Add(CameraOffset, LookVector);
				m_pCamera->SetOffset(CameraOffset);
			}
			m_iRButtionCount++;
		}
	}
}

void Archer::SecondSkillUp(const XMFLOAT3& CameraAxis)
{
	if (m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType == ANIMATION_TYPE_HALF)
	{
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_nType = ANIMATION_TYPE_LOOP;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fSpeed = 1.0f;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = true;
		m_CameraLook = CameraAxis;
	}
	if (m_pCamera == nullptr)
		m_bESkillClicked = false;
}

void Archer::ShootArrow()
{
	m_nProjectiles = (m_nProjectiles < MAX_ARROW) ? m_nProjectiles : m_nProjectiles % MAX_ARROW;
	if (m_bQSkillClicked == true)
	{// 3개의 화살을 발사하는 스킬 실행
		for (int i = 0; i < 3; ++i)
		{
			m_nProjectiles = (m_nProjectiles < MAX_ARROW) ? m_nProjectiles : m_nProjectiles % MAX_ARROW;
			XMFLOAT3 objectLook = GetObjectLook();
			XMFLOAT3 objectRight = GetRight();
			XMFLOAT3 objPosition = GetPosition();
			// XMFLOAT3 targetPos = Vector3::Add(objPosition, objectLook, 150.0f);
			//if (m_pCamera)
			//{
			//	m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(objectLook.x, m_pCamera->GetLookVector().y, objectLook.z);
			//	m_pProjectiles[m_nProjectiles]->m_xmf3direction.x = objectLook.x * cos((1 - i) * (3.14 / 6)) - objectLook.z * sin((1 - i) * (3.14 / 6));
			//	m_pProjectiles[m_nProjectiles]->m_xmf3direction.z = objectLook.x * sin((1 - i) * (3.14 / 6)) + objectLook.z * cos((1 - i) * (3.14 / 6));
			//	m_pProjectiles[m_nProjectiles]->m_xmf3direction.y = m_pCamera->GetLookVector().y + 0.5f;
			//}
			//else
			//{
			//	m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, -sin(m_xmf3RotateAxis.y * 3.141592 / 180.0f), GetObjectLook().z);
			//	m_pProjectiles[m_nProjectiles]->m_xmf3direction.x = objectLook.x * cos((1 - i) * (3.14 / 6)) - objectLook.z * sin((1 - i) * (3.14 / 6));
			//	m_pProjectiles[m_nProjectiles]->m_xmf3direction.z = objectLook.x * sin((1 - i) * (3.14 / 6)) + objectLook.z * cos((1 - i) * (3.14 / 6));
			//	m_pProjectiles[m_nProjectiles]->m_xmf3direction.y = -sin(m_xmf3RotateAxis.y * 3.141592 / 180.0f) + 0.5f;
			//}
			objPosition.y = 6.0f + (i % 2) * 4.0f;
			objPosition = Vector3::Add(objPosition, objectRight, (1 - i) * 4.0f);
			objPosition = Vector3::Add(objPosition, objectLook, 1.0f);

			m_pProjectiles[m_nProjectiles]->m_xmf3direction = Vector3::Normalize(objectLook);
			m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = objPosition;
			m_pProjectiles[m_nProjectiles]->SetPosition(objPosition);
			m_pProjectiles[m_nProjectiles]->m_fSpeed = 250.0f;
			m_pProjectiles[m_nProjectiles]->m_bActive = true;
			// static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_xmf3TargetPos = targetPos;
			// static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_ArrowType = 1;
			// static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_ArrowPos = 0.0f;
			g_NetworkHelper.SendArrowAttackPacket(objPosition, m_pProjectiles[m_nProjectiles]->m_xmf3direction, 
					m_pProjectiles[m_nProjectiles]->m_fSpeed);

			m_nProjectiles++;
		}

		m_bQSkillClicked = false;
	}
	else if (m_bESkillClicked == true)
	{
		// 1개의 화살을 발사 && 데미지 및 스피드 수정
		// 스피드에 비례하여 데미지 설정	// 서버로는 스피드만 전송
		float chargingTime = m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition;
		float fullTime = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fLength * 0.7f;
		float arrowSpeed = pow((chargingTime / fullTime), 2);

		if (m_pCamera)
			m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, m_pCamera->GetLookVector().y, GetObjectLook().z);
		else
			m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, -sin(m_projectilesLookY * 3.141592 / 180.0f), GetObjectLook().z);

		m_pProjectiles[m_nProjectiles]->m_xmf3direction = Vector3::Normalize(m_pProjectiles[m_nProjectiles]->m_xmf3direction);
		m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = XMFLOAT3(GetPosition().x, GetPosition().y + 4.0f, GetPosition().z);
		m_pProjectiles[m_nProjectiles]->SetPosition(m_pProjectiles[m_nProjectiles]->m_xmf3startPosition);
		m_pProjectiles[m_nProjectiles]->m_fSpeed = (chargingTime / fullTime > 0.5f) ? arrowSpeed * 400.0f : -1.0f;
		//static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_xmf3TargetPos = XMFLOAT3(0.0f, -1.0f, 0.0f);
		//static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_ArrowType = 0;
		// 최대 화살 속도 400

		// 추후 화살 데미지 설정 추가
		if (m_pProjectiles[m_nProjectiles]->m_fSpeed > 10)
		{
			m_pProjectiles[m_nProjectiles]->m_bActive = true;
			g_NetworkHelper.SendArrowAttackPacket(m_pProjectiles[m_nProjectiles]->m_xmf3startPosition, 
				m_pProjectiles[m_nProjectiles]->m_xmf3direction, m_pProjectiles[m_nProjectiles]->m_fSpeed);
			m_nProjectiles++;
		}

		m_bESkillClicked = false;

		Character::RbuttonUp(m_CameraLook);
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
	else if (m_bOnAttack == true)
	{// 1개의 화살을 발사하는 기본 공격 실행
		if (m_pCamera)
			m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, m_pCamera->GetLookVector().y, GetObjectLook().z);
		else
			m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, -sin(m_projectilesLookY * 3.141592 / 180.0f), GetObjectLook().z);

		m_pProjectiles[m_nProjectiles]->m_xmf3direction = Vector3::Normalize(m_pProjectiles[m_nProjectiles]->m_xmf3direction);
		m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = XMFLOAT3(GetPosition().x, GetPosition().y + 4.0f, GetPosition().z);
		m_pProjectiles[m_nProjectiles]->SetPosition(m_pProjectiles[m_nProjectiles]->m_xmf3startPosition);
		m_pProjectiles[m_nProjectiles]->m_fSpeed = 150.0f;
		m_pProjectiles[m_nProjectiles]->m_bActive = true;
		//static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_xmf3TargetPos = XMFLOAT3(0.0f, -1.0f, 0.0f);
		//static_cast<Arrow*>(m_pProjectiles[m_nProjectiles])->m_ArrowType = 0;
		g_NetworkHelper.SendArrowAttackPacket(m_pProjectiles[m_nProjectiles]->m_xmf3startPosition, 
			m_pProjectiles[m_nProjectiles]->m_xmf3direction, m_pProjectiles[m_nProjectiles]->m_fSpeed);
		m_nProjectiles++;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
}

void Archer::ShootArrow(const XMFLOAT3& xmf3StartPos, const XMFLOAT3& xmf3Direction, const float fSpeed)
{
	// 플레이어 캐릭터가 아닐 때
	m_nProjectiles = (m_nProjectiles < MAX_ARROW) ? m_nProjectiles : m_nProjectiles % MAX_ARROW;
	if (m_bQSkillClicked == true)
	{
		for (int i = 0; i < 3; ++i)
		{
			m_nProjectiles = (m_nProjectiles < MAX_ARROW) ? m_nProjectiles : m_nProjectiles % MAX_ARROW;
			m_pProjectiles[m_nProjectiles]->m_xmf3direction = xmf3Direction;
			m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = xmf3StartPos;
			m_pProjectiles[m_nProjectiles]->SetPosition(xmf3StartPos);
			m_pProjectiles[m_nProjectiles]->m_fSpeed = 250.0f;
			m_pProjectiles[m_nProjectiles]->m_bActive = true;
			m_nProjectiles++;
		}
		m_bQSkillClicked = false;
	}
	else if (m_bESkillClicked == true)
	{
		m_pProjectiles[m_nProjectiles]->m_xmf3direction = xmf3Direction;
		m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = xmf3StartPos;
		m_pProjectiles[m_nProjectiles]->SetPosition(xmf3StartPos);
		m_pProjectiles[m_nProjectiles]->m_fSpeed = fSpeed;
		m_pProjectiles[m_nProjectiles]->m_bActive = true;

		m_nProjectiles++;
		m_bESkillClicked = false;

		Character::RbuttonUp();
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
	else if (m_bOnAttack == true)
	{
		m_pProjectiles[m_nProjectiles]->m_xmf3direction = xmf3Direction;
		m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = xmf3StartPos;
		m_pProjectiles[m_nProjectiles]->SetPosition(xmf3StartPos);
		m_pProjectiles[m_nProjectiles]->m_fSpeed = fSpeed;
		m_pProjectiles[m_nProjectiles]->m_bActive = true;
		m_nProjectiles++;
		m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[CharacterAnimation::CA_ATTACK]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	}
}

//void Archer::ShadowRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender, ShaderComponent* pShaderComponent)
//{
//	GameObject::ShadowRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender, pShaderComponent);
//}

Tanker::Tanker() : Character()
{
	m_fHp = 600.0f;
	m_fMaxHp = 600.0f;
	m_fSpeed = 75.0f;
	m_fDamage = 50.0f;

	m_skillCoolTime = { std::chrono::seconds(15), std::chrono::seconds(0) };
	m_skillDuration = { std::chrono::seconds(5), std::chrono::seconds(0) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0], std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
}

Tanker::~Tanker()
{

}

void Tanker::Attack(float fSpeed)
{
	if (m_pCamera)
		g_NetworkHelper.SendMeleeAttackPacket(GetLook());
}

void Tanker::RbuttonClicked(float fTimeElapsed)
{
	if (m_pCamera && !m_iRButtionCount)
	{
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 90.0f);
		m_iRButtionCount++;
	}
}

void Tanker::RbuttonUp(const XMFLOAT3& CameraAxis)
{
	Character::RbuttonUp(CameraAxis);
}

void Tanker::FirstSkillDown()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]);
	if (m_skillCoolTime[0] > duration) return;
	m_bQSkillClicked = true;
	m_skillInputTime[0] = std::chrono::high_resolution_clock::now();

#ifdef LOCAL_TASK
	StartEffect(0);
#endif
}

void Tanker::Move(float fDistance)
{
	//fDistance *= m_fSpeed;
	DIRECTION tempDir = m_currentDirection;
	if (((tempDir & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tempDir & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::LEFT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::RIGHT);
	}
	if (((tempDir & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tempDir & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::FRONT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::BACK);
	}

	if (!m_bRButtonClicked)
	{
		switch (tempDir)
		{
		case DIRECTION::FRONT:
		case DIRECTION::FRONT | DIRECTION::RIGHT:
		case DIRECTION::RIGHT:
		case DIRECTION::BACK | DIRECTION::RIGHT:
		case DIRECTION::BACK:
		case DIRECTION::BACK | DIRECTION::LEFT:
		case DIRECTION::LEFT:
		case DIRECTION::FRONT | DIRECTION::LEFT:
			MoveForward(fDistance);
		default: break;
		}
	}
	else
	{
		//fDistance /= 3;
		switch (tempDir)
		{
		case DIRECTION::IDLE: break;
		case DIRECTION::FRONT: MoveForward(fDistance); break;
		case DIRECTION::FRONT | DIRECTION::RIGHT: MoveDiagonal(1, 1, fDistance); break;
		case DIRECTION::RIGHT: MoveStrafe(fDistance); break;
		case DIRECTION::BACK | DIRECTION::RIGHT: MoveDiagonal(-1, 1, fDistance);  break;
		case DIRECTION::BACK: MoveForward(-fDistance); break;
		case DIRECTION::BACK | DIRECTION::LEFT: MoveDiagonal(-1, -1, fDistance); break;
		case DIRECTION::LEFT: MoveStrafe(-fDistance); break;
		case DIRECTION::FRONT | DIRECTION::LEFT: MoveDiagonal(1, -1, fDistance); break;
		default: break;
		}
	}
}

void Tanker::Animate(float fTimeElapsed)
{
	if (m_bRButtonClicked)
		RbuttonClicked(fTimeElapsed);

	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	if (GameEnd)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_VICTORY)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	bool RButtonAnimation = false;
	bool UpperLock = false;

	switch (AfterAnimation.first)
	{
	case CharacterAnimation::CA_ATTACK:
	{
		if (CheckAnimationEnd(CA_ATTACK) == false)
		{
			UpperLock = true;
		}
		break;
	}
	}

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bRButtonClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_FIRSTSKILL;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else if (m_bQSkillClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_SECONDSKILL;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else if (!UpperLock)
	{
		if (m_bRButtonClicked)
		{
			AfterAnimation.first = CharacterAnimation::CA_FIRSTSKILL;
			AfterAnimation.second = CharacterAnimation::CA_FIRSTSKILL;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
		}
		else if (m_bQSkillClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_SECONDSKILL;
			AfterAnimation.second = CharacterAnimation::CA_SECONDSKILL;
		}
		else						// IDLE
		{
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}


	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimations)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}

	if (CheckAnimationEnd(CA_ATTACK) == true)
	{
		Attack();
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
	}
	if (CheckAnimationEnd(CA_SECONDSKILL) == true)
	{
		for (int i = 0; i < 4; ++i)
		{
			m_pProjectiles[i]->m_bActive = true;
			m_pProjectiles[i]->m_xmf3startPosition = GetPosition();
			m_pProjectiles[i]->m_xmf3startPosition.y += 16.0f;
		}
		m_bQSkillClicked = false;
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_SECONDSKILL].m_bAnimationEnd = false;
	}

	for (int i = 0; i < m_pProjectiles.size(); ++i)
	{
		if (m_pProjectiles[i])
		{
			float fProgress = static_cast<EnergyBall*>(m_pProjectiles[i])->m_fProgress;
			fProgress = std::clamp(fProgress / (1.0f - 0.3f), 0.3f, 1.0f);
			m_pProjectiles[i]->SetinitScale(fProgress, fProgress, fProgress);
			m_pProjectiles[i]->Animate(fTimeElapsed);
		}
	}

	MoveObject();
	Move(50 * fTimeElapsed);
	GameObject::Animate(fTimeElapsed);
}

void Tanker::SetSkillBall(Projectile* pBall)
{
	if (m_nProjectiles < 4)
	{
		m_pProjectiles[m_nProjectiles] = pBall;
		m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 16.0f, 0.0f)));
		m_pProjectiles[m_nProjectiles]->SetLook(GetObjectLook());
		m_pProjectiles[m_nProjectiles]->m_bActive = false;
		static_cast<EnergyBall*>(m_pProjectiles[m_nProjectiles])->SetHostRole(ROLE::TANKER);
		switch (m_nProjectiles)
		{
		case 0: static_cast<EnergyBall*>(m_pProjectiles[0])->SetTarget(ROLE::WARRIOR); break;
		case 1: static_cast<EnergyBall*>(m_pProjectiles[1])->SetTarget(ROLE::ARCHER); break;
		case 2: static_cast<EnergyBall*>(m_pProjectiles[2])->SetTarget(ROLE::PRIEST); break;
		case 3: static_cast<EnergyBall*>(m_pProjectiles[3])->SetTarget(ROLE::TANKER); break;
		default: break;
		}
		m_nProjectiles++;
	}
}

void Tanker::StartEffect(int nSkillNum)
{
	// 모든 플레이어 버프 표시 활성화
	// HP 바에 실드 부분 추가
	EffectObject** shieldEffects = gGameFramework.GetScene()->GetObjectManager()->GetShieldEffectArr();
	for (int i = 0; i < 4; ++i)
	{
		shieldEffects[i]->m_bActive = true;
	}
}

void Tanker::EndEffect(int nSkillNum)
{
	// 이펙트 렌더 비활성화
	// 실드(HP바) UI 비활성화
	EffectObject** shieldEffects = gGameFramework.GetScene()->GetObjectManager()->GetShieldEffectArr();
	for (int i = 0; i < 4; ++i)
	{
		shieldEffects[i]->m_bActive = false;
	}
}

Priest::Priest() : Character()
{
	m_fHp = 480.0f;
	m_fMaxHp = 480.0f;
	m_fSpeed = 150.0f;
	m_fDamage = 80.0f;

	m_skillCoolTime = { std::chrono::seconds(15), std::chrono::seconds(7) };
	m_skillDuration = { std::chrono::seconds(9), std::chrono::seconds(3) };
	m_skillInputTime = { std::chrono::high_resolution_clock::now() - m_skillCoolTime[0], std::chrono::high_resolution_clock::now() - m_skillCoolTime[1] };
}

Priest::~Priest()
{
}

void Priest::RbuttonClicked(float fTimeElapsed)
{
}

void Priest::Move(float fDistance)
{
	//fDistance *= m_fSpeed;
	DIRECTION tempDir = m_currentDirection;
	if (((tempDir & DIRECTION::LEFT) == DIRECTION::LEFT) &&
		((tempDir & DIRECTION::RIGHT) == DIRECTION::RIGHT))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::LEFT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::RIGHT);
	}
	if (((tempDir & DIRECTION::FRONT) == DIRECTION::FRONT) &&
		((tempDir & DIRECTION::BACK) == DIRECTION::BACK))
	{
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::FRONT);
		tempDir = (DIRECTION)(tempDir ^ DIRECTION::BACK);
	}

	//if (!m_bRButtonClicked)
	//{
	//	switch (tempDir)
	//	{
	//	case DIRECTION::FRONT:
	//	case DIRECTION::FRONT | DIRECTION::RIGHT:
	//	case DIRECTION::RIGHT:
	//	case DIRECTION::BACK | DIRECTION::RIGHT:
	//	case DIRECTION::BACK:
	//	case DIRECTION::BACK | DIRECTION::LEFT:
	//	case DIRECTION::LEFT:
	//	case DIRECTION::FRONT | DIRECTION::LEFT:
	//		MoveForward(fDistance);
	//	default: break;
	//	}
	//}
	//else
	//{
		//fDistance /= 3;
	switch (tempDir)
	{
	case DIRECTION::IDLE: break;
	case DIRECTION::FRONT: MoveForward(fDistance); break;
	case DIRECTION::FRONT | DIRECTION::RIGHT: MoveDiagonal(1, 1, fDistance); break;
	case DIRECTION::RIGHT: MoveStrafe(fDistance); break;
	case DIRECTION::BACK | DIRECTION::RIGHT: MoveDiagonal(-1, 1, fDistance);  break;
	case DIRECTION::BACK: MoveForward(-fDistance); break;
	case DIRECTION::BACK | DIRECTION::LEFT: MoveDiagonal(-1, -1, fDistance); break;
	case DIRECTION::LEFT: MoveStrafe(-fDistance); break;
	case DIRECTION::FRONT | DIRECTION::LEFT: MoveDiagonal(1, -1, fDistance); break;
	default: break;
	}
	//}
}

void Priest::Animate(float fTimeElapsed)
{
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}
	if (GameEnd)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CharacterAnimation::CA_VICTORY)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations.first = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->m_CurrentAnimations.second = CharacterAnimation::CA_VICTORY;
			m_pSkinnedAnimationController->SetTrackEnable(m_pSkinnedAnimationController->m_CurrentAnimations);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}

	if (m_pHealRange->m_bActive)
	{
		m_fHealTime += fTimeElapsed;
		if (m_fHealTime > 10.0f)
		{
			m_fHealTime = 0.0f;
			m_pHealRange->m_bActive = false;
			m_bQSkillClicked = false;
		}
	}

	pair<CharacterAnimation, CharacterAnimation> AfterAnimation = m_pSkinnedAnimationController->m_CurrentAnimations;
	bool UpperLock = false;
	switch (AfterAnimation.first)
	{
	case CharacterAnimation::CA_ATTACK:
	{
		if (CheckAnimationEnd(CA_ATTACK) == false)
			UpperLock = true;
		break;
	}
	}

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bRButtonClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
		else						// 그냥 움직이기
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_MOVE;
			AfterAnimation.second = CharacterAnimation::CA_MOVE;
		}
	}
	else
	{
		if (m_bRButtonClicked)
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;
		}
		else if (m_bLButtonClicked)	// 공격
		{
			if (!UpperLock) AfterAnimation.first = CharacterAnimation::CA_ATTACK;
			AfterAnimation.second = CharacterAnimation::CA_ATTACK;

		}
		else if (!UpperLock)					// IDLE
		{
			AfterAnimation.first = CharacterAnimation::CA_IDLE;
			AfterAnimation.second = CharacterAnimation::CA_IDLE;
		}
	}

	if (AfterAnimation != m_pSkinnedAnimationController->m_CurrentAnimations)
	{
		m_pSkinnedAnimationController->m_CurrentAnimations = AfterAnimation;
		m_pSkinnedAnimationController->SetTrackEnable(AfterAnimation);
	}

	if (CheckAnimationEnd(CA_ATTACK) == true)
	{
		if(m_pCamera) Attack();
		m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_bAnimationEnd = false;
	}

	if (m_pHealRange->m_bActive)
	{
		UpdateEffect();
	}

	for (int i = 0; i < m_pProjectiles.size(); ++i)
		if(m_pProjectiles[i]) m_pProjectiles[i]->Animate(fTimeElapsed);

	MoveObject();
	Move(50 * fTimeElapsed);
	GameObject::Animate(fTimeElapsed);
}

void Priest::RbuttonUp(const XMFLOAT3& CameraAxis)
{
}

void Priest::Attack(float fSpeed)
{
	m_nProjectiles = (m_nProjectiles < 10) ? m_nProjectiles : m_nProjectiles % 10;
	m_pProjectiles[m_nProjectiles]->m_xmf3direction = XMFLOAT3(GetObjectLook().x, m_pCamera->GetLookVector().y, GetObjectLook().z);
	m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = XMFLOAT3(GetPosition().x, GetPosition().y + 4.0f, GetPosition().z);
	m_pProjectiles[m_nProjectiles]->SetPosition(m_pProjectiles[m_nProjectiles]->m_xmf3startPosition);
	m_pProjectiles[m_nProjectiles]->m_fSpeed = fSpeed;
	m_pProjectiles[m_nProjectiles]->m_bActive = true;
	g_NetworkHelper.SendBallAttackPacket(m_pProjectiles[m_nProjectiles]->m_xmf3startPosition, m_pProjectiles[m_nProjectiles]->m_xmf3direction, fSpeed);
	m_nProjectiles++;
}

void Priest::Attack(const XMFLOAT3& xmf3StartPos, const XMFLOAT3& xmf3Direction, const float fSpeed)
{
	m_nProjectiles = (m_nProjectiles < 10) ? m_nProjectiles : m_nProjectiles % 10;
	m_pProjectiles[m_nProjectiles]->m_xmf3direction = xmf3Direction;
	m_pProjectiles[m_nProjectiles]->m_xmf3startPosition = xmf3StartPos;
	m_pProjectiles[m_nProjectiles]->SetPosition(xmf3StartPos);
	m_pProjectiles[m_nProjectiles]->m_fSpeed = fSpeed;
	m_pProjectiles[m_nProjectiles]->m_bActive = true;
	m_nProjectiles++;
}

void Priest::SetEnergyBall(Projectile* pEnergyBall)
{
	if (m_nProjectiles < 10)
	{
		m_pProjectiles[m_nProjectiles] = new EnergyBall();
		m_pProjectiles[m_nProjectiles] = static_cast<Projectile*>(pEnergyBall);
		m_pProjectiles[m_nProjectiles]->SetPosition(Vector3::Add(GetPosition(), XMFLOAT3(0.0f, 7.5f, 100.0f)));
		m_pProjectiles[m_nProjectiles]->SetLook(GetObjectLook());
		m_pProjectiles[m_nProjectiles]->m_bActive = false;
		static_cast<EnergyBall*>(m_pProjectiles[m_nProjectiles])->SetHostRole(ROLE::PRIEST);
		m_nProjectiles++;
	}
}

void Priest::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	for (int i = 0; i < m_pProjectiles.size(); ++i)
		if (m_pProjectiles[i])
			m_pProjectiles[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);

	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

void Priest::MoveObject()
{
	XMFLOAT3 xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(m_xmf3RotateAxis.y));
	XMFLOAT3 xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	xmf3Look = Vector3::TransformNormal(xmf3Look, xmmtxRotate);
	SetLook(xmf3Look);
}

void Priest::FirstSkillDown()
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_skillInputTime[0]);
	if (m_skillCoolTime[0] > duration) return;
	m_bQSkillClicked = true;
	m_skillInputTime[0] = std::chrono::high_resolution_clock::now();
#ifdef LOCAL_TASK
	StartEffect(0);
#endif
}

void Priest::FirstSkillUp()
{
}

void Priest::StartEffect(int nSkillNum)
{
	m_pHealRange->m_bActive = true;
	UpdateEffect();
}

void Priest::EndEffect(int nSkillNum)
{
	m_pHealRange->m_bActive = false;
	EffectObject** HealingEffects = gGameFramework.GetScene()->GetObjectManager()->GetHealingEffectArr();
	for (int i = 0; i < 4; ++i)
	{
		HealingEffects[i]->m_bActive = false;
	}
	m_bQSkillClicked = false;
}

void Priest::UpdateEffect()
{
	array<Character*, 4> pOtherCharacter;
	pOtherCharacter[0] = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::WARRIOR);
	pOtherCharacter[1] = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::PRIEST);
	pOtherCharacter[2] = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::TANKER);
	pOtherCharacter[3] = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(ROLE::ARCHER);

	XMFLOAT3 myPos = GetPosition();

	int i = 0;
	for (auto p : pOtherCharacter)
	{
		XMFLOAT3 targetPos = p->GetPosition();
		float distance = Vector3::Length(Vector3::Subtract(targetPos, myPos));
		EffectObject** HealingEffects = gGameFramework.GetScene()->GetObjectManager()->GetHealingEffectArr();
		HealingEffects[i++]->m_bActive = static_cast<bool>(distance < 75.0f);
	}
}

//void Priest::ShadowRender(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender, ShaderComponent* pShaderComponent)
//{
//	for (int i = 0; i < m_pProjectiles.size(); ++i)
//		if (m_pProjectiles[i])
//			m_pProjectiles[i]->Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
//
//	GameObject::ShadowRender(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender, pShaderComponent);
//}

Monster::Monster() : Character()
{
	m_fHp = 2500;
	m_fMaxHp = 2500;
}

Monster::~Monster()
{
}

void Monster::Animate(float fTimeElapsed)
{
	if (m_fHp < FLT_EPSILON)
	{
		if (m_pSkinnedAnimationController->m_CurrentAnimation != BOSS_ANIMATION::BA_DIE)
		{
			m_pSkinnedAnimationController->m_CurrentAnimation = BOSS_ANIMATION::BA_DIE;
			m_pSkinnedAnimationController->SetTrackEnable(BOSS_ANIMATION::BA_DIE, 2);
		}
		GameObject::Animate(fTimeElapsed);
		return;
	}

	if (m_bMoveState)	// 움직이는 중
	{
		if (m_bOnSkill)
		{
			// 움직이는 스킬 (DASH_FORWARD)
			// 이동방식 회의 후 구현 위치
		}
		else
		{
			XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			//XMFLOAT3 desPlayerPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			if (m_roleDesPlayer != ROLE::NONE_SELECT) {
				XMFLOAT3 desPlayerPos = g_Logic.GetPostion(m_roleDesPlayer);
				XMFLOAT3 desPlayerVector = Vector3::Subtract(desPlayerPos, GetPosition());
				float playerDistance = Vector3::Length(desPlayerVector);
				desPlayerVector = Vector3::Normalize(desPlayerVector);
				m_lockBossRoute.lock();
				if (playerDistance < 120.0f && m_BossRoute.empty()) {
					m_lockBossRoute.unlock();
					/*m_lockBossRoute.lock();
					if (!m_BossRoute.empty())
						int currentNodeIdx = m_BossRoute.front();
					m_lockBossRoute.unlock();
					bool bossAndPlayerOnSameIdx = g_bossMapData.GetTriangleMesh(currentNodeIdx).IsOnTriangleMesh(desPlayerPos);*/

					//if (bossAndPlayerOnSameIdx) {
					float ChangingAngle = Vector3::Angle(desPlayerVector, GetLook());
					if (ChangingAngle > 1.6f) {
						bool OnRight = (Vector3::DotProduct(GetRight(), desPlayerVector) > 0) ? true : false;
						if (OnRight) {
							Rotate(&up, 90.0f * fTimeElapsed);
							m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
						}
						else {
							Rotate(&up, -90.0f * fTimeElapsed);
							m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
						}
					}
					if (playerDistance >= 42.0f)
						MoveForward(50 * fTimeElapsed);
					Character::Animate(fTimeElapsed);
					return;
					//m_position = Vector3::Add(m_position, Vector3::ScalarProduct(desPlayerVector, fTimeElapsed, false));//틱마다 움직임					

					//Rotate(&up, -90.0f * fTimeElapsed);
					//m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
					//임시로 했음 수정해야됨
				//}
				}
				else {
					//m_lockBossRoute.lock();
					if (!m_BossRoute.empty()) {
						int currentNodeIdx = m_BossRoute.front();
						m_lockBossRoute.unlock();
						XMFLOAT3 destinationNodeCenter = g_bossMapData.GetTriangleMesh(currentNodeIdx).GetCenter();	//노드의 위치
						m_xmf3Destination = destinationNodeCenter;
						XMFLOAT3 desNodeVector = Vector3::Subtract(m_xmf3Destination, GetPosition());
						float desNodeDistance = Vector3::Length(desNodeVector);
						desNodeVector = Vector3::Normalize(desNodeVector);
						bool OnRight = (Vector3::DotProduct(GetRight(), Vector3::Normalize(desNodeVector)) > 0) ? true : false;
						float ChangingAngle = Vector3::Angle(desNodeVector, GetLook());

						bool isOnNode = g_bossMapData.GetTriangleMesh(currentNodeIdx).IsOnTriangleMesh(m_position);

						if (desNodeDistance > 70.0f) { //목적지와 거리가 10이상 이라면
							if (desNodeDistance > 50.0f) {
								if (ChangingAngle > 40.0f) {
									if (OnRight) {
										Rotate(&up, 90.0f * fTimeElapsed);
										m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
									}
									else {
										Rotate(&up, -90.0f * fTimeElapsed);
										m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
									}
								}
								else {
									if (ChangingAngle > 1.6f) {
										if (OnRight) {
											Rotate(&up, 90.0f * fTimeElapsed);
											m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
										}
										else {
											Rotate(&up, -90.0f * fTimeElapsed);
											m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
										}
									}
									MoveForward(50 * fTimeElapsed);
								}
							}
							else {//현재 노드에 가까울때
								if (ChangingAngle > 1.6f)
								{
									if (OnRight) {
										Rotate(&up, 90.0f * fTimeElapsed);
										m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
									}
									else {
										Rotate(&up, -90.0f * fTimeElapsed);
										m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
									}
								}
								MoveForward(50 * fTimeElapsed);
							}
							//std::cout << "BossPos: " << m_position.x << "0, " << m_position.z << std::endl;
						}
						else {
							m_lockBossRoute.lock();
							//m_onIdx = m_BossRoute.front();
							m_BossRoute.pop();
							if (m_BossRoute.size() != 0) {
								DirectX::XMFLOAT3 center = g_bossMapData.GetTriangleMesh(m_BossRoute.front()).GetCenter();
								m_xmf3Destination = center;//목적지 다음 노드의 센터
							}
							m_lockBossRoute.unlock();
						}


					}
					else m_lockBossRoute.unlock();


					//else if (Vector3::Length(des) >= 14.0f) {
					//	//SetPosition(m_xmf3Destination);
					//}
				}
			}
		}
		GameObject::Animate(fTimeElapsed);
	}
}

void Monster::Move(float fDsitance)
{

}

void Monster::InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos)
{
	XMFLOAT3 playerInterpolationVector = Vector3::Subtract(recvPos, GetPosition());
	float playerPosDistance = Vector3::Length(playerInterpolationVector);
	chrono::utc_clock::time_point playerCurrentUTC_Time = chrono::utc_clock::now();
	double durationTime = chrono::duration_cast<chrono::microseconds>(playerCurrentUTC_Time - recvTime).count();
	durationTime = (double)durationTime / 1000.0f;//microseconds to mill
	durationTime = (double)durationTime / 1000.0f;//milliseconds to sec
	float playerInterpolationDistance = playerPosDistance - (float)durationTime * 50.0f;//length - v*t

	if (playerPosDistance < DBL_EPSILON) {
		m_interpolationDistance = 0.0f;
	}
	else if (abs(playerInterpolationDistance) > 50.0f) {
		//cout << "client playerPos: " << playerPos.x << ", " << playerPos.z << endl;
		//cout << "server playerPos: " << recvPacket->userState[i].pos.x << ", " << recvPacket->userState[i].pos.z << endl;
		SetPosition(recvPos);
	}
	else if (abs(playerInterpolationDistance) < 5.0f) {
		m_interpolationDistance = 0.0f;
	}
	else {
		m_interpolationDistance = abs(playerInterpolationDistance);
		m_interpolationVector = Vector3::Normalize(playerInterpolationVector);
	}
}

Projectile::Projectile(entity_id eid) : GameObject(eid)
{
	m_xmf3startPosition = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	m_xmf3direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_fSpeed = 0.0f;
	m_bActive = false;
	m_RAttack = false;
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

Arrow::Arrow() : Projectile()
{
	m_fSpeed = 150.0f;
	//m_xmf3TargetPos = XMFLOAT3(0.0f, -1.0f, 0.0f);
	//m_ArrowType = 0;
	//m_ArrowPos = 0;
	// Y값이 마우스 회전 범위 안쪽이면 일반 화살 아니면 꺾이는 화살
}

Arrow::~Arrow()
{
}

void Arrow::Animate(float fTimeElapsed)
{
	if (m_bActive == false) return;
	XMFLOAT3 xmf3CurrentPos = GetPosition();
	float moveDist = Vector3::Length(Vector3::Subtract(xmf3CurrentPos, m_xmf3startPosition));
	if (moveDist > 200.0f)
	{
		m_bActive = false;
		m_RAttack = false;
		return;
	}

	if (Vector3::Length(xmf3CurrentPos) >= PLAYER_MAX_RANGE)
	{
		m_bActive = false;
		m_RAttack = false;
	}

	//if (m_ArrowType == 1)
	//{
	//	if (m_ArrowPos > 1.0f)
	//	{
	//		m_bActive = false;
	//		return;
	//	}

	//	Monster* bossMonster = gGameFramework.GetScene()->GetObjectManager()->GetBossMonster();
	//	m_xmf3TargetPos = bossMonster->GetPosition();
	//	XMFLOAT3 controlPoint = Vector3::Add(m_xmf3startPosition, m_xmf3direction, 100.0f);
	//	XMFLOAT3 firstVec = Vector3::Subtract(controlPoint, m_xmf3startPosition);
	//	XMFLOAT3 secondVec = Vector3::Subtract(m_xmf3TargetPos, controlPoint);

	//	XMFLOAT3 tempPoint1 = Vector3::Add(Vector3::ScalarProduct(m_xmf3startPosition, (1 - m_ArrowPos), false), Vector3::ScalarProduct(controlPoint, m_ArrowPos, false));
	//	XMFLOAT3 tempPoint2 = Vector3::Add(Vector3::ScalarProduct(controlPoint, (1 - m_ArrowPos), false), Vector3::ScalarProduct(m_xmf3TargetPos, m_ArrowPos, false));
	//	XMFLOAT3 curPos = Vector3::Add(Vector3::ScalarProduct(tempPoint1, (1 - m_ArrowPos), false), Vector3::ScalarProduct(tempPoint2, m_ArrowPos, false));

	//	SetPosition(curPos);
	//	SetLook(Vector3::Subtract(tempPoint2, tempPoint1));
	//	m_ArrowPos += fTimeElapsed;	// 스플라인 곡선에서의 t의 역할
	//}
	//else
	//{
		SetLook(m_xmf3direction);
		MoveForward(fTimeElapsed * m_fSpeed);
	//}

	if (m_VisualizeSPBB) m_VisualizeSPBB->SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y, GetPosition().z));
}

void Arrow::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
}

EnergyBall::EnergyBall() : Projectile(SQUARE_ENTITY)
{
	m_fSpeed = 75.0f;
	m_fProgress = 0.0f;
}

EnergyBall::~EnergyBall()
{
}

void EnergyBall::Animate(float fTimeElapsed)
{
	if (m_bActive == false) return;
	switch (m_HostRole)
	{
	case ROLE::PRIEST:
	{
		XMFLOAT3 rev = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_Angle += 1280.0f * fTimeElapsed * (3.14 / 180.0f);
		rev.x = m_xmf3direction.x * cos(m_Angle) - m_xmf3direction.z * sin(m_Angle);
		rev.z = m_xmf3direction.x * sin(m_Angle) + m_xmf3direction.z * cos(m_Angle);
		rev = Vector3::Normalize(rev);
		SetLook(rev);
		Move(m_xmf3direction, fTimeElapsed * m_fSpeed);
		XMFLOAT3 xmf3CurrentPos = GetPosition();
		if (Vector3::Length(xmf3CurrentPos) >= PLAYER_MAX_RANGE) m_bActive = false;
		if (Vector3::Length(Vector3::Subtract(xmf3CurrentPos, m_xmf3startPosition)) > 200.0f)
		{
			m_bActive = false;
		}
	}
	break;
	case ROLE::TANKER:
	{
		if (m_fProgress > 1.0f)
		{
			m_bActive = false;
			m_fProgress = 0.0f;
			return;
		}

		Character* Target = gGameFramework.GetScene()->GetObjectManager()->GetChracterInfo(m_Target);
		XMFLOAT3 TargetPos = Target->GetPosition();
		TargetPos.y += 8.0f;
		m_xmf3direction = Vector3::Normalize(Vector3::Subtract(TargetPos, m_xmf3startPosition));
		m_xmf3direction.y = 0.6f;

		XMFLOAT3 controlPoint = Vector3::Add(m_xmf3startPosition, m_xmf3direction, 100.0f);
		XMFLOAT3 firstVec = Vector3::Subtract(controlPoint, m_xmf3startPosition);
		XMFLOAT3 secondVec = Vector3::Subtract(TargetPos, controlPoint);

		XMFLOAT3 FirstLinePoint= Vector3::Add(Vector3::ScalarProduct(m_xmf3startPosition, (1 - m_fProgress), false), Vector3::ScalarProduct(controlPoint, m_fProgress, false));
		XMFLOAT3 SecondLinePoint = Vector3::Add(Vector3::ScalarProduct(controlPoint, (1 - m_fProgress), false), Vector3::ScalarProduct(TargetPos, m_fProgress, false));
		XMFLOAT3 curPos = Vector3::Add(Vector3::ScalarProduct(FirstLinePoint, (1 - m_fProgress), false), Vector3::ScalarProduct(SecondLinePoint, m_fProgress, false));

		SetPosition(curPos);
		// SetLook(Vector3::Subtract(SecondLinePoint, FirstLinePoint));
		m_fProgress += fTimeElapsed * 0.8f;
	}
	break;
	}
	if (m_VisualizeSPBB) m_VisualizeSPBB->SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y, GetPosition().z));
}

void EnergyBall::Move(XMFLOAT3 dir, float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	xmf3Position = Vector3::Add(xmf3Position, dir, fDistance);
	GameObject::SetPosition(xmf3Position);
}

void EnergyBall::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, bool bPrerender)
{
	if (m_bActive)
	{
		GameObject::Render(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, bPrerender);
	}
}

TrailObject::TrailObject(entity_id eid) : GameObject(eid)
{

}

TrailObject::~TrailObject()
{
}

void TrailObject::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

NormalMonster::NormalMonster() : Character()
{
}

NormalMonster::~NormalMonster()
{
}

void NormalMonster::Animate(float fTimeElapsed)
{
	//	if (m_fHp < FLT_EPSILON)
	//	{
	//		if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CA_DIE)
	//		{
	//			pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_DIE, CharacterAnimation::CA_DIE };
	//			m_pSkinnedAnimationController->m_CurrentAnimations = NextAnimations;
	//			m_pSkinnedAnimationController->SetTrackEnable(NextAnimations);
	//		}
	//		return;
	//	}
	//	if (gGameFramework.GetCurrentGameState() == GAME_STATE::GS_FIRST_STAGE)
	//	{
	//		if (CheckAnimationEnd(CA_FIRSTSKILL) == true)
	//		{
	//			pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_IDLE, CharacterAnimation::CA_IDLE };
	//			m_pSkinnedAnimationController->m_CurrentAnimations = NextAnimations;
	//			m_pSkinnedAnimationController->SetTrackEnable(NextAnimations);
	//		}
	//
	//	}
	//	else
	//	{
	//		if (gGameFramework.GetCurrentGameState() == GAME_STATE::GS_SECOND_STAGE_FIRST_PHASE)
	//		{
	//			return;
	//		}
	//		else
	//		{
	//			if (m_bCanActive == false)
	//			{
	//				if (CheckAnimationEnd(CA_FIRSTSKILL) == true)
	//				{
	//					m_bCanActive = true;
	//					pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_IDLE, CharacterAnimation::CA_IDLE };
	//					m_pSkinnedAnimationController->m_CurrentAnimations = NextAnimations;
	//					m_pSkinnedAnimationController->SetTrackEnable(NextAnimations);
	//				}
	//				else
	//				{
	//					GameObject::Animate(fTimeElapsed);
	//					return;
	//				}
	//			}
	//#ifdef LOCAL_TASK
	//			//if (!m_bHaveTarget)
	//			//{
	//			//	// 플레이어 4명 포지션과 거리 계산해서 목표 설정
	//			//	XMFLOAT3 currentPos = GetPosition();
	//			//	float farDistance = FLT_MAX;
	//			//	for (auto& session : g_Logic.m_inGamePlayerSession)
	//			//	{
	//			//		if (session.m_id == -1) continue;
	//			//		XMFLOAT3 targetPos = session.m_currentPlayGameObject->GetPosition();
	//			//		XMFLOAT3 toTarget = Vector3::Subtract(targetPos, currentPos);
	//			//		float targetLength = Vector3::Length(toTarget);
	//			//		if (targetLength < farDistance)
	//			//		{
	//			//			m_bHaveTarget = true;
	//			//			m_iTargetID = session.m_id;
	//			//			farDistance = targetLength;
	//			//		}
	//			//	}
	//			//}
	//#endif
	//		}
	//	}

		/*if (m_bHaveTarget)
		{

			XMFLOAT3 MyPos = GetPosition();
			XMFLOAT3 des = XMFLOAT3(m_xmf3Destination.x - MyPos.x, 0.0f, m_xmf3Destination.z - MyPos.z);
			float distance = Vector3::Length(des);

			if (distance < 30.0f)
			{
				m_bMoveState = false;
				m_bOnAttack = true;
			}
			else if (distance >= 30.0f)
			{
				m_bOnAttack = false;
				m_bMoveState = true;
				SetLook(Vector3::Normalize(des));
				MoveForward(25 * fTimeElapsed);
			}
		}
		else
		{
			m_bMoveState = false;
			m_bOnAttack = false;
		}

		pair<CharacterAnimation, CharacterAnimation> NextAnimations = { CharacterAnimation::CA_NOTHING, CharacterAnimation::CA_NOTHING };
		if (m_bMoveState)
		{
			if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CA_MOVE)
				NextAnimations = { CharacterAnimation::CA_MOVE, CharacterAnimation::CA_MOVE };
		}
		else if (m_bOnAttack)
		{
			if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CA_ATTACK)
			{
				NextAnimations = { CharacterAnimation::CA_ATTACK, CharacterAnimation::CA_ATTACK };
				m_pSkinnedAnimationController->m_pAnimationTracks[CharacterAnimation::CA_ATTACK].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
			}
		}
		else
		{
			if (m_pSkinnedAnimationController->m_CurrentAnimations.first != CA_IDLE)
				NextAnimations = { CharacterAnimation::CA_IDLE, CharacterAnimation::CA_IDLE };
		}

		if (NextAnimations.first != CharacterAnimation::CA_NOTHING)
		{
			m_pSkinnedAnimationController->m_CurrentAnimations = NextAnimations;
			m_pSkinnedAnimationController->SetTrackEnable(NextAnimations);
		}*/

#ifdef LOCAL_TASK
		//if (!m_bHaveTarget)
		//{
		//	XMFLOAT3 currentPos = GetPosition();
		//	for (auto& session : g_Logic.m_inGamePlayerSession)
		//	{
		//		if (session.m_id == -1) continue;
		//		XMFLOAT3 targetPos = session.m_currentPlayGameObject->GetPosition();
		//		XMFLOAT3 toTarget = Vector3::Subtract(targetPos, currentPos);
		//		float targetLength = Vector3::Length(toTarget);
		//		if (targetLength < 200.0f)
		//		{
		//			m_iTargetID = session.m_id;
		//			m_bHaveTarget = true;
		//			break;
		//		}
		//	}
		//}
#else
static XMFLOAT3 up = XMFLOAT3(0, 1, 0);
	XMFLOAT3 rightVector = GetRight();
	XMFLOAT3 MyPos = GetPosition();
	XMFLOAT3 myLook = GetLook();
	XMFLOAT3 desVector = Vector3::Subtract(m_desPos, MyPos);
	//std::cout << "Look: " << myLook.x << ", " << myLook.y << ", " << myLook.z << endl;
	float desDis = Vector3::Length(desVector);
	float frontDotRes = Vector3::DotProduct(desVector, GetLook());
	desVector = Vector3::Normalize(desVector);	
	//std::cout << "desVector: " << desVector.x << ", " << desVector.y << ", " << desVector.z << endl;

	if (desDis >= 80.0f) {		
		MyPos = Vector3::Add(MyPos, m_interpolationVector, m_interpolationDistance * fTimeElapsed);
		SetPosition(MyPos);
		return;
	}

	if (desDis <= 50.0f) {//근접 했을때
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨 - 방향이 맞지 않음
			float rightDotRes = Vector3::DotProduct(desVector, rightVector);
			if (rightDotRes >= 0) {
				Rotate(&up, 90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;
				 
			}
			else {
				Rotate(&up, -90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
			}
			return;
		}

	}
	else {
		if (frontDotRes <= MONSTER_ABLE_ATTACK_COS_VALUE) {//방향 바꿔야됨
			float rightDotRes = Vector3::DotProduct(desVector, rightVector);
			if (rightDotRes >= 0) {
				Rotate(&up, 90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y += 90.0f * fTimeElapsed;

			}
			else {
				Rotate(&up, -90.0f * fTimeElapsed);
				m_xmf3rotateAngle.y -= 90.0f * fTimeElapsed;
			}
		}
		MyPos = Vector3::Add(MyPos, Vector3::ScalarProduct(GetLook(), 30.0f * fTimeElapsed, false));//틱마다 움직임
		MyPos = Vector3::Add(MyPos, m_interpolationVector, m_interpolationDistance * fTimeElapsed);
		SetPosition(MyPos);
		return;
	}
	return;

#endif

	GameObject::Animate(fTimeElapsed);
}

void NormalMonster::Move(float fDsitance)
{
}

void NormalMonster::InterpolateMove(chrono::utc_clock::time_point& recvTime, XMFLOAT3& recvPos)
{
	XMFLOAT3 playerInterpolationVector = Vector3::Subtract(recvPos, GetPosition());
	float playerPosDistance = Vector3::Length(playerInterpolationVector);
	chrono::utc_clock::time_point playerCurrentUTC_Time = chrono::utc_clock::now();
	double durationTime = chrono::duration_cast<chrono::microseconds>(playerCurrentUTC_Time - recvTime).count();
	durationTime = (double)durationTime / 1000.0f;//microseconds to mill
	durationTime = (double)durationTime / 1000.0f;//milliseconds to sec
	float playerInterpolationDistance = playerPosDistance - (float)durationTime * 30.0f;//length - v*t

	if (playerPosDistance < DBL_EPSILON) {
		m_interpolationDistance = 0.0f;
	}
	else if (abs(playerInterpolationDistance) > 30.0f) {
		//cout << "client playerPos: " << playerPos.x << ", " << playerPos.z << endl;
		//cout << "server playerPos: " << recvPacket->userState[i].pos.x << ", " << recvPacket->userState[i].pos.z << endl;
		SetPosition(recvPos);
	}
	else if (abs(playerInterpolationDistance) < 5.0f) {
		m_interpolationDistance = 0.0f;
	}
	else {
		m_interpolationDistance = abs(playerInterpolationDistance);
		m_interpolationVector = Vector3::Normalize(playerInterpolationVector);
	}
}
