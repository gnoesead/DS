#include "pch.h"
#include "..\Public\RotationMapObject.h"

#include "GameInstance.h"

CRotationMapObject::CRotationMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CRotationMapObject::CRotationMapObject(const CRotationMapObject& rhs)
	: CMapObject(rhs)
{
}

HRESULT CRotationMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRotationMapObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_MapObject_Info.iMapObjectType = MAPOBJECT_ROTATION;

	return S_OK;
}

void CRotationMapObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_bTriggerTurnRoom || pGameInstance->Get_DIKeyDown(DIK_L) )
	{
		m_bTriggerTurnRoom = false;
		m_bTurn = true;
		_uint iRan = 0;

		if ((_int)(fabsf(m_vTargetRotAngle.x / 90.f)) % 2 == 1)
			iRan = (rand() % 2);
		else
			iRan = (rand() % 4);

		switch (iRan)
		{
		case ROT_X_PLUS:
			m_vTargetRotAngle.x += 90.f;
			m_eRotation = ROT_X_PLUS;
			break;
		case ROT_X_MINUS:
			m_vTargetRotAngle.x -= 90.f;
			m_eRotation = ROT_X_MINUS;
			break;
		case ROT_Z_PLUS:
			m_vTargetRotAngle.z += 90.f;
			m_eRotation = ROT_Z_PLUS;
			break;
		case ROT_Z_MINUS:
			m_vTargetRotAngle.z -= 90.f;
			m_eRotation = ROT_Z_MINUS;
			break;
		}
	}

	if (m_bTurn)
		TurnRoom(TimeDelta);

	Safe_Release(pGameInstance);
}

void CRotationMapObject::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CRotationMapObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CRotationMapObject* CRotationMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRotationMapObject* pInstance = new CRotationMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRotationMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRotationMapObject::Clone(void* pArg)
{
	CRotationMapObject* pInstance = new CRotationMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRotationMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotationMapObject::Free()
{
	__super::Free();
}

void CRotationMapObject::TurnRoom(_double TimeDelta)
{
	switch (m_eRotation)
	{
	case ROT_X_PLUS:
		m_vRotAngle.x += XMConvertToRadians(90.f / (_float)m_RotTime);
		if (m_vRotAngle.x > m_vTargetRotAngle.x)
		{
			m_vRotAngle.x = m_vTargetRotAngle.x;
			m_bTurn = false;
		}
		break;
	case ROT_X_MINUS:
		m_vRotAngle.x -= XMConvertToRadians(90.f / (_float)m_RotTime);
		if (m_vRotAngle.x < m_vTargetRotAngle.x)
		{
			m_vRotAngle.x = m_vTargetRotAngle.x;
			m_bTurn = false;
		}
		break;
	case ROT_Z_PLUS:
		m_vRotAngle.z += XMConvertToRadians(90.f / (_float)m_RotTime);
		if (m_vRotAngle.z > m_vTargetRotAngle.z)
		{
			m_vRotAngle.z = m_vTargetRotAngle.z;
			m_bTurn = false;
		}
		break;
	case ROT_Z_MINUS:
		m_vRotAngle.z -= XMConvertToRadians(90.f / (_float)m_RotTime);
		if (m_vRotAngle.z < m_vTargetRotAngle.z)
		{
			m_vRotAngle.z = m_vTargetRotAngle.z;
			m_bTurn = false;
		}
		break;
	default:
		break;
	}

	m_pTransformCom->Rotation(_float3(m_vRotAngle.x, 0.f, m_vRotAngle.z));
}
