#include "pch.h"
#include "..\Public\Camera_Free.h"

#include "GameInstance.h"

#include "Player.h"
#include "Character.h"
#include "Fade_Manager.h"
#include "Camera_Manager.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX("Failed to Initialize_Prototype : Camera_Free");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed to Initialize : Camera_Free");
		return E_FAIL;
	}

	Ready_CutInFinish();




	return S_OK;
}

void CCamera_Free::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_bIs_Pos_Set == false) {
		m_bIs_Pos_Set = true;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 136.f,0.f,136.f,1.f });
	}
	
	if (pGameInstance->Get_DIKeyDown(DIK_0))
	{
		if (false == m_bCamChange)
			m_bCamChange = true;
		else if (true == m_bCamChange)
			m_bCamChange = false;
	}
	if (pGameInstance->Get_DIKeyDown(DIK_9))
	{
		if (false == m_bLockMouse)
			m_bLockMouse = true;
		else if (true == m_bLockMouse)
			m_bLockMouse = false;
	}
	

	if (true == m_bLockMouse)
		LockMouse();

	

	Safe_Release(pGameInstance);



	__super::Tick(dTimeDelta);
}

void CCamera_Free::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE) {

		m_Village_Cam_TimeAcc += (_float)dTimeDelta;

		if (m_Village_Cam_TimeAcc < 4.f) {
			CCameraManager::GetInstance()->Zoom_Fix(200.f);
		}
	}*/


	// Camera_Shake
	if (CCameraManager::GetInstance()->Get_Is_Shake_On()) {

		Shake(CCameraManager::GetInstance()->Get_Shake_Time(), CCameraManager::GetInstance()->Get_Shake_Power());

		CCameraManager::GetInstance()->Set_Is_Shake_On(false);
	}

	// Player
	CTransform* m_pTargetTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));
	m_vTargetPos = m_pTargetTransformCom->Get_State(CTransform::STATE_POSITION);

	// Battle_Target
	m_Is_Battle = CFadeManager::GetInstance()->Get_Is_Battle();

	if (pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Monster"), m_Battle_Target_Num) != nullptr) {

		CCharacter* pMon = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Monster"), m_Battle_Target_Num));

		m_Battle_Target_MaxNum = (_uint)pGameInstance->Get_GameObject_ListSize(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Monster")) - 1;

		CTransform* m_pBattleTargetTransformCom = pMon->Get_TransformCom();

		m_vBattleTargetPos = m_pBattleTargetTransformCom->Get_State(CTransform::STATE_POSITION);
	}
	else if (pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Boss")) != nullptr) {

		CCharacter* pBoss = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Boss")));

		CTransform* m_pBattleTargetTransformCom = pBoss->Get_TransformCom();

		m_vBattleTargetPos = m_pBattleTargetTransformCom->Get_State(CTransform::STATE_POSITION);
	}
	else {
		m_Is_Battle = false;
	}


	// Zoom
	if (m_Is_Battle == false) {
		if (pGameInstance->Get_DIKeyState(DIK_S)) {
			CCameraManager::GetInstance()->Zoom_Fix(2.f);
		}
	}

	m_Zoom = CCameraManager::GetInstance()->Get_Zoom();

    // Focus
	m_Is_Focus_On = CCameraManager::GetInstance()->Get_Is_Focus_On();
	m_vFocusPos = CCameraManager::GetInstance()->Get_Focus_Pos();

	// Center
	XMVectorSetY(m_vBattleTargetPos, 0.f);
	m_vBattleCenter = (m_vTargetPos + m_vBattleTargetPos) * 0.5f;

	// Lock_Free
	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE || pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE) {
		m_bIs_LockFree = false;
	}
	else {
		m_bIs_LockFree = true;
	}

	// Lock_On_Change
	if (pGameInstance->Get_DIKeyDown(DIK_RSHIFT)) {
		
		m_Battle_Target_Num++;

		if (m_Battle_Target_Num > m_Battle_Target_MaxNum) {
			m_Battle_Target_Num = 0;
		}
	}

	// Combo_On
	_float dist = XMVectorGetX(XMVector3Length(m_vTargetPos - m_vBattleTargetPos));

	if (pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")) != nullptr) {

		CCharacter* pPlayer = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 0));

		if (pPlayer->Get_Status().iAttackCombo > 1 || pPlayer->Get_Status().iHitCombo > 1) {
			m_bIs_Combo_On = true;
		}
		else {
			m_bIs_Combo_On = false;
		}

		if (dist > 7.f) {
			m_bIs_Combo_On = false;
		}
		
	}

	

	// 플레이어와 몬스터에게 컷씬 상태인지 , 어떤 컷씬인지 받아옴
	if (pGameInstance->Get_DIKeyDown(DIK_7)) {
		m_Is_Cut_In = true;
		m_Cut_In_IsDone = false;
		m_Cut_In_Finish_Type = 0;
	}


	// 카메라 함수
	if (m_bCamChange == true)
	{
		// CutIn
		if (m_Is_Cut_In == true && m_Cut_In_IsDone == false) {

			m_vOffSet = { 0.f, 1.5f, 0.f, 0.f };
			m_vLookOffSet = { 0.f, 1.7f, 0.f, 0.f };

			CutInCamera(dTimeDelta);
		}
		else if (m_Is_Focus_On == true) {

			m_fDistance = { 6.f + m_Zoom};
			m_vOffSet = { 0.f, 1.8f, 0.f, 0.f };
			m_vLookOffSet = { 0.f, 1.f, 0.f, 0.f };
			m_fLookDamping = { 7.f };
			m_fDamping = { 5.f };

			FocusCamera(dTimeDelta);
		}
		else {
			// adventure
			if (m_Is_Battle != true) {

				if (m_bIs_LockFree != true) {
					m_fDistance = { 3.f + m_Zoom };
					m_vOffSet = { 0.f, 1.2f, 0.f, 0.f };
					m_vLookOffSet = { 0.f, 1.2f, 0.f, 0.f };
					m_fLookDamping = { 6.f };
					m_fDamping = { 7.f };
				}
				else {
					m_fDistance = { 6.f + m_Zoom };
					m_vOffSet = { 0.f, 1.8f, 0.f, 0.f };
					m_vLookOffSet = { 0.f, 1.f, 0.f, 0.f };
					m_fLookDamping = { 7.f };
					m_fDamping = { 5.f };
				}

				AdventureCamera(dTimeDelta);

			}
			// Battle
			else if (m_Is_Battle == true) {

				// Side
				if (m_bIs_Combo_On == true) {

					m_fDistance = { 5.3f };
					m_vOffSet = { 0.f, 1.1f, 0.f, 0.f };
					m_vLookOffSet = { 0.f, 0.f, 0.f, 0.f };
					m_fLookDamping = { 7.f };
					m_fDamping = { 6.f };

					SideCamera(dTimeDelta);
				}
				else {
					m_fDistance = { 6.f + m_Zoom };
					m_vOffSet = { 0.f, 1.8f, 0.f, 0.f };
					m_vLookOffSet = { 0.f, 1.f, 0.f, 0.f };
					m_fLookDamping = { 7.f };
					m_fDamping = { 5.f };

					BattleCamera(dTimeDelta);
				}

			}

		}

	}
	else
	{
		FreeCamera(dTimeDelta);
	}

	XMStoreFloat4(&m_fCameraLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	Safe_Release(pGameInstance);
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::FreeCamera(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
		m_pTransformCom->Go_Straight(dTimeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_DOWN) & 0x80)
		m_pTransformCom->Go_Backward(dTimeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
		m_pTransformCom->Go_Left(dTimeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
		m_pTransformCom->Go_Right(dTimeDelta);

	_long MouseMove = { 0 };

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
	{
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (dTimeDelta * MouseMove * Get_Sensitivity()));
	}

	if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), (dTimeDelta * MouseMove * Get_Sensitivity()));
	}

	Safe_Release(pGameInstance);
}

void CCamera_Free::AdventureCamera(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });

	_vector vCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	Turn_Camera(dTimeDelta);

	_vector vDist = vCamPosition - m_vTargetPos;

	_float dist = XMVectorGetX(XMVector3Length(vDist));

	m_vDist = { XMVectorGetX(m_vDist), 0.12f ,XMVectorGetZ(m_vDist), 0.f };
	m_vDist = XMVector3Normalize(m_vDist);

	_vector vDest = m_vTargetPos + m_vOffSet + (m_vDist * m_fDistance);

	_float t = (_float)dTimeDelta * m_fDamping;

	if (t > 1.0f)
		t = 1.0f;
	else if (t < 0.f)
		t = 0.0f;

	_vector CamPos = XMVectorLerp(vCamPosition, vDest, t);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CamPos);

	_vector NewLook = m_vTargetPos + m_vLookOffSet - CamPos;

	NewLook = { XMVectorGetX(NewLook), XMVectorGetY(NewLook) * 0.f ,XMVectorGetZ(NewLook), XMVectorGetW(NewLook) };
	NewLook = XMVector3Normalize(NewLook);

	_float New_t = (_float)dTimeDelta * m_fLookDamping;

	m_pTransformCom->LerpVector(NewLook, New_t);

	
	Safe_Release(pGameInstance);
}

void CCamera_Free::BattleCamera(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });

	_vector vCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_vDist = m_vTargetPos - m_vBattleTargetPos;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(-m_vCameraAngle));

	m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

	m_vDist = { XMVectorGetX(m_vDist), 0.1f ,XMVectorGetZ(m_vDist), XMVectorGetW(m_vDist) };

	m_vDist = XMVector3Normalize(m_vDist);

	_vector vDest = m_vTargetPos + m_vOffSet + (m_vDist * m_fDistance);

	_float t = (_float)dTimeDelta * m_fDamping;

	_vector CamPos = XMVectorLerp(vCamPosition, vDest, t);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CamPos);

	_vector NewLook = m_vBattleCenter + m_vLookOffSet - CamPos;

	NewLook = { XMVectorGetX(NewLook), XMVectorGetY(NewLook) * 0.f ,XMVectorGetZ(NewLook), XMVectorGetW(NewLook) };
	NewLook = XMVector3Normalize(NewLook);

	_float New_t = (_float)dTimeDelta * m_fLookDamping;
	m_pTransformCom->LerpVector(NewLook, New_t);


	Safe_Release(pGameInstance);
}

void CCamera_Free::FocusCamera(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });

	_vector vCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_vDist = vCamPosition - m_vFocusPos;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(0.f));

	m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

	m_vDist = { XMVectorGetX(m_vDist), 0.1f ,XMVectorGetZ(m_vDist), XMVectorGetW(m_vDist) };

	m_vDist = XMVector3Normalize(m_vDist);

	_vector vDest = m_vFocusPos + m_vOffSet + (m_vDist * m_fDistance);

	_float t = (_float)dTimeDelta * m_fDamping;

	_vector CamPos = XMVectorLerp(vCamPosition, vDest, t);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CamPos);


	_vector NewLook = m_vFocusPos + m_vLookOffSet - CamPos;

	NewLook = { XMVectorGetX(NewLook), XMVectorGetY(NewLook) * 0.f ,XMVectorGetZ(NewLook), XMVectorGetW(NewLook) };
	NewLook = XMVector3Normalize(NewLook);

	_float New_t = (_float)dTimeDelta * m_fLookDamping;
	m_pTransformCom->LerpVector(NewLook, New_t);


	Safe_Release(pGameInstance);
}

void CCamera_Free::SideCamera(_double dTimeDelta)
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });

	_vector vCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vRight = m_vTargetPos - m_vBattleTargetPos;


	m_vDist = XMVector3Cross(vRight, vUp);

	m_vDist = { XMVectorGetX(m_vDist), 0.f ,XMVectorGetZ(m_vDist), XMVectorGetW(m_vDist) };

	m_vDist = XMVector3Normalize(m_vDist);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(8.f));

	m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

	_vector vDest = m_vBattleCenter + m_vOffSet + (m_vDist * m_fDistance);

	_float t = (_float)dTimeDelta * m_fDamping;

	_vector CamPos = XMVectorLerp(vCamPosition, vDest, t);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CamPos);


	_vector NewLook = m_vBattleCenter + m_vLookOffSet - CamPos;

	NewLook = { XMVectorGetX(NewLook), 0.5f ,XMVectorGetZ(NewLook), XMVectorGetW(NewLook) };
	NewLook = XMVector3Normalize(NewLook);

	_float New_t = (_float)dTimeDelta * m_fLookDamping;
	m_pTransformCom->LerpVector(NewLook, New_t);


	Safe_Release(pGameInstance);
}

void CCamera_Free::CutInCamera(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });

	_vector vCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	CutInCamDesc Cam = m_Cut_In_Finish[m_Cut_In_Finish_Type][m_Cut_In_Finish_Cam_Num];

	CutInFinish(dTimeDelta, Cam);

	m_Cut_In_Finish_TimeAcc += (_float)dTimeDelta;

	if (m_Cut_In_Finish_TimeAcc > Cam.fLifeTime) {

		m_Cut_In_Finish_TimeAcc = 0.f;

		m_Cut_In_Finish_Cam_Num++;

		if (m_Cut_In_Finish_Cam_Num > m_Cut_In_Finish[m_Cut_In_Finish_Type].size() - 1) {

			m_Cut_In_Finish_Cam_Num = 0;
			m_Cut_In_IsDone = true;
			m_Is_Cut_In = false;

			Safe_Release(pGameInstance);
			return;
		}

	}


	Safe_Release(pGameInstance);
}



void CCamera_Free::CutInFinish(_double dTimeDelta, const CutInCamDesc& Desc)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (Desc.bTarget_Is_Player)
		m_vDist = m_vBattleTargetPos - m_vTargetPos;
	else
		m_vDist = m_vTargetPos - m_vBattleTargetPos;

	m_vDist = XMVector3Normalize(m_vDist);

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, m_vDist));

	_matrix RotationMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(Desc.Angle_Verti));

	m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

	RotationMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(Desc.Angle_Hori));

	m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);


	_vector vDest = m_vTargetPos + m_vOffSet + Desc.vOffSet + (m_vDist * Desc.fDistance);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDest);


	if (Desc.bLookTarget_Is_Player)
		m_pTransformCom->LookAt(m_vTargetPos + m_vLookOffSet + Desc.vOffSet);
	else
		m_pTransformCom->LookAt(m_vBattleTargetPos + m_vLookOffSet + Desc.vOffSet);


	Safe_Release(pGameInstance);
}

void CCamera_Free::Ready_CutInFinish()
{
	CutInCamDesc TanjiroCam1 = { true,true, 20.f, -20.f, 7.f, 0.8f };
	CutInCamDesc TanjiroCam2 = { true,true, 20.f, -20.f, 2.f, 0.5f };
	CutInCamDesc TanjiroCam3 = { true,true, 150.f, 20.f, 3.f, 0.8f };
	CutInCamDesc TanjiroCam4 = { true,true, 20.f, 5.f, 8.f, 1.2f};
	CutInCamDesc TanjiroCam5 = { true,false, 0.f, -30.f, 3.f, 2.3f, {0.f,4.f,0.f} };
	CutInCamDesc TanjiroCam6 = { true,false, 0.f, -10.f, 1.f, 1.5f };

	m_Cut_In_Finish[TANJIRO_FINISH].push_back(TanjiroCam1);
	m_Cut_In_Finish[TANJIRO_FINISH].push_back(TanjiroCam2);
	m_Cut_In_Finish[TANJIRO_FINISH].push_back(TanjiroCam3);
	m_Cut_In_Finish[TANJIRO_FINISH].push_back(TanjiroCam4);
	m_Cut_In_Finish[TANJIRO_FINISH].push_back(TanjiroCam5);
	m_Cut_In_Finish[TANJIRO_FINISH].push_back(TanjiroCam6);
}

void CCamera_Free::Turn_Camera(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });

	m_vAdventureCameraAngle = 0.f;

	if (pGameInstance->Get_DIKeyState(DIK_LEFTARROW) & 0x80) {

		m_vAdventureCameraAngle -= (_float)TimeDelta * 100.f;

		_matrix		RotationMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(m_vAdventureCameraAngle));

		m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

	}
	else if (pGameInstance->Get_DIKeyState(DIK_RIGHTARROW) & 0x80) {

		m_vAdventureCameraAngle += (_float)TimeDelta * 100.f;

		_matrix		RotationMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(m_vAdventureCameraAngle));

		m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

	}
	

	Safe_Release(pGameInstance);

}

void CCamera_Free::LockMouse()
{
	POINT		ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	//ClientToScreen(g_hWnd, &ptMouse);
	//SetCursorPos(ptMouse.x, ptMouse.y);
	//ShowCursor(false);
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Free::Free()
{
	__super::Free();
}