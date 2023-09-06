#include "pch.h"
#include "..\Public\RotationMapObject.h"

#include "GameInstance.h"
#include "Player.h"

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

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
    {
        if (m_MapObject_Info.iRenderGroup <= 4)
        {
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
            _uint iNavi = pPlayer->Get_CurNaviMesh();

            if (iNavi != CLandObject::NAVI_HOUSE_4_0)
            {
                Safe_Release(pGameInstance);
                return;
            }
        }
    }
    
    Safe_Release(pGameInstance);

    __super::Tick(TimeDelta);

	if (m_bTriggerTurnRoom)
	{
		m_bTriggerTurnRoom = false;
		m_bTurn = true;

		switch (m_eRotation)
		{
		case ROT_X_PLUS:
			m_vTargetRotAngle.x += 90.f;
			break;
		case ROT_X_MINUS:
			m_vTargetRotAngle.x -= 90.f;
			break;
		case ROT_Z_PLUS:
			m_vTargetRotAngle.z += 90.f;
			break;
		case ROT_Z_MINUS:
			m_vTargetRotAngle.z -= 90.f;
			break;
		}
	}

    if (m_bTurn)
        TurnRoom(TimeDelta);

   
}

void CRotationMapObject::LateTick(_double TimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);


    if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
    {
        if (m_MapObject_Info.iRenderGroup <= 4)
        {
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
            _uint iNavi = pPlayer->Get_CurNaviMesh();

            if (iNavi != CLandObject::NAVI_HOUSE_4_0)
            {
                Safe_Release(pGameInstance);
                return;
            }
        }
    }

    Safe_Release(pGameInstance);

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