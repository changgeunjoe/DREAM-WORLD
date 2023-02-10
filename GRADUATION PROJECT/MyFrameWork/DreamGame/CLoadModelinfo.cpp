#include "CLoadModelinfo.h"
#include"GameObject.h"

CLoadedModelInfoCompnent::~CLoadedModelInfoCompnent()
{
	if (m_ppSkinnedMeshes) delete[] m_ppSkinnedMeshes;
}

void CLoadedModelInfoCompnent::PrepareSkinning()
{
	int nSkinnedMesh = 0;
	m_ppSkinnedMeshes = new SkinnedMeshComponent * [m_nSkinnedMeshes];
	m_pModelRootObject->FindAndSetSkinnedMesh(m_ppSkinnedMeshes, &nSkinnedMesh);


	for (int i = 0; i < m_nSkinnedMeshes; i++) m_ppSkinnedMeshes[i]->PrepareSkinning(m_pModelRootObject);
}

