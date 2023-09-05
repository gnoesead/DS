#include "pch.h"
#include "..\Public\ParticleManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CParticleManager)

CParticleManager::CParticleManager()
{
}

void CParticleManager::PlayParticle(const char* pPoolTag, _ubyte bShaderPass, _float3 vPos, _double dSpriteSpeed, _int2 vSpriteCount, _float fLifeTime, const _tchar* pTextureTag, const _tchar* pBufferTag)
{
	CCustomParticle::CUSTOMPARTDESC		CustomPartDesc;
	CustomPartDesc.bShaderPass = bShaderPass;
	CustomPartDesc.vPosition = vPos;
	CustomPartDesc.dSpriteSpeed = dSpriteSpeed;
	CustomPartDesc.vSpriteCount = vSpriteCount;
	CustomPartDesc.fLifeTime = fLifeTime;
	wsprintf(CustomPartDesc.szTextureTag, pTextureTag);
	wsprintf(CustomPartDesc.szBufferTag, pBufferTag);
	strcpy_s(CustomPartDesc.szPoolTag, pPoolTag);

	CustomPartDesc.VIB_CustomPartDesc.bChangeOption = 0;
	CustomPartDesc.VIB_CustomPartDesc.iSizeOption = 1;
	CustomPartDesc.VIB_CustomPartDesc.vDirOption = {1, 1, 1};

	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.vRange = { 2.f, 2.f, 2.f };
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMinScale = 0.3f;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMaxScale = 1.f;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMinSpeed = 0.3f;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMaxSpeed = 1.f;


	Reuse_Particle(pPoolTag, &CustomPartDesc);
}

void CParticleManager::Create_Particle(const char* pPoolTag, CCustomParticle::CUSTOMPARTDESC* pCustomPartDesc, CGameInstance* pGameInstance)
{
	CCustomParticle::CUSTOMPARTDESC		CustomPartDesc;
	ZeroMemory(&CustomPartDesc, sizeof CustomPartDesc);

	CustomPartDesc = *pCustomPartDesc;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_CustomParticle"), &CustomPartDesc, true)))
	{
		MSG_BOX("Failed to Create_Particle");
		return;
	}
}

void CParticleManager::Collect_Particle(const char* pPoolTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return;

	list<CGameObject*>* pParticleList = Find_ParticleList(pPoolTag);

	if (nullptr == pParticleList)
	{
		pParticleList = new list<CGameObject*>;

		(*pParticleList).emplace_back(pGameObject);

		m_ParticlePool.emplace(pPoolTag, pParticleList);
	}
	else
		(*pParticleList).emplace_back(pGameObject);
}

void CParticleManager::Reuse_Particle(const char* pPoolTag, CCustomParticle::CUSTOMPARTDESC* pCustomPartDesc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CGameObject* pParticle = nullptr;

	list<CGameObject*>* pParticleList = Find_ParticleList(pPoolTag);
	
	if (nullptr == pParticleList)
	{
		pParticleList = new list<CGameObject*>;

		m_ParticlePool.emplace(pPoolTag, pParticleList);

		Create_Particle(pPoolTag, pCustomPartDesc, pGameInstance);
		++m_iCount;
	}
	else
	{
		if (pParticleList->empty())
		{
			Create_Particle(pPoolTag, pCustomPartDesc, pGameInstance);
			++m_iCount;
		}
		else
		{
			pParticle = (*pParticleList).front();
			(*pParticleList).pop_front();

			if (nullptr == pParticle)
				return;

			dynamic_cast<CCustomParticle*>(pParticle)->Reset_Particle(pCustomPartDesc);

			pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_Particle"), pParticle, true);
		}
	}

	Safe_Release(pGameInstance);
}

list<CGameObject*>* CParticleManager::Find_ParticleList(const char* pPoolTag)
{
	auto iter = find_if(m_ParticlePool.begin(), m_ParticlePool.end(), CTag_Finder_Char(pPoolTag));

	if (m_ParticlePool.end() == iter)
		return nullptr;

	return iter->second;
}

void CParticleManager::Free()
{
	for (auto Pair : m_ParticlePool)
	{
		for (auto& pParticle : (*Pair.second))
			Safe_Release(pParticle);

		Pair.second->clear();
		Safe_Delete(Pair.second);
	}

	m_ParticlePool.clear();
}
