#include "pch.h"
#include "..\Public\EffectW_Manager.h"

#include "GameInstance.h"
#include "EffectW.h"

CEffectW_Manager* CEffectW_Manager::m_pInstance = nullptr;

CEffectW_Manager::CEffectW_Manager()
{

}

CEffectW_Manager::~CEffectW_Manager()
{
	Release();
}

HRESULT CEffectW_Manager::Add_Effect_In_Pool(EFFECTW_TYPE eEffectType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();

	_uint iIdx = (_uint)(pGameInstance->Get_GameObject_ListSize(iCurIdx, TEXT("Layer_EffectW")) - 1);
	CEffectW* pEffectW = dynamic_cast<CEffectW*>(pGameInstance->Get_GameObject(iCurIdx, TEXT("Layer_EffectW"), iIdx));

	if (nullptr == pEffectW)
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	switch (eEffectType)
	{
	case EFFECT_GROUNDSMOKE:
		m_GroundSmokePool.push_back(pEffectW);
		break;
	case EFFECT_SWAMPWATER:
		m_SwampWaterPool.push_back(pEffectW);
		break;
	case EFFECT_STONEPARTICLE:
		m_StoneParticlePool.push_back(pEffectW);
		break;
	case EFFECT_SWAMPSMOKE:
		m_SwampSmokePool.push_back(pEffectW);
		break;
	default:
		break;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CEffectW_Manager::Play(EFFECTW_TYPE eEffectType, void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CEffectW_Manager : 인자 없음 ");
		return;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();

	switch (eEffectType)
	{
	case EFFECT_GROUNDSMOKE:
	{
		if (m_GroundSmokePool.empty())
		{
			if (FAILED(pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_EffectW"), TEXT("Prototype_GameObject_GroundSmoke"), pArg, true)))
			{
				MSG_BOX("Failed to Add  GroundSmoke");
				Safe_Release(pGameInstance);
				return;
			}
		}
		else
			Reuse_Effect(eEffectType, pArg);
		break;
	}
	case EFFECT_SWAMPWATER:
	{
		if (m_SwampWaterPool.empty())
		{
			if (FAILED(pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_EffectW"), TEXT("Prototype_GameObject_WaterParticleEffect"), pArg, true)))
			{
				MSG_BOX("Failed to Add  WaterParticleEffect");
				Safe_Release(pGameInstance);
				return;
			}
		}
		else
			Reuse_Effect(eEffectType, pArg);
		break;
	}
	case EFFECT_STONEPARTICLE:
	{
		if (m_StoneParticlePool.empty())
		{
			if (FAILED(pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_EffectW"), TEXT("Prototype_GameObject_StoneParticle"), pArg, true)))
			{
				MSG_BOX("Failed to Add  StoneParticle");
				Safe_Release(pGameInstance);
				return;
			}
		}
		else
			Reuse_Effect(eEffectType, pArg);
		break;
	}
	case EFFECT_SWAMPSMOKE:
	{
		if (m_SwampSmokePool.empty())
		{
			if (FAILED(pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_EffectW"), TEXT("Prototype_GameObject_Swamp_SmokeEffect"), pArg, true)))
			{
				MSG_BOX("Failed to Add Swamp_SmokeEffect");
				Safe_Release(pGameInstance);
				return;
			}
		}
		else
			Reuse_Effect(eEffectType, pArg);
		break;
	}
		
	default:
		break;
	}
	
	Safe_Release(pGameInstance);
}

void CEffectW_Manager::Reuse_Effect(EFFECTW_TYPE eEffectType, void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();

	if (nullptr == pArg)
	{
		MSG_BOX("CEffectW_Manager : Reuse_Effect 인자 없음 ");
		return;
	}

	switch (eEffectType)
	{
	case EFFECT_GROUNDSMOKE:
	{
		CEffectW* pEffect = m_GroundSmokePool.front();
		m_GroundSmokePool.pop_front();
		pEffect->Set_Dead(false);
		pEffect->Reset_Data(pArg);
		

		pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_EffectW"), pEffect, true);
		break;
	}

	case EFFECT_SWAMPWATER:
	{
		CEffectW* pEffect = m_SwampWaterPool.front();
		m_SwampWaterPool.pop_front();
		pEffect->Set_Dead(false);
		pEffect->Reset_Data(pArg);
		
		pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_EffectW"), pEffect, true);
		break;
	}
	case EFFECT_STONEPARTICLE:
	{
		CEffectW* pEffect = m_StoneParticlePool.front();
		m_StoneParticlePool.pop_front();
		pEffect->Set_Dead(false);
		pEffect->Reset_Data(pArg);

		pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_EffectW"), pEffect, true);
		break;
	}
	case EFFECT_SWAMPSMOKE:
	{
		CEffectW* pEffect = m_SwampSmokePool.front();
		m_SwampSmokePool.pop_front();
		pEffect->Set_Dead(false);
		pEffect->Reset_Data(pArg);

		pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_EffectW"), pEffect, true);
		break;
	}
	
	default:
		break;
	}

	Safe_Release(pGameInstance);
}

void CEffectW_Manager::Collect_Effect(EFFECTW_TYPE eEffectType , class CEffectW* pEffectW)
{
	if (nullptr == pEffectW)
		return;

	switch (eEffectType)
	{
	case EFFECT_GROUNDSMOKE:
		m_GroundSmokePool.push_back(pEffectW);
		break;
	case EFFECT_SWAMPWATER:
		m_SwampWaterPool.push_back(pEffectW);
		break;
	case EFFECT_STONEPARTICLE:
		m_StoneParticlePool.push_back(pEffectW);
		break;
	case EFFECT_SWAMPSMOKE:
		m_SwampSmokePool.push_back(pEffectW);
		break;
	}
}

void CEffectW_Manager::Release(void)
{
	for (auto& pEffect : m_GroundSmokePool)
		Safe_Release(pEffect);

	m_GroundSmokePool.clear();

	for (auto& pEffect : m_SwampWaterPool)
		Safe_Release(pEffect);

	m_SwampWaterPool.clear();

	for (auto& pEffect : m_StoneParticlePool)
		Safe_Release(pEffect);

	m_StoneParticlePool.clear();

	for (auto& pEffect : m_SwampSmokePool)
		Safe_Release(pEffect);

	m_SwampSmokePool.clear();
}
