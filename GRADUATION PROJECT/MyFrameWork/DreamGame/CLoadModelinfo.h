#pragma once
#include"stdafx.h"

class GameObject;
class SkinnedMeshComponent;
class CLoadedModelInfo
{
public:
	CLoadedModelInfo() { };
	~CLoadedModelInfo();

	GameObject* m_pModelRootObject = NULL;

	int 							m_nSkinnedMeshes = 0;
	SkinnedMeshComponent**					m_ppSkinnedMeshes = NULL; //[SkinnedMeshes], Skinned Mesh Cache

	//CAnimationSets* m_pAnimationSets = NULL;

public:
	void PrepareSkinning();
};
