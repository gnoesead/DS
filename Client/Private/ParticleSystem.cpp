#include "pch.h"
#include "..\Public\ParticleSystem.h"

#include "GameInstance.h"
#include "Effect_Mesh.h"
#include "EffectPlayer.h"

#include "Character.h"

CParticleSystem::CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& rhs)
	: CGameObject(rhs)
	, m_iNumEffects(rhs.m_iNumEffects)
	, m_iLevelIndex(rhs.m_iLevelIndex)
{
	//for (auto Part : rhs.m_PartEffects)
	//	m_PartEffects.push_back(dynamic_cast<CParticleSystem*>(Part->Clone()));

	//if (nullptr != rhs.m_pEffect)
	//	m_pEffect = dynamic_cast<CEffect*>(rhs.m_pEffect->Clone());
}

HRESULT CParticleSystem::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleSystem::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_iLevelIndex, pArg, sizeof(_uint));

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CParticleSystem::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_bCollect)
	{
		if (m_pParent)
			m_pParent->Set_isCollect(true);

		m_bCollect = false;

		if(!m_isParticle)
			CEffectPlayer::Get_Instance()->Collect_ParticleSystem(this);
		else
		{
			if (!m_pParent)
				CEffectPlayer::Get_Instance()->Collect_EffectParticle(m_pTag, this);
		}
		m_isCollect = true;

		m_bStop = true;
		m_bPlay = false;
	}

	if (m_bPlay && (nullptr != m_pEffect))
	{
		m_dTimeAccTotal += dTimeDelta;
		m_dTimeAccCycle += dTimeDelta;

		if (nullptr != m_pEffect)
		{
			int iOption = m_pEffect->Get_StartLifeTimeOption();

			if (CEffect::OP_CONSTANT == iOption)
			{
				if (m_dTimeAccCycle > m_pEffect->Get_Duration() + m_pEffect->Get_StartLifeTimeMin())
					m_dTimeAccCycle = 0.0;
			}
			else if (CEffect::OP_RAND_TWO_CONSTANT == iOption)
			{
				if (m_dTimeAccCycle > m_pEffect->Get_Duration() + m_pEffect->Get_StartLifeTimeMax())
					m_dTimeAccCycle = 0.0;
			}
		}
	}
	else if (m_bStop)
	{
		m_dTimeAccTotal = 0.0;
		m_dTimeAccCycle = 0.0;
	}

	m_pTransformCom->Scaling(m_vScale);
	m_pTransformCom->Rotation(m_vRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPosition.x, m_vPosition.y, m_vPosition.z, 1.f));

	//for (auto Parts : m_PartEffects)
	//	Parts->Tick(dTimeDelta);
}

void CParticleSystem::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	if (nullptr != m_pParentTransform)	
	{
		if (nullptr != m_pEffect)     // ±øÅë X
		{
			if (m_pEffect->Get_SimulationSpace() == CEffect::SPACE_LOCAL)
				XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());
			else
			{
				//if (m_bInitialTransformSetting)
				//{
				//	m_bInitialTransformSetting = false;
				//	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());
				//}

				XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * Convert::ToMatrix(m_pParent->m_ParentWorldMatrix));
			}

			if (m_pEffect->Get_IsSetYToGround())
			{
				CGameInstance* pGameInstance = CGameInstance::GetInstance();
				Safe_AddRef(pGameInstance);

				CCharacter* pPlayer = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 0));

				Safe_Release(pGameInstance);

				if(nullptr != pPlayer)
					m_WorldMatrix._42 = pPlayer->Get_LandY() + m_vPosition.y;
			}
		}
		else    // ±øÅë
			XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransform->Get_WorldMatrix());
	}

	//for (auto Parts : m_PartEffects)
	//	Parts->LateTick(dTimeDelta);
}

HRESULT CParticleSystem::Render(void)
{
	return S_OK;
}

void CParticleSystem::Set_PartsParent(CTransform* pTransformCom)
{
	for (auto Part : m_PartEffects)
		Part->Set_Parent(pTransformCom, this);
}

HRESULT CParticleSystem::Create_Effect(int eEffectType)
{
	if (nullptr == m_pEffect)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		//Safe_AddRef(pGameInstance);

		switch (eEffectType)
		{
		case 0:		// EFFECT_TEXTURE
		{
			pGameInstance->Add_GameObject(m_iLevelIndex, TEXT("ParticleSystem_EffectTexture"), TEXT("Prototype_GameObject_EffectTexture"));
			size_t iSize = pGameInstance->Get_GameObject_ListSize(m_iLevelIndex, TEXT("ParticleSystem_EffectTexture"));
			m_pEffect = dynamic_cast<CEffect*>(pGameInstance->Get_GameObject(m_iLevelIndex, TEXT("ParticleSystem_EffectTexture"), (_uint)iSize - 1));
		}
		break;
		case 1:		// EFFECT_MESH
		{
			pGameInstance->Add_GameObject(m_iLevelIndex, TEXT("ParticleSystem_EffectMesh"), TEXT("Prototype_GameObject_EffectMesh"));
			size_t iSize = pGameInstance->Get_GameObject_ListSize(m_iLevelIndex, TEXT("ParticleSystem_EffectMesh"));
			m_pEffect = dynamic_cast<CEffect*>(pGameInstance->Get_GameObject(m_iLevelIndex, TEXT("ParticleSystem_EffectMesh"), (_uint)iSize - 1));
		}
		break;
		case 2:		// EFFECT_PARTICLE
		{
			pGameInstance->Add_GameObject(m_iLevelIndex, TEXT("ParticleSystem_EffectParticle"), TEXT("Prototype_GameObject_EffectParticle"));
			size_t iSize = pGameInstance->Get_GameObject_ListSize(m_iLevelIndex, TEXT("ParticleSystem_EffectParticle"));
			m_pEffect = dynamic_cast<CEffect*>(pGameInstance->Get_GameObject(m_iLevelIndex, TEXT("ParticleSystem_EffectParticle"), (_uint)iSize - 1));
		}
		break;
		}

		Safe_AddRef(m_pEffect);

		//m_pEffect->Set_EffectType(eEffectType);

		m_pEffect->Set_Parent(m_pTransformCom, this);

		//Safe_Release(pGameInstance);
	}
	else
		MSG_BOX("Effect already exists. \nDelete it first. Before you create it.");

	return S_OK;
}

HRESULT CParticleSystem::Clone_Effect(int eEffectType)
{
	if (nullptr == m_pEffect)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		//Safe_AddRef(pGameInstance);

		int iLevelIndex = LEVEL_STATIC;

		switch (eEffectType)
		{
		case 0:		// EFFECT_TEXTURE
		{
			m_pEffect = dynamic_cast<CEffect*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_EffectTexture"), &iLevelIndex));
		}
		break;
		case 1:		// EFFECT_MESH
		{
			m_pEffect = dynamic_cast<CEffect*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_EffectMesh"), &iLevelIndex));
		}
		break;
		case 2:		// EFFECT_PARTICLE
		{
			m_pEffect = dynamic_cast<CEffect*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_EffectParticle"), &iLevelIndex));
		}
		break;
		}

		//Safe_AddRef(m_pEffect);

		m_pEffect->Set_Parent(m_pTransformCom, this);

		//Safe_Release(pGameInstance);
	}
	else
		MSG_BOX("Effect already exists. \nDelete it first. Before you create it.");

	return S_OK;
}

HRESULT CParticleSystem::Add_Component_Texture(_uint iLevelIndex, const _tchar* pComponentTag, int eType)
{
	return m_pEffect->Add_Component_Texture(iLevelIndex, pComponentTag, eType);
}

HRESULT CParticleSystem::Add_Component_Model(_uint iLevelIndex, const _tchar* pComponentTag)
{
	return dynamic_cast<CEffect_Mesh*>(m_pEffect)->Add_Component_Model(iLevelIndex, pComponentTag);
}

void CParticleSystem::Play_Parts(_bool isPlaying)
{
	for (auto Part : m_PartEffects)
	{
		if (isPlaying)
			Part->m_bPlay = true;
		else
			Part->m_bPlay = false;
		Part->m_bStop = false;
	}
}

void CParticleSystem::Stop_Parts()
{
	for (auto Part : m_PartEffects)
	{
		Part->m_bStop = true;
		Part->m_bPlay = false;
	}
}

void CParticleSystem::Clear(void)
{
	m_pEffect = nullptr;

	m_iNumEffects = 0;
	m_PartEffects.clear();

	m_dTimeAccTotal = 0.0;
	m_dTimeAccCycle = 0.0;

	m_bInitialTransformSetting = true;

	m_pParent = nullptr;
}

HRESULT CParticleSystem::Add_Components(void)
{
	CTransform::TRANSFORMDESC	Desc;
	ZeroMemory(&Desc, sizeof Desc);

	Desc.dSpeedPerSec = 50;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &Desc)))
		return E_FAIL;

	for (auto Effect : m_PartEffects)
		Effect->Add_Components();

	if (nullptr == m_pEffect)
		return S_OK;

	return S_OK;
}

HRESULT CParticleSystem::SetUp_ShaderResources(void)
{
	for (auto Effect : m_PartEffects)
		Effect->SetUp_ShaderResources();

	if (nullptr == m_pEffect)
		return S_OK;

	return S_OK;
}

CParticleSystem* CParticleSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticleSystem* pInstance = new CParticleSystem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CParticleSystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticleSystem::Clone(void* pArg)
{
	CParticleSystem* pInstance = new CParticleSystem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CParticleSystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticleSystem::Free(void)
{
	__super::Free();

	for (auto Parts : m_PartEffects)
		Safe_Release(Parts);

	Safe_Release(m_pEffect);

	Safe_Release(m_pTransformCom);
}
