#pragma once
#include "CAnimationSets.h"

class SkinnedMeshComponent;
class GameObject;
class CLoadedModelInfoCompnent;
class CAnimationTrack
{
public:
	CAnimationTrack() { }
	~CAnimationTrack();

public:
	BOOL 							m_bEnable = true;
	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = -ANIMATION_CALLBACK_EPSILON;
	float 							m_fWeight = 1.0f;

	int 							m_nAnimationSet = 0;

	int 							m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong
	bool							m_bAnimationEnd = false;
	int 							m_nCallbackKeys = 0;

	CALLBACKKEY* m_pCallbackKeys = NULL;

	CAnimationCallbackHandler* m_pAnimationCallbackHandler = NULL;

public:
	void SetAnimationSet(int nAnimationSet) { m_nAnimationSet = nAnimationSet; }

	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
	void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
	void SetWeight(float fWeight) { m_fWeight = fWeight; }

	void SetPosition(float fPosition) { m_fPosition = fPosition; }
	float UpdatePosition(float fTrackPosition, float fTrackElapsedTime, const CAnimationSet* CAnimation);

	void SetCallbackKeys(int nCallbackKeys);
	void SetCallbackKey(int nKeyIndex, float fTime, void* pData);
	void SetAnimationCallbackHandler(CAnimationCallbackHandler* pCallbackHandler);

	void HandleCallback();
};

class CAnimationController
{
public:
	CAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfoCompnent* pModel);
	~CAnimationController();

public:
	float 							m_fTime = 0.0f;

	int 							m_nAnimationTracks = 0;
	CAnimationTrack* m_pAnimationTracks = NULL;

	int				m_nLowerBodyAnimation = CharacterAnimation::CA_IDLE;
	int				m_nUpperBodyAnimation = CharacterAnimation::CA_IDLE;
	pair<CharacterAnimation, CharacterAnimation> m_CurrentAnimation = { CharacterAnimation::CA_IDLE, CharacterAnimation::CA_IDLE };

	CAnimationSets* m_pAnimationSets = NULL;



	int 							m_nSkinnedMeshes = 0;
	SkinnedMeshComponent** m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

	ID3D12Resource** m_ppd3dcbSkinningBoneTransforms = NULL; //[SkinnedMeshes]
	XMFLOAT4X4** m_ppcbxmf4x4MappedSkinningBoneTransforms = NULL; //[SkinnedMeshes]

public:
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void SetTrackAnimationSet(int nAnimationSet);

	//void SetTrackEnable(int nAnimationTrack, bool bEnable);
	void SetTrackEnable(int nAnimationTrack, int nPos);
	void SetTrackEnable(pair<CharacterAnimation, CharacterAnimation> nAnimationTracks);
	void SetTrackPosition(int nAnimationTrack, float fPosition);
	void SetTrackSpeed(int nAnimationTrack, float fSpeed);
	void SetTrackWeight(int nAnimationTrack, float fWeight);

	void SetCallbackKeys(int nAnimationTrack, int nCallbackKeys);
	void SetCallbackKey(int nAnimationTrack, int nKeyIndex, float fTime, void* pData);
	void SetAnimationCallbackHandler(int nAnimationTrack, CAnimationCallbackHandler* pCallbackHandler);

	void AdvanceTime(float fElapsedTime, GameObject* pRootGameObject);

	void SetAnimationBlending(bool bAnimationBlending) { m_bAnimationBlending = bAnimationBlending; }
	bool GetAnimationBlending() { return m_bAnimationBlending; }

public:
	// bool							m_bRootMotion = false;
	GameObject* m_pModelRootObject = NULL;

	// GameObject* m_pRootMotionObject = NULL;
	// XMFLOAT3						m_xmf3FirstRootMotionPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// void SetRootMotion(bool bRootMotion) { m_bRootMotion = bRootMotion; }

	// virtual void OnRootMotion(GameObject* pRootGameObject) { }
private:
	bool							m_bAnimationBlending = false;
};
