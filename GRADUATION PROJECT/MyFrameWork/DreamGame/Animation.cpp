#include "stdafx.h"
#include "Animation.h"
#include "CLoadModelinfo.h"
#include "GameObject.h"

CAnimationTrack::~CAnimationTrack()
{
	if (m_pCallbackKeys) delete[] m_pCallbackKeys;
	if (m_pAnimationCallbackHandler) delete m_pAnimationCallbackHandler;
}

float CAnimationTrack::UpdatePosition(float fTrackPosition, float fTrackElapsedTime, const CAnimationSet* CAnimation)
{
	float fAnimationLength = CAnimation->m_fLength;
	float TrackElapsedTime = fTrackElapsedTime * m_fSpeed;
	switch (CAnimation->m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		if (m_fPosition < 0.0f)
		{
			m_fPosition = 0.0f;
			m_fProgressRate = 0.0f;
		}
		else
		{
			m_fPosition = fTrackPosition + TrackElapsedTime;
			m_fProgressRate = m_fPosition / fAnimationLength;
			if (m_fPosition > fAnimationLength)
			{
				m_bAnimationEnd = true;
				m_fPosition = -ANIMATION_CALLBACK_EPSILON;
				m_fProgressRate = 0.0f;
				return(fAnimationLength);
			}
		}
		break;
	}
	case ANIMATION_TYPE_HALF:
	{
		if (m_fPosition < 0.0f)
		{
			m_fPosition = 0.0f;
			m_fProgressRate = 0.0f;
		}
		else
		{
			m_fPosition = fTrackPosition + TrackElapsedTime;
			m_fProgressRate = m_fPosition / fAnimationLength;
			if (m_fPosition > fAnimationLength * 0.5)
			{
				m_fPosition = fAnimationLength * 0.5;
			}
		}
		break;
	}
	case ANIMATION_TYPE_ONCE:
		if (m_bAnimationEnd) return fAnimationLength;
		if (m_fPosition < 0.0f)
		{
			m_fPosition = 0.0f;
			m_fProgressRate = 0.0f;
		}
		else
		{
			m_fPosition = fTrackPosition + TrackElapsedTime;
			m_fProgressRate = m_fPosition / fAnimationLength;
			if (m_fPosition > fAnimationLength)
			{
				m_fPosition = fAnimationLength;
				m_fProgressRate = 1.0f;
				m_bAnimationEnd = true;
				return(fAnimationLength);
			}
		}
		break;
	case ANIMATION_TYPE_REVERSE:
		if (m_bAnimationEnd) return 0.0f;
		if (m_fPosition < 0.0f)
		{
			m_fPosition = fAnimationLength;
			m_fProgressRate = 0.0f;
		}
		else
		{
			m_fPosition = fTrackPosition - TrackElapsedTime;
			m_fProgressRate = (fAnimationLength - m_fPosition) / fAnimationLength;
			if (m_fPosition < 0.0f)
			{
				m_fPosition = fAnimationLength;
				m_fProgressRate = 0.0f;
				m_bAnimationEnd = true;
				return(0.0f);
			}
		}
	}

	return(m_fPosition);
}

void CAnimationTrack::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationTrack::SetCallbackKey(int nKeyIndex, float fTime, void* pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationTrack::SetAnimationCallbackHandler(CAnimationCallbackHandler* pCallbackHandler)
{
	m_pAnimationCallbackHandler = pCallbackHandler;
}

void CAnimationTrack::HandleCallback()
{
	if (m_pAnimationCallbackHandler)
	{
		for (int i = 0; i < m_nCallbackKeys; i++)
		{
			if (::IsEqual(m_pCallbackKeys[i].m_fTime, m_fPosition, ANIMATION_CALLBACK_EPSILON))
			{
				if (m_pCallbackKeys[i].m_pCallbackData)
					m_pAnimationCallbackHandler->HandleCallback(m_pCallbackKeys[i].m_pCallbackData, m_fPosition);
				break;
			}
		}
	}
}

CAnimationController::CAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfoCompnent* pModel)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks = new CAnimationTrack[m_nAnimationTracks];

	m_pAnimationSets = pModel->m_pAnimationSets;
	m_pAnimationSets->AddRef();

	m_pModelRootObject = pModel->m_pModelRootObject;

	m_nSkinnedMeshes = pModel->m_nSkinnedMeshes;
	m_ppSkinnedMeshes = new SkinnedMeshComponent * [m_nSkinnedMeshes];
	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i] = pModel->m_ppSkinnedMeshes[i];

	m_ppd3dcbSkinningBoneTransforms = new ID3D12Resource * [m_nSkinnedMeshes];
	m_ppcbxmf4x4MappedSkinningBoneTransforms = new XMFLOAT4X4 * [m_nSkinnedMeshes];

	UINT ncbElementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255); //256의 배수
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Map(0, NULL, (void**)&m_ppcbxmf4x4MappedSkinningBoneTransforms[i]);
	}
}

CAnimationController::~CAnimationController()
{
	if (m_pAnimationTracks) delete[] m_pAnimationTracks;

	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppd3dcbSkinningBoneTransforms[i]->Unmap(0, NULL);
		m_ppd3dcbSkinningBoneTransforms[i]->Release();
	}
	if (m_ppd3dcbSkinningBoneTransforms) delete[] m_ppd3dcbSkinningBoneTransforms;
	if (m_ppcbxmf4x4MappedSkinningBoneTransforms) delete[] m_ppcbxmf4x4MappedSkinningBoneTransforms;

	if (m_pAnimationSets) m_pAnimationSets->Release();

	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
}

void CAnimationController::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	for (int i = 0; i < m_nSkinnedMeshes; i++)
	{
		m_ppSkinnedMeshes[i]->m_pd3dcbSkinningBoneTransforms = m_ppd3dcbSkinningBoneTransforms[i];
		m_ppSkinnedMeshes[i]->m_pcbxmf4x4MappedSkinningBoneTransforms = m_ppcbxmf4x4MappedSkinningBoneTransforms[i];
	}
}

void CAnimationController::SetTrackAnimationSet(int nAnimationSet)
{
	if (m_pAnimationTracks)
	{
		for (int i = 0; i < nAnimationSet; i++)
		{
			m_pAnimationTracks[i].m_nAnimationSet = i;
			m_pAnimationTracks[i].SetEnable(false);
		}
	}
}

//
//void CAnimationController::SetTrackEnable(int nAnimationTrack, bool bEnable)
//{
//	if (m_pAnimationTracks) 
//		m_pAnimationTracks[nAnimationTrack].SetEnable(bEnable);
//}

void CAnimationController::SetTrackEnable(int nAnimationTrack, int nPos)
{
	if (m_pAnimationTracks)
	{
		switch (nPos)
		{
		case 0:	
			m_nUpperBodyAnimation = nAnimationTrack; 
			m_pAnimationTracks[m_nUpperBodyAnimation].m_fPosition = 0.0f;
			break;
		case 1:	
			m_nLowerBodyAnimation = nAnimationTrack; 
			m_pAnimationTracks[m_nLowerBodyAnimation].m_fPosition = 0.0f;
			break;
		case 2:	
			m_pAnimationTracks[m_nUpperBodyAnimation].m_fPosition = 0.0f;
			m_pAnimationTracks[m_nLowerBodyAnimation].m_fPosition = 0.0f;
			m_nUpperBodyAnimation = nAnimationTrack;
			m_nLowerBodyAnimation = nAnimationTrack; 
			break;
		}
	}
}

void CAnimationController::SetTrackEnable(pair<CharacterAnimation, CharacterAnimation> nAnimationTracks)
{
	if (m_pAnimationTracks)
	{
		m_nUpperBodyAnimation = nAnimationTracks.first;
		m_nLowerBodyAnimation = nAnimationTracks.second;
	}
}

void CAnimationController::SetTrackPosition(int nAnimationTrack, float fPosition)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetPosition(fPosition);
}

void CAnimationController::SetTrackSpeed(int nAnimationTrack, float fSpeed)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetSpeed(fSpeed);
}

void CAnimationController::SetTrackWeight(int nAnimationTrack, float fWeight)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetWeight(fWeight);
}

void CAnimationController::ResetTrack()
{
	if (m_pAnimationTracks)
	{
		for (int i = 0; i < m_nAnimationTracks; ++i)
		{
			m_pAnimationTracks[i].m_fPosition = -ANIMATION_CALLBACK_EPSILON;
			m_pAnimationTracks[i].m_bAnimationEnd = false;
		}
	}
	if (m_pAnimationSets)
	{
		for (int i = 0; i < m_pAnimationSets->m_nAnimationSets; ++i)
		{
			m_pAnimationSets->m_pAnimationSets[i]->m_fPosition = -ANIMATION_CALLBACK_EPSILON;
		}
	}
}

void CAnimationController::SetCallbackKeys(int nAnimationTrack, int nCallbackKeys)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetCallbackKeys(nCallbackKeys);
}

void CAnimationController::SetCallbackKey(int nAnimationTrack, int nKeyIndex, float fTime, void* pData)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetCallbackKey(nKeyIndex, fTime, pData);
}

void CAnimationController::SetAnimationCallbackHandler(int nAnimationTrack, CAnimationCallbackHandler* pCallbackHandler)
{
	if (m_pAnimationTracks) m_pAnimationTracks[nAnimationTrack].SetAnimationCallbackHandler(pCallbackHandler);
}

void CAnimationController::AdvanceTime(float fElapsedTime, GameObject* pRootGameObject)
{
	m_fTime += fElapsedTime;
	if (m_pAnimationTracks)
	{
		int temp = 0;
		for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++)
		{
			m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = Matrix4x4::Zero();
			if (!strncmp(m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_pstrFrameName, "Spine1_M", 8))	// 골반을 기준으로 상하체 위치 확인
				temp = j;
		}

		if (m_nUpperBodyAnimation != m_nLowerBodyAnimation)
		{
			// 상체 애니메이션
			CAnimationSet* pLAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[m_nLowerBodyAnimation].m_nAnimationSet];
			float fUPosition = m_pAnimationTracks[m_nLowerBodyAnimation].UpdatePosition(m_pAnimationTracks[m_nLowerBodyAnimation].m_fPosition, fElapsedTime, pLAnimationSet);
			for (int j = 0; j < temp; j++)
			{
				XMFLOAT4X4 xmf4x4Transform = m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent;
				XMFLOAT4X4 xmf4x4TrackTransform = pLAnimationSet->GetSRT(j, fUPosition);
				xmf4x4Transform = Matrix4x4::Add(xmf4x4Transform, Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[m_nLowerBodyAnimation].m_fWeight));
				m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = xmf4x4Transform;
			}
			m_pAnimationTracks[m_nLowerBodyAnimation].HandleCallback();

			// 하체 애니메이션
			CAnimationSet* pUAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[m_nUpperBodyAnimation].m_nAnimationSet];
			float fLPosition = m_pAnimationTracks[m_nUpperBodyAnimation].UpdatePosition(m_pAnimationTracks[m_nUpperBodyAnimation].m_fPosition, fElapsedTime, pUAnimationSet);
			for (int j = temp; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++)
			{
				XMFLOAT4X4 xmf4x4Transform = m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent;
				XMFLOAT4X4 xmf4x4TrackTransform = pUAnimationSet->GetSRT(j, fLPosition);
				xmf4x4Transform = Matrix4x4::Add(xmf4x4Transform, Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[m_nUpperBodyAnimation].m_fWeight));
				m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = xmf4x4Transform;
			}
			m_pAnimationTracks[m_nUpperBodyAnimation].HandleCallback();
		}
		else
		{
			CAnimationSet* pAnimationSet = m_pAnimationSets->m_pAnimationSets[m_pAnimationTracks[m_nUpperBodyAnimation].m_nAnimationSet];
			float fLPosition = m_pAnimationTracks[m_nUpperBodyAnimation].UpdatePosition(m_pAnimationTracks[m_nUpperBodyAnimation].m_fPosition, fElapsedTime, pAnimationSet);
			for (int j = 0; j < m_pAnimationSets->m_nAnimatedBoneFrames; j++)
			{
				XMFLOAT4X4 xmf4x4Transform = m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent;
				XMFLOAT4X4 xmf4x4TrackTransform = pAnimationSet->GetSRT(j, fLPosition);
				xmf4x4Transform = Matrix4x4::Add(xmf4x4Transform, Matrix4x4::Scale(xmf4x4TrackTransform, m_pAnimationTracks[m_nUpperBodyAnimation].m_fWeight));
				m_pAnimationSets->m_ppAnimatedBoneFrameCaches[j]->m_xmf4x4ToParent = xmf4x4Transform;
			}
			m_pAnimationTracks[m_nUpperBodyAnimation].HandleCallback();
		}

		pRootGameObject->UpdateTransform(NULL);
	}
}
