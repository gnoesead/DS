#include "pch.h"
#include "..\Public\StaticMapObject.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Camera_Manager.h"
#include "MonsterManager.h"

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

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
	{
		if (_tcscmp(m_MapObject_Info.szMeshName, TEXT("Prototype_Component_Model_BigMountain")))
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
			if (pPlayer->Get_CurNaviMesh() == CLandObject::NAVI_VILLAGE_BATTLE)
			{
				if (m_MapObject_Info.iRenderGroup == 0)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
			else
			{
				if (m_MapObject_Info.iRenderGroup == 3 || m_MapObject_Info.iRenderGroup == 2 || m_MapObject_Info.iRenderGroup == 1)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
		}
	}
	else if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
	{
		if (m_MapObject_Info.iRenderGroup <= 4)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
			_uint iNavi = pPlayer->Get_CurNaviMesh();

			if (iNavi == CLandObject::NAVI_HOUSE_0_0)
			{
				if (m_MapObject_Info.iRenderGroup != 0)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
			else if (iNavi == CLandObject::NAVI_HOUSE_1_0 || iNavi == CLandObject::NAVI_HOUSE_1_1)
			{
				if (m_MapObject_Info.iRenderGroup != 1)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
			else if (iNavi == CLandObject::NAVI_HOUSE_2_0)
			{
				if (m_MapObject_Info.iRenderGroup != 2)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
			else if (iNavi == CLandObject::NAVI_HOUSE_3_0)
			{
				if (m_MapObject_Info.iRenderGroup != 3)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
			else if (iNavi == CLandObject::NAVI_HOUSE_4_0)
			{
				if (m_MapObject_Info.iRenderGroup != 4)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
		}
	}

	m_bBlocked = false;

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
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
	{
		if (_tcscmp(m_MapObject_Info.szMeshName, TEXT("Prototype_Component_Model_BigMountain")))
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
			if (pPlayer->Get_CurNaviMesh() == CLandObject::NAVI_VILLAGE_BATTLE)
			{
				if (m_MapObject_Info.iRenderGroup == 0)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
			else
			{
				if (m_MapObject_Info.iRenderGroup == 3 || m_MapObject_Info.iRenderGroup == 2 || m_MapObject_Info.iRenderGroup == 1)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
		}
	}
	else if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
	{
		if (m_MapObject_Info.iRenderGroup <= 3)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
			_uint iNavi = pPlayer->Get_CurNaviMesh();

			if (iNavi == CLandObject::NAVI_HOUSE_0_0)
			{
				if (m_MapObject_Info.iRenderGroup != 0)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
			else if (iNavi == CLandObject::NAVI_HOUSE_1_0 || iNavi == CLandObject::NAVI_HOUSE_1_1)
			{
				if (m_MapObject_Info.iRenderGroup != 1)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
			else if (iNavi == CLandObject::NAVI_HOUSE_2_0)
			{
				if (m_MapObject_Info.iRenderGroup != 2)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
			else if (iNavi == CLandObject::NAVI_HOUSE_3_0)
			{
				if (m_MapObject_Info.iRenderGroup != 3)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
			else if (iNavi == CLandObject::NAVI_HOUSE_4_0)
			{
				if (m_MapObject_Info.iRenderGroup != 4)
				{
					Safe_Release(pGameInstance);
					return;
				}
			}
		}
	}

	__super::LateTick(TimeDelta);

	if ((m_MapObject_Info.iSceneType == SCENE_VILLAGE) && ((m_MapObject_Info.iRenderGroup == 1) || (m_MapObject_Info.iRenderGroup == 2)))
	{
		Check_Camera();

		if (m_bBlocked)
		{
			m_fAlpha -= 4.f * (_float)TimeDelta;

			if (m_fAlpha < 0.1f)
				m_fAlpha = 0.1f;
		}
		else
			m_fAlpha += 4.f * (_float)TimeDelta;

		if (m_fAlpha > 1.f)
			m_fAlpha = 1.f;
	}

	if (_tcscmp(m_MapObject_Info.szMeshName, TEXT("Prototype_Component_Model_Far_01a")) ||
		_tcscmp(m_MapObject_Info.szMeshName, TEXT("Prototype_Component_Model_Far_02a")) ||
		_tcscmp(m_MapObject_Info.szMeshName, TEXT("Prototype_Component_Model_Far_04a")) ||

		(true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f)))
	{
		if (m_bBlocked)
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_TRANSPARENTWALL, this)))
			{
				Safe_Release(pGameInstance);
				return;
			}
		}
		else if (m_MapObject_Info.iRenderGroup == 6 || m_MapObject_Info.iRenderGroup == 7 || m_MapObject_Info.iRenderGroup == 8 || m_MapObject_Info.iRenderGroup == 9)
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EffectNoBloom, this)))
			{
				Safe_Release(pGameInstance);
				return;
			}
		}
		else
		{
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			{
				Safe_Release(pGameInstance);
				return;
			}
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

	if (INTERACTION_ROOMCHANGE0 == m_MapObject_Info.iInteractionType)
	{
		if (Compute::DistCheck(vPlayerPos, vMyPos, 10.f))
		{
			static _bool b = false;

			if (!b)
			{
				_vector vOffset = { 0.f , -1.f , 0.f , 0.f };

				CCameraManager::GetInstance()->Focus_On(vMyPos + vOffset, 2.f);
				b = true;
			}
		}
	}


	// 종이와의 거리
	if (pGameInstance->Get_DIKeyDown(DIK_F) && Compute::DistCheck(vPlayerPos, vMyPos, 2.f))
	{
		m_bChageRoomRealTrigger = true;
	}

	if (m_bChageRoomRealTrigger)
	{
		m_AccTime += TimeDelta;

		if (m_AccTime >= 3.0)
		{
			m_bChangeRoomTrigger = true;
			m_bChageRoomRealTrigger = false;
			m_AccTime = 0.0;
		}
	}

	
	if (m_bChangeRoomTrigger)
	{
		if (m_AccTime == 0.0)
		{
			CCameraManager::GetInstance()->Camera_Shake(1.2, 30);
			m_pRendererCom->Set_RadialBlur();
		}

		m_AccTime += TimeDelta;

		if (!m_bSetInvert && m_AccTime > 0.9)
		{
			m_bSetInvert = true;
			m_pRendererCom->Set_Invert();
		}

		if (m_AccTime >= 1.0)
		{
			m_pRendererCom->Set_Invert();
			m_pRendererCom->Set_RadialBlur();
			m_bChageRoom = true;
			m_bChangeRoomTrigger = false;
		}
	}

	_vector vNextPos;

	if (m_bChageRoom)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 0));
		CPlayer* pPlayer_2 = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 1));

		if (nullptr == pPlayer)
			return;
		if (nullptr == pPlayer_2)
			return;

		CTransform* pTransform = pPlayer->Get_TransformCom();
		_float4 PrePos;

		switch (iInteractionType)
		{
		case INTERACTION_ROOMCHANGE0:
		{vNextPos = XMVectorSet(80.f, 0.f, 5.f, 1.f);
		pPlayer->Change_NaviMesh(CLandObject::NAVI_HOUSE_1_0); // 네비 매쉬 변경
		pPlayer_2->Change_NaviMesh(CLandObject::NAVI_HOUSE_1_0); // 네비 매쉬 변경

		//젠이츠npc에 보내주기
		CMonsterManager::GetInstance()->Set_Zenitsu_IndexPlus(true);
		CMonsterManager::GetInstance()->Set_ResetIndeX_Player(1);

		
		XMStoreFloat4(&PrePos, pTransform->Get_State(CTransform::STATE_POSITION));
		CMonsterManager::GetInstance()->Set_PlayerPrePos(PrePos);

		CLandObject::NAVI_TYPE eType = pPlayer->Get_CurNaviMesh();
		break; }
		case INTERACTION_ROOMCHANGE1:
		vNextPos = XMVectorSet(77.f, 0.f, 62.f, 1.f);
			pPlayer->Change_NaviMesh(CLandObject::NAVI_HOUSE_1_1); // 네비 매쉬 변경
			pPlayer_2->Change_NaviMesh(CLandObject::NAVI_HOUSE_1_1); // 네비 매쉬 변경

			//젠이츠npc에 보내주기
			CMonsterManager::GetInstance()->Set_Zenitsu_IndexPlus(true);
			CMonsterManager::GetInstance()->Set_ResetIndeX_Player(2);

			
			XMStoreFloat4(&PrePos, pTransform->Get_State(CTransform::STATE_POSITION));
			CMonsterManager::GetInstance()->Set_PlayerPrePos(PrePos);

			break;
		case INTERACTION_ROOMCHANGE2:
		vNextPos = XMVectorSet(189.f, 0.f, 30.f, 1.f);
			pPlayer->Change_NaviMesh(CLandObject::NAVI_HOUSE_3_0); // 네비 매쉬 변경
			pPlayer_2->Change_NaviMesh(CLandObject::NAVI_HOUSE_3_0); // 네비 매쉬 변경

			//젠이츠npc에 보내주기
			CMonsterManager::GetInstance()->Set_Zenitsu_IndexPlus(true);
			CMonsterManager::GetInstance()->Set_ResetIndeX_Player(3);

			
			XMStoreFloat4(&PrePos, pTransform->Get_State(CTransform::STATE_POSITION));
			CMonsterManager::GetInstance()->Set_PlayerPrePos(PrePos);

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

	if (m_pRendererCom->Get_GrayScale())
	{
		if (true == pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
		{
			CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));

			_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

			if(Compute::DistCheck(vPlayerPos , m_pTransformCom->Get_State(CTransform::STATE_POSITION) , 25.f))
				m_bSmellOn = true;
		}
	}

	if (g_bSmellReset)
		m_bSmellOn = false;


	if(m_bSmellOn)
	{
		m_fAlpha += 0.33f * (_float)TimeDelta;

		if (m_fAlpha > 1.f)
			m_fAlpha = 1.f;
	}
	else
	{
		m_fAlpha -= 0.33f * (_float)TimeDelta;

		if (m_fAlpha < 0.f)
			m_fAlpha = 0.f;
	}

	Safe_Release(pGameInstance);
}

void CStaticMapObject::Check_Camera()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iLevelIdx = pGameInstance->Get_CurLevelIdx();

	if (CLandObject::NAVI_VILLAGE_BATTLE != dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(iLevelIdx, TEXT("Layer_Player"), 0))->Get_CurNaviMesh())
	{
		Safe_Release(pGameInstance);
		return;
	}

	_float4 vCameraPos = pGameInstance->Get_CameraPosition();

	_vector vCamPos = XMLoadFloat4(&vCameraPos);
	_vector vPlayerPos = dynamic_cast<CTransform*>(pGameInstance->Get_Component(iLevelIdx, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION)
							+ XMVectorSet(0.f , 2.f , 0.f , 0.f);

	if (m_MapObject_Info.iRenderGroup == 2 && XMVectorGetX(vCamPos) <= 433.f)
	{
		m_bBlocked = false;
		Safe_Release(pGameInstance);
		return;
	}
	else if (m_MapObject_Info.iRenderGroup == 1 && XMVectorGetX(vCamPos) > 416.f)
	{
		m_bBlocked = false;
		Safe_Release(pGameInstance);
		return;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		CMesh* pMesh = m_pModelCom->Get_Mesh(i);
		if (nullptr == pMesh)
			return;
			
		if (((m_MapObject_Info.iRenderGroup == 1) && !m_bBlocked) || (((m_MapObject_Info.iRenderGroup == 2) && !m_bBlocked)))
		{
			if (Is_Blocked_Camera(g_hWnd, g_iWinSizeX, g_iWinSizeY, pMesh, vCamPos , vPlayerPos))
			{
				m_bBlocked = true;
			}
		}
	}

	

	Safe_Release(pGameInstance);
}

_bool CStaticMapObject::Is_Blocked_Camera(HWND hWnd, const _uint& iWinSizeX, const _uint& iWinSizeY, CMesh* pMapObjectMesh, _fvector vCamPos , _fvector vPlayerPos)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vRayPos = vCamPos;

	_vector vRayDir = vPlayerPos - vRayPos;


	vRayDir = XMVector3Normalize(vRayDir);

	// 월드 -> 로컬
	_matrix WorldMatrix_Inverse = m_pTransformCom->Get_WorldMatrix_Inverse();

	vRayPos = XMVector3TransformCoord(vRayPos, WorldMatrix_Inverse);
	vRayDir = XMVector3TransformNormal(vRayDir, WorldMatrix_Inverse);

	vRayDir = XMVector3Normalize(vRayDir);


	_uint iIndexNum = pMapObjectMesh->Get_NumIndices();
	_uint iVerticesNum = pMapObjectMesh->Get_NumVertices();

	VTXMODEL* pVertices = { nullptr };
	_ulong* pIndices = { nullptr };

	pVertices = pMapObjectMesh->Get_Vertices();
	pIndices = pMapObjectMesh->Get_Indices();

	_float	fDist = 0.f;

	for (_uint i = 0; i < iIndexNum / 3; ++i)
	{
		_vector v0 = XMLoadFloat3(&pVertices[pIndices[3 * i + 0]].vPosition);
		_vector v1 = XMLoadFloat3(&pVertices[pIndices[3 * i + 1]].vPosition);
		_vector v2 = XMLoadFloat3(&pVertices[pIndices[3 * i + 2]].vPosition);

		if (TriangleTests::Intersects(vRayPos, vRayDir,
			v0, v1, v2, fDist))
		{
			Safe_Release(pGameInstance);
			return true;
		}

		/*if (TriangleTests::Intersects(vRayPos + XMVectorSet(0.f , 1.f , 0.f , 0.f), vRayDir,
			v0, v1, v2, fDist))
		{
			return true;
		}

		if (TriangleTests::Intersects(vRayPos + XMVectorSet(0.f, -1.f, 0.f, 0.f), vRayDir,
			v0, v1, v2, fDist))
		{
			return true;
		}

		if (TriangleTests::Intersects(vRayPos + XMVectorSet(0.f, 0.f, 1.f, 0.f), vRayDir,
			v0, v1, v2, fDist))
		{
			return true;
		}

		if (TriangleTests::Intersects(vRayPos + XMVectorSet(0.f, 0.f, -1.f, 0.f), vRayDir,
			v0, v1, v2, fDist))
		{
			return true;
		}*/


	}

	Safe_Release(pGameInstance);

	return false;
}
