#include "pch.h"
#include "..\Public\AtkCollider.h"

#include "GameInstance.h"
#include "Camera_Manager.h"
#include "AtkCollManager.h"
#include "Player_Battle_Combo.h"
#include "GroundSmoke.h"


CAtkCollider::CAtkCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CAtkCollider::CAtkCollider(const CAtkCollider& rhs)
	: CGameObject(rhs)
{
}

_bool CAtkCollider::Get_IsAttack(CGameObject* pHitObj)
{
	if (nullptr == pHitObj)
		return false;

	if (1 > m_AtkObj.size())
		return false;
	else
	{
		_uint iCount = { 0 };

		for (auto iter = m_AtkObj.begin(); iter != m_AtkObj.end(); iter++)
		{
			if (pHitObj == (*iter))
				return true;

			iCount++;
		}

		if (m_AtkObj.size() <= iCount)
		{
			return false;
		}
	}

	return false;
}

void CAtkCollider::Reset_AtkCollider(ATKCOLLDESC* pAtkCollDesc)
{
	ZeroMemory(&m_AtkCollDesc, sizeof m_AtkCollDesc);

	m_AtkCollDesc = *pAtkCollDesc;
	Safe_AddRef(m_AtkCollDesc.pParentTransform);

	Setting_AtkCollDesc();

	m_pColliderCom->ReMake_Collider(m_AtkCollDesc.ColliderDesc.vPosition, m_AtkCollDesc.ColliderDesc.vSize.x, m_pTransformCom->Get_WorldMatrix());
	Set_Dead(false);
	m_dTimeAcc = 0.0;

	Setting_Trigger();

	if (true == m_AtkCollDesc.bBullet)
	{
		if (nullptr != m_AtkCollDesc.pEffectTag)
			CEffectPlayer::Get_Instance()->Play(m_AtkCollDesc.pEffectTag, m_pTransformCom, &m_AtkCollDesc.EffectWorldDesc);
	}
}

void CAtkCollider::Add_AtkObejct(CGameObject* pHitObj)
{
	if (nullptr == pHitObj)
		return;

	if (1 > m_AtkObj.size())
		m_AtkObj.emplace_back(pHitObj);
	else
	{
		_uint iCount = { 0 };

		for (auto iter = m_AtkObj.begin(); iter != m_AtkObj.end(); iter++)
		{
			if (pHitObj == (*iter))
				break;

			iCount++;
		}

		if (m_AtkObj.size() <= iCount)
		{
			m_AtkObj.emplace_back(pHitObj);
		}
	}
}

HRESULT CAtkCollider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAtkCollider::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_AtkCollDesc, pArg, sizeof m_AtkCollDesc);
	Safe_AddRef(m_AtkCollDesc.pParentTransform);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Setting_Trigger();

	if (true == m_AtkCollDesc.bBullet)
	{
		CEffectPlayer::Get_Instance()->Play(m_AtkCollDesc.pEffectTag, m_pTransformCom, &m_AtkCollDesc.EffectWorldDesc);
	}

	return S_OK;
}

void CAtkCollider::Tick(_double dTimeDelta)
{
	if (true == m_isDead)
		return;

	__super::Tick(dTimeDelta);

	m_dTimeAcc += dTimeDelta;

	Update_Trigger(dTimeDelta);


	if (m_pColliderCom->Get_Coll())
	{
		++m_iCollCount;
	}
}

void CAtkCollider::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
	{
		Level_House_Dead(dTimeDelta);
	}
	else if (pGameInstance->Get_CurLevelIdx() == LEVEL_FINALBOSS)
	{
		Level_FinalBoss_Dead(dTimeDelta);
	}
	else
	{
		if (m_AtkCollDesc.dLifeTime < m_dTimeAcc)
		Reset_Dead();
	}

	Safe_Release(pGameInstance);
#ifdef _DEBUG
	if (FAILED(m_pRendererCom->Add_DebugGroup(m_pColliderCom)))
		return;
#endif // _DEBUG
}

HRESULT CAtkCollider::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	return S_OK;
}


void CAtkCollider::Update_Trigger(_double dTimeDelta)
{
	switch (m_AtkCollDesc.eBulletType)
	{
	case CAtkCollider::TYPE_DEFAULT:
		Tick_Default(dTimeDelta);
		break;
	case CAtkCollider::TYPE_BULLET:
		Tick_BaseBullet(dTimeDelta);
		break;
	case CAtkCollider::TYPE_AKAZA_BULLET_EFFECT:
		Tick_AkazaBullet(dTimeDelta);
		break;
	case CAtkCollider::TYPE_KYOGAI_BULLET:
		Tick_KyogaiBullet(dTimeDelta);
		break;
	case CAtkCollider::TYPE_KYOGAI_DELAY_BULLET:
		Tick_KyogaiDelayBullet(dTimeDelta);
		break;
	case CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET:
		Tick_KyogaiVerticalBullet(dTimeDelta);
		break;
	case CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET:
		Tick_KyogaiHorizonBullet(dTimeDelta);
		break;
	case CAtkCollider::TYPE_KYOGAI_LIAR_BULLET:
		Tick_KyogaiLiarBullet(dTimeDelta);
		break;
	case CAtkCollider::TYPE_BULLET_WEB:
		Tick_WebBullet(dTimeDelta);
		break;
	case CAtkCollider::TYPE_BULLET_WEB_FULL:
		Tick_WebBullet(dTimeDelta);
		break;
	}
}

void CAtkCollider::Setting_Trigger()
{
	switch (m_AtkCollDesc.eBulletType)
	{

	case CAtkCollider::TYPE_BULLET:
		Setting_BaseBullet();
		break;
	case CAtkCollider::TYPE_AKAZA_BULLET_EFFECT:
		Setting_AkazaBullet();
		break;
	case CAtkCollider::TYPE_KYOGAI_BULLET:
		Setting_KyogaiBullet();
		break;
	case CAtkCollider::TYPE_KYOGAI_DELAY_BULLET:
		Setting_KyogaiDelayBullet();
		break;
	case CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET:
		Setting_KyogaiVerticalBullet();
		break;
	case CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET:
		Setting_KyogaiHorizonBullet();
		break;
	case CAtkCollider::TYPE_KYOGAI_LIAR_BULLET:
		Setting_KyogaiLiarBullet();
		break;
	case CAtkCollider::TYPE_BULLET_WEB:
		Setting_WebBullet();
		break;
	case CAtkCollider::TYPE_BULLET_WEB_FULL:
		Setting_WebBullet_Full();
		break;
	}
}

void CAtkCollider::Tick_Default(_double dTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix() * m_AtkCollDesc.pParentTransform->Get_WorldMatrix(), dTimeDelta);
}

void CAtkCollider::Tick_BaseBullet(_double dTimeDelta)
{
	if (true == m_AtkCollDesc.bBullet)
	{

		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);

		CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
		EffectWorldDesc.fScale = 0.5f;

		m_pTransformCom->Go_Straight(dTimeDelta * m_AtkCollDesc.Speed);
		// 두번 째 인자에 원하는 원하는 시간(주기 넣어주면 됨)

		if (Event_Time(dTimeDelta, 0.10, m_dTimeAcc))
		{
			CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
		}
		if (Event_Time(dTimeDelta, 0.25, m_dTimeAcc))
		{
			CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
		}
		if (Event_Time(dTimeDelta, 0.40, m_dTimeAcc))
		{
			CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
		}
		if (Event_Time(dTimeDelta, 0.55, m_dTimeAcc))
		{
			CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
		}
		if (Event_Time(dTimeDelta, 0.60, m_dTimeAcc))
		{
			CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
		}
		if (Event_Time(dTimeDelta, 0.75, m_dTimeAcc))
		{
			CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
		}
		if (Event_Time(dTimeDelta, 0.9, m_dTimeAcc))
		{
			CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
		}
		if (Event_Time(dTimeDelta, 1.05, m_dTimeAcc))
		{
			CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
		}
	}

}

void CAtkCollider::Tick_AkazaBullet(_double dTimeDelta)
{
	if (true == m_AtkCollDesc.bBullet)
	{

		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);

		m_pTransformCom->Go_Straight(dTimeDelta * m_AtkCollDesc.Speed);

	}
}

void CAtkCollider::Tick_KyogaiBullet(_double dTimeDelta)
{
	if (true == m_AtkCollDesc.bBullet)
	{
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);

		if (m_dTimeAcc > 0.5)
		{
			m_pTransformCom->Go_Straight(dTimeDelta * m_AtkCollDesc.Speed);
		}

	}
}

void CAtkCollider::Tick_KyogaiDelayBullet(_double dTimeDelta)
{
	if (true == m_AtkCollDesc.bBullet)
	{
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);

		if (m_dTimeAcc > 0.5)
			m_pTransformCom->Go_Dir(dTimeDelta * m_AtkCollDesc.Speed, XMLoadFloat4(&m_AtkCollDesc.AtkDir));

	}

}

void CAtkCollider::Tick_KyogaiVerticalBullet(_double dTimeDelta)
{
	if (true == m_AtkCollDesc.bBullet)
	{
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);

		if (m_dTimeAcc > 0.5)
			m_pTransformCom->Go_Straight(dTimeDelta * m_AtkCollDesc.Speed);
	}
}

void CAtkCollider::Tick_KyogaiHorizonBullet(_double dTimeDelta)
{
	if (true == m_AtkCollDesc.bBullet)
	{
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);

		if (m_dTimeAcc > 0.5)
			m_pTransformCom->Go_Straight(dTimeDelta * m_AtkCollDesc.Speed);
	}
}

void CAtkCollider::Tick_KyogaiLiarBullet(_double dTimeDelta)
{
	if (true == m_AtkCollDesc.bBullet)
	{
		_vector vTargetPos = m_AtkCollDesc.pParentTransform->Get_State(CTransform::STATE_POSITION);
		_vector vOwnerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);

		if (0.5 < m_dTimeAcc)
		{
			if (Compute::DistCheck(vTargetPos, vOwnerPos, 6.f) == true && m_bStop == false && m_dStopAcc == 0.0)
				m_bStop = true;

			if (m_bStop == true)
			{
				m_dStopAcc += dTimeDelta;
				if (m_dStopAcc > 0.30)
				{
					m_bStop = false;
					m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), Random::Generate_Float(0.f, 360.f));
				}
			}

			if (false == m_bStop)
				m_pTransformCom->Go_Straight(dTimeDelta * m_AtkCollDesc.Speed);
		}

		if (1.5 < m_dTimeAcc && m_dTimeAcc <= 1.5 + dTimeDelta)
		{
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), Random::Generate_Float(0.f, 360.f));
			//m_AtkCollDesc.Speed *= 3.f;
		}
	}
}

void CAtkCollider::Tick_WebBullet(_double dTimeDelta)
{
	if (true == m_AtkCollDesc.bBullet)
	{
		m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);

		if (m_dTimeAcc > 0.5)
			m_pTransformCom->Go_Dir(dTimeDelta * m_AtkCollDesc.Speed, _vector{ 0.0f, 0.0f, -1.0f, 0.0f });
	}
}


void CAtkCollider::Setting_BaseBullet()
{
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * m_AtkCollDesc.pParentTransform->Get_WorldMatrix());
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPos += XMLoadFloat4(&m_AtkCollDesc.AtkDir) * 2.f;
	m_pTransformCom->LookAt(vPos);
}

void CAtkCollider::Setting_AkazaBullet()
{
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * m_AtkCollDesc.pParentTransform->Get_WorldMatrix());
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPos += XMLoadFloat4(&m_AtkCollDesc.AtkDir) * 2.f;
	m_pTransformCom->LookAt(vPos);
}

void CAtkCollider::Setting_KyogaiBullet()
{
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * m_AtkCollDesc.pParentTransform->Get_WorldMatrix());

	_vector vOriginPos = { 126.536f, 0.f, 123.840f, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOriginPos);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += XMVector3Normalize(XMLoadFloat4(&m_AtkCollDesc.AtkDir)) * 17.f;
	vPos = XMVectorSetY(vPos, 0.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	_vector vTargetPos = m_AtkCollDesc.pParentTransform->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt_FixY(vTargetPos);

	m_vDir = vTargetPos - vPos;
	m_vDir = XMVector3Normalize(m_vDir);
}

void CAtkCollider::Setting_KyogaiDelayBullet()
{
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * m_AtkCollDesc.pParentTransform->Get_WorldMatrix());

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, 0.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_vDir = XMLoadFloat4(&m_AtkCollDesc.AtkDir);
	m_vDir = XMVector3Normalize(m_vDir);
}

void CAtkCollider::Setting_KyogaiVerticalBullet()
{
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * m_AtkCollDesc.pParentTransform->Get_WorldMatrix());

	_vector vOriginPos = { 126.536f, 0.f, 123.840f, 1.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOriginPos);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += XMVector3Normalize(XMLoadFloat4(&m_AtkCollDesc.AtkDir)) * 17.f;
	vPos = XMVectorSetY(vPos, 0.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->LookAt_FixY(vOriginPos);

	m_vDir = vOriginPos - vPos;
	m_vDir = XMVector3Normalize(m_vDir);

}

void CAtkCollider::Setting_KyogaiHorizonBullet()
{
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * m_AtkCollDesc.pParentTransform->Get_WorldMatrix());

	_vector vOriginPos = { 126.536f, 0.f, 123.840f, 1.f };


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOriginPos);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += XMVector3Normalize(XMLoadFloat4(&m_AtkCollDesc.AtkDir)) * 17.f;
	vPos = XMVectorSetY(vPos, 0.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->LookAt_FixY(vOriginPos);

	m_vDir = vOriginPos - vPos;
	m_vDir = XMVector3Normalize(m_vDir);


}

void CAtkCollider::Setting_KyogaiLiarBullet()
{
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * m_AtkCollDesc.pParentTransform->Get_WorldMatrix());

	_vector vOriginPos = { 126.536f, 0.f, 123.840f, 1.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOriginPos);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos += XMVector3Normalize(XMLoadFloat4(&m_AtkCollDesc.AtkDir)) * 17.f;
	vPos = XMVectorSetY(vPos, 0.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->LookAt_FixY(vOriginPos);

	m_vDir = vOriginPos - vPos;
	m_vDir = XMVector3Normalize(m_vDir);
}

void CAtkCollider::Setting_WebBullet()
{
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * m_AtkCollDesc.pParentTransform->Get_WorldMatrix());

	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	//_vector vOriginPos = { Random::Generate_Float(201.37f, 208.8f), 7.4f, PlayerPos.z + 35.0f, 1.f };
	_vector vOriginPos = { Random::Generate_Float(201.37f, 208.8f), 7.4f, 381.8f, 1.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOriginPos);
}

void CAtkCollider::Setting_WebBullet_Full()
{
	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * m_AtkCollDesc.pParentTransform->Get_WorldMatrix());

	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_int index = CAtkCollManager::GetInstance()->Get_WebFull_Index();
	_float fX = 0.0f;

	fX = 201.5f + (_float)index * 0.5f;

	//_vector vOriginPos = { fX, 7.4f, PlayerPos.z + 35.0f, 1.f };
	_vector vOriginPos = { fX, 7.4f, 381.8f, 1.f };

	if (index <= 13)
		index++;
	else
		index = 0;
	CAtkCollManager::GetInstance()->Set_WebFull_Index(index);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOriginPos);
}

void CAtkCollider::Level_House_Dead(_double dTimeDelta)
{
	if (m_AtkCollDesc.bBullet == true)
	{
		if (m_dTimeAcc > 0.5)
		{
			Check_OutLine();
		}
	}
	

	if (m_AtkCollDesc.dLifeTime < m_dTimeAcc)
		Reset_Dead();
}

void CAtkCollider::Level_FinalBoss_Dead(_double dTimeDelta)
{
	if (m_AtkCollDesc.bBullet == true && m_AtkCollDesc.eBulletType == CAtkCollider::TYPE_BULLET)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float vPosX = XMVectorGetX(vPos);
		_float vPosY = XMVectorGetY(vPos);
		_float vPosZ = XMVectorGetZ(vPos);

		if ( (157.f < vPosX) || (vPosX < 100.f) || (157.f < vPosZ) || (vPosZ < 100.f) || (vPosY < 0.f))
		{
			if (vPosY < 0.1f)
			{
				//아카자 개방 장풍 땅에 닿았을 때 이펙트 여기에 추가

				CEffectPlayer::EFFECTWORLDDESC EffectDesc;
				EffectDesc.fScale = 0.5f;

				CEffectPlayer::Get_Instance()->Play("Akaza_Stomp_Small", m_pTransformCom, &EffectDesc);

				Create_GroundSmoke(CGroundSmoke::SMOKE_JENITSU_HIKI);
				Create_GroundSmoke(CGroundSmoke::SMOKE_SMESHSPREAD);
			}
			CCameraManager::GetInstance()->Camera_Shake(0.5, 150);
			Reset_Dead();
		}
		if (m_AtkObj.size() >= 1)
			Reset_Dead();
	}
	
	if (m_AtkCollDesc.dLifeTime < m_dTimeAcc)
	{
		if (m_AtkCollDesc.bBullet == true && m_AtkCollDesc.eBulletType == CAtkCollider::TYPE_AKAZA_BULLET_EFFECT)
		{
			//아카자 와다다다패턴 이펙트 죽기전 파티클
		}
		Reset_Dead();
	}
}

void CAtkCollider::Check_OutLine()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float vMovePosX = m_AtkCollDesc.ColliderDesc.vPosition.x;

	_float vPosX = XMVectorGetX(vPos);
	_float vPosZ = XMVectorGetZ(vPos);

	if (((140.f < vPosZ) || (vPosZ < 108.f)))
		vPos = XMVectorSetX(vPos, vPosX + vMovePosX);

	if (((143.f < vPosX) || (vPosX < 109.f)))
		vPos = XMVectorSetZ(vPos, vPosZ + vMovePosX);

	if (m_dTimeAcc > 0.9)
	{
		if (((143.f < vPosX) || (vPosX < 109.f)) || ((140.f < vPosZ) || (vPosZ < 106.5f)))
		{
			if (m_bLineOut == false)
			{
				m_bLineOut = true;
				// 이펙트 생성

				CGameInstance* pGameInstance = CGameInstance::GetInstance();
				Safe_AddRef(pGameInstance);
				_uint iCurIdx = pGameInstance->Get_CurLevelIdx();

				Create_GroundSmoke(CGroundSmoke::SMOKE_BLADEDISAPPEAR, vPos);
				Safe_Release(pGameInstance);
			}
		}

	}

}

void CAtkCollider::Reset_Dead()
{
	Safe_Release(m_AtkCollDesc.pParentTransform);
	m_AtkCollDesc.pParentTransform = nullptr;
	CAtkCollManager::GetInstance()->Collect_Collider(this);
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	m_bLineOut = false;

	m_AtkObj.clear();
	m_dTimeAcc = 0.0;
	m_dStopAcc = 0.0;
	m_iCollCount = 0;

	Set_Dead();
}

void CAtkCollider::Create_GroundSmoke(CGroundSmoke::SMOKE_TYPE eSmokeType, _fvector vOffsetPos)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();

	CEffectW::EFFECTWDESC EffectWDesc;
	EffectWDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vOffsetPos;
	EffectWDesc.eEffectWType = CEffectW_Manager::EFFECT_GROUNDSMOKE;
	EffectWDesc.iNumX = 6; EffectWDesc.iNumY = 6;

	switch (eSmokeType)
	{
	case CGroundSmoke::SMOKE_BLADEDISAPPEAR:
		EffectWDesc.vPos = vOffsetPos;
		EffectWDesc.vStartPosX = { -0.f,0.f }; EffectWDesc.vStartPosY = { 0.7f,1.2f }; EffectWDesc.vStartPosZ = { -0.0f,0.0 };
		EffectWDesc.vFrameSpeed = { 0.02f , 0.03f };
		EffectWDesc.vStartSizeX = { 2.5f ,	3.0f }; EffectWDesc.vStartSizeY = { 6.8f , 7.f };
		EffectWDesc.vSpeedX = { 0.0f , 0.0f }; EffectWDesc.vSpeedY = { 4.f , 4.5f }; EffectWDesc.vSpeedZ = { 0.0f , 0.f };
		EffectWDesc.vSizeSpeedX = { 0.5f , 1.0f }; EffectWDesc.vSizeSpeedY = { 3.0f , 4.0f };
		EffectWDesc.vStartFrame = { 0.f , 8.f };
		EffectWDesc.fGravity = { 12.f };

		if (LEVEL_HOUSE == iCurIdx)
			EffectWDesc.vColor = { 0.8f, 0.9f };
		else
			EffectWDesc.vColor = { 0.4f, 0.6f };

		for (_uint i = 0; i < 1; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);
		break;
	case CGroundSmoke::SMOKE_BLADE:

		EffectWDesc.vStartPosX = { -0.f,0.f }; EffectWDesc.vStartPosY = { 0.0f,0.2f }; EffectWDesc.vStartPosZ = { 5.0f,5.0 };
		EffectWDesc.vFrameSpeed = { 0.005f , 0.010f };
		EffectWDesc.vStartSizeX = { 0.5f ,	1.0f }; EffectWDesc.vStartSizeY = { 0.5f , 1.0f };
		EffectWDesc.vSpeedX = { 0.0f , 0.0f }; EffectWDesc.vSpeedY = { 0.5f , 1.0f }; EffectWDesc.vSpeedZ = { 0.0f , 0.f };
		EffectWDesc.vSizeSpeedX = { 0.0f , 0.0f }; EffectWDesc.vSizeSpeedY = { 0.5f , 1.0f };
		EffectWDesc.vStartFrame = { 0.f , 4.f };

		for (_uint i = 0; i < 1; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_GROUNDSMOKE, &EffectWDesc);
		break;
	default:
		break;
	}

	Safe_Release(pGameInstance);

}

void CAtkCollider::Setting_AtkCollDesc()
{
	//풀링메모리 재사용하는거라 초기화 필요
	m_pColliderCom->Set_Hit_Small(false);
	m_pColliderCom->Set_Hit_ConnectSmall(false);
	m_pColliderCom->Set_Hit_Big(false);
	m_pColliderCom->Set_Hit_Blow(false);
	m_pColliderCom->Set_Hit_Spin(false);
	m_pColliderCom->Set_Hit_Upper(false);
	m_pColliderCom->Set_Hit_BigBlow(false);
	m_pColliderCom->Set_Hit_Bound(false);
	m_pColliderCom->Set_Hit_CutScene(false);
	m_pColliderCom->Set_Hit_Hekireki(false);
	m_pColliderCom->Set_Hit_Swamp(false);
	m_pColliderCom->Set_Hit_Web(false);

	//값 넣어주기
	if (TYPE_SMALL == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_Small(true);
	else if (TYPE_CONNECTSMALL == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_ConnectSmall(true);
	else if (TYPE_BIG == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_Big(true);
	else if (TYPE_BLOW == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_Blow(true);
	else if (TYPE_BIGBLOW == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_BigBlow(true);
	else if (TYPE_SPIN == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_Spin(true);
	else if (TYPE_UPPER == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_Upper(true);
	else if (TYPE_BOUND == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_Bound(true);
	else if (TYPE_CUTSCENE == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_CutScene(true);
	else if (TYPE_HEKIREKI == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_Hekireki(true);
	else if (TYPE_SWAMP == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_Swamp(true);
	else if (TYPE_WEB == m_AtkCollDesc.eAtkType)
		m_pColliderCom->Set_Hit_Web(true);

	m_pColliderCom->Set_AtkDir(m_AtkCollDesc.AtkDir);
	m_pColliderCom->Set_fDamage(m_AtkCollDesc.fDamage);

}


HRESULT CAtkCollider::Add_Components()
{
	/* for.Com_Sphere */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom, &m_AtkCollDesc.ColliderDesc)))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CAtkCollider");
		return E_FAIL;
	}
	Setting_AtkCollDesc();

	m_AtkCollDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_AtkCollDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_AtkCollDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CAtkCollider");
		return E_FAIL;
	}

#ifdef _DEBUG
	/* for.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed to Add_Com_Renderer : CAtkCollider");
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

CAtkCollider* CAtkCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAtkCollider* pInstance = new CAtkCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAtkCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAtkCollider::Clone(void* pArg)
{
	CAtkCollider* pInstance = new CAtkCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAtkCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAtkCollider::Free()
{
	__super::Free();

	m_AtkObj.clear();

	if (nullptr != m_AtkCollDesc.pParentTransform)
		Safe_Release(m_AtkCollDesc.pParentTransform);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);

#ifdef _DEBUG
	Safe_Release(m_pRendererCom);
#endif // _DEBUG
}
