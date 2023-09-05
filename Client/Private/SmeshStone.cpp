#include "pch.h"
#include "..\Public\SmeshStone.h"

#include "GameInstance.h"
#include "EffectPlayer.h"

CSmeshStone::CSmeshStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CSmeshStone::CSmeshStone(const CSmeshStone& rhs)
	: CGameObject(rhs)
{

}

HRESULT CSmeshStone::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSmeshStone::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vPos);

	

	

	m_pTransformCom->Set_Speed(1.f);
	
	return S_OK;
}

void CSmeshStone::Tick(_double TimeDelta) 
{
	__super::Tick(TimeDelta);
	
	m_TimeAcc += TimeDelta;

	if (!m_bPlay)
	{
		if (m_TimeAcc > 0.1f)
		{
			CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
			EffectWorldDesc.fScale = 2.f;
			CEffectPlayer::Get_Instance()->Play("Swamp_Stone", m_pTransformCom, &EffectWorldDesc);

			m_bPlay = true;
		}

		
	}

	if (m_TimeAcc > 1.5f)
	{
		m_pTransformCom->Go_Down(TimeDelta);
	}

	if (m_TimeAcc > 5.f)
		m_isDead = true;
}

void CSmeshStone::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

}

HRESULT CSmeshStone::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSmeshStone::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}


CSmeshStone* CSmeshStone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSmeshStone* pInstance = new CSmeshStone(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSmeshStone");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CSmeshStone::Clone(void* pArg)
{
	CSmeshStone* pInstance = new CSmeshStone(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSmeshStone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSmeshStone::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}
