#include "pch.h"
#include "..\Public\SmellBundle.h"

#include "GameInstance.h"
#include "EffectPlayer.h"

_bool	g_bSmellReset = false;

CSmellBundle::CSmellBundle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CSmellBundle::CSmellBundle(const CSmellBundle& rhs)
	: CGameObject(rhs)
{

}

HRESULT CSmellBundle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSmellBundle::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vPos);

	CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
	EffectWorldDesc.vPosition.y += 0.6f;	
	EffectWorldDesc.fScale = 0.9f;

	if(TYPE_NORMAL == m_EffectDesc.eType)
		CEffectPlayer::Get_Instance()->Play("SmellBundle", m_pTransformCom, &EffectWorldDesc);
	else if (TYPE_DISAPEEAR == m_EffectDesc.eType)
		CEffectPlayer::Get_Instance()->Play("SmellBundle_Disappear", m_pTransformCom, &EffectWorldDesc);
	
	
	return S_OK;
}

void CSmellBundle::Tick(_double TimeDelta) 
{
	__super::Tick(TimeDelta);

	if (g_bSmellReset && m_EffectDesc.eType == TYPE_WORLD)
	{
		m_TimeAcc += TimeDelta;

		if (m_TimeAcc > 1.5)
		{
			g_bSmellReset = false;
			m_TimeAcc = 0.0;
		}
	}

	if (TYPE_NORMAL == m_EffectDesc.eType)
		PlayerCheck(TimeDelta);
}

void CSmellBundle::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

}

HRESULT CSmellBundle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSmellBundle::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

void CSmellBundle::PlayerCheck(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vPlayerPos = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (Compute::DistCheck(vPlayerPos, vMyPos, 3.f) && pGameInstance->Get_DIKeyDown(DIK_F))
	{
		EFFECTDESC EffectDesc;
		EffectDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		EffectDesc.eType = TYPE_DISAPEEAR;

		if (FAILED(pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_SmellBundle"),
			TEXT("Prototype_GameObject_SmellBundle"), &EffectDesc)))
			return;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -300.f, 0.f, 1.f));

		g_bSmellReset = true;
	}

	Safe_Release(pGameInstance);
}

CSmellBundle* CSmellBundle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSmellBundle* pInstance = new CSmellBundle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSmellBundle");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CSmellBundle::Clone(void* pArg)
{
	CSmellBundle* pInstance = new CSmellBundle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSmellBundle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSmellBundle::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
}
