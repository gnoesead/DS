#include "pch.h"
#include "..\Public\EffectPartsObject.h"

#include "GameInstance.h"
#include "Bone.h"

CEffectPartsObject::CEffectPartsObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffectPartsObject::CEffectPartsObject(const CEffectPartsObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffectPartsObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectPartsObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_PartsDesc, pArg, sizeof m_PartsDesc);
	Safe_AddRef(m_PartsDesc.pBone);
	Safe_AddRef(m_PartsDesc.pParentTransform);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(_float3(100.0f, 100.0f, 100.0f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CEffectPartsObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CEffectPartsObject::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() *
		m_PartsDesc.pBone->Get_OffsetMatrix() *
		Remove::Scale(m_PartsDesc.pBone->Get_CombinedTransformationMatrix()) *
		m_PartsDesc.pBone->Get_PivotMatrix() *
		m_PartsDesc.pParentTransform->Get_WorldMatrix());
}

void CEffectPartsObject::Play_Effect(const char* pEffectTag, CEffectPlayer::EFFECTWORLDDESC* pEffectWorldDesc)
{
	//CEffectPlayer::Get_Instance()->PlayPartsEffect(pEffectTag, this, pEffectWorldDesc);
}

HRESULT CEffectPartsObject::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

CEffectPartsObject* CEffectPartsObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffectPartsObject* pInstance = new CEffectPartsObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffectPartsObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffectPartsObject::Clone(void* pArg)
{
	CEffectPartsObject* pInstance = new CEffectPartsObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffectPartsObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectPartsObject::Free()
{
	__super::Free();

	Safe_Release(m_PartsDesc.pBone);
	Safe_Release(m_PartsDesc.pParentTransform);

	Safe_Release(m_pTransformCom);
}
