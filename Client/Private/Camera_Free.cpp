#include "pch.h"
#include "..\Public\Camera_Free.h"

#include "GameInstance.h"

#include "Player.h"
#include "Character.h"
#include "Fade_Manager.h"
#include "Camera_Manager.h"

#include "PlayerManager.h"
#include "Fade_Manager.h"


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

		if (pGameInstance->Get_CurLevelIdx() == LEVEL_GAMEPLAY)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 130.f, 0.f, 140.f, 1.f });
		if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 573.f, 4.5f, 242.f, 1.f });
		if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 8.f, 0.f, 10.f, 1.f });
		if (pGameInstance->Get_CurLevelIdx() == LEVEL_TRAIN)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 205.57f, 7.38f, 224.0f, 1.f });
		if (pGameInstance->Get_CurLevelIdx() == LEVEL_FINALBOSS)
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

	
	m_Swap_TimeAcc += (_float)dTimeDelta;


	Safe_Release(pGameInstance);

	

	__super::Tick(dTimeDelta);
}

void CCamera_Free::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	//_vector pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

//  Dist_Update
	if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 1) {
		m_bIs_Dist_Update = CCameraManager::GetInstance()->Get_Is_Dist_Update();
	}
	else {
		m_Hekireki_Dir = 1;
	}

// Camera_Shake
	if (CCameraManager::GetInstance()->Get_Is_Shake_On()) {

		Shake(CCameraManager::GetInstance()->Get_Shake_Time(), CCameraManager::GetInstance()->Get_Shake_Power());

		CCameraManager::GetInstance()->Set_Is_Shake_On(false);
	}

// Player
	if (pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")) != nullptr) {

		CCharacter* pPlayer = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), m_Player_Index));

		CTransform* m_pTargetTransformCom = pPlayer->Get_TransformCom();

		if (m_Swap_TimeAcc >= 1.2f)
			m_vTargetPos = m_pTargetTransformCom->Get_State(CTransform::STATE_POSITION);

		m_fLandY = pPlayer->Get_LandY();

		m_vCutInTargetPos = m_vTargetPos + 5.f * m_pTargetTransformCom->Get_State(CTransform::STATE_LOOK);
	}

	m_Player_Index = CPlayerManager::GetInstance()->Get_PlayerIndex();

	if (m_Player_Index != m_Pre_Player_Index) {
		m_Pre_Player_Index = m_Player_Index;
		m_Swap_TimeAcc = 0.f;
	}

	if ((Convert::ToFloat4(m_vPreTargetPos).x != Convert::ToFloat4(m_vTargetPos).x)
		|| (Convert::ToFloat4(m_vPreTargetPos).y != Convert::ToFloat4(m_vTargetPos).y)
		|| (Convert::ToFloat4(m_vPreTargetPos).z != Convert::ToFloat4(m_vTargetPos).z)) 
	{
		m_vPreTargetPos = m_vTargetPos;
	}


// Battle_Target
	m_Is_Battle = CFadeManager::GetInstance()->Get_Is_Battle();

	if (pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Monster"), 0) != nullptr) {

		m_Battle_Target_MaxNum = (_int)pGameInstance->Get_GameObject_ListSize(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Monster")) - 1;

		if (m_Battle_Target_Num <= (_uint)m_Battle_Target_MaxNum) {

			CCharacter* pMon = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Monster"), m_Battle_Target_Num));

			CTransform* m_pBattleTargetTransformCom = pMon->Get_TransformCom();

			m_vBattleTargetPos = m_pBattleTargetTransformCom->Get_State(CTransform::STATE_POSITION);
			m_vBattleTargetPos_Offer = m_pBattleTargetTransformCom->Get_State(CTransform::STATE_POSITION);


			if (pMon->Get_Status().fHp <= 0) {
				m_Lock_On_UI_Render = false;
			}
			else {
				m_Lock_On_UI_Render = true;
			}

		}

		m_Lock_On_Is_Boss = false;

	}
	else if (pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Boss")) != nullptr) {

		CCharacter* pBoss = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Boss")));

		CTransform* m_pBattleTargetTransformCom = pBoss->Get_TransformCom();

		m_vBattleTargetPos = m_pBattleTargetTransformCom->Get_State(CTransform::STATE_POSITION);
		m_vBattleTargetPos_Offer = m_pBattleTargetTransformCom->Get_State(CTransform::STATE_POSITION);

		m_Lock_On_Is_Boss = true;

		m_vCutInBattleTargetPos = m_vBattleTargetPos + 5.f * m_pBattleTargetTransformCom->Get_State(CTransform::STATE_LOOK);
	}
	else {
		m_Is_Battle = false;

	}


// �Ͽ콺�� Ư��ó��
	if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE) {
		
		if (CFadeManager::GetInstance()->Get_Is_House_Boss_Encounter() == true) {

			if (pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Boss")) != nullptr) {

				CCharacter* pBoss = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Boss")));

				CTransform* m_pBattleTargetTransformCom = pBoss->Get_TransformCom();

				m_vBattleTargetPos = m_pBattleTargetTransformCom->Get_State(CTransform::STATE_POSITION);
				m_vBattleTargetPos_Offer = m_pBattleTargetTransformCom->Get_State(CTransform::STATE_POSITION);

				m_Lock_On_Is_Boss = true;

				m_vCutInBattleTargetPos = m_vBattleTargetPos + 5.f * m_pBattleTargetTransformCom->Get_State(CTransform::STATE_LOOK);
			}
			else {
				m_Is_Battle = false;

			}

		}
	}



// ������ Ư��ó��
	if (pGameInstance->Get_CurLevelIdx() == LEVEL_TRAIN) {
		m_Is_Battle = true;

		m_vBattleTargetPos = { 204.81f,7.3f,414.47f,1.f };
	}



//  LockOn
	if (m_Is_Battle == true) {
		if (pGameInstance->Get_DIKeyDown(DIK_R)) {
			CCameraManager::GetInstance()->Set_Is_Battle_LockFree(!m_bIs_Battle_LockFree);
		}

		m_bIs_Battle_LockFree = CCameraManager::GetInstance()->Get_Is_Battle_LockFree();
	}

// Lock_On_Change
	if (pGameInstance->Get_DIKeyDown(DIK_RSHIFT)) {
		m_Battle_Target_Num++;

		if (m_Battle_Target_Num > (_uint)m_Battle_Target_MaxNum) {
			m_Battle_Target_Num = 0;
		}
	}

// Zoom
	if (m_Is_Battle == false) {
		if (pGameInstance->Get_DIKeyState(DIK_S)) {
			//CCameraManager::GetInstance()->Zoom_Fix(2.f);
		}
	}

	m_Zoom = CCameraManager::GetInstance()->Get_Zoom();

// Focus
	m_Is_Focus_On = CCameraManager::GetInstance()->Get_Is_Focus_On();
	m_vFocusPos = CCameraManager::GetInstance()->Get_Focus_Pos();

// Center
	m_vBattleTargetPos = XMVectorSetY(m_vBattleTargetPos, m_fLandY);

	if (m_Swap_TimeAcc < 1.2f)
		m_vTargetPos = XMVectorSetY(m_vTargetPos, m_fLandY);

	m_vBattleCenter = (m_vTargetPos + m_vBattleTargetPos) * 0.5f;



// Combo_On
	if (pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")) != nullptr) {
		_int PlayerIndex = CPlayerManager::GetInstance()->Get_PlayerIndex();
		CCharacter* pPlayer = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), PlayerIndex));

		if (pPlayer->Get_Status().iAttackCombo > 1 || pPlayer->Get_Status().iHitCombo > 1) {
			m_bIs_Combo_On = true;	//���̵� ķ ����Ʈ �۾� �� �ּ� �� ��
		}
		else {
			m_bIs_Combo_On = false;	//���̵� ķ ����Ʈ �۾� �� �ּ� �� ��
		}

		if (pGameInstance->Get_CurLevelIdx() == LEVEL_TRAIN) {
			m_bIs_Combo_On = false;
		}

		_float Dist = Convert::GetLength(m_vTargetPos - m_vBattleTargetPos);

		if (Dist > 7) {
			m_bIs_Side_Off = true;
		}
		else {
			m_bIs_Side_Off = false;
		}

		if (PlayerIndex == 0 && m_bIs_Combo_On == true && m_bIs_Side_Off == false) {
			m_Hekireki_Dir = 1.f;
		}

		// ������ Ư��ó��
		if (pGameInstance->Get_CurLevelIdx() == LEVEL_TRAIN) {
			m_Is_Battle = true;

			m_bIs_Side_Off = true;

			m_vCameraAngle = 0.f;
		}
		else {
			m_vCameraAngle = 10.f;
		}

	}

	//=============================================
	// ���̵�ķ ����Ʈ �۾� �� �ּ� ����
	// Side_Cam_Key
	if (pGameInstance->Get_DIKeyDown(DIK_GRAVE)) {
		//m_bIs_Combo_On = !m_bIs_Combo_On;
	}
	//=============================================

// Cut In
	if (CCameraManager::GetInstance()->Get_Is_Cut_In_On() == true) {

		CCameraManager::GetInstance()->Set_Is_Cut_In_On(false);

		if (m_Is_Cut_In == false && m_Cut_In_IsDone == true) {
			m_Cut_In_Dist = m_vDist;
			m_Is_Cut_In = true;
			m_Cut_In_IsDone = false;
			m_Cut_In_Finish_Type = CCameraManager::GetInstance()->Get_Cut_In_Finish_Type();
		}

	}

// ī�޶� �ƾ� ������ �ƴ���
	CCameraManager::GetInstance()->Set_Cut_In_IsDone(m_Cut_In_IsDone);

	// ī�޶� �Լ�
	if (m_bCamChange == true)
	{
		// CutIn
		if (m_Is_Cut_In == true && m_Cut_In_IsDone == false) {

			m_vOffSet = { 0.f, 1.5f, 0.f, 0.f };
			m_vLookOffSet = { 0.f, 1.7f, 0.f, 0.f };
			m_fDamping = { 5.f };

			CutInCamera(dTimeDelta);
		}
		// Focus
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

				m_fDistance = { 3.f + m_Zoom };
				m_vOffSet = { 0.f, 1.f, 0.f, 0.f };
				m_vLookOffSet = { 0.f, 0.2f, 0.f, 0.f };
				m_fLookDamping = { 7.f };
				m_fDamping = { 6.3f };

				//NewAdventureCamera(dTimeDelta);

				SuperNewAdventureCamera(dTimeDelta);

			}
			// Battle
			else if (m_Is_Battle == true) {

				// Side
				if (m_bIs_Combo_On == true && m_bIs_Side_Off == false) {

					m_fDistance = { 5.3f };
					m_vOffSet = { 0.f, 1.1f, 0.f, 0.f };
					m_vLookOffSet = { 0.f, 0.f, 0.f, 0.f };
					m_fLookDamping = { 7.f };
					m_fDamping = { 6.f };

					SideCamera(dTimeDelta);
				}
				// Origin
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
	// FreeCam
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

void CCamera_Free::Change_Hekireki_Dir()
{
	if (m_bIs_Side_Off == false && m_bIs_Combo_On == true)
		m_Hekireki_Dir *= -1;
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

void CCamera_Free::NewAdventureCamera(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });

	_vector vCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	

	//New_Turn_Camera(dTimeDelta);


	// ���콺�� ȸ��
	_long		MouseMove = 0;

	if (fabs(MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X)))
	{
		_vector vCamPos  = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vCamLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vCamRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vTargetPos + m_vOffSet + (vCamLook * m_fDistance));

		m_pTransformCom->Turn({ 0.f,1.f,0.f }, XMConvertToRadians(90.0f) * _double(dTimeDelta * MouseMove * 0.03));

		_vector vDest = m_vTargetPos + m_vOffSet + (-m_pTransformCom->Get_State(CTransform::STATE_LOOK) * m_fDistance);

	
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDest);

	}
	
		
	if (fabs(MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y)))
	{
		_vector vCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vCamLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vCamRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vTargetPos + m_vOffSet + (vCamLook * m_fDistance));

		m_pTransformCom->Turn(vCamRight , XMConvertToRadians(90.0f) * _double(dTimeDelta * MouseMove * 0.02));

		_vector vDest = m_vTargetPos + m_vOffSet + (-m_pTransformCom->Get_State(CTransform::STATE_LOOK) * m_fDistance);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDest);

	}
	
	_vector vDest = m_vTargetPos + m_vOffSet + (-m_pTransformCom->Get_State(CTransform::STATE_LOOK) * m_fDistance);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDest);
	

	Safe_Release(pGameInstance);
}

void CCamera_Free::SuperNewAdventureCamera(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });

	_vector vCamPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	New_Turn_Camera(dTimeDelta);

	m_vDist = { XMVectorGetX(m_vDist), 0.3f ,XMVectorGetZ(m_vDist), 0.f };

	m_vDist = XMVector3Normalize(m_vDist);

	_vector vDest = m_vTargetPos + m_vOffSet + (m_vDist * m_fDistance);

	_float t = (_float)dTimeDelta * m_fDamping;

	_vector CamPos = XMVectorLerp(vCamPosition, vDest, t);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CamPos);

	_vector NewLook = {};

	_vector Dist = { XMVectorGetX(m_vDist), 0.f ,XMVectorGetZ(m_vDist), 0.f };


	_vector Look = m_vTargetPos + m_vLookOffSet - 100.f * Dist;

	NewLook = m_vTargetPos + m_vLookOffSet - 100.f * Dist - CamPos;


	NewLook = { XMVectorGetX(NewLook), -20.f , XMVectorGetZ(NewLook), XMVectorGetW(NewLook) };

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

	
	if (m_bIs_Dist_Update) {

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false) {
			m_vOffSet = { 0.f, 1.8f, 0.f, 0.f };
			m_vLookOffSet = { 0.f, 1.f, 0.f, 0.f };
			m_fDistance = 6.f + m_Zoom;

			m_vDist = m_vTargetPos - m_vBattleTargetPos;

			_matrix		RotationMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(-m_vCameraAngle));

			m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

			m_vDist = { XMVectorGetX(m_vDist), 0.1f ,XMVectorGetZ(m_vDist), XMVectorGetW(m_vDist) };

			m_vDist = XMVector3Normalize(m_vDist);
		}
		else {
			m_vOffSet = { 0.f, 2.f, 0.f, 0.f };
			m_vLookOffSet = { 0.f, 0.f, 0.f, 0.f };

			m_fDistance = 7.f + m_Zoom;

			Turn_Camera(dTimeDelta);

			m_vDist = { XMVectorGetX(m_vDist), 0.3f ,XMVectorGetZ(m_vDist), 0.f };

			m_vDist = XMVector3Normalize(m_vDist);

		}
	}

	_vector vDest = m_vTargetPos + m_vOffSet + (m_vDist * m_fDistance);

	_float t = (_float)dTimeDelta * m_fDamping;

	_vector CamPos = XMVectorLerp(vCamPosition, vDest, t);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CamPos);

	_vector NewLook = {};

	if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		NewLook = m_vBattleCenter + m_vLookOffSet - CamPos;
	else {

		_vector Dist = { XMVectorGetX(m_vDist), 0.f ,XMVectorGetZ(m_vDist), 0.f };

		NewLook = m_vTargetPos + m_vLookOffSet - 100.f * Dist - CamPos;
	}
		

	if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		NewLook = { XMVectorGetX(NewLook), XMVectorGetY(NewLook) * 0.f ,XMVectorGetZ(NewLook), XMVectorGetW(NewLook) };
	else
		NewLook = { XMVectorGetX(NewLook), -40.f , XMVectorGetZ(NewLook), XMVectorGetW(NewLook) };

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

	_vector vRight = {};

	if (m_bIs_Dist_Update) {

		vRight = m_vTargetPos - m_vBattleTargetPos;

		m_vDist = XMVector3Cross(m_Hekireki_Dir * vRight, vUp);

		m_vDist = { XMVectorGetX(m_vDist), 0.f ,XMVectorGetZ(m_vDist), XMVectorGetW(m_vDist) };

		m_vDist = XMVector3Normalize(m_vDist);

		_matrix		RotationMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(8.f));

		m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);
	}
	
	_vector vDest = {};

	vDest = m_vBattleCenter + m_vOffSet + (m_vDist * m_fDistance);
	
	_float t = (_float)dTimeDelta * m_fDamping;

	_vector CamPos = XMVectorLerp(vCamPosition, vDest, t);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, CamPos);

	_vector NewLook = {};

	NewLook = m_vBattleCenter + m_vLookOffSet - CamPos;
	
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

			if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0 && m_Cut_In_Finish_Type == TANJIRO_EXECUTION) {
				m_Hekireki_Dir = -1.f;
			}

			m_vDist = m_Cut_In_Dist * m_Hekireki_Dir;

			m_fDistance = { 6.f + m_Zoom };
			m_vOffSet = { 0.f, 1.8f, 0.f, 0.f };
			m_vLookOffSet = { 0.f, 1.f, 0.f, 0.f };
			m_fLookDamping = { 7.f };
			m_fDamping = { 5.f };

			_vector vDest = m_vTargetPos + m_vOffSet + (m_vDist * m_fDistance);

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDest);

			_vector NewLook = m_vBattleCenter + m_vLookOffSet;

			m_pTransformCom->LookAt(NewLook);
			
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
		m_vDist = m_vCutInTargetPos - m_vTargetPos;
	else
		m_vDist = m_vCutInBattleTargetPos - m_vBattleTargetPos;

	m_vDist = XMVector3Normalize(m_vDist);

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, m_vDist));

	_matrix RotationMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(Desc.Angle_Verti));

	m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

	RotationMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(Desc.Angle_Hori));

	m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

	_vector vDest = {};

	if (Desc.bTarget_Is_Player)
		vDest = m_vTargetPos + m_vOffSet + Desc.vOffSet + (m_vDist * Desc.fDistance);
	else
		vDest = m_vBattleTargetPos + m_vOffSet + Desc.vOffSet + (m_vDist * Desc.fDistance);
	
	if (Desc.bIs_Lerp == true) {
		_float t = (_float)dTimeDelta * m_fDamping;

		_vector CamPos = XMVectorLerp(m_pTransformCom->Get_State(CTransform::STATE_POSITION), vDest, t);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, CamPos);
	}
	else {
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDest);
	}
	
	if (Desc.bLookTarget_Is_Player)
		m_pTransformCom->LookAt(m_vTargetPos + m_vLookOffSet + Desc.vLookOffSet + Desc.vOffSet);
	else
		m_pTransformCom->LookAt(m_vBattleTargetPos + m_vLookOffSet + Desc.vLookOffSet + Desc.vOffSet);


	Safe_Release(pGameInstance);
}

void CCamera_Free::Ready_CutInFinish()
{
	
	// ź���� ����
	CutInCamDesc Tanjiro_Awake_Cam1 = { true,true, 3.f, 0.f, 0.67f, 2.f , false , {0.f, -0.2f,0.f} , {0.f, -0.8f,0.f} };
	CutInCamDesc Tanjiro_Awake_Cam2 = { true,true, 10.f, 0.f, 1.9f, 1.5f , true, {0.f,-0.7f,0.f} };
	
	m_Cut_In_Finish[TANJIRO_AWAKE].push_back(Tanjiro_Awake_Cam1);
	m_Cut_In_Finish[TANJIRO_AWAKE].push_back(Tanjiro_Awake_Cam2);

	// ��ī�� ����
	CutInCamDesc Akaza_Awake_1 = { false,false, 0.f, 0.f, 1.2f, 1.7f , false , {0.f, -0.3f ,0.f} };
	CutInCamDesc Akaza_Awake_2 = { false,false, 5.f, 0.f, 0.8f, 0.3f , true , {0.f, -0.3f ,0.f} };
	CutInCamDesc Akaza_Awake_3 = { false,false, 5.f, 5.f, 1.f, 2.5f , true , {0.f, -0.6f ,0.f} };

	m_Cut_In_Finish[AKAZA_AWAKE].push_back(Akaza_Awake_1);
	m_Cut_In_Finish[AKAZA_AWAKE].push_back(Akaza_Awake_2);
	m_Cut_In_Finish[AKAZA_AWAKE].push_back(Akaza_Awake_3);

	// ������ ����
	CutInCamDesc Zenitsu_Awake_1 = { true,true, 0.f, 0.f, 1.3f, 0.7f , false , {0.f, -0.8f ,0.f} };
	CutInCamDesc Zenitsu_Awake_2 = { true,true, 0.f, 0.f, 1.f, 0.5f , true , {0.f, -0.8f ,0.f} };
	CutInCamDesc Zenitsu_Awake_3 = { true,true, 0.f, 0.f, 2.8f, 2.2f , true , {0.f, -0.8f ,0.f} };

	m_Cut_In_Finish[ZENITSU_AWAKE].push_back(Zenitsu_Awake_1);
	m_Cut_In_Finish[ZENITSU_AWAKE].push_back(Zenitsu_Awake_2);
	m_Cut_In_Finish[ZENITSU_AWAKE].push_back(Zenitsu_Awake_3);

	// ��찡�� ����
	CutInCamDesc Kyogai_Awake_1 = { false,false, 0.f, 0.f, 1.5f, 0.6f , false , {0.f, 0.6f ,0.f} };
	CutInCamDesc Kyogai_Awake_2 = { false,false, 0.f, 0.f, 3.f, 1.f , true , {0.f, 0.3f ,0.f} };
	CutInCamDesc Kyogai_Awake_3 = { false,false, 0.f, 0.f, 6.f, 2.f , true , {0.f, 0.f ,0.f} };

	m_Cut_In_Finish[KYOGAI_AWAKE].push_back(Kyogai_Awake_1);
	m_Cut_In_Finish[KYOGAI_AWAKE].push_back(Kyogai_Awake_2);
	m_Cut_In_Finish[KYOGAI_AWAKE].push_back(Kyogai_Awake_3);


	// ź���� ó��
	CutInCamDesc Tanjiro_Execution_1 = { true,true, 90.f, 0.f, 0.8f, 0.6f , false , {0.f, 0.6f ,0.f} };
	CutInCamDesc Tanjiro_Execution_2 = { true,true, 30.f, -10.f, 3.f, 2.f , false , {0.f,-1.f,0.f} };

	m_Cut_In_Finish[TANJIRO_EXECUTION].push_back(Tanjiro_Execution_1);
	m_Cut_In_Finish[TANJIRO_EXECUTION].push_back(Tanjiro_Execution_2);


	// ź���� ����(�׽�Ʈ)
	CutInCamDesc TanjiroCam1 = { true,true, 20.f, -20.f, 7.f, 0.8f };
	CutInCamDesc TanjiroCam2 = { true,true, 20.f, -20.f, 2.f, 0.5f };
	CutInCamDesc TanjiroCam3 = { true,true, 150.f, 20.f, 3.f, 0.8f };
	CutInCamDesc TanjiroCam4 = { true,true, 20.f, 5.f, 8.f, 1.2f };
	CutInCamDesc TanjiroCam5 = { true,false, 0.f, -30.f, 3.f, 2.3f, false, {0.f,4.f,0.f} };
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

void CCamera_Free::New_Turn_Camera(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	// ���콺�� ȸ��
	_long		MouseMove = 0;

	if (fabs(MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X)))
	{
		_matrix		RotationMatrix = XMMatrixRotationAxis({ 0.f,1.f,0.f }, XMConvertToRadians(90.0f) * _float(TimeDelta * MouseMove * 0.03f));

		m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

	}

	_vector vUp = XMVector3Normalize({ 0.f,1.f,0.f });

	m_vDist = XMVector3Normalize(m_vDist);
	
	/*_float angle = acos(XMVectorGetX(XMVector3Dot(vUp, m_vDist)));

	if (fabs(MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y)))
	{

		if (MouseMove > 0) {
			if (angle > XMConvertToRadians(20.f)) {
				_matrix		RotationMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(90.0f) * _float(TimeDelta * MouseMove * 0.0f));

				m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

			}

		}
		else {
			if (angle < XMConvertToRadians(100.f)) {
				_matrix		RotationMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(90.0f) * _float(TimeDelta * MouseMove * 0.0f));

				m_vDist = XMVector3TransformNormal(m_vDist, RotationMatrix);

			}
		}

	}*/

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