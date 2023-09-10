#include "pch.h"
#include "..\Public\ParticleManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CParticleManager)

CParticleManager::CParticleManager()
{
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

void CParticleManager::PlayParticle(const char* pPoolTag, const _tchar* pBufferTag, const _tchar* pTextureTag, 
	_float3 vPos, _float fLifeTime, _float fMinScale, _float fMaxScale, _float fMinSpeed, _float fMaxSpeed,
	_float3 vMinRange, _float3 vMaxRange, _float fTickPerSize, CCustomParticle::CUSTOM_PARTICLE_PASS eShaderPass,
	_double dSpriteSpeed, _int2 vSpriteCount, _bool bSpriteRandomStart, const _tchar* pRampTag, _float fRampMax)
{
	CCustomParticle::CUSTOMPARTDESC		CustomPartDesc;

	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.vMinRange = vMinRange;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.vMaxRange = vMaxRange;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMinScale = fMinScale;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMaxScale = fMaxScale;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMinSpeed = fMinSpeed;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMaxSpeed = fMaxSpeed;

	CustomPartDesc.VIB_CustomPartDesc.fLifeTime = fLifeTime;
	CustomPartDesc.VIB_CustomPartDesc.dSpriteSpeed = dSpriteSpeed;
	CustomPartDesc.VIB_CustomPartDesc.vSpriteCount = vSpriteCount;
	CustomPartDesc.VIB_CustomPartDesc.bSpriteRandomStart = bSpriteRandomStart;

	CustomPartDesc.eShaderPass = eShaderPass;
	CustomPartDesc.vPosition = vPos;

	wsprintf(CustomPartDesc.szTextureTag, pTextureTag);
	wsprintf(CustomPartDesc.szBufferTag, pBufferTag);
	strcpy_s(CustomPartDesc.szPoolTag, pPoolTag);

	CustomPartDesc.VIB_CustomPartDesc.fTickPerSize = fTickPerSize;
	CustomPartDesc.VIB_CustomPartDesc.vTickPerDir = { 1.f, 1.f, 1.f };
	CustomPartDesc.VIB_CustomPartDesc.vDirOption = { 0, 0, 0 };
	CustomPartDesc.VIB_CustomPartDesc.eParticleType = CVIBuffer_CustomParticle::TYPE_SIZE;

	if (nullptr != pRampTag)
	{
		CustomPartDesc.fRampMax = fRampMax;
		wsprintf(CustomPartDesc.szRampTextureTag, pRampTag);
		CustomPartDesc.bUseRamp = true;
	}

	Reuse_Particle(pPoolTag, &CustomPartDesc);
}

void CParticleManager::PlayParticle(const char* pPoolTag, const _tchar* pBufferTag, const _tchar* pTextureTag, 
	_float3 vPos, _float fLifeTime, _float fMinScale, _float fMaxScale, _float fMinSpeed, _float fMaxSpeed,
	_float3 vMinRange, _float3 vMaxRange, _float3 vTickPerDir, _int3 vDirOption, CCustomParticle::CUSTOM_PARTICLE_PASS eShaderPass,
	_double dSpriteSpeed, _int2 vSpriteCount, _bool bSpriteRandomStart, const _tchar* pRampTag, _float fRampMax)
{
	CCustomParticle::CUSTOMPARTDESC		CustomPartDesc;

	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.vMinRange = vMinRange;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.vMaxRange = vMaxRange;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMinScale = fMinScale;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMaxScale = fMaxScale;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMinSpeed = fMinSpeed;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMaxSpeed = fMaxSpeed;

	CustomPartDesc.VIB_CustomPartDesc.fLifeTime = fLifeTime;
	CustomPartDesc.VIB_CustomPartDesc.dSpriteSpeed = dSpriteSpeed;
	CustomPartDesc.VIB_CustomPartDesc.vSpriteCount = vSpriteCount;
	CustomPartDesc.VIB_CustomPartDesc.bSpriteRandomStart = bSpriteRandomStart;

	CustomPartDesc.eShaderPass = eShaderPass;
	CustomPartDesc.vPosition = vPos;

	wsprintf(CustomPartDesc.szTextureTag, pTextureTag);
	wsprintf(CustomPartDesc.szBufferTag, pBufferTag);
	strcpy_s(CustomPartDesc.szPoolTag, pPoolTag);

	CustomPartDesc.VIB_CustomPartDesc.fTickPerSize = 1.f;
	CustomPartDesc.VIB_CustomPartDesc.vTickPerDir = vTickPerDir;
	CustomPartDesc.VIB_CustomPartDesc.vDirOption = vDirOption;
	CustomPartDesc.VIB_CustomPartDesc.eParticleType = CVIBuffer_CustomParticle::TYPE_DIR;

	if (nullptr != pRampTag)
	{
		CustomPartDesc.fRampMax = fRampMax;
		wsprintf(CustomPartDesc.szRampTextureTag, pRampTag);
		CustomPartDesc.bUseRamp = true;
	}

	Reuse_Particle(pPoolTag, &CustomPartDesc);
}

void CParticleManager::PlayParticle(const char* pPoolTag, const _tchar* pBufferTag, const _tchar* pTextureTag,
	_float3 vPos, _float fLifeTime, _float fMinScale, _float fMaxScale, _float fMinSpeed, _float fMaxSpeed,
	_float3 vMinRange, _float3 vMaxRange, _float fTickPerSize, _float3 vTickPerDir, _int3 vDirOption, CCustomParticle::CUSTOM_PARTICLE_PASS eShaderPass,
	_double dSpriteSpeed, _int2 vSpriteCount, _bool bSpriteRandomStart, const _tchar* pRampTag, _float fRampMax)
{
	CCustomParticle::CUSTOMPARTDESC		CustomPartDesc;

	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.vMinRange = vMinRange;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.vMaxRange = vMaxRange;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMinScale = fMinScale;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMaxScale = fMaxScale;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMinSpeed = fMinSpeed;
	CustomPartDesc.VIB_CustomPartDesc.InstanceDesc.fMaxSpeed = fMaxSpeed;

	CustomPartDesc.VIB_CustomPartDesc.fLifeTime = fLifeTime;
	CustomPartDesc.VIB_CustomPartDesc.dSpriteSpeed = dSpriteSpeed;
	CustomPartDesc.VIB_CustomPartDesc.vSpriteCount = vSpriteCount;
	CustomPartDesc.VIB_CustomPartDesc.bSpriteRandomStart = bSpriteRandomStart;

	CustomPartDesc.eShaderPass = eShaderPass;
	CustomPartDesc.vPosition = vPos;
	
	wsprintf(CustomPartDesc.szTextureTag, pTextureTag);
	wsprintf(CustomPartDesc.szBufferTag, pBufferTag);
	strcpy_s(CustomPartDesc.szPoolTag, pPoolTag);

	CustomPartDesc.VIB_CustomPartDesc.fTickPerSize = fTickPerSize;
	CustomPartDesc.VIB_CustomPartDesc.vTickPerDir = vTickPerDir;
	CustomPartDesc.VIB_CustomPartDesc.vDirOption = vDirOption;
	CustomPartDesc.VIB_CustomPartDesc.eParticleType = CVIBuffer_CustomParticle::TYPE_SIZE_DIR;

	if (nullptr != pRampTag)
	{
		CustomPartDesc.fRampMax = fRampMax;
		wsprintf(CustomPartDesc.szRampTextureTag, pRampTag);
		CustomPartDesc.bUseRamp = true;
	}

	Reuse_Particle(pPoolTag, &CustomPartDesc);
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
