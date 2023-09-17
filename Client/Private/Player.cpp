#include "pch.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "Camera_Free.h"
#include "Fade_Manager.h"
#include "Monster.h"

#include "PlayerManager.h"
#include "MonsterManager.h"

#include "EffectPlayer.h"
#include "OptionManager.h"
#include "Camera_Manager.h"

#include "WebManager.h"
#include "Battle_UI_Manager.h"


CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CCharacter(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Add_BoxJump_Info();		// 박스 정보 입력 (안원)

	// 점광원 달기
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*LIGHTDESC tLightInfo;
	ZeroMemory(&tLightInfo, sizeof tLightInfo);

	tLightInfo.eType = LIGHTDESC::TYPE_POINT;

	XMStoreFloat4(&tLightInfo.vLightPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	tLightInfo.fLightRange = 15.f;
	if (pGameInstance->Get_CurLevelIdx() == LEVEL_FINALBOSS)
		tLightInfo.vLightDiffuse = _float4(0.3f, 0.3f, 0.6f, 1.f);
	else
		tLightInfo.vLightDiffuse = _float4(0.5f, 0.5f, 0.5f, 1.f);
	tLightInfo.vLightAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.vLightSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	pGameInstance->Add_Light(m_pDevice, m_pContext, tLightInfo, m_pTransformCom);*/

	//level판명
	m_iLevelCur = pGameInstance->Get_CurLevelIdx();

	Safe_Release(pGameInstance);


	return S_OK;
}

void CPlayer::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	//m_pRendererCom->Set_BloomRatio(0.5f);

	if (COptionManager::GetInstance()->Get_Graphic_Option(1) == 0) {
		m_pRendererCom->Set_SSAO(true);
	}
	else {
		m_pRendererCom->Set_SSAO(false);
	}


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	_bool m_Is_Battle = CFadeManager::GetInstance()->Get_Is_Battle();

	if (m_Is_Battle == false) {
		m_ePlayerState = { PLAYER_ADVENTURE };
	}
	else {
		m_ePlayerState = { PLAYER_BATTLE };		
	}

	if (pGameInstance->Get_CurLevelIdx() != LEVEL_VILLAGE && pGameInstance->Get_CurLevelIdx() != LEVEL_HOUSE) {
		m_ePlayerState = { PLAYER_BATTLE };
		CFadeManager::GetInstance()->Set_Is_Battle(true);
	}
	

	Safe_Release(pGameInstance);


	if (true == m_isDead)
		return;

	if (m_isSwampBinding == false)
		Key_Input(dTimeDelta);

	m_ePlayerType;

	if (m_ePlayerState == PLAYER_BATTLE)
	{
		if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0 && m_ePlayerType == PLAYER_TANJIRO)
		{
			if (m_Moveset.m_iAwaken == 2) {
				m_pRendererCom->Set_GrayScale_On(true);
			}
			else {
				m_pRendererCom->Set_GrayScale_On(false);
			}
		}
		else if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 1 && m_ePlayerType == PLAYER_ZENITSU)
		{
			if (m_Moveset.m_iAwaken == 2) {
				m_pRendererCom->Set_GrayScale_On(true);
			}
			else {
				m_pRendererCom->Set_GrayScale_On(false);
			}
		}
	}

}

void CPlayer::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	if (m_isSwampHit == false)
		Set_Height();

	Check_Change_Position(dTimeDelta);

	if (m_isLand_Roof)
		m_eCurNavi = m_eNextNavi;



#ifdef _DEBUG
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_tchar	m_szFPS[MAX_PATH] = TEXT("");
	_sntprintf_s(m_szFPS, MAX_PATH, TEXT("MP : %.2f"), m_StatusDesc.fMp);
	if (FAILED(pGameInstance->Draw_Font(TEXT("Font_Default"), m_szFPS, _float2(640.f, 0.f), _float2(0.5f, 0.5f))))
		return;
	Safe_Release(pGameInstance);
#endif // DEBUG

#ifdef _DEBUG
	if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom[m_eCurNavi])))
		return;
#endif
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Render_ShadowDepth()
{
	if (FAILED(__super::Render_ShadowDepth()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Dir_Setting(_bool Reverse)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//카메라 방향 구해놓기
	CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera"), 0));
	_float4 CameraLook = pCamera->Get_CameraLook();
	CameraLook.y = 0.0f;
	CameraLook.w = 0.0f;
	_vector vLook = XMVector4Normalize(XMLoadFloat4(&CameraLook));
	_vector	vUp = { 0.0f, 1.0f, 0.0f , 0.0f };
	_vector crossLeft = XMVector3Cross(vLook, vUp);

	//45degree look
	_vector quaternionRotation = XMQuaternionRotationAxis(vUp, XMConvertToRadians(45.0f));
	_vector v45Rotate = XMVector3Rotate(vLook, quaternionRotation);

	//135degree look
	_vector quaternionRotation2 = XMQuaternionRotationAxis(vUp, XMConvertToRadians(135.0f));
	_vector v135Rotate = XMVector3Rotate(vLook, quaternionRotation2);



	if (Reverse)
	{
		v45Rotate = -v45Rotate;
		v135Rotate = -v135Rotate;
	}

	//Dir설정
	if (pGameInstance->Get_DIKeyState(DIK_W) && pGameInstance->Get_DIKeyState(DIK_A))
	{
		XMStoreFloat4(&m_Moveset.m_Input_Dir, -v135Rotate);
		XMStoreFloat4(&m_Moveset.m_Input_Dir, XMVector4Normalize(XMLoadFloat4(&m_Moveset.m_Input_Dir)));
	}
	else if (pGameInstance->Get_DIKeyState(DIK_W) && pGameInstance->Get_DIKeyState(DIK_D))
	{
		XMStoreFloat4(&m_Moveset.m_Input_Dir, v45Rotate);
		XMStoreFloat4(&m_Moveset.m_Input_Dir, XMVector4Normalize(XMLoadFloat4(&m_Moveset.m_Input_Dir)));
	}
	else if (pGameInstance->Get_DIKeyState(DIK_S) && pGameInstance->Get_DIKeyState(DIK_A))
	{
		XMStoreFloat4(&m_Moveset.m_Input_Dir, -v45Rotate);
		XMStoreFloat4(&m_Moveset.m_Input_Dir, XMVector4Normalize(XMLoadFloat4(&m_Moveset.m_Input_Dir)));
	}
	else if (pGameInstance->Get_DIKeyState(DIK_S) && pGameInstance->Get_DIKeyState(DIK_D))
	{
		XMStoreFloat4(&m_Moveset.m_Input_Dir, v135Rotate);
		XMStoreFloat4(&m_Moveset.m_Input_Dir, XMVector4Normalize(XMLoadFloat4(&m_Moveset.m_Input_Dir)));
	}
	else
	{
		if (pGameInstance->Get_DIKeyState(DIK_W))
		{
			XMStoreFloat4(&m_Moveset.m_Input_Dir, vLook);
		}
		else if (pGameInstance->Get_DIKeyState(DIK_S))
		{
			XMStoreFloat4(&m_Moveset.m_Input_Dir, -vLook);
		}
		else if (pGameInstance->Get_DIKeyState(DIK_A))
		{
			XMStoreFloat4(&m_Moveset.m_Input_Dir, crossLeft);
		}
		else if (pGameInstance->Get_DIKeyState(DIK_D))
		{
			XMStoreFloat4(&m_Moveset.m_Input_Dir, -crossLeft);
		}
	}

	Safe_Release(pGameInstance);
}

_bool CPlayer::Get_LockOn_MonPos()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera"), 0));
	_vector MonPos = pCamera->Get_Battle_Target_Pos();
	_bool	IsBattle = pCamera->Get_Is_Battle();

	if (IsBattle)
		XMStoreFloat4(&m_LockOnPos, MonPos);

	Safe_Release(pGameInstance);

	return IsBattle;
}

_float CPlayer::Get_Distance_To_LockOnPos()
{
	Get_LockOn_MonPos();

	_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = XMLoadFloat4(&m_LockOnPos);
	_vector vDir = XMVector3Normalize(vMonsterPos - vPlayerPos);
	_float fDistance = Convert::GetLength(vMonsterPos - vPlayerPos);

	return fDistance;
}

_vector CPlayer::Get_Dir_To_LockOnPos()
{
	Get_LockOn_MonPos();

	_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = XMLoadFloat4(&m_LockOnPos);
	_vector vDir = XMVector3Normalize(vMonsterPos - vPlayerPos);

	return vDir;
}

void CPlayer::Trigger_Hit(_double dTimeDelta)
{
	/*CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//CMonster* pMonster = dynamic_cast<CMonster*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Boss")));


	Safe_Release(pGameInstance);*/



	if (m_Moveset.m_isDownMotion == false && m_isSwamp_Escape == false)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CEffectPlayer::EFFECTWORLDDESC Effect3WorldDesc;
		Effect3WorldDesc.vPosition.y += 0.8f;

		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
		

			if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) {
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect();
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				_uint iRanNum = Random::Generate_Int(0, 1);
				if (iRanNum == 0) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect5", m_pTransformCom, &EffectWorldDesc);
				}
				else if (iRanNum == 1) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect7", m_pTransformCom, &EffectWorldDesc);
				}

				EffectWorldDesc.fScale = 0.7f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				//CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}

			m_dDelay_ComboReset_2 = 0.0;

			if (m_Moveset.m_State_Battle_Guard && !m_isSkilling)
			{
				m_isGuardHit = true;

			}
			else
			{
				m_StatusDesc.iHitCombo++;
				m_Moveset.m_Down_Dmg_Small = true;
			}
		}

		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);

			if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) {
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect();
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				_uint iRanNum = Random::Generate_Int(0, 1);
				if (iRanNum == 0) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect5", m_pTransformCom, &EffectWorldDesc);
				}
				else if (iRanNum == 1) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect7", m_pTransformCom, &EffectWorldDesc);
				}
				
				EffectWorldDesc.fScale = 0.7f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				//CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}

			m_dDelay_ComboReset_2 = 0.0;

			if (m_Moveset.m_State_Battle_Guard && !m_isSkilling)
				m_isGuardHit = true;
			else
			{
				m_StatusDesc.iHitCombo++;
				m_Moveset.m_Down_Dmg_ConnectSmall = true;
			}
		}

		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Big())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
			if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) {
			
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect();
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				_uint iRanNum = Random::Generate_Int(0, 1);
				if (iRanNum == 0) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect5", m_pTransformCom, &EffectWorldDesc);
				}
				else if (iRanNum == 1) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect7", m_pTransformCom, &EffectWorldDesc);
				}

				EffectWorldDesc.fScale = 0.7f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				//CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			

			m_dDelay_ComboReset_2 = 0.0;

			if (m_Moveset.m_State_Battle_Guard && !m_isSkilling)
				m_isGuardHit = true;
			else
			{
				m_StatusDesc.iHitCombo++;
				m_Moveset.m_Down_Dmg_Big = true;
			}

			//pGameInstance->Time_Slow(0.4, 0.3);
		}


		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
			if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) {

				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect();
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				_uint iRanNum = Random::Generate_Int(0, 1);
				if (iRanNum == 0) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect5", m_pTransformCom, &EffectWorldDesc);
				}
				else if (iRanNum == 1) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect7", m_pTransformCom, &EffectWorldDesc);
				}

				EffectWorldDesc.fScale = 0.7f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				//CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}

			m_dDelay_ComboReset_2 = 0.0;

			if (m_Moveset.m_State_Battle_Guard && !m_isSkilling)
				m_isGuardHit = true;
			else
			{
				m_StatusDesc.iHitCombo++;
				m_Moveset.m_Down_Dmg_Blow = true;
			}

			//pGameInstance->Time_Slow(0.6, 0.3);
		}

		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_BigBlow())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_BigBlow(false);
			if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) {
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect();
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				_uint iRanNum = Random::Generate_Int(0, 1);
				if (iRanNum == 0) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect5", m_pTransformCom, &EffectWorldDesc);
				}
				else if (iRanNum == 1) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect7", m_pTransformCom, &EffectWorldDesc);
				}

				EffectWorldDesc.fScale = 0.7f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				//CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}

			m_dDelay_ComboReset_2 = 0.0;

			if (m_Moveset.m_State_Battle_Guard && !m_isSkilling)
				m_isGuardHit = true;
			else
			{
				m_StatusDesc.iHitCombo++;
				m_Moveset.m_Down_Dmg_BigBlow = true;
			}

			//pGameInstance->Time_Slow(0.6, 0.3);
		}

		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Upper())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
			if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) {
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect();
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

				_uint iRanNum = Random::Generate_Int(0, 1);
				if (iRanNum == 0) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect5", m_pTransformCom, &EffectWorldDesc);
				}
				else if (iRanNum == 1) {
					EffectWorldDesc.fScale = 1.4f;
					CEffectPlayer::Get_Instance()->Play("Hit_Effect7", m_pTransformCom, &EffectWorldDesc);
				}
				EffectWorldDesc.fScale = 0.7f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.f;

				//CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}


			m_dDelay_ComboReset_2 = 0.0;

			if (m_Moveset.m_State_Battle_Guard && !m_isSkilling)
				m_isGuardHit = true;
			else
			{
				m_StatusDesc.iHitCombo++;
				m_Moveset.m_Down_Dmg_Upper = true;
			}
		}

		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Swamp())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_Swamp(false);

			if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) {
				
			}

			m_Moveset.m_Down_Dmg_Swamp = true;
		}

		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Web())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_Web(false);

			if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) {
				
			}

			m_Moveset.m_Down_Dmg_Web = true;

		}


		if (CPlayerManager::GetInstance()->Get_Hit_WebShot() && CPlayerManager::GetInstance()->Get_PlayerIndex() == m_ePlayerType)
		{
			CPlayerManager::GetInstance()->Set_Hit_WebShot(false);

			m_Moveset.m_Down_Dmg_Web = true;

			m_isWebbing = true;
			m_dDelay_Webbing = 0.0;


		}
		if (m_isWebbing)
		{
			m_dDelay_Webbing += dTimeDelta;
			if (m_dDelay_Webbing > 0.45f)
			{
				m_dDelay_Webbing = 0.0;
				m_isWebbing = false;
				CBattle_UI_Manager::GetInstance()->Set_Web_UI_On(0);
			}
		}


		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Hekireki())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);
			

			m_dDelay_ComboReset_2 = 0.0;

			if (m_Moveset.m_State_Battle_Guard && !m_isSkilling)
				m_isGuardHit = true;
			else
			{
				m_StatusDesc.iHitCombo++;
				m_Moveset.m_Down_Dmg_SwampUpper = true;
			}

			//pGameInstance->Time_Slow(0.3, 0.5);
		}
		Safe_Release(pGameInstance);
	}
	else
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_BigBlow(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Swamp(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Web(false);

		m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);
	}
}

void CPlayer::Key_Input(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region Test


	if (pGameInstance->Get_DIKeyDown(DIK_V))
	{
		m_isSpecialHit = true;
	}


	if (pGameInstance->Get_DIKeyDown(DIK_V))
	{
		if (m_isCanNavi)
			m_isCanNavi = false;
		else
			m_isCanNavi = true;
	}

	if (pGameInstance->Get_DIKeyState(DIK_B))
	{
		m_StatusDesc.fSpecial += 1.1f;
		m_StatusDesc.fMp += 1.1f;
	}


	if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) {

		if (pGameInstance->Get_DIKeyDown(DIK_Z))
		{
			m_pRendererCom->Set_Invert();
		}

		if (pGameInstance->Get_DIKeyDown(DIK_C))
		{
			m_pRendererCom->Set_RadialBlur();
		}

	}
	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), -dTimeDelta);
#pragma endregion


	if (CPlayerManager::GetInstance()->Get_PlayerIndex() == m_ePlayerType)
	{
		//쿄우가이 돌아가는 룸
		if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE && CMonsterManager::GetInstance()->Get_Kyogai_On())
		{
			if (pGameInstance->Get_DIKeyDown(DIK_SPACE))
			{
				m_isJump_TurnRoom = true;
				m_isJump_Room_X = false;
				m_isJump_Room_Z = false;
				Jumping(3.0f, 0.07f);

				if (m_ePlayerType == PLAYER_TANJIRO)
				{
					m_pModelCom->Set_Animation(83);
				}
				else if (m_ePlayerType == PLAYER_ZENITSU)
				{
					m_pModelCom->Set_Animation(56);
				}
			}
		}
		// 125.11f, 0.05f, 121.95f, 1.0f
		if (m_isJump_TurnRoom)
		{
			_float4		PlayerPos;
			XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			_float	fMove = 0.12f;

			if (PlayerPos.x < 123.1f)
				PlayerPos.x += fMove;
			else if (127.1f < PlayerPos.x)
				PlayerPos.x -= fMove;
			else
				m_isJump_Room_X = true;

			if (PlayerPos.z < 120.0f)
				PlayerPos.z += fMove;
			else if (124.0f < PlayerPos.z)
				PlayerPos.z -= fMove;
			else
				m_isJump_Room_Z = true;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&PlayerPos));
			if (m_isJump_Room_X && m_isJump_Room_Z)
			{
				m_isJump_TurnRoom = false;
				m_isJump_Room_X = false;
				m_isJump_Room_Z = false;
			}
		}

		Trigger_Hit(dTimeDelta);

		if (m_isSwapping_State == false)
		{
			if (m_Moveset.m_isHitMotion == false)
			{
				if (m_Moveset.m_isRestrict_Adventure == false)
				{
					Key_Input_Battle_Move(dTimeDelta);
				}


				if (m_ePlayerState == PLAYER_ADVENTURE)
				{
					Key_Input_Adventure(dTimeDelta);
				}
				else if (m_ePlayerState == PLAYER_BATTLE)
				{
					Key_Input_Battle_Jump(dTimeDelta);

					Key_Input_Battle_Attack(dTimeDelta);

					Key_Input_Battle_ChargeAttack(dTimeDelta);

					Key_Input_Battle_Skill(dTimeDelta);

					Key_Input_Battle_Guard(dTimeDelta);

					Key_Input_Battle_Dash(dTimeDelta);

					Key_Input_Battle_Awaken(dTimeDelta);

					Key_Input_Battle_Special(dTimeDelta);
				}
			}
			else
			{
				Key_Input_Down(dTimeDelta);
			}
		}
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Battle_Move(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//무브키를 누르고 있는 상태
	if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_S)
		|| pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_D))
	{
		m_Moveset.m_State_Battle_Run = true;
		m_dTime_MoveKey = 0.0;
	}
	else
	{
		m_Moveset.m_State_Battle_Run = false;
	}

	Dir_Setting(false);


	//키를 누를 시
	if (!m_isCool_MoveKey)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_W) || pGameInstance->Get_DIKeyDown(DIK_S)
			|| pGameInstance->Get_DIKeyDown(DIK_A) || pGameInstance->Get_DIKeyDown(DIK_D))
		{
			if (m_Moveset.m_isRestrict_Move == false)
				m_Moveset.m_Down_Battle_Run = true;
		}
	}

	// 키를 뗄 시
	if (pGameInstance->Get_DIKeyUp(DIK_W) || pGameInstance->Get_DIKeyUp(DIK_S)
		|| pGameInstance->Get_DIKeyUp(DIK_A) || pGameInstance->Get_DIKeyUp(DIK_D))
	{
		if (m_Moveset.m_isRestrict_Charge == false)
			m_isCool_MoveKey = true;
	}

	// 키를 뗄 시 자연스러움 추가
	m_dTime_MoveKey += dTimeDelta;
	if (0.1f < m_dTime_MoveKey && m_isCool_MoveKey)
	{
		m_isCool_MoveKey = false;

		if (m_Moveset.m_isRestrict_Move == false)
			m_Moveset.m_Up_Battle_Run = true;
	}


	//무빙제한 상태에서 누르고 있을 시
	if (m_Moveset.m_isRestrict_Move)
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_S) || pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_D))
			m_Moveset.m_isPressing_While_Restrict = true;
		else
			m_Moveset.m_isPressing_While_Restrict = false;
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Battle_Jump(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);



	if (false == m_Moveset.m_isRestrict_KeyInput && false == m_Moveset.m_isRestrict_Jump)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_K))
		{
			if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_S) || pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_D))
			{
				m_Moveset.m_Down_Battle_JumpMove = true;
				m_Moveset.m_Down_Battle_Jump = false;

				Dir_Setting(false);

			}
			else
			{
				m_Moveset.m_Down_Battle_JumpMove = false;
				m_Moveset.m_Down_Battle_Jump = true;
			}

		}
	}

	// Jump attack
	if (m_isJumpOn)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_J) && m_isFirst_JumpAtk)
		{
			//점프 트랙 공격 (이동키 + J키)
			if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_S) || pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_D))
			{
				if (false == m_Moveset.m_isRestrict_JumpCombo) {
					if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0)
						m_Moveset.m_Down_Battle_Jump_TrackAttack = true;
				}
				else
					m_Moveset.m_Down_Battle_Jump_Attack = true;
			}
			// 점프 콤보 공격
			else
			{
				m_Moveset.m_Down_Battle_Jump_Attack = true;
			}

		}
	}


	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Battle_Attack(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (false == m_Moveset.m_isRestrict_KeyInput && false == m_Moveset.m_isRestrict_Jump && false == m_Moveset.m_isRestrict_Charge)
	{
		// 콤보공격
		if (pGameInstance->Get_DIKeyDown(DIK_J))
		{
			m_Moveset.m_Down_Battle_Combo = true;

			//콤보 분기용
			if (pGameInstance->Get_DIKeyState(DIK_W))
			{
				m_Moveset.m_Down_Battle_Combo_Up = true;
				m_Moveset.m_Down_Battle_Combo_Down = false;
			}
			else if (pGameInstance->Get_DIKeyState(DIK_S))
			{
				m_Moveset.m_Down_Battle_Combo_Up = false;
				m_Moveset.m_Down_Battle_Combo_Down = true;
			}
			else
			{
				m_Moveset.m_Down_Battle_Combo_Up = false;
				m_Moveset.m_Down_Battle_Combo_Down = false;
			}
		}
	}


	if (m_isCan_Surge)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_J))
		{
			m_Moveset.m_Down_Battle_Combo_Surge = true;
		}
	}
	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Battle_ChargeAttack(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//m_dDelay_Charge_J += dTimeDelta;
	//m_dDelay_Charge_W += dTimeDelta;

	if (false == m_Moveset.m_isRestrict_KeyInput && false == m_Moveset.m_isRestrict_Jump)
	{
		// 차지공격
		if (pGameInstance->Get_DIKeyDown(DIK_H))
		{
			m_Moveset.m_Down_Battle_Charge = true;
			//m_dDelay_Charge_J = 0.0;
		}


		if (pGameInstance->Get_DIKeyUp(DIK_H))
		{
			m_Moveset.m_Up_Battle_Charge = true;
		}

		/*if (pGameInstance->Get_DIKeyState(DIK_H))
		{
			m_dDelay_Charge += dTimeDelta;
		}*/
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Battle_Skill(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//if(m_isCan_Air_Hekireki && pGameInstance->Get_DIKeyDown(DIK_I))
	//	m_Moveset.m_Down_Skill_Normal = true;

	m_dDelay_CanSkill += dTimeDelta;
	if (false == m_Moveset.m_isRestrict_KeyInput || (m_dDelay_CanSkill > 1.0 && m_Moveset.m_isRestrict_KeyInput))
	{
		if (pGameInstance->Get_DIKeyDown(DIK_I) /* && m_isCan_Air_Hekireki == false */)
		{
			if (pGameInstance->Get_DIKeyState(DIK_O))
			{
				m_Moveset.m_Down_Skill_Guard = true;
			}
			else if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_S)
				|| pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_D))
			{
				m_Moveset.m_Down_Skill_Move = true;
			}
			else
			{
				m_Moveset.m_Down_Skill_Normal = true;
			}
		}
	}
	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Battle_Guard(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//카메라 방향 구해놓기
	CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera"), 0));
	_float4 CameraLook = pCamera->Get_CameraLook();
	CameraLook.y = 0.0f;
	CameraLook.w = 0.0f;
	_vector vLook = XMVector4Normalize(XMLoadFloat4(&CameraLook));

	if (m_isCan_GuardCancel || (m_Moveset.m_isRestrict_Throw == false  && m_isComboing == false && m_isJumpOn == false && m_Moveset.m_isRestrict_KeyInput == false))
	{
		if (pGameInstance->Get_DIKeyDown(DIK_O))
		{
			m_Moveset.m_Down_Battle_Guard = true;
			XMStoreFloat4(&m_Moveset.m_Input_Dir, vLook);
		}


		if (pGameInstance->Get_DIKeyState(DIK_O))
		{
			m_Moveset.m_State_Battle_Guard = true;


		}
		else
		{
			m_Moveset.m_State_Battle_Guard = false;
		}


		if (pGameInstance->Get_DIKeyUp(DIK_O))
		{
			if (m_Moveset.m_isRestrict_Jump == false && m_Moveset.m_isRestrict_KeyInput == false)
				m_Moveset.m_Up_Battle_Guard = true;
			m_isGuarding = false;
		}
	}

	if (m_Moveset.m_State_Battle_Guard && m_Moveset.m_Down_Battle_Guard == false && m_isGuarding == false)
	{
		m_Moveset.m_Down_Battle_Guard = true;
		m_isGuarding = true;
	}


	if (m_Moveset.m_isRestrict_Throw == false)
	{
		/*
		//잡기공격 ( O키 + J키)
		if (pGameInstance->Get_DIKeyState(DIK_O) )
		{
			if (pGameInstance->Get_DIKeyDown(DIK_J))
				m_Moveset.m_Down_Battle_Throw = true;
			else if (pGameInstance->Get_DIKeyDown(DIK_W) || pGameInstance->Get_DIKeyDown(DIK_S) || pGameInstance->Get_DIKeyDown(DIK_A) || pGameInstance->Get_DIKeyDown(DIK_D))
				m_Moveset.m_Down_Battle_Push = true;
		}*/
	}

	if (m_isMaintain_Guard)
	{
		m_isMaintain_Guard = false;
		m_isThrowing = false;

		if (pGameInstance->Get_DIKeyState(DIK_O))
			m_Moveset.m_Down_Battle_Guard = true;
	}




	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Battle_Dash(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//카메라 방향 구해놓기
	CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera"), 0));
	_float4 CameraLook = pCamera->Get_CameraLook();
	CameraLook.y = 0.0f;
	CameraLook.w = 0.0f;
	_vector vLook = XMVector4Normalize(XMLoadFloat4(&CameraLook));


	if (pGameInstance->Get_DIKeyDown(DIK_L))
	{
		//스텝
		if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_S) || pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_D))
		{
			if (m_Moveset.m_isRestrict_DoubleStep == false)
			{
				if (pGameInstance->Get_DIKeyState(DIK_W) && m_Moveset.m_isRestrict_Step == false)
				{
					Dir_Setting(true);
					XMStoreFloat4(&m_vLook, vLook);
					m_Moveset.m_Down_Battle_Step = true;

					m_isForward = true;
					m_isBack = false;
					m_isLeft = false;
					m_isRight = false;

				}
				else if (pGameInstance->Get_DIKeyState(DIK_S) && m_Moveset.m_isRestrict_Step == false)
				{
					Dir_Setting(true);
					XMStoreFloat4(&m_vLook, vLook);
					m_Moveset.m_Down_Battle_Step = true;

					m_isForward = false;
					m_isBack = true;
					m_isLeft = false;
					m_isRight = false;
				}
				else if (pGameInstance->Get_DIKeyState(DIK_A))
				{
					Dir_Setting(true);
					XMStoreFloat4(&m_vLook, vLook);

					m_Moveset.m_Down_Battle_Step = true;

					m_isForward = false;
					m_isBack = false;
					m_isLeft = true;
					m_isRight = false;
				}
				else if (pGameInstance->Get_DIKeyState(DIK_D))
				{
					Dir_Setting(true);
					XMStoreFloat4(&m_vLook, vLook);
					m_Moveset.m_Down_Battle_Step = true;

					m_isForward = false;
					m_isBack = false;
					m_isLeft = false;
					m_isRight = true;
				}
			}
		}
		//대시
		else
		{
			if (m_Moveset.m_isRestrict_Dash == false)
			{
				XMStoreFloat4(&m_Moveset.m_Input_Dir, vLook);
				m_Moveset.m_Down_Battle_Dash = true;
			}
		}
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Battle_Awaken(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_Moveset.m_iAwaken != 0)
	{
		m_StatusDesc.dAwaken_TimeAcc -= dTimeDelta;

		if (m_StatusDesc.dAwaken_TimeAcc <= 0.0)
		{
			m_StatusDesc.dAwaken_TimeAcc = m_StatusDesc.dAwaken_Duration;
			m_Moveset.m_iAwaken = 0;
			m_StatusDesc.iAwaken = 0;
			m_StatusDesc.isNormal_First = true;
		}
	}


	if (pGameInstance->Get_DIKeyDown(DIK_Q) && m_StatusDesc.iSpecial_Cnt >= 1)
	{
		m_Moveset.m_Down_Battle_Awaken = true;
		m_StatusDesc.iSpecial_Cnt--;

		m_StatusDesc.fMp = m_StatusDesc.fMp_Max;

		if (m_Moveset.m_iAwaken == 0)
		{
			m_Moveset.m_iAwaken = 1;
			m_StatusDesc.iAwaken = 1;
			m_StatusDesc.isAwaken_First = true;
			m_StatusDesc.dAwaken_TimeAcc = m_StatusDesc.dAwaken_Duration;
		}
		else if (m_Moveset.m_iAwaken == 1)
		{
			CCameraManager::GetInstance()->Set_Is_Cut_In_On(true);
			if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0)
				CCameraManager::GetInstance()->Set_Cut_In_Finish_Type(CCamera_Free::TANJIRO_AWAKE);
			else if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 1)
				CCameraManager::GetInstance()->Set_Cut_In_Finish_Type(CCamera_Free::ZENITSU_AWAKE);

			m_Moveset.m_iAwaken = 2;
			m_StatusDesc.iAwaken = 2;
			m_StatusDesc.dAwaken_TimeAcc = m_StatusDesc.dAwaken_Duration;
		}
	}


	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Battle_Special(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_Moveset.m_isRestrict_Special == false)
	{
		if (m_Moveset.m_isRestrict_Step == false)
		{
			if (pGameInstance->Get_DIKeyDown(DIK_E))
			{
				m_Moveset.m_Down_Battle_Special = true;
			}
		}
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Down(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_S) || pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_D))
		m_Moveset.m_isPressing_While_Restrict = true;
	else
		m_Moveset.m_isPressing_While_Restrict = false;


	if (m_Moveset.m_isDownMotion)
	{
		m_dDelay_GetUp += dTimeDelta;

		if (m_dDelay_GetUp > 0.15f && m_Moveset.m_isGetUpMotion == false)
		{
			if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_S) || pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_D))
			{
				m_dDelay_GetUp = 0.0;
				m_Moveset.m_Down_GetUp_Move = true;
				Dir_Setting(false);


			}

			if (pGameInstance->Get_DIKeyDown(DIK_K))
			{
				m_dDelay_GetUp = 0.0;
				m_Moveset.m_Down_GetUp = true;
			}
		}
	}

	if (m_Moveset.m_isGetUpMotion)
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_S) || pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_Moveset.m_isPressing_While_Restrict = true;
		}
		else
		{
			m_Moveset.m_isPressing_While_Restrict = false;
		}
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_Adventure(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	NAVI_TYPE eNextNavi = NAVI_END;

	//박스 위치
	_int index = 0;
	_bool Check_Box = false;
	for (auto BoxJump : m_vecBoxPos)
	{
		_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vBoxPos = XMLoadFloat4(&BoxJump.BoxPos);

		_vector Difference = XMVectorSubtract(vBoxPos, vPlayerPos);
		_vector squaredDistance = XMVector3LengthSq(Difference);




		XMStoreFloat(&m_fDistanceTo_Box, XMVectorSqrt(squaredDistance));

		if (m_fDistanceTo_Box < 4.0)
		{
			m_isCan_Jump_To_Box = true;
			m_isCan_Jump_RoofOn = BoxJump.RoofOn;
			m_iBoxIndex = index;
			m_Dir_ScondJump_Box = BoxJump.Dir_SecondJump;

			// 지붕 위에 있을때
			if (m_isPlayerStatus_OnRoof)
			{
				eNextNavi = BoxJump.eNextNavi2;
			}
			else // 지붕 아래에 있을때
			{
				eNextNavi = BoxJump.eNextNavi;
			}

			Check_Box = true;

			XMStoreFloat4(&m_vPlayerToBoxDir, XMVector4Normalize(Difference));

		}

		if (Check_Box == false)
		{
			m_isCan_Jump_To_Box = false;
		}

		index++;
	}

	//박스에 점프하기
	if (m_Moveset.m_isRestrict_Adventure == false)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_SPACE) && m_isCan_Jump_To_Box)
		{
			m_isCan_Jump_To_Box = false;

			m_Moveset.m_Down_ADV_Jump_To_Object = true;

			m_eNextNavi = eNextNavi;

			if (eNextNavi == NAVI_VILLAGE_INSIDEWALL1)
			{
				m_iSection = 4;
				m_iSection_Sub = 4;
			}
			else if (eNextNavi == NAVI_VILLAGE_MAINROAD1)
			{
				if (m_isSection_RoofOn)
				{
					m_iSection = 3;
					m_iSection_Sub = 3;
				}
				else
				{
					m_iSection = 2;
					m_iSection_Sub = 1;
				}
			}
			else if (eNextNavi == NAVI_VILLAGE_ROOF)
			{
				m_iSection = 2;
				m_iSection_Sub = 3;
				m_isSection_RoofOn = true;
			}
			else if (eNextNavi == NAVI_VILLAGE_WALL || eNextNavi == NAVI_VILLAGE_MAINROAD2)
			{
				m_iSection = 5;
				m_iSection_Sub = 5;
			}
		}
	}
	else
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_S) || pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_Moveset.m_isPressing_While_Restrict = true;
		}
		else
		{
			m_Moveset.m_isPressing_While_Restrict = false;
		}
	}



	Safe_Release(pGameInstance);
}

void CPlayer::Key_Input_PlayerChange(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//서포트 키
	if (pGameInstance->Get_DIKeyDown(DIK_U) && pGameInstance->Get_CurLevelIdx() != LEVEL_VILLAGE)
	{
		if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0)
			CPlayerManager::GetInstance()->Set_PlayerIndex(1);
		else if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 1)
			CPlayerManager::GetInstance()->Set_PlayerIndex(0);

		CPlayerManager::GetInstance()->Set_First_Player_Change(true);
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Add_BoxJump_Info()
{
	BOXJUMP BoxJump;
	BoxJump.BoxPos = { 593.44f, 4.5f, 280.47f, 1.0f };
	BoxJump.RoofOn = false;
	BoxJump.Dir_SecondJump = { 0.0f, 0.0f, 1.0f, 0.0f };
	BoxJump.eNextNavi = NAVI_VILLAGE_INSIDEWALL1;
	m_vecBoxPos.emplace_back(BoxJump);

	BoxJump.BoxPos = { 599.432f, 4.5f, 282.523f, 1.0f };
	BoxJump.RoofOn = false;
	BoxJump.Dir_SecondJump = { 0.0f, 0.0f, -1.0f, 0.0f };
	BoxJump.eNextNavi = NAVI_VILLAGE_MAINROAD1;
	m_vecBoxPos.emplace_back(BoxJump);

	BoxJump.BoxPos = { 577.975f, 4.5f, 296.395f, 1.0f };
	BoxJump.RoofOn = false;
	BoxJump.Dir_SecondJump = { 0.0f, 0.0f, 1.0f, 0.0f };
	BoxJump.eNextNavi = NAVI_VILLAGE_INSIDEWALL2;
	m_vecBoxPos.emplace_back(BoxJump);

	BoxJump.BoxPos = { 590.961f, 4.5f, 298.454f, 1.0f };
	BoxJump.RoofOn = false;
	BoxJump.Dir_SecondJump = { 0.0f, 0.0f, -1.0f, 0.0f };
	BoxJump.eNextNavi = NAVI_VILLAGE_INSIDEWALL1;
	m_vecBoxPos.emplace_back(BoxJump);

	BoxJump.BoxPos = { 606.956f, 4.5f, 283.695f, 1.0f };
	BoxJump.RoofOn = true;
	BoxJump.Dir_SecondJump = { 0.0f, 0.0f, -1.0f, 0.0f };
	BoxJump.eNextNavi = NAVI_VILLAGE_ROOF;
	BoxJump.eNextNavi2 = NAVI_VILLAGE_MAINROAD1;
	m_vecBoxPos.emplace_back(BoxJump);

	BoxJump.BoxPos = { 581.932f, 4.5f, 321.025f, 1.0f };
	BoxJump.RoofOn = true;
	BoxJump.Dir_SecondJump = { 0.0f, 0.0f, 1.0f, 0.0f };
	BoxJump.eNextNavi = NAVI_VILLAGE_WALL;
	m_vecBoxPos.emplace_back(BoxJump);

	BoxJump.BoxPos = { 580.268f, 4.5f, 347.568f, 1.0f };
	BoxJump.RoofOn = true;
	BoxJump.Dir_SecondJump = { 0.0f, 0.0f, -1.0f, 0.0f };
	BoxJump.eNextNavi = NAVI_VILLAGE_WALL;
	BoxJump.eNextNavi2 = NAVI_VILLAGE_MAINROAD2;
	m_vecBoxPos.emplace_back(BoxJump);
}

void CPlayer::Check_Change_Position(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vInteractionPos;

	_uint iLevelIdx = pGameInstance->Get_CurLevelIdx();

	if (LEVEL_HOUSE == iLevelIdx)
	{
		if (!m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_1A])
		{
			vInteractionPos = { 67.f , 3.f , 19.9f , 1.f };

			if (NAVI_HOUSE_0_0 == m_eCurNavi && pGameInstance->Get_DIKeyDown(DIK_F4))
			{
				m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_1A] = true;
				m_dChangePositionAccTime = 0.0;


			}
			/*
			if (Compute::DistCheck(vPlayerPos, vInteractionPos, 4.f))
			{
				m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_1A] = true;
				m_dChangePositionAccTime = 0.0;
			}*/
		}

		if (!m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_1B])
		{
			//vInteractionPos = { 67.f , 3.f , 19.9f , 1.f };


			if (NAVI_HOUSE_2_0 == m_eCurNavi && pGameInstance->Get_DIKeyDown(DIK_9))
			{
				m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_1B] = true;
				m_dChangePositionAccTime = 0.0;
			}
		}


		if (!m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_2A])
		{
			vInteractionPos = { 131.f , 3.f , 57.f , 1.f };
			if (NAVI_HOUSE_0_0 == m_eCurNavi && pGameInstance->Get_DIKeyDown(DIK_F3))
			{
				m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_2A] = true;
				CPlayerManager::GetInstance()->Set_KyogaiMap(m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_2A]);
				m_dChangePositionAccTime = 0.0;
			}
			if (Compute::DistCheck(vPlayerPos, vInteractionPos, 4.f))
			{
				m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_2A] = true;
				CPlayerManager::GetInstance()->Set_KyogaiMap(m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_2A]);
				m_dChangePositionAccTime = 0.0;
			}
		}
	}
	else if (LEVEL_VILLAGE == iLevelIdx)
	{
		if (!m_bChangePositionTrigger[CHANGE_POSITON_VILLAGE_1A])
		{
			vInteractionPos = { 600.f, 4.5f, 317.f, 1.f };

			if (NAVI_VILLAGE_MAINROAD1 == m_eCurNavi && pGameInstance->Get_DIKeyDown(DIK_F3))
			{
				m_bChangePositionTrigger[CHANGE_POSITON_VILLAGE_1A] = true;
				m_dChangePositionAccTime = 0.0;

				CMonsterManager::GetInstance()->Set_BattleOn_Swamp(true);

			}

			if (Compute::DistCheck(vPlayerPos, vInteractionPos, 4.f))
			{
				m_bChangePositionTrigger[CHANGE_POSITON_VILLAGE_1A] = true;
				m_dChangePositionAccTime = 0.0;

				CMonsterManager::GetInstance()->Set_BattleOn_Swamp(true);
			}

		}
		if (!m_bChangePositionTrigger[CHANGE_POSITON_VILLAGE_1B])
		{
			if (NAVI_VILLAGE_BATTLE == m_eCurNavi && pGameInstance->Get_DIKeyDown(DIK_9))
			{
				m_bChangePositionTrigger[CHANGE_POSITON_VILLAGE_1B] = true;
				m_dChangePositionAccTime = 0.0;
			}
		}

		if (m_eCurNavi == NAVI_VILLAGE_BATTLE)
			m_pRendererCom->Set_Village_Battle(true);
		else
			m_pRendererCom->Set_Village_Battle(false);

	}


	for (_uint i = 0; i < CHANGE_POSITON_END; ++i)
	{
		if (m_bChangePositionTrigger[i] && !m_bChangePosition[i])
		{
			m_dChangePositionAccTime += TimeDelta;

			if (m_dChangePositionAccTime >= 1.0)
			{
				m_bChangePosition[i] = true;
				m_dChangePositionAccTime = 0.0;
			}
		}
	}

	_vector vNextPos = { 0.f, 0.f , 0.f , 1.f };
	_float4 PlayerDir = { 0.0f, 0.0f , 1.0f, 0.0f };
	_tchar szBGM1[MAX_PATH] = TEXT("");
	_tchar szBGM2[MAX_PATH] = TEXT("");

	for (_uint i = 0; i < CHANGE_POSITON_END; ++i)
	{
		if (m_bChangePosition[i] && !m_bChangePositionFinish[i])
		{
			switch (i)
			{
			case CHANGE_POSITON_HOUSE_1A: // 자코
				vNextPos = XMVectorSet(43.f, 0.f, 120.f, 1.f);
				Change_NaviMesh(CLandObject::NAVI_HOUSE_2_0);

				XMStoreFloat4(&PlayerDir, XMVector4Normalize(_vector{ 0.3f, 0.0f, -1.0f, 0.0f }));
				m_pTransformCom->Set_Look(PlayerDir);
				m_pModelCom->Set_Animation(82); // battle_idle
				break;
			case CHANGE_POSITON_HOUSE_1B: // 자코끝
				vNextPos = XMVectorSet(67.f, 0.f, 19.9f, 1.f);
				Change_NaviMesh(CLandObject::NAVI_HOUSE_1_1);

				XMStoreFloat4(&PlayerDir, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));
				m_pTransformCom->Set_Look(PlayerDir);
				m_pModelCom->Set_Animation(0); // Adv_Idle

				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::BGM);
				wsprintf(szBGM1, TEXT("BGM_House.mp3"));
				CSoundMgr::Get_Instance()->PlayBGM(szBGM1, 0.6f);

				break;
			case CHANGE_POSITON_HOUSE_2A: // 쿄우가이
				vNextPos = XMVectorSet(118.f, 0.f, 136.6f, 1.f);
				Change_NaviMesh(CLandObject::NAVI_HOUSE_4_0);

				XMStoreFloat4(&PlayerDir, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));
				m_pTransformCom->Set_Look(PlayerDir);
				m_pModelCom->Set_Animation(82); // battle_idle
				break;
			case CHANGE_POSITON_VILLAGE_1A: // 스왐프
				vNextPos = XMVectorSet(426.55f, 3.0f, 301.92f, 1.f);
				Change_NaviMesh(CLandObject::NAVI_VILLAGE_BATTLE);

				XMStoreFloat4(&PlayerDir, XMVector4Normalize(_vector{ 0.0f, 0.0f, -1.0f, 0.0f }));
				m_pTransformCom->Set_Look(PlayerDir);
				m_pModelCom->Set_Animation(82); // battle_idle
				break;
			case CHANGE_POSITON_VILLAGE_1B://스왐프 끝
				vNextPos = XMVectorSet(600.f, 4.5f, 317.f, 1.f);
				Change_NaviMesh(CLandObject::NAVI_VILLAGE_MAINROAD1);

				XMStoreFloat4(&PlayerDir, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));
				m_pTransformCom->Set_Look(PlayerDir);
				m_pModelCom->Set_Animation(0); // Adv_Idle

				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::BGM);
				wsprintf(szBGM2, TEXT("BGM_Village.mp3"));
				CSoundMgr::Get_Instance()->PlayBGM(szBGM2, 0.6f);

				break;
			default:
				break;
			}

			// 카메라와 플레이어 이동
			_vector vDist = vNextPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION); // 다음 이동지역과 플레이어 위치 차이

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNextPos);

			CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera"), 0));
			CTransform* pCameraTransform = pCamera->Get_Transform();

			_vector vCameraPos = pCameraTransform->Get_State(CTransform::STATE_POSITION);
			pCameraTransform->Set_State(CTransform::STATE_POSITION, vCameraPos + vDist);

			m_bChangePositionFinish[i] = true;
		}
	}

	Safe_Release(pGameInstance);
}

void CPlayer::Player_Change_Setting_Status(_double dTimeDelta)
{
	if (CPlayerManager::GetInstance()->Get_First_Setting_Status())
	{
		CPlayerManager::GetInstance()->Set_First_Setting_Status(false);

		m_StatusDesc.fHp = CPlayerManager::GetInstance()->Get_Hp();
		m_StatusDesc.fMp = CPlayerManager::GetInstance()->Get_Mp();
		m_StatusDesc.iSpecial_Cnt = CPlayerManager::GetInstance()->Get_Special_Cnt();
		m_StatusDesc.fSpecial = CPlayerManager::GetInstance()->Get_Special();
		m_StatusDesc.fSupport = CPlayerManager::GetInstance()->Get_Support();

		m_isSwap_OnSky = false;
		m_Moveset.m_isHitMotion = false;

		m_Moveset.m_isDownMotion = false;
		m_Moveset.m_isGetUpMotion = false;

		m_Moveset.m_isRestrict_Move = false;
		m_Moveset.m_isRestrict_KeyInput = false;
		m_Moveset.m_isRestrict_Jump = false;
		m_Moveset.m_isRestrict_JumpCombo = false;
		m_Moveset.m_isRestrict_Throw = false;
		m_Moveset.m_isRestrict_Charge = false;
		m_Moveset.m_isRestrict_Dash = false;
		m_Moveset.m_isRestrict_Step = false;
		m_Moveset.m_isRestrict_DoubleStep = false;
		m_Moveset.m_isRestrict_Special = false;

		m_Moveset.m_isRestrict_Adventure = false;

		m_Moveset.m_Down_Battle_Combo = false;
		m_Moveset.m_Down_Battle_Jump_Attack = false;
		m_Moveset.m_Down_Battle_Jump_TrackAttack = false;

		Set_FallingStatus(5.5f, 0.05f);

		m_isJump_Move = false;
		if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0)
		{
			m_pModelCom->Set_Animation(85);
		}
		else if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 1)
		{
			m_pModelCom->Set_Animation(58);
		}

		m_dDelay_Swapping_Pos = 0.0;

		m_isSwapping_State = true;
	}

	if (m_isSwapping_State)
	{
		_float4 MyPos;
		XMStoreFloat4(&MyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (MyPos.y <= m_fLand_Y + 0.01f)
		{
			m_isSwapping_State = false;
		}
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_CurLevelIdx() != LEVEL_VILLAGE)
	{
		m_dDelay_Swapping_Pos += dTimeDelta;
		if (m_dDelay_Swapping_Pos < 0.35f) //0.85
		{
			_float4 SwappingPos = CPlayerManager::GetInstance()->Get_Swaping_Pos();
			_float4 MyPos;
			XMStoreFloat4(&MyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			SwappingPos.y = MyPos.y;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&SwappingPos));
		}
	}
	Safe_Release(pGameInstance);
}


HRESULT CPlayer::Add_Components()
{
	m_CharacterDesc.NaviDesc.iCurrentIndex = 0;

	/* for.Com_Navigation_Village_MainRoad1 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_MainRoad1"),
		TEXT("Com_Navigation_Village_MainRoad1"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_MAINROAD1], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_MainRoad1: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Village_MainRoad2 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_MainRoad2"),
		TEXT("Com_Navigation_Village_MainRoad2"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_MAINROAD2], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_MainRoad2 : CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Village_InsideWall1*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_InsideWall1"),
		TEXT("Com_Navigation_Village_InsideWall1"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_INSIDEWALL1], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_InsideWall1: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Village_InsideWall2*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_InsideWall2"),
		TEXT("Com_Navigation_Village_InsideWall2"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_INSIDEWALL2], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_InsideWall2: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Village_Roof*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_Roof"),
		TEXT("Com_Navigation_Village_Roof"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_ROOF], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_Roof: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Village_Wall*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_Wall"),
		TEXT("Com_Navigation_Village_Wall"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_WALL], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_Wall: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Village_Battle*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_Battle"),
		TEXT("Com_Navigation_Village_Battle"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_BATTLE], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_Battle: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_House_0_0*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_0_0"),
		TEXT("Com_Navigation_House_0_0"), (CComponent**)&m_pNavigationCom[NAVI_HOUSE_0_0], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_House_0_0: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_House_1_0*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_1_0"),
		TEXT("Com_Navigation_House_1_0"), (CComponent**)&m_pNavigationCom[NAVI_HOUSE_1_0], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_House_1_0: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_House_1_1*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_1_1"),
		TEXT("Com_Navigation_House_1_1"), (CComponent**)&m_pNavigationCom[NAVI_HOUSE_1_1], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_House_1_1: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_House_2_0*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_2_0"),
		TEXT("Com_Navigation_House_2_0"), (CComponent**)&m_pNavigationCom[NAVI_HOUSE_2_0], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_House_2_0: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_House_3_0*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_3_0"),
		TEXT("Com_Navigation_House_3_0"), (CComponent**)&m_pNavigationCom[NAVI_HOUSE_3_0], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_House_3_0: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_House_4_0*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_4_0"),
		TEXT("Com_Navigation_House_4_0"), (CComponent**)&m_pNavigationCom[NAVI_HOUSE_4_0], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_House_4_0: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Train*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Train"),
		TEXT("Com_Navigation_Train"), (CComponent**)&m_pNavigationCom[NAVI_TRAIN], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Train: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Acaza*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Acaza"),
		TEXT("Com_Navigation_Acaza"), (CComponent**)&m_pNavigationCom[NAVI_ACAZA], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Acaza: CPlayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{

	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();
}
