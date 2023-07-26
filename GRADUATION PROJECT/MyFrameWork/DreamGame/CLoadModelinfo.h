#pragma once
#include"stdafx.h"
#include"ComponentBase.h"

class CAnimationSets;
class GameObject;
class SkinnedMeshComponent;
class CLoadedModelInfoCompnent :public ComponentBase
{
public:
	CLoadedModelInfoCompnent() { };
	~CLoadedModelInfoCompnent();

	GameObject* m_pModelRootObject = NULL;

	GameObject* m_pWeaponStart;
	GameObject* m_pWeaponEnd;
	GameObject* m_pWeapon;

	int 							m_nSkinnedMeshes = 0;
	SkinnedMeshComponent** m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

	CAnimationSets* m_pAnimationSets = NULL;

public:
	void PrepareSkinning();
};

