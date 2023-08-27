#include "pch.h"
#include "..\Public\SwampShot.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "Player.h"
#include "PlayerManager.h"

#include "SwampManager.h"

CSwampShot::CSwampShot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSwampShot::CSwampShot(const CSwampShot& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSwampShot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwampShot::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		memcpy(&m_ShotDesc, pArg, sizeof m_ShotDesc);
	}

	if (FAILED(__super::Initialize(&m_ShotDesc.WorldInfo)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	if (nullptr != pArg)
	{
		//초기 위치 설정
		m_pTransformCom->Scaling(_float3{1.0f, 1.0f, 1.0f});
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_WorldInfo.vPosition));
	}

	return S_OK;
}

void CSwampShot::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	_vector AtkDir = { 0.0f, 0.0f, 1.0f, 0.0f };

	if (m_isFirst)
	{
		m_isFirst = false;

		_vector AtkDir = { 0.0f, 0.0f, 1.0f, 0.0f };
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.0f, 0.0f), 5.0,
			CAtkCollider::TYPE_HEKIREKI, AtkDir, 3.0f);
	}




	if (true == m_isDead)
		return;
}

void CSwampShot::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);


#ifdef _DEBUG
	/*if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
		return;*/
#endif
}

HRESULT CSwampShot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CSwampShot::Make_AttackColl(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime, 
	CAtkCollider::ATK_TYPE AtkType, _vector vDir, _float fDmg, 
	CAtkCollider::BULLET_TYPE eBulletType)
{
	CAtkCollider::ATKCOLLDESC AtkCollDesc;
	ZeroMemory(&AtkCollDesc, sizeof AtkCollDesc);

	AtkCollDesc.ColliderDesc.vSize = Size;
	AtkCollDesc.ColliderDesc.vPosition = Pos;

	AtkCollDesc.dLifeTime = DurationTime;

	AtkCollDesc.pParentTransform = m_pTransformCom;

	AtkCollDesc.eAtkType = AtkType;
	AtkCollDesc.eBulletType = eBulletType;

	AtkCollDesc.fDamage = fDmg;

	AtkCollDesc.bBullet = false;

	XMStoreFloat4(&AtkCollDesc.AtkDir, XMVector4Normalize(vDir));
	CAtkCollManager::GetInstance()->Reuse_Collider(pLayerTag, &AtkCollDesc);
}

HRESULT CSwampShot::Add_Components()
{
	
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.dSpeedPerSec = 5.0;
	TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CSwampShot");
		return E_FAIL;
	}


	return S_OK;
}

CSwampShot* CSwampShot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwampShot* pInstance = new CSwampShot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSwampShot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSwampShot::Clone(void* pArg)
{
	CSwampShot* pInstance = new CSwampShot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSwampShot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwampShot::Free()
{
	Safe_Release(m_pTransformCom);

	__super::Free();
}
