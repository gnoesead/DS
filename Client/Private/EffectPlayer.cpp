#include "pch.h"
#include "..\Public\EffectPlayer.h"

#include "ParticleSystem.h"
#include "Effect_Mesh.h"
#include "Effect_Texture.h"

#include "GameInstance.h"
#include "Effect_Particle.h"
#include "EffectPartsObject.h"

CEffectPlayer* CEffectPlayer::m_pInstance = nullptr;

CEffectPlayer::CEffectPlayer()
{

}

CEffectPlayer::~CEffectPlayer()
{
	Release();

}

CParticleSystem* CEffectPlayer::Reuse_Effect(const char* pTag, class CTransform* pTransformCom)
{
	int iNumEffects = { 0 };
	_float3 vFloat3 = { 0.f, 0.f, 0.f };
	int iEnum = { 0 };
	int iNum = { 0 };

	list<CParticleSystem*>* pList = Find_ParticlePool(pTag);

	if (nullptr == pList)
	{
		CParticleSystem* pParentParticleSystemOrigin = Find_ParticleSystem(pTag);

		if (nullptr == pParentParticleSystemOrigin)
		{
			MSG_BOX("해당 태그를 가진 이펙트가 존재하지 않습니다.");
			return nullptr;
		}

		CParticleSystem* pParentParticleSystem = Reuse_ParticleSystem();

		if (nullptr == pParentParticleSystem)
			return nullptr;

		iNumEffects = pParentParticleSystemOrigin->Get_NumEffects();
		pParentParticleSystem->Set_NumEffects(iNumEffects);

		pParentParticleSystem->Set_ParentMatrix(pTransformCom->Get_WorldFloat4x4());

		for (int i = 0; i < iNumEffects; ++i)
		{
			CParticleSystem* pParticleSystemOrigin = pParentParticleSystemOrigin->Get_Part(i);
			CParticleSystem* pParticleSystem = Reuse_ParticleSystem();
			pParentParticleSystem->Add_Parts(pParticleSystem);

			vFloat3 = pParticleSystemOrigin->Get_Postion();
			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vPosition.x, vFloat3.y + m_EffectWorldDesc.vPosition.y, vFloat3.z + m_EffectWorldDesc.vPosition.z };
			pParticleSystem->Set_Position(vFloat3);

			vFloat3 = pParticleSystemOrigin->Get_Rotation();
			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vRotation.x, vFloat3.y + m_EffectWorldDesc.vRotation.y, vFloat3.z + m_EffectWorldDesc.vRotation.z };
			pParticleSystem->Set_Rotation(vFloat3);

			vFloat3 = pParticleSystemOrigin->Get_Scale();
			vFloat3 = { vFloat3.x * m_EffectWorldDesc.fScale, vFloat3.y * m_EffectWorldDesc.fScale, vFloat3.z * m_EffectWorldDesc.fScale };
			pParticleSystem->Set_Scale(vFloat3);

			CEffect* pEffectOrigin = pParticleSystemOrigin->Get_Effect();
			CEffect* pEffect = nullptr;
			iEnum = pEffectOrigin->Get_EffectType();

			switch (iEnum)
			{
			case CEffect::EFFECT_TEXTURE:
				pEffect = Reuse_EffectTexture(pParticleSystem->Get_Transform(), pParticleSystem);
				break;
			case CEffect::EFFECT_MESH:
				pEffect = Reuse_EffectMesh(pParticleSystem->Get_Transform(), pParticleSystem);
				break;
			}

			pParticleSystem->Set_Effect(pEffect);

			CEffect::EFFECTDESC* pEffectDescOrigin;

			pEffectDescOrigin = pEffectOrigin->Get_EffectDesc();
			pEffect->Set_EffectDesc(pEffectDescOrigin);
			pEffect->Set_PlaySpeed(m_EffectWorldDesc.dSpeed);

			pEffect->Reserve_BurstList(pEffectDescOrigin->iNumBursts);

			for (int j = 0; j < pEffectDescOrigin->iNumBursts; ++j)
			{
				CEffect::BURSTDESC BurstDesc;
				ZeroMemory(&BurstDesc, sizeof BurstDesc);

				BurstDesc = *pEffectOrigin->Get_BurstDesc(j);

				pEffect->Add_BurstDesc(BurstDesc);
			}

			for (int j = 0; j < 3; ++j)
			{
				pEffect->Reserve_SizeOverLifeTime(j, pEffectDescOrigin->iNumSizes[j]);

				for (int k = 0; k < pEffectDescOrigin->iNumSizes[j]; ++k)
				{
					CEffect::LIFETIMEVALUE LifetimeValue;
					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

					LifetimeValue = *pEffectOrigin->Get_SizeOverLifeTime(j, k);

					pEffect->Add_SizeOverLifeTime(j, LifetimeValue);
				}
			}

			for (int j = 0; j < 3; ++j)
			{
				pEffect->Reserve_RotationOverLifeTime(j, pEffectDescOrigin->iNumRotations[j]);

				for (int k = 0; k < pEffectDescOrigin->iNumRotations[j]; ++k)
				{
					CEffect::LIFETIMEVALUE LifetimeValue;
					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

					LifetimeValue = *pEffectOrigin->Get_RotationOverLifeTime(j, k);

					pEffect->Add_RotationOverLifeTime(j, LifetimeValue);
				}
			}

			for (int j = 0; j < 3; ++j)
			{
				pEffect->Reserve_PositionOverLifeTime(j, pEffectDescOrigin->iNumPositions[j]);

				for (int k = 0; k < pEffectDescOrigin->iNumPositions[j]; ++k)
				{
					CEffect::LIFETIMEVALUE LifetimeValue;
					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

					LifetimeValue = *pEffectOrigin->Get_PositionOverLifeTime(j, k);

					pEffect->Add_PositionOverLifeTime(j, LifetimeValue);
				}
			}

			pEffect->Reserve_AlphaOverLifeTime(pEffectDescOrigin->iNumAlpha);

			for (int j = 0; j < pEffectDescOrigin->iNumAlpha; ++j)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				LifetimeValue = *pEffectOrigin->Get_AlphaOverLifeTime(j);

				pEffect->Add_AlphaOverLifeTime(LifetimeValue);
			}

			for (int j = 0; j < pEffectDescOrigin->iNumRateOverTime; ++j)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				LifetimeValue = *pEffectOrigin->Get_RateOverTime(j);

				pEffect->Add_RateOverLifetime(LifetimeValue);
			}

			for (int j = 0; j < pEffectDescOrigin->iNumSpeedOverLifeTimes; ++j)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				LifetimeValue = *pEffectOrigin->Get_SpeedOverLifeTime(j);

				pEffect->Add_SpeedOverLifetime(LifetimeValue);
			}

			for (int j = 0; j < pEffectDescOrigin->iNumGravityModiferOverLifetimes; ++j)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				LifetimeValue = *pEffectOrigin->Get_GravityOverLifeTime(j);

				pEffect->Add_GravityModiferOverLifetime(LifetimeValue);
			}

			for (int j = 0; j < pEffectDescOrigin->iNumFrameOverTime; ++j)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				LifetimeValue = *pEffectOrigin->Get_FrameOverTime(j);

				pEffect->Add_FrameOverLifetime(LifetimeValue);
			}

			float fOrder = pEffect->Get_Order();
			pEffect->Set_Order(fOrder);

			if (CEffect::EFFECT_TEXTURE == iEnum)
			{
				CEffect_Texture* pEffectTextureOrigin = dynamic_cast<CEffect_Texture*>(pEffectOrigin);

				_float2 vCameraRightPos = pEffectTextureOrigin->Get_CameraRightLookPos();

				CEffect_Texture* pEffectTexture = dynamic_cast<CEffect_Texture*>(pEffect);
				pEffectTexture->Set_CameraRightLookPos(vCameraRightPos);
			}

			// Key
			if (CEffect::EFFECT_MESH == iEnum)
			{
				const _tchar* pTag = pEffectOrigin->Get_ModelKey();
				pEffect->Set_ModelKey(pTag);
				if (lstrlen(pTag) >= 1)
					dynamic_cast<CEffect_Mesh*>(pEffect)->Add_Component_Model(LEVEL_STATIC, pTag);
			}

			for (int j = 0; j < TEX_END; ++j)
			{
				const _tchar* pTextureTag = pEffectOrigin->Get_TextureKey(j);
				pEffect->Set_TextureKey(j, pTextureTag);
				if (lstrlen(pTextureTag) < 1)
					continue;
				pEffect->Add_Component_Texture(LEVEL_STATIC, pTextureTag, j);
			}

			pEffect->Set_Initial_Data();
		}

		pParentParticleSystem->Set_PartsParent(pTransformCom);
		pParentParticleSystem->Set_isPlaying(true);
		pParentParticleSystem->Set_isStopped(false);
		pParentParticleSystem->Play_Parts(true);

		return pParentParticleSystem;
	}
	else
		return Reuse_EffectParticle(pTag, pTransformCom);
}

//CParticleSystem* CEffectPlayer::Reuse_PartsEffect(const char* pTag, CEffectPartsObject* pPartsObj)
//{
//	int iNumEffects = { 0 };
//	_float3 vFloat3 = { 0.f, 0.f, 0.f };
//	int iEnum = { 0 };
//	int iNum = { 0 };
//
//	list<CParticleSystem*>* pList = Find_ParticlePool(pTag);
//
//	if (nullptr == pList)
//	{
//		CParticleSystem* pParentParticleSystemOrigin = Find_ParticleSystem(pTag);
//
//		if (nullptr == pParentParticleSystemOrigin)
//		{
//			MSG_BOX("해당 태그를 가진 이펙트가 존재하지 않습니다.");
//			return nullptr;
//		}
//
//		CParticleSystem* pParentParticleSystem = Reuse_ParticleSystem();
//
//		if (nullptr == pParentParticleSystem)
//			return nullptr;
//
//		iNumEffects = pParentParticleSystemOrigin->Get_NumEffects();
//		pParentParticleSystem->Set_NumEffects(iNumEffects);
//
//		pParentParticleSystem->Set_ParentMatrix(Convert::ToFloat4x4(pPartsObj->Get_WorldMatrix()));
//
//		for (int i = 0; i < iNumEffects; ++i)
//		{
//			CParticleSystem* pParticleSystemOrigin = pParentParticleSystemOrigin->Get_Part(i);
//			CParticleSystem* pParticleSystem = Reuse_ParticleSystem();
//			pParentParticleSystem->Add_Parts(pParticleSystem);
//			pParticleSystem->Set_IsParts(true);
//
//			vFloat3 = pParticleSystemOrigin->Get_Postion();
//			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vPosition.x, vFloat3.y + m_EffectWorldDesc.vPosition.y, vFloat3.z + m_EffectWorldDesc.vPosition.z };
//			pParticleSystem->Set_Position(vFloat3);
//
//			vFloat3 = pParticleSystemOrigin->Get_Rotation();
//			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vRotation.x, vFloat3.y + m_EffectWorldDesc.vRotation.y, vFloat3.z + m_EffectWorldDesc.vRotation.z };
//			pParticleSystem->Set_Rotation(vFloat3);
//
//			vFloat3 = pParticleSystemOrigin->Get_Scale();
//			vFloat3 = { vFloat3.x * m_EffectWorldDesc.fScale, vFloat3.y * m_EffectWorldDesc.fScale, vFloat3.z * m_EffectWorldDesc.fScale };
//			pParticleSystem->Set_Scale(vFloat3);
//
//			CEffect* pEffectOrigin = pParticleSystemOrigin->Get_Effect();
//			CEffect* pEffect = nullptr;
//			iEnum = pEffectOrigin->Get_EffectType();
//
//			switch (iEnum)
//			{
//			case CEffect::EFFECT_TEXTURE:
//				pEffect = Reuse_EffectTexture(pParticleSystem->Get_Transform(), pParticleSystem);
//				break;
//			case CEffect::EFFECT_MESH:
//				pEffect = Reuse_EffectMesh(pParticleSystem->Get_Transform(), pParticleSystem);
//				break;
//			}
//
//			pParticleSystem->Set_Effect(pEffect);
//
//			CEffect::EFFECTDESC* pEffectDescOrigin;
//
//			pEffectDescOrigin = pEffectOrigin->Get_EffectDesc();
//			pEffect->Set_EffectDesc(pEffectDescOrigin);
//			pEffect->Set_PlaySpeed(m_EffectWorldDesc.dSpeed);
//
//			pEffect->Reserve_BurstList(pEffectDescOrigin->iNumBursts);
//
//			for (int j = 0; j < pEffectDescOrigin->iNumBursts; ++j)
//			{
//				CEffect::BURSTDESC BurstDesc;
//				ZeroMemory(&BurstDesc, sizeof BurstDesc);
//
//				BurstDesc = *pEffectOrigin->Get_BurstDesc(j);
//
//				pEffect->Add_BurstDesc(BurstDesc);
//			}
//
//			for (int j = 0; j < 3; ++j)
//			{
//				pEffect->Reserve_SizeOverLifeTime(j, pEffectDescOrigin->iNumSizes[j]);
//
//				for (int k = 0; k < pEffectDescOrigin->iNumSizes[j]; ++k)
//				{
//					CEffect::LIFETIMEVALUE LifetimeValue;
//					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//					LifetimeValue = *pEffectOrigin->Get_SizeOverLifeTime(j, k);
//
//					pEffect->Add_SizeOverLifeTime(j, LifetimeValue);
//				}
//			}
//
//			for (int j = 0; j < 3; ++j)
//			{
//				pEffect->Reserve_RotationOverLifeTime(j, pEffectDescOrigin->iNumRotations[j]);
//
//				for (int k = 0; k < pEffectDescOrigin->iNumRotations[j]; ++k)
//				{
//					CEffect::LIFETIMEVALUE LifetimeValue;
//					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//					LifetimeValue = *pEffectOrigin->Get_RotationOverLifeTime(j, k);
//
//					pEffect->Add_RotationOverLifeTime(j, LifetimeValue);
//				}
//			}
//
//			for (int j = 0; j < 3; ++j)
//			{
//				pEffect->Reserve_PositionOverLifeTime(j, pEffectDescOrigin->iNumPositions[j]);
//
//				for (int k = 0; k < pEffectDescOrigin->iNumPositions[j]; ++k)
//				{
//					CEffect::LIFETIMEVALUE LifetimeValue;
//					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//					LifetimeValue = *pEffectOrigin->Get_PositionOverLifeTime(j, k);
//
//					pEffect->Add_PositionOverLifeTime(j, LifetimeValue);
//				}
//			}
//
//			pEffect->Reserve_AlphaOverLifeTime(pEffectDescOrigin->iNumAlpha);
//
//			for (int j = 0; j < pEffectDescOrigin->iNumAlpha; ++j)
//			{
//				CEffect::LIFETIMEVALUE LifetimeValue;
//				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//				LifetimeValue = *pEffectOrigin->Get_AlphaOverLifeTime(j);
//
//				pEffect->Add_AlphaOverLifeTime(LifetimeValue);
//			}
//
//			for (int j = 0; j < pEffectDescOrigin->iNumRateOverTime; ++j)
//			{
//				CEffect::LIFETIMEVALUE LifetimeValue;
//				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//				LifetimeValue = *pEffectOrigin->Get_RateOverTime(j);
//
//				pEffect->Add_RateOverLifetime(LifetimeValue);
//			}
//
//			for (int j = 0; j < pEffectDescOrigin->iNumSpeedOverLifeTimes; ++j)
//			{
//				CEffect::LIFETIMEVALUE LifetimeValue;
//				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//				LifetimeValue = *pEffectOrigin->Get_SpeedOverLifeTime(j);
//
//				pEffect->Add_SpeedOverLifetime(LifetimeValue);
//			}
//
//			for (int j = 0; j < pEffectDescOrigin->iNumGravityModiferOverLifetimes; ++j)
//			{
//				CEffect::LIFETIMEVALUE LifetimeValue;
//				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//				LifetimeValue = *pEffectOrigin->Get_GravityOverLifeTime(j);
//
//				pEffect->Add_GravityModiferOverLifetime(LifetimeValue);
//			}
//
//			for (int j = 0; j < pEffectDescOrigin->iNumFrameOverTime; ++j)
//			{
//				CEffect::LIFETIMEVALUE LifetimeValue;
//				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);
//
//				LifetimeValue = *pEffectOrigin->Get_FrameOverTime(j);
//
//				pEffect->Add_FrameOverLifetime(LifetimeValue);
//			}
//
//			float fOrder = pEffect->Get_Order();
//			pEffect->Set_Order(fOrder);
//
//			if (CEffect::EFFECT_TEXTURE == iEnum)
//			{
//				CEffect_Texture* pEffectTextureOrigin = dynamic_cast<CEffect_Texture*>(pEffectOrigin);
//
//				_float2 vCameraRightPos = pEffectTextureOrigin->Get_CameraRightLookPos();
//
//				CEffect_Texture* pEffectTexture = dynamic_cast<CEffect_Texture*>(pEffect);
//				pEffectTexture->Set_CameraRightLookPos(vCameraRightPos);
//			}
//
//			// Key
//			if (CEffect::EFFECT_MESH == iEnum)
//			{
//				const _tchar* pTag = pEffectOrigin->Get_ModelKey();
//				pEffect->Set_ModelKey(pTag);
//				if (lstrlen(pTag) >= 1)
//					dynamic_cast<CEffect_Mesh*>(pEffect)->Add_Component_Model(LEVEL_STATIC, pTag);
//			}
//
//			for (int j = 0; j < TEX_END; ++j)
//			{
//				const _tchar* pTextureTag = pEffectOrigin->Get_TextureKey(j);
//				pEffect->Set_TextureKey(j, pTextureTag);
//				if (lstrlen(pTextureTag) < 1)
//					continue;
//				pEffect->Add_Component_Texture(LEVEL_STATIC, pTextureTag, j);
//			}
//
//			pEffect->Set_Initial_Data();
//		}
//
//		pParentParticleSystem->Set_PartsParent(nullptr);
//		pParentParticleSystem->Set_isPlaying(true);
//		pParentParticleSystem->Set_isStopped(false);
//		pParentParticleSystem->Play_Parts(true);
//
//		pParentParticleSystem->Set_IsParts(true);
//		pParentParticleSystem->Set_ParentPartsObject(pPartsObj);
//
//		return pParentParticleSystem;
//	}
//	else
//		return Reuse_PartsEffectParticle(pTag, pPartsObj);
//}

void CEffectPlayer::Collect_ParticleSystem(CParticleSystem* pParticleSystem)
{ 
	if (nullptr == pParticleSystem)
		return;

	m_ParticleSystemPool.push_back(pParticleSystem);
}

CParticleSystem* CEffectPlayer::Reuse_ParticleSystem()
{
	CParticleSystem* pParticleSystem = nullptr;

	if (m_ParticleSystemPool.empty())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		_uint iLevelIndex = LEVEL_STATIC;

		//pParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex));

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem"), TEXT("Prototype_GameObject_ParticleSystem") ,&iLevelIndex , false)))
			MSG_BOX("Failed to Add ParticleSystem");

		size_t iSize = pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("Layer_ParticleSystem"));
		pParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem"), (_uint)iSize - 1));

		Safe_Release(pGameInstance);
	}
	else
	{
		pParticleSystem = m_ParticleSystemPool.front();
		m_ParticleSystemPool.pop_front();
		pParticleSystem->Clear();

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem"), pParticleSystem);

		Safe_Release(pGameInstance);
	}

	if (nullptr == pParticleSystem)
		return nullptr;

	pParticleSystem->Set_Collect(false);
	pParticleSystem->Set_IsParts(false);

	return pParticleSystem;
}

void CEffectPlayer::Collect_EffectTexture(CEffect* pEffect)
{
	if (nullptr == pEffect)
		return;

	m_EffectTexturePool.push_back(pEffect);
}

CEffect* CEffectPlayer::Reuse_EffectTexture(CTransform* pParentTransformCom, CParticleSystem* pParent)
{
	CEffect* pEffectTexture = nullptr;

	if (m_EffectTexturePool.empty())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		//pEffectTexture = dynamic_cast<CEffect*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_EffectTexture"), nullptr));

		pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectTexture"), TEXT("Prototype_GameObject_EffectTexture"));
		size_t iSize = pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectTexture"));
		pEffectTexture = dynamic_cast<CEffect*>(pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectTexture"), (_uint)iSize - 1));

		Safe_Release(pGameInstance);
	}
	else
	{
		pEffectTexture = m_EffectTexturePool.front();
		m_EffectTexturePool.pop_front();
		pEffectTexture->Clear();

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectTexture"), pEffectTexture);

		Safe_Release(pGameInstance);
	}

	pEffectTexture->Set_Parent(pParentTransformCom, pParent);

	if (nullptr == pEffectTexture)
		return nullptr;

	pEffectTexture->Set_Collect(false);
	pEffectTexture->Reset_Data();

	return pEffectTexture;
}

void CEffectPlayer::Collect_EffectMesh(CEffect* pEffect)
{
	if (nullptr == pEffect)
		return;

	m_EffectMeshPool.push_back(pEffect);
}

CEffect* CEffectPlayer::Reuse_EffectMesh(CTransform* pParentTransformCom, CParticleSystem* pParent)
{
	CEffect* pEffectMesh = nullptr;

	if (m_EffectMeshPool.empty())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		//pEffectMesh = dynamic_cast<CEffect*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_EffectMesh"), nullptr));

		pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectMesh"), TEXT("Prototype_GameObject_EffectMesh"));
		size_t iSize = pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectMesh"));
		pEffectMesh = dynamic_cast<CEffect*>(pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectMesh"), (_uint)iSize - 1));

		Safe_Release(pGameInstance);
	}
	else
	{
		pEffectMesh = m_EffectMeshPool.front();
		m_EffectMeshPool.pop_front();
		pEffectMesh->Clear();

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectMesh"), pEffectMesh);

		Safe_Release(pGameInstance);
	}

	pEffectMesh->Set_Parent(pParentTransformCom, pParent);

	if (nullptr == pEffectMesh)
		return nullptr;

	pEffectMesh->Set_Collect(false);
	pEffectMesh->Reset_Data();

	return pEffectMesh;
}

void CEffectPlayer::Collect_EffectParticle(const char* pTag, CParticleSystem* pParticleSystem)
{
	if (nullptr == pParticleSystem)
		return;

	list<class CParticleSystem*>* pList = Find_ParticlePool(pTag);

	if (nullptr == pList)
	{
		MSG_BOX("해당 태그를 가진 파티클 이펙트가 존재하지 않습니다.");
		return;
	}

	pList->push_back(pParticleSystem);
}

//CParticleSystem* CEffectPlayer::Reuse_PartsEffectParticle(const char* pTag, CEffectPartsObject* pPartsObj)
//{
//	CParticleSystem* pParentParticleSystem = nullptr;
//
//	_float3 vFloat3 = { 0.f, 0.f, 0.f };
//
//	CParticleSystem* pParentParticleSystemOrigin = Find_ParticleSystem(pTag);
//
//	if (nullptr == pParentParticleSystemOrigin)
//	{
//		MSG_BOX("해당 태그를 가진 이펙트가 존재하지 않습니다.");
//		return nullptr;
//	}
//
//	if (m_EffectParticlePool.empty())
//	{
//		list<class CParticleSystem*>* pList = Find_ParticlePool(pTag);
//
//		if (nullptr == pList)
//		{
//			MSG_BOX("해당 태그를 가진 파티클 이펙트가 존재하지 않습니다.");
//			return nullptr;
//		}
//
//		Add_Particles_In_Pool(pTag);
//
//		pParentParticleSystem = pList->front();
//		pList->pop_front();
//
//		CGameInstance* pGameInstance = CGameInstance::GetInstance();
//		Safe_AddRef(pGameInstance);
//		pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pParentParticleSystem);
//
//		int iNumEffects = pParentParticleSystemOrigin->Get_NumEffects();
//		for (int i = 0; i < iNumEffects; ++i)
//		{
//			CParticleSystem* pParticleSystemOrigin = pParentParticleSystemOrigin->Get_Part(i);
//			CParticleSystem* pParticleSystem = nullptr;
//			pParticleSystem = pParentParticleSystemOrigin->Get_Part(i);
//			pParticleSystem->Set_IsParts(true);
//
//			if (nullptr == pParticleSystem)
//			{
//				Safe_Release(pGameInstance);
//				return nullptr;
//			}
//
//			vFloat3 = pParticleSystemOrigin->Get_Postion();
//			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vPosition.x, vFloat3.y + m_EffectWorldDesc.vPosition.y, vFloat3.z + m_EffectWorldDesc.vPosition.z };
//			pParticleSystem->Set_Position(vFloat3);
//
//			vFloat3 = pParticleSystemOrigin->Get_Rotation();
//			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vRotation.x, vFloat3.y + m_EffectWorldDesc.vRotation.y, vFloat3.z + m_EffectWorldDesc.vRotation.z };
//			pParticleSystem->Set_Rotation(vFloat3);
//
//			vFloat3 = pParticleSystemOrigin->Get_Scale();
//			vFloat3 = { vFloat3.x * m_EffectWorldDesc.fScale, vFloat3.y * m_EffectWorldDesc.fScale, vFloat3.z * m_EffectWorldDesc.fScale };
//			pParticleSystem->Set_Scale(vFloat3);
//
//			pParticleSystem->Set_Collect(false);
//
//			pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pParticleSystem);
//
//			CEffect* pEffect = pParticleSystem->Get_Effect();
//
//			pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pEffect);
//
//			if (nullptr == pEffect)
//			{
//				Safe_Release(pGameInstance);
//				return nullptr;
//			}
//
//			pEffect->Set_Collect(false);
//			pEffect->Reset_Data();
//		}
//
//		Safe_Release(pGameInstance);
//	}
//	else
//	{
//		list<class CParticleSystem*>* pList = Find_ParticlePool(pTag);
//
//		if (nullptr == pList)
//		{
//			MSG_BOX("해당 태그를 가진 파티클 이펙트가 존재하지 않습니다.");
//			return nullptr;
//		}
//
//		pParentParticleSystem = pList->front();
//		pList->pop_front();
//		//pEffectParticle->Clear();
//
//		CGameInstance* pGameInstance = CGameInstance::GetInstance();
//		Safe_AddRef(pGameInstance);
//
//		pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pParentParticleSystem);
//
//		int iNumEffects = pParentParticleSystemOrigin->Get_NumEffects();
//		for (int i = 0; i < iNumEffects; ++i)
//		{
//			CParticleSystem* pParticleSystemOrigin = pParentParticleSystemOrigin->Get_Part(i);
//			CParticleSystem* pParticleSystem = nullptr;
//			pParticleSystem = pParentParticleSystem->Get_Part(i);
//
//			if (nullptr == pParticleSystem)
//			{
//				Safe_Release(pGameInstance);
//				return nullptr;
//			}
//
//			vFloat3 = pParticleSystemOrigin->Get_Postion();
//			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vPosition.x, vFloat3.y + m_EffectWorldDesc.vPosition.y, vFloat3.z + m_EffectWorldDesc.vPosition.z };
//			pParticleSystem->Set_Position(vFloat3);
//
//			vFloat3 = pParticleSystemOrigin->Get_Rotation();
//			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vRotation.x, vFloat3.y + m_EffectWorldDesc.vRotation.y, vFloat3.z + m_EffectWorldDesc.vRotation.z };
//			pParticleSystem->Set_Rotation(vFloat3);
//
//			vFloat3 = pParticleSystemOrigin->Get_Scale();
//			vFloat3 = { vFloat3.x * m_EffectWorldDesc.fScale, vFloat3.y * m_EffectWorldDesc.fScale, vFloat3.z * m_EffectWorldDesc.fScale };
//			pParticleSystem->Set_Scale(vFloat3);
//
//			pParticleSystem->Set_Collect(false);
//
//			pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pParticleSystem);
//
//			CEffect_Particle* pEffect = dynamic_cast<CEffect_Particle*>(pParticleSystem->Get_Effect());
//
//			pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pEffect);
//
//			if (nullptr == pEffect)
//			{
//				Safe_Release(pGameInstance);
//				return nullptr;
//			}
//
//			pEffect->Set_Collect(false);
//			pEffect->Reset_Data();
//			pEffect->Set_Parent(nullptr, pParticleSystem);
//		}
//
//		Safe_Release(pGameInstance);
//	}
//
//
//
//	if (nullptr == pParentParticleSystem)
//		return nullptr;
//
//	pParentParticleSystem->Set_Collect(false);
//
//	pParentParticleSystem->Set_IsParts(true);
//	pParentParticleSystem->Set_ParentPartsObject(pPartsObj);
//
//	pParentParticleSystem->Set_ParentMatrix(Convert::ToFloat4x4(pPartsObj->Get_WorldMatrix()));			// 이거 해줘야 함
//
//	pParentParticleSystem->Set_PartsParent(nullptr);
//	pParentParticleSystem->Set_isPlaying(true);
//	pParentParticleSystem->Set_isStopped(false);
//	pParentParticleSystem->Play_Parts(true);
//
//
//	return pParentParticleSystem;
//}

CParticleSystem* CEffectPlayer::Reuse_EffectParticle(const char* pTag, CTransform* pParentTransformCom)
{
	CParticleSystem* pParentParticleSystem = nullptr;

	_float3 vFloat3 = { 0.f, 0.f, 0.f };

	CParticleSystem* pParentParticleSystemOrigin = Find_ParticleSystem(pTag);

	if (nullptr == pParentParticleSystemOrigin)
	{
		MSG_BOX("해당 태그를 가진 이펙트가 존재하지 않습니다.");
		return nullptr;
	}

	if (m_EffectParticlePool.empty())
	{
		list<class CParticleSystem*>* pList = Find_ParticlePool(pTag);

		if (nullptr == pList)
		{
			MSG_BOX("해당 태그를 가진 파티클 이펙트가 존재하지 않습니다.");
			return nullptr;
		}

		Add_Particles_In_Pool(pTag);

		pParentParticleSystem = pList->front();
		pList->pop_front();

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pParentParticleSystem);

		int iNumEffects = pParentParticleSystemOrigin->Get_NumEffects();
		for (int i = 0; i < iNumEffects; ++i)
		{
			CParticleSystem* pParticleSystemOrigin = pParentParticleSystemOrigin->Get_Part(i);
			CParticleSystem* pParticleSystem = nullptr;
			pParticleSystem = pParentParticleSystemOrigin->Get_Part(i);

			if (nullptr == pParticleSystem)
			{
				Safe_Release(pGameInstance);
				return nullptr;
			}

			vFloat3 = pParticleSystemOrigin->Get_Postion();
			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vPosition.x, vFloat3.y + m_EffectWorldDesc.vPosition.y, vFloat3.z + m_EffectWorldDesc.vPosition.z };
			pParticleSystem->Set_Position(vFloat3);

			vFloat3 = pParticleSystemOrigin->Get_Rotation();
			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vRotation.x, vFloat3.y + m_EffectWorldDesc.vRotation.y, vFloat3.z + m_EffectWorldDesc.vRotation.z };
			pParticleSystem->Set_Rotation(vFloat3);

			vFloat3 = pParticleSystemOrigin->Get_Scale();
			vFloat3 = { vFloat3.x * m_EffectWorldDesc.fScale, vFloat3.y * m_EffectWorldDesc.fScale, vFloat3.z * m_EffectWorldDesc.fScale };
			pParticleSystem->Set_Scale(vFloat3);

			pParticleSystem->Set_Collect(false);

			pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pParticleSystem);

			CEffect* pEffect = pParticleSystem->Get_Effect();

			pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pEffect);

			if (nullptr == pEffect)
			{
				Safe_Release(pGameInstance);
				return nullptr;
			}

			pEffect->Set_Collect(false);
			pEffect->Reset_Data();
		}

		Safe_Release(pGameInstance);
	}
	else
	{
		list<class CParticleSystem*>* pList = Find_ParticlePool(pTag);

		if (nullptr == pList)
		{
			MSG_BOX("해당 태그를 가진 파티클 이펙트가 존재하지 않습니다.");
			return nullptr;
		}

		pParentParticleSystem = pList->front();
		pList->pop_front();
		//pEffectParticle->Clear();

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pParentParticleSystem);

		int iNumEffects = pParentParticleSystemOrigin->Get_NumEffects();
		for (int i = 0; i < iNumEffects; ++i)
		{
			CParticleSystem* pParticleSystemOrigin = pParentParticleSystemOrigin->Get_Part(i);
			CParticleSystem* pParticleSystem = nullptr;
			pParticleSystem = pParentParticleSystem->Get_Part(i);

			if (nullptr == pParticleSystem)
			{
				Safe_Release(pGameInstance);
				return nullptr;
			}

			vFloat3 = pParticleSystemOrigin->Get_Postion();
			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vPosition.x, vFloat3.y + m_EffectWorldDesc.vPosition.y, vFloat3.z + m_EffectWorldDesc.vPosition.z };
			pParticleSystem->Set_Position(vFloat3);

			vFloat3 = pParticleSystemOrigin->Get_Rotation();
			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vRotation.x, vFloat3.y + m_EffectWorldDesc.vRotation.y, vFloat3.z + m_EffectWorldDesc.vRotation.z };
			pParticleSystem->Set_Rotation(vFloat3);

			vFloat3 = pParticleSystemOrigin->Get_Scale();
			vFloat3 = { vFloat3.x * m_EffectWorldDesc.fScale, vFloat3.y * m_EffectWorldDesc.fScale, vFloat3.z * m_EffectWorldDesc.fScale };
			pParticleSystem->Set_Scale(vFloat3);

			pParticleSystem->Set_Collect(false);

			pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pParticleSystem);

			CEffect_Particle* pEffect = dynamic_cast<CEffect_Particle*>(pParticleSystem->Get_Effect());

			pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Layer_ParticleSystem_EffectParticle"), pEffect);

			if (nullptr == pEffect)
			{
				Safe_Release(pGameInstance);
				return nullptr;
			}

			pEffect->Set_Collect(false);
			pEffect->Reset_Data();
			pEffect->Set_Parent(pParentTransformCom, pParticleSystem);
		}
		
		Safe_Release(pGameInstance);
	}



	if (nullptr == pParentParticleSystem)
		return nullptr;

	pParentParticleSystem->Set_Collect(false);
	pParentParticleSystem->Set_IsParts(false);
	pParentParticleSystem->Set_ParentPartsObject(nullptr);

	pParentParticleSystem->Set_ParentMatrix(pParentTransformCom->Get_WorldFloat4x4());			// 이거 해줘야 함

	pParentParticleSystem->Set_PartsParent(pParentTransformCom);
	pParentParticleSystem->Set_isPlaying(true);
	pParentParticleSystem->Set_isStopped(false);
	pParentParticleSystem->Play_Parts(true);


	return pParentParticleSystem;
}

void CEffectPlayer::Play(const char* pEffectTag, class CTransform* pTransformCom, EFFECTWORLDDESC* pEffectWorldDesc)
{
	CParticleSystem* pParticleSystem = Find_ParticleSystem(pEffectTag);

	if (nullptr == pParticleSystem)
		return;

	if (nullptr != pEffectWorldDesc)
		m_EffectWorldDesc = (*pEffectWorldDesc);
	else
	{
		m_EffectWorldDesc.fScale = { 1.f };
		m_EffectWorldDesc.vRotation = { 0.f, 0.f, 0.f };
		m_EffectWorldDesc.vPosition = { 0.f, 0.f, 0.f };
		m_EffectWorldDesc.dSpeed = { 1.0 };
	}

	//pParticleSystem->Set_PartsParent(pTransformCom);
	//pParticleSystem->Set_isPlaying(true);
	//pParticleSystem->Set_isStopped(false);
	//pParticleSystem->Play_Parts(true);

	Reuse_Effect(pEffectTag, pTransformCom);
}

//void CEffectPlayer::PlayPartsEffect(const char* pEffectTag, CEffectPartsObject* pPartsObj, EFFECTWORLDDESC* pEffectWorldDesc)
//{
//	CParticleSystem* pParticleSystem = Find_ParticleSystem(pEffectTag);
//
//	if (nullptr == pParticleSystem)
//		return;
//
//	if (nullptr != pEffectWorldDesc)
//		m_EffectWorldDesc = (*pEffectWorldDesc);
//	else
//	{
//		m_EffectWorldDesc.fScale = { 1.f };
//		m_EffectWorldDesc.vRotation = { 0.f, 0.f, 0.f };
//		m_EffectWorldDesc.vPosition = { 0.f, 0.f, 0.f };
//		m_EffectWorldDesc.dSpeed = { 1.0 };
//	}
//
//	Reuse_PartsEffect(pEffectTag, pPartsObj);
//}

void CEffectPlayer::Stop(const char* pEffectTag)
{
	CParticleSystem* pParticleSystem = Find_ParticleSystem(pEffectTag);

	if (nullptr == pParticleSystem)
		return;

	pParticleSystem->Set_isStopped(true);
	pParticleSystem->Set_isPlaying(false);
	pParticleSystem->Stop_Parts();
}

CParticleSystem* CEffectPlayer::Find_ParticleSystem(const char* pEffectTag)
{
	auto iter = find_if(m_Effects.begin(), m_Effects.end(), CTag_Finder_Char(pEffectTag));

	if (m_Effects.end() == iter)
		return nullptr;

	return iter->second;
}

list<class CParticleSystem*>* CEffectPlayer::Find_ParticlePool(const char* pEffectTag)
{
	auto iter = find_if(m_EffectParticlePool.begin(), m_EffectParticlePool.end(), CTag_Finder_Char(pEffectTag));

	if (m_EffectParticlePool.end() == iter)
		return nullptr;

	return iter->second;
}

_bool CEffectPlayer::Find_ModelKey(const _tchar* pModelKey)
{
	auto iter = m_pModelKeyList.begin();

	for (; iter != m_pModelKeyList.end(); ++iter)
	{
		if (false == lstrcmpW(pModelKey, *iter))
			return true;
	}

	if (iter == m_pModelKeyList.end())
		return false;

	return false;
}

_bool CEffectPlayer::Find_TextureKey(int iTextureIndex, const _tchar* pTextureKey)
{
	auto iter = m_pTextureKeyList[iTextureIndex].begin();

	for (; iter != m_pTextureKeyList[iTextureIndex].end(); ++iter)
	{
		if (false == lstrcmpW(pTextureKey, *iter))
			return true;
	}
		
	if (iter == m_pTextureKeyList[iTextureIndex].end())
		return false;

	return false;
}

void CEffectPlayer::Add_ParticlePool(const char* pEffectTag)
{
	list<class CParticleSystem*>* pList = Find_ParticlePool(pEffectTag);

	if (nullptr != pList)
	{
		MSG_BOX("Pool이 이미 존재합니다.");
		return;
	}

	pList = new list<class CParticleSystem*>;

	m_EffectParticlePool.emplace(pEffectTag, pList);
}

HRESULT CEffectPlayer::Add_Particles_In_Pool(const char* pEffectTag, int iCnt)
{
	list<class CParticleSystem*>* pList = Find_ParticlePool(pEffectTag);

	if (nullptr == pList)
	{
		MSG_BOX("해당 태그를 가진 파티클 이펙트 Pool이 존재하지 않습니다.");
		return E_FAIL;
	}

	int iNumEffects = { 0 };
	_float3 vFloat3 = { 0.f, 0.f, 0.f };
	int iEnum = { 2 };
	int iNum = { 0 };

	CParticleSystem* pParentParticleSystemOrigin = Find_ParticleSystem(pEffectTag);

	if (nullptr == pParentParticleSystemOrigin)
	{
		MSG_BOX("해당 태그를 가진 이펙트가 존재하지 않습니다.");
		return E_FAIL;
	}


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	for (int i = 0; i < iCnt; ++i)
	{
		_uint iLevelIndex = LEVEL_STATIC;
		CParticleSystem* pParentParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex));

		if (nullptr == pParentParticleSystem)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

		iNumEffects = pParentParticleSystemOrigin->Get_NumEffects();
		pParentParticleSystem->Set_NumEffects(iNumEffects);

		//pParentParticleSystem->Set_ParentMatrix(pTransformCom->Get_WorldFloat4x4());

		for (int i = 0; i < iNumEffects; ++i)
		{
			CParticleSystem* pParticleSystemOrigin = pParentParticleSystemOrigin->Get_Part(i);
			CParticleSystem* pParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex));
			pParentParticleSystem->Add_Parts(pParticleSystem);

			vFloat3 = pParticleSystemOrigin->Get_Postion();
			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vPosition.x, vFloat3.y + m_EffectWorldDesc.vPosition.y, vFloat3.z + m_EffectWorldDesc.vPosition.z };
			pParticleSystem->Set_Position(vFloat3);

			vFloat3 = pParticleSystemOrigin->Get_Rotation();
			vFloat3 = { vFloat3.x + m_EffectWorldDesc.vRotation.x, vFloat3.y + m_EffectWorldDesc.vRotation.y, vFloat3.z + m_EffectWorldDesc.vRotation.z };
			pParticleSystem->Set_Rotation(vFloat3);

			vFloat3 = pParticleSystemOrigin->Get_Scale();
			vFloat3 = { vFloat3.x * m_EffectWorldDesc.fScale, vFloat3.y * m_EffectWorldDesc.fScale, vFloat3.z * m_EffectWorldDesc.fScale };
			pParticleSystem->Set_Scale(vFloat3);

			CEffect_Particle* pEffectOrigin = dynamic_cast<CEffect_Particle*>(pParticleSystemOrigin->Get_Effect());

			CEffect_Particle* pEffect = dynamic_cast<CEffect_Particle*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_EffectParticle"), &iLevelIndex));

			if (nullptr == pEffect)
			{
				Safe_Release(pGameInstance);
				return E_FAIL;
			}

			pParticleSystem->Set_Effect(pEffect);

			pParticleSystem->Set_IsParticle(true);
			pParticleSystem->Set_Tag(pEffectTag);

			CEffect::EFFECTDESC* pEffectDescOrigin;

			pEffectDescOrigin = pEffectOrigin->Get_EffectDesc();
			pEffect->Set_EffectDesc(pEffectDescOrigin);
			pEffect->Set_PlaySpeed(m_EffectWorldDesc.dSpeed);

			pEffect->Set_CameraRightLookPos(pEffectOrigin->Get_CameraRightLookPos());
			pEffect->Set_Order(pEffectOrigin->Get_Order());

			pEffect->Reserve_BurstList(pEffectDescOrigin->iNumBursts);

			for (int j = 0; j < pEffectDescOrigin->iNumBursts; ++j)
			{
				CEffect::BURSTDESC BurstDesc;
				ZeroMemory(&BurstDesc, sizeof BurstDesc);

				BurstDesc = *pEffectOrigin->Get_BurstDesc(j);

				pEffect->Add_BurstDesc(BurstDesc);
			}

			for (int j = 0; j < 3; ++j)
			{
				pEffect->Reserve_SizeOverLifeTime(j, pEffectDescOrigin->iNumSizes[j]);

				for (int k = 0; k < pEffectDescOrigin->iNumSizes[j]; ++k)
				{
					CEffect::LIFETIMEVALUE LifetimeValue;
					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

					LifetimeValue = *pEffectOrigin->Get_SizeOverLifeTime(j, k);

					pEffect->Add_SizeOverLifeTime(j, LifetimeValue);
				}
			}

			for (int j = 0; j < 3; ++j)
			{
				pEffect->Reserve_RotationOverLifeTime(j, pEffectDescOrigin->iNumRotations[j]);

				for (int k = 0; k < pEffectDescOrigin->iNumRotations[j]; ++k)
				{
					CEffect::LIFETIMEVALUE LifetimeValue;
					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

					LifetimeValue = *pEffectOrigin->Get_RotationOverLifeTime(j, k);

					pEffect->Add_RotationOverLifeTime(j, LifetimeValue);
				}
			}

			for (int j = 0; j < 3; ++j)
			{
				pEffect->Reserve_PositionOverLifeTime(j, pEffectDescOrigin->iNumPositions[j]);

				for (int k = 0; k < pEffectDescOrigin->iNumPositions[j]; ++k)
				{
					CEffect::LIFETIMEVALUE LifetimeValue;
					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

					LifetimeValue = *pEffectOrigin->Get_PositionOverLifeTime(j, k);

					pEffect->Add_PositionOverLifeTime(j, LifetimeValue);
				}
			}

			pEffect->Reserve_AlphaOverLifeTime(pEffectDescOrigin->iNumAlpha);

			for (int j = 0; j < pEffectDescOrigin->iNumAlpha; ++j)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				LifetimeValue = *pEffectOrigin->Get_AlphaOverLifeTime(j);

				pEffect->Add_AlphaOverLifeTime(LifetimeValue);
			}

			for (int j = 0; j < pEffectDescOrigin->iNumRateOverTime; ++j)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				LifetimeValue = *pEffectOrigin->Get_RateOverTime(j);

				pEffect->Add_RateOverLifetime(LifetimeValue);
			}

			for (int j = 0; j < pEffectDescOrigin->iNumSpeedOverLifeTimes; ++j)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				LifetimeValue = *pEffectOrigin->Get_SpeedOverLifeTime(j);

				pEffect->Add_SpeedOverLifetime(LifetimeValue);
			}

			for (int j = 0; j < pEffectDescOrigin->iNumGravityModiferOverLifetimes; ++j)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				LifetimeValue = *pEffectOrigin->Get_GravityOverLifeTime(j);

				pEffect->Add_GravityModiferOverLifetime(LifetimeValue);
			}

			for (int j = 0; j < pEffectDescOrigin->iNumFrameOverTime; ++j)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				LifetimeValue = *pEffectOrigin->Get_FrameOverTime(j);

				pEffect->Add_FrameOverLifetime(LifetimeValue);
			}

			float fOrder = pEffect->Get_Order();
			pEffect->Set_Order(fOrder);

			for (int j = 0; j < TEX_END; ++j)
			{
				const _tchar* pTextureTag = pEffectOrigin->Get_TextureKey(j);
				pEffect->Set_TextureKey(j, pTextureTag);
				if (lstrlen(pTextureTag) < 1)
					continue;
				pEffect->Add_Component_Texture(LEVEL_STATIC, pTextureTag, j);
			}

			pEffect->Set_Initial_Data();
		}

		pParentParticleSystem->Set_IsParticle(true);
		pParentParticleSystem->Set_Tag(pEffectTag);

		pList->push_back(pParentParticleSystem);
	}

	Safe_Release(pGameInstance);

	//pParentParticleSystem->Set_PartsParent(pTransformCom);
	//pParentParticleSystem->Set_isPlaying(true);
	//pParentParticleSystem->Set_isStopped(false);
	//pParentParticleSystem->Play_Parts(true);

	return S_OK;
}

void CEffectPlayer::Release()
{
	for (auto Pair : m_Effects)
		Safe_Release(Pair.second);

	m_Effects.clear();

	for (auto Tag : m_EffectTags)
		Safe_Delete_Array(Tag);

	m_EffectTags.clear();

	for (auto Key : m_pModelKeyList)
		Safe_Delete_Array(Key);

	m_pModelKeyList.clear();

	for (int i = 0; i < TEX_END; ++i)
	{
		for (auto Key : m_pTextureKeyList[i])
			Safe_Delete_Array(Key);

		m_pTextureKeyList[i].clear();
	}

	for (auto pParticleSystem : m_ParticleSystemPool)
		Safe_Release(pParticleSystem);
	
	m_ParticleSystemPool.clear();

	for (auto pEffect : m_EffectTexturePool)
		Safe_Release(pEffect);

	m_EffectTexturePool.clear();

	for (auto pEffect : m_EffectMeshPool)
		Safe_Release(pEffect);

	m_EffectMeshPool.clear();


	for (auto Pair : m_EffectParticlePool)
	{
		for (auto pParticleSystem : (*Pair.second))
			Safe_Release(pParticleSystem);

		Pair.second->clear();
		Safe_Delete(Pair.second);
	}

	m_EffectParticlePool.clear();
}