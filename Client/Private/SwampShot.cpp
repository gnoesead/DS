#include "pch.h"
#include "..\Public\SwampShot.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "Player.h"
#include "PlayerManager.h"

#include "SwampManager.h"
#include "Swamp.h"
#include "WaterParticleEffect.h"

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
		//ÃÊ±â À§Ä¡ ¼³Á¤
		m_pTransformCom->Scaling(_float3{1.0f, 1.0f, 1.0f});
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_WorldInfo.vPosition));
	}

	if (m_ShotDesc.iType == 4)
	{
		m_DuDudgePos[0] = { 430.18f, 3.35f, 320.6f, 1.0f };
		m_DuDudgePos[1] = { 419.84f, 3.35f, 294.57f, 1.0f };
		m_DuDudgePos[2] = { 423.05f, 3.35f, 320.9f, 1.0f };
		m_DuDudgePos[3] = { 430.24f, 3.35f, 294.36f, 1.0f };
		m_DuDudgePos[4] = { 424.16f, 3.35f, 312.7f, 1.0f };
		m_DuDudgePos[5] = { 419.02f, 3.35f, 308.9f, 1.0f };
		m_DuDudgePos[6] = { 430.5f, 3.35f, 308.3f, 1.0f };
		m_DuDudgePos[7] = { 420.01f, 3.35f, 325.34f, 1.0f };
		m_DuDudgePos[8] = { 426.0f, 3.35f, 301.7f, 1.0f };
		m_DuDudgePos[9] = { 423.14f, 3.35f, 294.6f, 1.0f };
	}

	if(m_ShotDesc.iType != 2)
		Create_MySwampEffect();

	return S_OK;
}

void CSwampShot::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	// 0:½Ì±Û, 1:Äõµå, 2:Å«ÀåÆÇ, 3:½º¿ÑÇÎ 4:µÎ´õÁã
	if (m_ShotDesc.iType == 0)
		Tick_Type_Single(dTimeDelta);
	else if (m_ShotDesc.iType == 1)
		Tick_Type_Quad(dTimeDelta);
	else if (m_ShotDesc.iType == 2)
		Tick_Type_Big(dTimeDelta);
	else if (m_ShotDesc.iType == 3)
		Tick_Type_Swamping(dTimeDelta);
	else if (m_ShotDesc.iType == 4)
		Tick_Type_DuDudge(dTimeDelta);


	m_dDelay_All += dTimeDelta;

	if (m_ShotDesc.iType == 4)
	{
		if (m_dDelay_All > 100.0)
			m_isDead = true;
	}
	else
	{
		if (m_dDelay_All > 10.0)
		{
			m_isDead = true;

			if(nullptr != m_pMySwamp)
				m_pMySwamp->Set_Pattern(CSwamp::PATTERN_THROWAWAY_NOWATEREEFCT);
		}
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

void CSwampShot::Tick_Type_Single(_double dTimeDelta)
{
	_vector AtkDir = { 0.0f, 0.0f, 1.0f, 0.0f };
	if (m_isFirst)
	{
		m_isFirst = false;

		_vector AtkDir = { 0.0f, 0.0f, 1.0f, 0.0f };
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 0.0f, 0.0f), 10.0,
			CAtkCollider::TYPE_SWAMP, AtkDir, 3.0f);
	}
}

void CSwampShot::Tick_Type_Quad(_double dTimeDelta)
{
	_vector AtkDir = { 0.0f, 0.0f, 1.0f, 0.0f };
	if (m_isFirst)
	{
		m_isFirst = false;

		_vector AtkDir = { 0.0f, 0.0f, 1.0f, 0.0f };
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 0.0f, 0.0f), 10.0,
			CAtkCollider::TYPE_SWAMP, AtkDir, 3.0f);
	}

	if (m_dDelay_All < 1.3f)
	{

	}
	else if (1.5f <= m_dDelay_All && m_dDelay_All < 2.0f)
	{
		m_pTransformCom->Go_Dir(dTimeDelta, -Calculate_Dir_From_Pos(m_ShotDesc.MonsterPos));

		Create_SwampWaterEffect(dTimeDelta);
	}
	else if (2.0f <= m_dDelay_All && m_dDelay_All < 4.5f)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		_int PlayerIndex = CPlayerManager::GetInstance()->Get_PlayerIndex();
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), PlayerIndex));
		_vector vPlayerPos = (pPlayer->Get_TransformCom())->Get_State(CTransform::STATE_POSITION);
		_float4 PlayerPos;
		XMStoreFloat4(&PlayerPos, vPlayerPos);

		m_pTransformCom->LerpVector(Calculate_Dir_From_Pos(PlayerPos), 0.0001f);
		m_pTransformCom->Go_Dir(dTimeDelta, Calculate_Dir_From_Pos(PlayerPos));

		Create_SwampWaterEffect(dTimeDelta);

		Safe_Release(pGameInstance);
	}
}

void CSwampShot::Tick_Type_Big(_double dTimeDelta)
{
	_vector AtkDir = { 0.0f, 0.0f, 1.0f, 0.0f };
	
	if (0.5f < m_dDelay_All )
	{
		if (m_isFirst)
		{
			m_isFirst = false;

			_vector AtkDir = { 0.0f, 0.0f, 1.0f, 0.0f };
			Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(5.0f, 5.0f, 5.0f), _float3(0.f, 0.0f, 0.0f), 3.0,
				CAtkCollider::TYPE_SWAMP, AtkDir, 3.0f);
		}
	}
}

void CSwampShot::Tick_Type_Swamping(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_int PlayerIndex = CPlayerManager::GetInstance()->Get_PlayerIndex();
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), PlayerIndex));
	_vector vPlayerPos = (pPlayer->Get_TransformCom())->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

	Safe_Release(pGameInstance);
}

void CSwampShot::Tick_Type_DuDudge(_double dTimeDelta)
{
	_vector AtkDir = { 0.0f, 0.0f, 1.0f, 0.0f };

	if (3.5f < m_dDelay_All)
	{
		if (m_isFirst)
		{
			m_isFirst = false;

			_vector AtkDir = { 0.0f, 0.0f, 1.0f, 0.0f };
			Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 0.0f, 0.0f), 100.0,
				CAtkCollider::TYPE_SWAMP, AtkDir, 3.0f);
		}

		if (Calculate_Distance_From_Pos(m_DuDudgePos[m_ShotDesc.iDududgeIndex]) > 0.2f)
		{
			m_pTransformCom->Go_Dir(dTimeDelta, Calculate_Dir_From_Pos(m_DuDudgePos[m_ShotDesc.iDududgeIndex]));
		}
	}
}

_vector CSwampShot::Calculate_Dir_From_Pos(_float4 Pos)
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float4 MyPos;
	XMStoreFloat4(&MyPos, vMyPos);

	Pos.y = MyPos.y;
	_vector vTarget = XMLoadFloat4(&Pos);

	return XMVector3Normalize(vTarget - vMyPos);
}

_float CSwampShot::Calculate_Distance_From_Pos(_float4 Pos)
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float4 MyPos;
	XMStoreFloat4(&MyPos, vMyPos);

	Pos.y = MyPos.y;
	_vector vTarget = XMLoadFloat4(&Pos);

	_float fDistance = Convert::GetLength(vTarget - vMyPos);

	return fDistance;
}

void CSwampShot::Create_SwampWaterEffect(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_dWaterEffectAccTime += dTimeDelta;
	m_dRemainSwampEffectAccTime += dTimeDelta;

	/*if (m_dWaterEffectAccTime > 0.12)
	{
		CWaterParticleEffect::EFFECTDESC EffectParticleDesc;
		EffectParticleDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f , -1.f , 0.f , 0.f);
		for (_uint i = 0; i < 8; ++i)
			pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_WaterParticleEffect"), &EffectParticleDesc);

		m_dWaterEffectAccTime = 0.0;
	}*/

	if (m_dRemainSwampEffectAccTime > 0.11)
	{
		CSwamp::EFFECTDESC EffectSwampDesc;
		EffectSwampDesc.eType = CSwamp::TYPE_REMAIN;
		EffectSwampDesc.pTransform = m_pTransformCom;
		pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Swamp"), &EffectSwampDesc);

		m_dRemainSwampEffectAccTime = 0.0;
	}

	Safe_Release(pGameInstance);
}

void CSwampShot::Create_MySwampEffect()
{
	// ´Ë ¼¼ÆÃ
	CSwamp::EFFECTDESC EffectDesc;
	EffectDesc.pOwner = this;
	EffectDesc.pTransform = m_pTransformCom;
	EffectDesc.eType = CSwamp::TYPE_TRAP;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iCurLevelIdx = pGameInstance->Get_CurLevelIdx();

	if (FAILED(pGameInstance->Add_GameObject(iCurLevelIdx, TEXT("Layer_Swamp"), TEXT("Prototype_GameObject_Swamp"), &EffectDesc)))
	{
		Safe_Release(pGameInstance);
		return;
	}

	m_pMySwamp = dynamic_cast<CSwamp*>(pGameInstance->Get_GameObject(iCurLevelIdx, TEXT("Layer_Swamp"), (_uint)pGameInstance->Get_GameObject_ListSize(iCurLevelIdx, TEXT("Layer_Swamp")) - 1));
	if (nullptr == m_pMySwamp)
	{
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
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
