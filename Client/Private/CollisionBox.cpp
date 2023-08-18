#include "pch.h"
#include "..\Public\CollisionBox.h"

#include "GameInstance.h"

CCollisionBox::CCollisionBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCollisionBox::CCollisionBox(const CCollisionBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCollisionBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollisionBox::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_CollisionBoxInfo, pArg, sizeof m_CollisionBoxInfo);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CollisionBoxInfo.vPos));
	m_pTransformCom->Scaling(m_CollisionBoxInfo.vScale);
	
	_float4 vColor;
	switch (m_CollisionBoxInfo.iCollisionType)
	{
	case COLLISION_NORMAL:
		vColor = _float4(0.f, 0.f, 1.f, 1.f);
		break;
	case COLLISION_NAVI:
		vColor = _float4(1.f, 1.f, 0.f, 1.f);
		break;
	case COLLISION_INTERACTION:
		vColor = _float4(1.f, 0.f, 1.f, 1.f);
		break;
	default:
		break;
	}

	m_pColliderCom->Set_Color(vColor);

	return S_OK;
}

void CCollisionBox::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix(), TimeDelta);
}

void CCollisionBox::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

#ifdef DEBUG
	if (FAILED(m_pRendererCom->Add_DebugGroup(m_pColliderCom)))
		return;
#endif // DEBUG

	
}

HRESULT CCollisionBox::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollisionBox::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Sphere */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof ColliderDesc);
	ColliderDesc.vSize = m_CollisionBoxInfo.vScale;
	ColliderDesc.vPosition = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CCollisionBox* CCollisionBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollisionBox* pInstance = new CCollisionBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollisionBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCollisionBox::Clone(void* pArg)
{
	CCollisionBox* pInstance = new CCollisionBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollisionBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollisionBox::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);

}
