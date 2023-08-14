#include "pch.h"
#include "..\Public\StaticMapObject.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Player.h"

CStaticMapObject::CStaticMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CStaticMapObject::CStaticMapObject(const CStaticMapObject& rhs)
	: CMapObject(rhs)
{
}

HRESULT CStaticMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticMapObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_MapObject_Info.iMapObjectType = MAPOBJECT_STATIC;

	if (7 == m_MapObject_Info.iRenderGroup)
		m_fAlpha = 0.f;

	return S_OK;
}

void CStaticMapObject::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	__super::Tick(TimeDelta);

	switch (m_MapObject_Info.iInteractionType)
	{
	case INTERACTION_DOOROPEN_MANUAL_L:
	case INTERACTION_DOOROPEN_MANUAL_R:
		Interaction_DoorOpen_Manual(TimeDelta);
		break;
	case INTERACTION_DOOROPEN_AUTO:
		Interaction_DoorOpen_Auto(TimeDelta);
		break;
	case INTERACTION_ROOMCHANGE0:
	case INTERACTION_ROOMCHANGE1:
	case INTERACTION_ROOMCHANGE2:
		Room_Change(TimeDelta, m_MapObject_Info.iInteractionType);
		break;
	default:
		break;
	}

	if (7 == m_MapObject_Info.iRenderGroup)
		Control_RenderSmell(TimeDelta);


	if (LEVEL_TRAIN == pGameInstance->Get_CurLevelIdx() && m_MapObject_Info.iRenderGroup != 1)
		Scroll(TimeDelta);

	Safe_Release(pGameInstance);
}

void CStaticMapObject::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (_tcscmp(m_MapObject_Info.szMeshName, TEXT("Prototype_Component_Model_Far_01a")) ||
		_tcscmp(m_MapObject_Info.szMeshName, TEXT("Prototype_Component_Model_Far_02a")) ||
		_tcscmp(m_MapObject_Info.szMeshName, TEXT("Prototype_Component_Model_Far_04a")) ||

		(true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f)))
	{
		if (m_MapObject_Info.iRenderGroup == 6 || m_MapObject_Info.iRenderGroup == 7)
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
				return;
		}
		else
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
				return;
		}
	}

	Safe_Release(pGameInstance);
}

HRESULT CStaticMapObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CStaticMapObject* CStaticMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStaticMapObject* pInstance = new CStaticMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStaticMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStaticMapObject::Clone(void* pArg)
{
	CStaticMapObject* pInstance = new CStaticMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStaticMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticMapObject::Free()
{
	__super::Free();
}

void CStaticMapObject::Interaction_DoorOpen_Manual(_double TimeDelta)
{
	m_pTransformCom->Set_Speed(2.7);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (!m_bOpen)
	{
		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));
		if (nullptr == pPlayerTransform)
		{
			MSG_BOX("Failed to Get Component PlayerTransform");
			return;
		}

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);	
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);		// 

		// 문과의 거리 체크
		if (Compute::DistCheck(vPlayerPos, vMyPos, 2.f))
		{
			// UI 띄우기


			// 키입력하면 열림
			if (pGameInstance->Get_DIKeyDown(DIK_F))
				m_bOpen = true;
		}
	}

	// 문 열림
	if (!m_bStop && m_bOpen)
	{
		if (INTERACTION_DOOROPEN_MANUAL_L == m_MapObject_Info.iInteractionType)
			m_pTransformCom->Go_Left(TimeDelta);
		else
			m_pTransformCom->Go_Right(TimeDelta);

		m_AccTime += TimeDelta;
	}

	// 시간지나면 멈춤
	if (m_AccTime >= 0.42)
		m_bStop = true;

	Safe_Release(pGameInstance);
}

void CStaticMapObject::Interaction_DoorOpen_Auto(_double TimeDelta)
{
	m_pTransformCom->Set_Speed(2.7);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (!m_bOpen)
	{
		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));
		if (nullptr == pPlayerTransform)
		{
			MSG_BOX("Failed to Get Component PlayerTransform");
			return;
		}

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		// 문과의 거리 체크
		if (Compute::DistCheck(vPlayerPos, vMyPos, 6.f))
			m_bOpen = true;
	}

	// 문 열림
	if (!m_bStop && m_bOpen)
	{
		m_pTransformCom->Go_Left(TimeDelta);

		m_AccTime += TimeDelta;
	}

	// 시간지나면 멈춤
	if (m_AccTime >= 0.42)
		m_bStop = true;

	Safe_Release(pGameInstance);
}

void CStaticMapObject::Room_Change(_double TimeDelta, _uint iInteractionType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (nullptr == pPlayerTransform)
	{
		MSG_BOX("Failed to Get Component PlayerTransform");
		return;
	}

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// 종이와의 거리
	if (Compute::DistCheck(vPlayerPos, vMyPos, 6.f))
		m_bChangeRoomTrigger = true;

	if (m_bChangeRoomTrigger)
	{
		m_AccTime += TimeDelta;

		if (m_AccTime >= 1.5)
		{
			m_bChageRoom = true;
			m_bChangeRoomTrigger = false;
		}
	}

	_vector vNextPos;

	if (m_bChageRoom)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
		if (nullptr == pPlayer)
			return;

		switch (iInteractionType)
		{
		case INTERACTION_ROOMCHANGE0:
			vNextPos = XMVectorSet(80.f, 0.f, 5.f, 1.f);
			pPlayer->Change_NaviMesh(CLandObject::NAVI_HOUSE_1_0); // 네비 매쉬 변경
			break;
		case INTERACTION_ROOMCHANGE1:
			vNextPos = XMVectorSet(77.f, 0.f, 62.f, 1.f);
			pPlayer->Change_NaviMesh(CLandObject::NAVI_HOUSE_1_1); // 네비 매쉬 변경
			break;
		case INTERACTION_ROOMCHANGE2:
			vNextPos = XMVectorSet(189.f, 0.f, 30.f, 1.f);
			pPlayer->Change_NaviMesh(CLandObject::NAVI_HOUSE_3_0); // 네비 매쉬 변경
			break;
		}

		// 카메라와 플레이어 이동
		_vector vDist = vNextPos - pPlayerTransform->Get_State(CTransform::STATE_POSITION); // 다음 이동지역과 플레이어 위치 차이

		pPlayerTransform->Set_State(CTransform::STATE_POSITION, vNextPos);

		CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera"), 0));
		CTransform* pCameraTransform = pCamera->Get_Transform();

		_vector vCameraPos = pCameraTransform->Get_State(CTransform::STATE_POSITION);
		pCameraTransform->Set_State(CTransform::STATE_POSITION, vCameraPos + vDist);

		m_bChageRoom = false;
	}

	Safe_Release(pGameInstance);
}

void CStaticMapObject::Control_RenderSmell(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_X))
	{
		m_bSmellOn = !m_bSmellOn;
	}

	if (!m_bSmellOn)
	{
		m_fAlpha -= 1.f * (_float)TimeDelta;

		if (m_fAlpha < 0.f)
			m_fAlpha = 0.f;
	}
	else
	{
		m_fAlpha += 1.f * (_float)TimeDelta;

		if (m_fAlpha > 1.f)
			m_fAlpha = 1.f;
	}

	Safe_Release(pGameInstance);
}
