#include "pch.h"
#include "..\Public\Player_Tanjiro.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"
#include "DialogManager.h"
#include "MissionManager.h"
#include "Battle_UI_Manager.h"

#include "PlayerManager.h"
#include "Fade_Manager.h"

#include "MonsterManager.h"
#include "WebManager.h"

#include "Camera_Manager.h"
#include "OptionManager.h"
#include "Camera_Free.h"
#include "GroundSmoke.h"
#include "WaterParticleEffect.h"
#include "EffectW_Manager.h"

#include "SwampManager.h"

#include "ParticleManager.h"

#include "WebManager.h"

CPlayer_Tanjiro::CPlayer_Tanjiro(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer(pDevice, pContext)
{
}

CPlayer_Tanjiro::CPlayer_Tanjiro(const CPlayer_Tanjiro& rhs)
	: CPlayer(rhs)
{
}

HRESULT CPlayer_Tanjiro::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Tanjiro::Initialize(void* pArg)
{
	m_ePlayerType = PLAYER_TANJIRO;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_pModelCom->Set_Animation(ANIM_BATTLE_IDLE);

	if (FAILED(Read_Animation_Control_File("Tanjiro.bin")))
	{
		MSG_BOX("Failed to AnimData Read : Tanjiro");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSword::SWORDDESC SwordDesc;
	ZeroMemory(&SwordDesc, sizeof SwordDesc);
	SwordDesc.m_PlayerName = CSword::PLAYER_TANJIRO;
	SwordDesc.pParentTransform = m_pTransformCom;
	SwordDesc.pBone = m_pModelCom->Get_Bone("R_HandCommon_1_Lct");
	m_pSword = dynamic_cast<CSword*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Sword"), &SwordDesc));

	CSwordHome::SWORDDESC SwordHomeDesc;
	ZeroMemory(&SwordHomeDesc, sizeof SwordHomeDesc);
	SwordHomeDesc.m_PlayerName = CSwordHome::PLAYER_TANJIRO;
	SwordHomeDesc.pParentTransform = m_pTransformCom;
	SwordHomeDesc.pBone = m_pModelCom->Get_Bone("L_Weapon_1");
	m_pSwordHome = dynamic_cast<CSwordHome*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_SwordHome"), &SwordHomeDesc));

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_FINALBOSS)
		m_pTransformCom->Set_Look(_float4{ 0.0f, 0.0f, -1.0f, 0.0f });
	else
		m_pTransformCom->Set_Look(_float4{ 0.0f, 0.0f, 1.0f, 0.0f });

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 150.f,0.f,150.f,1.f });


	//_vector{ 8.f, 0.f, 10.f, 1.f }
	m_ResetPos[0] = { 8.f, 0.f, 10.f, 1.f }; //첫지역
	m_ResetPos[1] = { 78.18f, 0.05f, 7.75f, 1.f }; // 처음 이동
	m_ResetPos[2] = { 75.1f, 0.05f, 67.63f, 1.f }; // 둘째 이동
	m_ResetPos[3] = { 198.1f, 0.05f, 32.95f, 1.f }; // 마지막 이동


	//CWebManager::GetInstance()->Set_TransformCom(m_pTransformCom);

	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();

	CAurora::EFFECTDESC AuroraDesc;
	AuroraDesc.pTransform = m_pTransformCom;
	AuroraDesc.pGameObject = this;
	AuroraDesc.eType = CAurora::TYPE_LOCAL;
	AuroraDesc.eCharacter = CAurora::CHARACTER_TANJIRO;
	AuroraDesc.eColor = CAurora::COLOR_SKY;

	for (_uint i = 0; i < 35; ++i)
		pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_Effect_Aurora"), TEXT("Prototype_GameObject_Aurora"), &AuroraDesc);

	/*if (iCurIdx == LEVEL_TRAIN || iCurIdx == LEVEL_HOUSE)
		m_fFar2 = 400.f;
	else*/
	m_fFar2 = 1.f;

	Safe_Release(pGameInstance);

	m_pModelCom->Set_LinearDuration(ANIM_BATTLE_IDLE, 0.4);

	return S_OK;
}

void CPlayer_Tanjiro::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_isSwap_OnSky == false)
	{
		m_pSword->Tick(dTimeDelta);
		m_pSwordHome->Tick(dTimeDelta);
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (!m_bSmell_Detection)
	{
		if (pGameInstance->Get_DIKeyDown(DIK_X))
		{
			m_bSmell_Detection = true;
			m_pRendererCom->Set_GrayScale();

			_tchar szSoundFile[MAX_PATH] = TEXT("ui_smellsense.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_SPECIAL, 0.4f);

		}
	}
	Smell_Detection(dTimeDelta);

	if (pGameInstance->Get_DIKeyDown(DIK_C))
	{
		m_pRendererCom->Set_RadialBlur();
		//m_pRendererCom->Set_GrayScale_On(false);
	}

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
	{
		if (m_isFirst_SwampUi == false)
		{
			//if (pGameInstance->Get_DIKeyDown(DIK_SPACE))
			if (CBattle_UI_Manager::GetInstance()->Get_Timing_Success())
			{
				CBattle_UI_Manager::GetInstance()->Set_Timing_Success(false);

				Jumping(2.5f, 0.08f);
				m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP);
				m_isSwamp_Escape = true;
				m_Moveset.m_isHitMotion = false;

				m_dDelay_SwampHit_Again = 0.0;

				m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_BigBlow(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Swamp(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Web(false);

				_tchar szSoundFile[MAX_PATH] = TEXT("st_swamp02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SKILL_EFFECT, 0.8f);

				Play_Sound_Atk(1, 0.76f);
			}
			//if (pGameInstance->Get_DIKeyDown(DIK_N))
			if (CBattle_UI_Manager::GetInstance()->Get_Timing_Failed())
			{
				CBattle_UI_Manager::GetInstance()->Set_Timing_Failed(false);

				Jumping(2.5f, 0.08f);
				m_pModelCom->Set_Animation(ANIM_FALL);
				m_isSwamp_Escape = true;
				m_Moveset.m_isHitMotion = false;

				m_isSwampBinding = false;

				m_dDelay_SwampHit_Again = 0.0;

				m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_BigBlow(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Swamp(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Web(false);

				_tchar szSoundFile[MAX_PATH] = TEXT("st_swamp02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SKILL_EFFECT, 0.8f);

				m_StatusDesc.fHp -= 6.0f;

				Play_Sound_Dmg(1, 0.76f);
			}
		}
	}


	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD8))
	{
		if (m_isStealthMode)
			m_isStealthMode = false;
		else
			m_isStealthMode = true;
	}

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD9))
	{
		//CSwampManager::GetInstance()->Set_Dmg(10.0f);
		CEffectPlayer::Get_Instance()->Play("Swamp_Explosion", m_pTransformCom);
	}

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD4))
	{
		m_StatusDesc.fHp -= 50.0f;
	}
	_int ak = m_pModelCom->Get_iCurrentAnimIndex();

	Safe_Release(pGameInstance);

	if (true == m_isDead)
		return;

	Key_Input_PlayerChange(dTimeDelta);
	//playerswap
	if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) //탄지로
	{
		Player_Change_Setting_Status(dTimeDelta);

		if (m_isSwapping_State == false)
			Animation_Control(dTimeDelta);
	}
	else
	{
		Player_Change(dTimeDelta);
	}

	if (m_isSwap_OnSky == false)
	{
		//애니메이션 처리
		m_pModelCom->Play_Animation(dTimeDelta);
		RootAnimation(dTimeDelta);

		//이벤트 콜
		EventCall_Control(dTimeDelta);

	}

	
	if (m_Moveset.m_iAwaken != 0)
	{


		if (55 == m_pModelCom->Get_iCurrentAnimIndex())
			m_isAuroraOn[0] = false;
		else
		{
			m_isAuroraOn[0] = true;
			m_dAwakenParticleAccTime += dTimeDelta;

			if (m_dAwakenParticleAccTime > 0.2)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Aurora_Particle0", m_pTransformCom);
				m_dAwakenParticleAccTime = 0.0;
			}
		}
	}
	else
		m_isAuroraOn[0] = false;

	if(pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
	CPlayerManager::GetInstance()->Set_Player_Pos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
}

void CPlayer_Tanjiro::LateTick(_double dTimeDelta)
{
	if (m_isSwap_OnSky == false)
	{
		m_pSword->LateTick(dTimeDelta);
		m_pSwordHome->LateTick(dTimeDelta);
	}

	if (m_isKyoRoomSuccess)
	{
		m_isKyoRoomSuccess = false;
		Play_Sound_Atk(1, 0.75f);
	}
	if (m_isKyoRoomDmg)
	{
		m_isKyoRoomDmg = false;
		Play_Sound_Dmg(1, 0.75f);
	}

	//playerswap
	if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) // 탄지로
	{
		__super::LateTick(dTimeDelta);

		if (m_isAirDashing == false)
			Gravity(dTimeDelta);

		if (m_isCan_AirDash)
		{
			m_dDelay_Can_AirDash += dTimeDelta;
			if (m_dDelay_Can_AirDash > 3.0f)
			{
				m_dDelay_Can_AirDash = 0.0;
				m_isCan_AirDash = false;
			}
		}

		CPlayerManager::GetInstance()->Set_PlayerPos_Change(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}


#ifdef _DEBUG
	if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom[m_eCurNavi])))
		return;
#endif
}

HRESULT CPlayer_Tanjiro::Render()
{
	if (m_isSwap_OnSky == false)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		m_pSword->Render();
		m_pSwordHome->Render();

#pragma region Player

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
		//Outline Render
		for (m_iMeshNum = 0; m_iMeshNum < iNumMeshes; m_iMeshNum++)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource(m_iMeshNum, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_ShaderBoneMatrices(m_iMeshNum, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			if (m_iMeshNum == 2)
				m_pShaderCom->Begin(2);
			else
			{
				if ((m_isSkilling == false) && (m_Moveset.m_iAwaken == 0)) { // 기본상태
					m_pShaderCom->Begin(1);
				}
				else if ((m_isSkilling == true) && (m_Moveset.m_iAwaken == 0)) { // 노 개방 스킬 썻을 때
					m_pShaderCom->Begin(9);
				}
				else if ((m_isSkilling == false) && (m_Moveset.m_iAwaken != 0)) { // 개방 했을 때
					if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_BATTLE_AWAKEN_COMPLETE_CUTSCENE)
						m_pShaderCom->Begin(1);
					else
						m_pShaderCom->Begin(5);
				}
				else if ((m_isSkilling == true) && (m_Moveset.m_iAwaken != 0)) { // 개방하고 스킬 썻을 때
					m_pShaderCom->Begin(9);
				}
			}

			m_pModelCom->Render(m_iMeshNum);
		}
		// Default Render
		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_ShaderBoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				return E_FAIL;

			m_pShaderCom->Begin(0);

			m_pModelCom->Render(i);
		}

#pragma endregion
	}
	return S_OK;
}

HRESULT CPlayer_Tanjiro::Render_ShadowDepth()
{
	if (FAILED(__super::Render_ShadowDepth()))
		return E_FAIL;

	return S_OK;
}


void CPlayer_Tanjiro::EventCall_Control(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0.0)
	{
		m_iEvent_Index = 0;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();



	_vector vPlayerDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float fDmg = 1.0f;
	if (m_Moveset.m_iAwaken == 1)
		fDmg = 1.2f;
	else if (m_Moveset.m_iAwaken == 2)
		fDmg = 1.5f;


	if (EventCallProcess())
	{
#pragma region Combo_Attack
		if (ANIM_ATK_COMBO == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo1", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo1", m_pTransformCom);

				Play_Sound_Atk(0, 0.75); // 0:small , 1:medium

				_tchar szSoundFile[MAX_PATH] = TEXT("st_sword04.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.4f);

				if (m_Moveset.m_iAwaken >= 1)
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("water_01.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_AWAKEN_0, 0.5f);
				}
			}
			else if (1 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo1_Decal", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo1_DecalParticle", m_pTransformCom);

				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo1_WaterParticle", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f * fDmg);
			}
		}
		if (22 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo2", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo2", m_pTransformCom);

				Play_Sound_Atk(0, 0.75f); // 0:small , 1:medium

				if (m_Moveset.m_iAwaken >= 1)
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("water_02.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_AWAKEN_1, 0.5f);
				}

			}
			else if (1 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo2_WaterParticle1", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f * fDmg);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_S.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_1, 0.4f);
			}
			else if (2 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo2_Decal", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo2_DecalParticle", m_pTransformCom);

				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo2_WaterParticle2", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f * fDmg);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.4f);


			}

		}
		if (23 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo3", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo3", m_pTransformCom);

				Play_Sound_Atk(1, 0.75f); // 0:small , 1:medium

				if (m_Moveset.m_iAwaken >= 1)
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("water_03.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_AWAKEN_0, 0.5f);
				}
			}
			else if (1 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo3_Decal", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo3_DecalParticle", m_pTransformCom);

				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo3_WaterParticle", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f * fDmg);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_S_1.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_1, 0.4f);
			}
		}

		if (24 == m_pModelCom->Get_iCurrentAnimIndex()) //Combo_Down
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Down", m_pTransformCom);
				else
				{
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo4_Down", m_pTransformCom);

					CCameraManager::GetInstance()->Side_Zoom_Out(2.f, 2.f);
				}

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.6f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_ComboDown_Kick", m_pTransformCom, &EffectWorldDesc);
				Create_GroundSmoke(CGroundSmoke::SMOKE_TANJIRO_COMBODOWN_KICK, vPlayerDir * 1.5f);

				Play_Sound_Atk(1, 0.75f); // 0:small , 1:medium
			}
			if (1 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BIG, vPlayerDir, 2.0f * fDmg);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_S.ogg");//hit_S
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.4f);

			}
			else if (2 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Down_Decal1", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Down_Decal1Particle", m_pTransformCom);

				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo4_Down_WaterParticle1", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f * fDmg);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_L.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.4f);

				if (m_Moveset.m_iAwaken >= 1)
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("water_01.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_AWAKEN_0, 0.5f);
				}
			}
			else if (3 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Down_Decal2", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Down_Decal2Particle", m_pTransformCom);

				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo4_Down_WaterParticle2", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.5f), 0.1,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 2.0f);

				_vector vPlusPos = vPlayerDir * 3.0f;

				Create_GroundSmoke(CGroundSmoke::SMOKE_TANJIRO_COMBODOWN_SPREAD, vPlusPos);
				Create_GroundSmoke(CGroundSmoke::SMOKE_TANJIRO_COMBODOWN_UPDOWN, vPlusPos);
				Create_StoneParticle(CStoneParticle::STONE_TANJIRO_COMBODOWN, vPlusPos);
				Create_SmeshStone(vPlusPos * 1.5f, 3.f);
				Camera_Shake(0.6);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_L.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.4f);


			}
		}
		if (25 == m_pModelCom->Get_iCurrentAnimIndex()) // Combo_Normal
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Normal", m_pTransformCom);
				else
				{
					CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
					EffectWorldDesc.fScale = 0.8f;
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo4", m_pTransformCom, &EffectWorldDesc);

					CCameraManager::GetInstance()->Side_Zoom_Out(2.f, 1.5f);
				}

				Play_Sound_Atk(1, 0.75f); // 0:small , 1:medium
			}
			else if (1 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo4_Normal_WaterParticle", m_pTransformCom);
			}
			else if (2 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BIG, vPlayerDir, 2.0f * fDmg);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_L.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.4f);

				if (m_Moveset.m_iAwaken >= 1)
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("water_02.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_AWAKEN_0, 0.5f);
				}
			}
		}
		if (26 == m_pModelCom->Get_iCurrentAnimIndex()) //Combo_Up
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Up", m_pTransformCom);
				else
				{
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo4_Up", m_pTransformCom);

					CCameraManager::GetInstance()->Side_Zoom_Out(2.f, 1.5f);
				}

				Play_Sound_Atk(1, 0.75f); // 0:small , 1:medium
			}
			if (1 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Up_Decal", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Up_DecalParticle", m_pTransformCom);

				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo4_Up_WaterParticle", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_UPPER, vPlayerDir, 2.0f * fDmg);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_L.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.4f);

				if (m_Moveset.m_iAwaken >= 1)
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("water_03.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_AWAKEN_0, 0.5f);
				}
			}
		}
		if (27 == m_pModelCom->Get_iCurrentAnimIndex()) //Combo_Surge_Attack
		{
			if (0 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.5,
					CAtkCollider::TYPE_CUTSCENE, vPlayerDir, 9.0f * fDmg);

				Play_Sound_Atk(1, 0.75f); // 0:small , 1:medium
			}
		}

		if (ANIM_ATK_SPECIAL_CUTSCENE == m_pModelCom->Get_iCurrentAnimIndex())
		{

		}


		if (ANIM_ATK_AIRCOMBO == m_pModelCom->Get_iCurrentAnimIndex()) //Combo_air
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo_Air1", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo_Air1", m_pTransformCom);

				//Play_Sound_Atk(0, 0.6f);
			}
			if (1 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo_Air1_WaterParticle", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.0f * fDmg);

				Play_Sound_Atk(1, 0.75f);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_S.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.4f);

				if (m_Moveset.m_iAwaken >= 1)
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("water_01.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_AWAKEN_0, 0.5f);
				}
			}
		}
		if (30 == m_pModelCom->Get_iCurrentAnimIndex()) //Combo_
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo_Air2", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo_Air2", m_pTransformCom);

			}
			if (1 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo_Air2_WaterParticle", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 2.0f * fDmg);

				Play_Sound_Atk(1, 0.75f);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_1, 0.4f);
			}
		}
#pragma endregion

#pragma region Super_Skill
		if (ANIM_ATK_SKILL_NORMAL == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CBattle_UI_Manager::GetInstance()->Set_Player_Type(0);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(0);

				CCameraManager::GetInstance()->Side_Zoom_Out(2.f, 2.f);
			}
			else if (1 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Mesh", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Tex", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Particle", m_pTransformCom);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y -= 4.f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Particle_SplashCircle", m_pTransformCom, &EffectWorldDesc);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				Camera_Shake(0.1, 30);
			}
			else if (2 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Particle_SplashFall", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.z += 4.f;
				EffectWorldDesc.vPosition.y += 2.f;
				EffectWorldDesc.fScale = 4.f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Wind2", m_pTransformCom, &EffectWorldDesc);

				Camera_Shake(0.1, 30);
			}
			else if (3 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Particle_SplashFall", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Particle_SplashCircle", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.z += 4.f;
				EffectWorldDesc.vPosition.y += 3.5f;
				EffectWorldDesc.fScale = 6.3f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Wind", m_pTransformCom, &EffectWorldDesc);

				Camera_Shake(0.1, 30);
			}
			else if (4 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Particle_WaterDrop", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.z += 4.f;
				EffectWorldDesc.vPosition.y += 1.1f;
				EffectWorldDesc.fScale = 5.4f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Wind", m_pTransformCom, &EffectWorldDesc);

				Camera_Shake(0.1, 30);
			}
			else if (5 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Particle_SplashCircle", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 10.0f * fDmg);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.z += 4.f;
				EffectWorldDesc.vPosition.y += 3.7f;
				EffectWorldDesc.fScale = 6.5f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Wind2", m_pTransformCom, &EffectWorldDesc);

				Camera_Shake(0.1, 30);
			}
			else if (6 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.z += 4.f;
				EffectWorldDesc.vPosition.y += 2.5f;
				EffectWorldDesc.fScale = 5.5f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Wind", m_pTransformCom, &EffectWorldDesc);

				Camera_Shake(0.1, 30);
			}
			else if (7 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Particle_SplashCircle", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_ParticleFountain", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.z += 4.f;
				EffectWorldDesc.vPosition.y += 1.5f;
				EffectWorldDesc.fScale = 4.3f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Wind2", m_pTransformCom, &EffectWorldDesc);

				Camera_Shake(0.1, 30);
			}
			else if (8 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.z += 4.f;
				EffectWorldDesc.vPosition.y += 2.0f;
				EffectWorldDesc.fScale = 3.0f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Wind2", m_pTransformCom, &EffectWorldDesc);

				Camera_Shake(0.1, 30);
			}
			else if (9 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Particle_SplashCircle", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.z += 4.f;
				EffectWorldDesc.vPosition.y += 4.0f;
				EffectWorldDesc.fScale = 5.5f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_Wind", m_pTransformCom, &EffectWorldDesc);

				Camera_Shake(0.1, 30);
			}
			else if (10 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				Camera_Shake(0.1, 30);
			}
			else if (11 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1_ParticleFountainMain", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				Camera_Shake(0.1, 30);
			}
		}

		if (ANIM_ATK_SKILL_MOVE == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				m_pRendererCom->Set_BloomRatio(0.25f);
				CBattle_UI_Manager::GetInstance()->Set_Player_Type(0);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(1);

				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super2", m_pTransformCom);

				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super2_ParticleSmoke", m_pTransformCom);
			}
		}
		if (40 == m_pModelCom->Get_iCurrentAnimIndex()) // SKILL_MOVE 도중
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super2_ParticleSmoke", m_pTransformCom);

				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);
				Camera_Shake(0.1, 40);
			}
			if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);
			}
			if (2 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super2_ParticleSmoke", m_pTransformCom);

				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);

				Camera_Shake(0.1, 40);
			}
			if (3 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super2_Particle_rad2", m_pTransformCom);
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f * fDmg);
			}
		}
		if (41 == m_pModelCom->Get_iCurrentAnimIndex()) // SKILL_MOVE 도중
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super2_ParticleSmoke", m_pTransformCom);

				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_BIG, vPlayerDir, 10.0f * fDmg);

				Camera_Shake(0.1, 40);
			}
			if (1 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super2_ParticleSmoke", m_pTransformCom);

				m_pRendererCom->Set_BloomRatio(1.f);
			}
			if (2 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super2_ParticleSmoke", m_pTransformCom);

				m_pRendererCom->Set_BloomRatio(1.f);
			}
			if (3 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super2_ParticleSmoke", m_pTransformCom);
			}
			if (4 == m_iEvent_Index)
			{
				m_pRendererCom->Set_BloomRatio(1.f);
			}

		}

		if (ANIM_ATK_SKILL_GUARD == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_Particle", m_pTransformCom);

				CCameraManager::GetInstance()->Zoom_Out(2.f, 2.f);

				CBattle_UI_Manager::GetInstance()->Set_Player_Type(0);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(2);
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(5.5f, 5.5f, 5.5f), _float3(0.f, 0.0f, 0.0f), 0.3,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 2.0f * fDmg);

				m_pRendererCom->Set_BloomRatio();
			}
			if (1 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.vPosition.y -= 2.5f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle0", m_pTransformCom, &EffectSideStepDesc);

				CBattle_UI_Manager::GetInstance()->Set_Player_Type(0);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(2);
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(5.5f, 5.5f, 5.5f), _float3(0.f, 0.0f, 0.0f), 0.3,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 3.0f * fDmg);
			}
			if (2 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle1", m_pTransformCom);

				CBattle_UI_Manager::GetInstance()->Set_Player_Type(0);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(2);
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(5.5f, 5.5f, 5.5f), _float3(0.f, 0.0f, 0.0f), 0.3,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 5.0f * fDmg);

				Camera_Shake(0.1, 50);
			}
			if (3 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.vPosition.y += 2.f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle2", m_pTransformCom, &EffectSideStepDesc);

				CBattle_UI_Manager::GetInstance()->Set_Player_Type(0);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(2);
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(5.5f, 5.5f, 5.5f), _float3(0.f, 0.0f, 0.0f), 0.3,
					CAtkCollider::TYPE_UPPER, vPlayerDir, 9.0f * fDmg);
			}
			if (4 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.vPosition.y -= 2.5f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle0", m_pTransformCom, &EffectSideStepDesc);
				
				Camera_Shake(0.1, 50);
			}
			if (5 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle1", m_pTransformCom);

				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc2;
				EffectSideStepDesc2.vPosition.y += 4.f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle3", m_pTransformCom, &EffectSideStepDesc2);
			}
			if (6 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.vPosition.y += 2.f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle2", m_pTransformCom, &EffectSideStepDesc);
			
				Camera_Shake(0.1, 50);
			}
			if (7 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.vPosition.y -= 2.5f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle0", m_pTransformCom, &EffectSideStepDesc);
			}
			if (8 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle1", m_pTransformCom);

				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc2;
				EffectSideStepDesc2.vPosition.y += 4.f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle3", m_pTransformCom, &EffectSideStepDesc2);
			
				Camera_Shake(0.1, 50);
			}
			if (9 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.vPosition.y += 2.f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle2", m_pTransformCom, &EffectSideStepDesc);

			}
			if (10 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.vPosition.y -= 2.5f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle0", m_pTransformCom, &EffectSideStepDesc);
			
				Camera_Shake(0.1, 50);
			}
			if (11 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle1", m_pTransformCom);

				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc2;
				EffectSideStepDesc2.vPosition.y += 4.f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle3", m_pTransformCom, &EffectSideStepDesc2);
			}
			if (12 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.vPosition.y += 2.f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3_ParticleCircle2", m_pTransformCom, &EffectSideStepDesc);
			}
		}
#pragma endregion

#pragma region Air_Attack
		if (ANIM_ATK_AIRTRACK == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 0.0f, 0.5f), 0.6,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.0f * fDmg);


			}

		}
		if (50 == m_pModelCom->Get_iCurrentAnimIndex())
		{

			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo_Air3", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo_Air3", m_pTransformCom);

				Play_Sound_Atk(2, 0.75f);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_05.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.5f);

				_tchar szSoundFile1[MAX_PATH] = TEXT("st_sword03.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::SWORD_1, 0.5f);
			}
			else if (1 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo_Air3_WaterParticle", m_pTransformCom);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y -= 1.5f;
				EffectWorldDesc.vPosition.z += 2.f;

				CEffectPlayer::Get_Instance()->Play("Tanjiro_Particle_W", m_pTransformCom, &EffectWorldDesc);
			}
		}

		if (51 == m_pModelCom->Get_iCurrentAnimIndex()) // 점프공격 착지
		{
			if (0 == m_iEvent_Index) // 0.05
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_DASHLAND);
				Create_StoneParticle(CStoneParticle::STONE_LAND);
			}
		}
#pragma endregion

#pragma region Dash_Tackle
		if (ANIM_BATTLE_DASH == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.5f, 1.5f), 5.0,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.0f * fDmg);
			}
		}
#pragma endregion

#pragma region Charge_Attack
		if (33 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
				{
					CEffectPlayer::Get_Instance()->Play("Tanjiro_Tilt", m_pTransformCom);
					CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
					EffectWorldDesc.fScale = 0.6f;
					EffectWorldDesc.vPosition.y -= 0.5f;
					CEffectPlayer::Get_Instance()->Play("Tanjiro_Tilt_Wind", m_pTransformCom, &EffectWorldDesc);
				}
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeTilt", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.7f);
			}
			else if (1 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeTilt_WaterParticle1", m_pTransformCom);

				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.5f, 1.5f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 2.0f * fDmg);

				_tchar szSoundFile[MAX_PATH] = TEXT("st_sword04.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_1, 0.7f);
			}
			else if (2 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.5f, 1.5f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 2.0f * fDmg);

				_tchar szSoundFile[MAX_PATH] = TEXT("st_sword07.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.7f);
			}
			else if (3 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken != 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeTilt_WaterParticle2", m_pTransformCom);

				CEffectPlayer::Get_Instance()->Play("Tanjiro_Tilt_Decal", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Tilt_DecalParticle", m_pTransformCom);

				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.5f, 1.5f), 0.1,
					CAtkCollider::TYPE_BLOW, vPlayerDir, 2.0f * fDmg);
			}
		}
		if (32 == m_pModelCom->Get_iCurrentAnimIndex()) // 차지 다 된모션
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Charge_Effect", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Charge_Particle", m_pTransformCom);
				Camera_Shake(0.05);

				_tchar szSoundFile[MAX_PATH] = TEXT("flash_01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::SWORD_0, 0.7f);
			}
		}

#pragma endregion

#pragma region Awake

		if (ANIM_BATTLE_AWAKEN == m_pModelCom->Get_iCurrentAnimIndex()) // 54 개방
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				CEffectPlayer::Get_Instance()->Play("Tanjiro_GaeBang", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_GaeBang_Particle", m_pTransformCom);
				Camera_Shake(0.1);
				m_pRendererCom->Set_RadialBlur();

			}
			else if (1 == m_iEvent_Index)	// 0.3
			{
				m_pRendererCom->Set_RadialBlur();

			}
		}
		if (ANIM_BATTLE_AWAKEN_COMPLETE_CUTSCENE == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.2f;
				EffectWorldDesc.vPosition.x -= 0.1f;
				EffectWorldDesc.vPosition.z -= 0.1f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Awake_Cutscene_Breath", m_pTransformCom, &EffectWorldDesc);
			
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc2;
				EffectWorldDesc2.vPosition.y += 0.3f;
				EffectWorldDesc2.vPosition.x -= 0.3f;
				EffectWorldDesc2.vPosition.z -= 0.3f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Awake_Cutscene_Sword", m_pTransformCom, &EffectWorldDesc2);
			}

			if (1 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.2f;
				EffectWorldDesc.vPosition.x -= 0.1f;
				EffectWorldDesc.vPosition.z -= 0.1f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Awake_Cutscene_Particle", m_pTransformCom, &EffectWorldDesc);

				CEffectPlayer::Get_Instance()->Play("Tanjiro_GaeBang_Particle", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Tanjiro_GaeBang_Particle", m_pTransformCom);
			}
		}

#pragma endregion

#pragma region Move & Hitted
		//if (4 == m_pModelCom->Get_iCurrentAnimIndex())	// 착지(탐험)
		//{
		//	if (0 == m_iEvent_Index)	// 0.0
		//		Create_GroundSmoke(CGroundSmoke::SMOKE_DASHLAND);
		//}

		if (9 == m_pModelCom->Get_iCurrentAnimIndex())	// 어드벤쳐 달리기
		{
			//if (0 == m_iEvent_Index)	// 0.0
			//	Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			//else if (1 == m_iEvent_Index)	// 0.28
			//	Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			//else if (2 == m_iEvent_Index)	// 0.7
			//	Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
		}

		if (10 == m_pModelCom->Get_iCurrentAnimIndex())	// 어드벤쳐 달리기멈춤
		{
			//if (0 == m_iEvent_Index)	// 0.0
			//	Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
		}

		if (80 == m_pModelCom->Get_iCurrentAnimIndex()) // 겁나달리기
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
			else if (1 == m_iEvent_Index)	// 0.05
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
			else if (2 == m_iEvent_Index)	// 0.10
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
			else if (3 == m_iEvent_Index)	// 0.15
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
			else if (4 == m_iEvent_Index)	// 0.20
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
			else if (5 == m_iEvent_Index)	// 0.25
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
			else if (6 == m_iEvent_Index)	// 0.30
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}

		}

		if (83 == m_pModelCom->Get_iCurrentAnimIndex())	// 점프
		{
			if (0 == m_iEvent_Index)
				Create_GroundSmoke(CGroundSmoke::SMOKE_JUMP);
		}

		if (86 == m_pModelCom->Get_iCurrentAnimIndex())	// 착지
		{
			if (0 == m_iEvent_Index)	// 0.02
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_DASHLAND);
				Create_StoneParticle(CStoneParticle::STONE_LAND);
			}
		}

		if (ANIM_BATTLE_RUN == m_pModelCom->Get_iCurrentAnimIndex())	// 달리기
		{

		}
		if (88 == m_pModelCom->Get_iCurrentAnimIndex())	// 달리기
		{
			if (0 == m_iEvent_Index)	// 0.0
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			else if (1 == m_iEvent_Index)	// 0.12
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			else if (2 == m_iEvent_Index)	// 0.24
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			else if (3 == m_iEvent_Index)	// 0.36
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			else if (4 == m_iEvent_Index)	// 0.48
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			else if (5 == m_iEvent_Index)	// 0.60
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			else if (6 == m_iEvent_Index)	// 0.72
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);


		}

		if (89 == m_pModelCom->Get_iCurrentAnimIndex())	// 달리기 Stop
		{
			if (0 == m_iEvent_Index)	// 0.05
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
		}
		/*ANIM_BATTLE_STEP_AB = 91, ANIM_BATTLE_STEP_AF = 92, ANIM_BATTLE_STEP_AL = 93, ANIM_BATTLE_STEP_AR = 94,
			ANIM_BATTLE_STEP_B = 95, ANIM_BATTLE_STEP_F = 96, ANIM_BATTLE_STEP_L = 97, ANIM_BATTLE_STEP_R = 99,*/
		if (95 == m_pModelCom->Get_iCurrentAnimIndex())	// Step
		{
			if (!m_isJumpOn)
			{
				if (0 == m_iEvent_Index)	// 0.0
				{
					Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);

					CEffectPlayer::Get_Instance()->Play("Tanjiro_Dash_Particle", m_pTransformCom);

					Play_Sound_Atk(0, 0.7f);
				}
			}
		}

		if (96 == m_pModelCom->Get_iCurrentAnimIndex())	// Step
		{
			if (!m_isJumpOn)
			{
				if (0 == m_iEvent_Index)	// 0.0
				{
					Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);

					CEffectPlayer::Get_Instance()->Play("Tanjiro_Dash_Particle", m_pTransformCom);

					Play_Sound_Atk(0, 0.7f);
				}
			}
		}

		if (97 == m_pModelCom->Get_iCurrentAnimIndex())	// Step
		{
			if (!m_isJumpOn)
			{
				if (0 == m_iEvent_Index)	// 0.0
					Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);

				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.fScale = 1.6f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_SideStep", m_pTransformCom, &EffectSideStepDesc);
				EffectSideStepDesc.vPosition.y -= 0.01f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_SideStep", m_pTransformCom, &EffectSideStepDesc);

				CEffectPlayer::Get_Instance()->Play("Tanjiro_Dash_Particle", m_pTransformCom);

				Play_Sound_Atk(0, 0.7f);
			}
		}

		if (98 == m_pModelCom->Get_iCurrentAnimIndex())	// Step
		{
			if (!m_isJumpOn)
			{
				if (0 == m_iEvent_Index)	// 0.0
					Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);

				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.fScale = 1.6f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_SideStep", m_pTransformCom, &EffectSideStepDesc);
				EffectSideStepDesc.vPosition.y -= 0.01f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_SideStep", m_pTransformCom, &EffectSideStepDesc);

				CEffectPlayer::Get_Instance()->Play("Tanjiro_Dash_Particle", m_pTransformCom);

				Play_Sound_Atk(0, 0.7f);
			}
		}

		if (99 == m_pModelCom->Get_iCurrentAnimIndex())	// Step
		{
			if (!m_isJumpOn)
			{
				if (0 == m_iEvent_Index)	// 0.0
					Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);

				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.fScale = 1.6f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_SideStep", m_pTransformCom, &EffectSideStepDesc);
				EffectSideStepDesc.vPosition.y -= 0.01f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_SideStep", m_pTransformCom, &EffectSideStepDesc);

				CEffectPlayer::Get_Instance()->Play("Tanjiro_Dash_Particle", m_pTransformCom);

				Play_Sound_Atk(0, 0.7f);
			}
		}

		if (100 == m_pModelCom->Get_iCurrentAnimIndex())	// Step
		{
			if (!m_isJumpOn)
			{
				if (0 == m_iEvent_Index)	// 0.0
					Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);

				CEffectPlayer::EFFECTWORLDDESC EffectSideStepDesc;
				EffectSideStepDesc.fScale = 1.6f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_SideStep", m_pTransformCom, &EffectSideStepDesc);
				EffectSideStepDesc.vPosition.y -= 0.01f;
				CEffectPlayer::Get_Instance()->Play("Tanjiro_SideStep", m_pTransformCom, &EffectSideStepDesc);

				CEffectPlayer::Get_Instance()->Play("Tanjiro_Dash_Particle", m_pTransformCom);

				Play_Sound_Atk(0, 0.7f);
			}
		}

		if (121 == m_pModelCom->Get_iCurrentAnimIndex())	// Blow 쓰러짐
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);

				Play_FallDownEffect();
				Play_Sound_BodyFall();
			}
		}


		if (126 == m_pModelCom->Get_iCurrentAnimIndex())	// 맞고 쓰러짐(2번)
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);

				Play_FallDownEffect();
				Play_Sound_BodyFall();
			}
			else if (1 == m_iEvent_Index)	// 0.58
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_Sound_BodyFall();
			}
		}

		if (128 == m_pModelCom->Get_iCurrentAnimIndex())	// 맞고 떼구르르 구름
		{
			if (0 == m_iEvent_Index)	// 0.2
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_Sound_BodyFall();
			}
			else if (1 == m_iEvent_Index)	// 0.5
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
			else if (2 == m_iEvent_Index)	// 0.93
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
			else if (3 == m_iEvent_Index)	// 1.1
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
			else if (4 == m_iEvent_Index)	// 1.31
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
			else if (5 == m_iEvent_Index)	// 1.6
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
		}

		if (133 == m_pModelCom->Get_iCurrentAnimIndex())	// Spin되면서 떨어짐
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_FallDownEffect();
				Play_Sound_BodyFall();
			}

		}

#pragma endregion


		m_iEvent_Index++;
	}

	Safe_Release(pGameInstance);
}

void CPlayer_Tanjiro::Animation_Control(_double dTimeDelta)
{
	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta);


	/*
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_DIKeyDown(DIK_Z))
		m_isBattleStart = true;
	Safe_Release(pGameInstance);
	*/
	if (m_isBattleStart && CMonsterManager::GetInstance()->Get_Akaza_On())
	{
		m_dDelay_BattleStart += dTimeDelta;
		if (m_dDelay_BattleStart > 0.3f) //3~
		{
			m_pModelCom->Set_Animation(ANIM_BATTLESTART);
			m_isBattleStart = false;
			m_dDelay_BattleStart = 0.0;
		}
	}


	Moving_Restrict();

	if (m_ePlayerState == PLAYER_BATTLE)
	{
		//사망시
		if (m_StatusDesc.fHp <= 0.0f && m_isPlayerTanjiroDead == false )
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			if(pGameInstance->Get_CurLevelIdx() == LEVEL_FINALBOSS)
				m_isPlayerTanjiroDead = true;

			Safe_Release(pGameInstance);
		}

		if (m_isPlayerTanjiroDead)
		{
			Event_Tanjiro_Death(dTimeDelta);
		}
		else
		{
			Animation_Control_Battle_Dmg(dTimeDelta);

			if (m_Moveset.m_isHitMotion == false)
			{
				Animation_Control_Battle_Jump(dTimeDelta);

				Animation_Control_Battle_Move(dTimeDelta);


				Animation_Control_Battle_Attack(dTimeDelta);

				Animation_Control_Battle_Charge(dTimeDelta);

				Animation_Control_Battle_Skill(dTimeDelta);

				Animation_Control_Battle_Guard(dTimeDelta);

				Animation_Control_Battle_Dash(dTimeDelta);

				Animation_Control_Battle_Awaken(dTimeDelta);

				Animation_Control_Battle_Special(dTimeDelta);

			}
		}
	}
	else if (m_ePlayerState == PLAYER_ADVENTURE)
	{
		Animation_Control_Adventure_Move(dTimeDelta);

		Animation_Control_Adventure_Act(dTimeDelta);
	}
}

void CPlayer_Tanjiro::Animation_Control_Battle_Move(_double dTimeDelta)
{
	//무빙제한시 리턴
	if (m_Moveset.m_isRestrict_Move)
	{
		return;
	}
	//무빙제한이 풀릴시 이동 설정, 버그수정
	if (m_Moveset.m_isPressing_While_Restrict)
	{
		m_Moveset.m_isPressing_While_Restrict = false;
		m_Moveset.m_Down_Battle_Run = true;
	}


	//무빙키입력들
	if (m_Moveset.m_Down_Battle_Run)
	{
		m_Moveset.m_Down_Battle_Run = false;
		m_pModelCom->Set_Animation(ANIM_BATTLE_RUN);
	}

	if (m_Moveset.m_State_Battle_Run)
	{
		m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		//m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.5f);
		m_fMove_Speed = 2.0f;

		if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_BATTLE_RUN || m_pModelCom->Get_iCurrentAnimIndex() == 88)
		{
			if (m_isSwamp_Escape == false)
			{
				if (m_isCanNavi)
				{
					//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange, m_pNavigationCom[m_eCurNavi]);
					Go_Straight_Constant(dTimeDelta, ANIM_BATTLE_RUN, m_fMove_Speed * m_fScaleChange);
					Go_Straight_Constant(dTimeDelta, 88, m_fMove_Speed * m_fScaleChange);
				}
				else
				{
					//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange);
					Go_Straight_Constant(dTimeDelta, ANIM_BATTLE_RUN, m_fMove_Speed * m_fScaleChange, true);
					Go_Straight_Constant(dTimeDelta, 88, m_fMove_Speed * m_fScaleChange, true);
				}
			}
		}
		//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed);
	}
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_BATTLE_IDLE)
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) || pGameInstance->Get_DIKeyState(DIK_A) || pGameInstance->Get_DIKeyState(DIK_S) || pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_Moveset.m_Down_Battle_Run = true;
		}
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == 88)
	{
		Play_Sound_Move(dTimeDelta, 0.16f);
	}
	Safe_Release(pGameInstance);

	if (m_Moveset.m_Up_Battle_Run)
	{
		m_Moveset.m_Up_Battle_Run = false;
		m_pModelCom->Set_Animation(ANIM_BATTLE_RUN_END);
	}
	Go_Straight_Deceleration(dTimeDelta, ANIM_BATTLE_RUN_END, m_fMove_Speed * m_fScaleChange, 0.15f);
}

void CPlayer_Tanjiro::Animation_Control_Battle_Jump(_double dTimeDelta)
{
	if (m_isSwamp_Escape == false && m_isSwampHit == false)
	{
		if (m_Moveset.m_Down_Battle_JumpMove)
		{
			m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
			//m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
			m_Moveset.m_Down_Battle_JumpMove = false;
			m_isJump_Move = true;

			m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP);
			Jumping(4.0f * m_fScaleChange, 0.2f * m_fScaleChange);

			m_pModelCom->Set_EarlyEnd(85, true);

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Small_Hue.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, 0.8f);
		}
		if (m_isJump_Move)
		{
			Go_Straight_Constant(dTimeDelta, ANIM_BATTLE_JUMP, m_fMove_Speed * 1.2f * m_fScaleChange);
			Go_Straight_Constant(dTimeDelta, 84, m_fMove_Speed * 1.2f * m_fScaleChange);
			Go_Straight_Constant(dTimeDelta, 85, m_fMove_Speed * 1.2f * m_fScaleChange);
			//Go_Straight_Deceleration(dTimeDelta, 86, m_fMove_Speed * 1.2f * m_fScaleChange, 0.36f * m_fScaleChange); // Down
		}
		Ground_Animation_Play(85, 86);
		m_pModelCom->Set_LinearDuration(ANIM_BATTLE_JUMP, 0.001f);
		m_pModelCom->Set_LinearDuration(84, 0.001f);
		m_pModelCom->Set_LinearDuration(85, 0.001f);
		m_pModelCom->Set_LinearDuration(86, 0.001f);


		if (m_Moveset.m_Down_Battle_Jump)
		{
			m_Moveset.m_Down_Battle_Jump = false;
			m_isJump_Move = false;

			m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP);
			Jumping(4.0f * m_fScaleChange, 0.2f * m_fScaleChange);

			m_pModelCom->Set_EarlyEnd(85, true);

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Small_Hue.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, 0.8f);
		}
	}
	//늪빠짐 점프
	else if (m_isSwamp_Escape || m_isSwampHit)
	{
		if (m_Moveset.m_Down_Battle_Jump)
		{
			m_Moveset.m_Down_Battle_Jump = false;
			m_isJump_Move = false;

			m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP);
			Jumping(0.3f, 0.02f);

			m_pModelCom->Set_EarlyEnd(85, true);
		}
	}


	//공중 공격 콤보
	if (m_Moveset.m_Down_Battle_Jump_Attack)
	{
		m_Moveset.m_Down_Battle_Jump_Attack = false;
		m_isJump_Move = false;

		if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
			m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));

		//콤보 첫 애니메이션 설정
		if (m_pModelCom->Get_Combo_Doing() == false)
		{
			m_pModelCom->Set_Combo_Doing(true);
			m_pModelCom->Set_Animation(ANIM_ATK_AIRCOMBO);
			m_pModelCom->Set_EarlyEnd(ANIM_ATK_AIRCOMBO, true, 0.5f);

			JumpStop(0.3);
		}
		//아닐경우, 다음 콤보로 진행
		else
		{
			m_pModelCom->Set_Combo_Trigger(true);

			m_pModelCom->Set_EarlyEnd(30, true, 0.7f);
			JumpStop(0.55);
			m_isFirst_JumpAtk = false;
		}
	}
	m_pModelCom->Set_EarlyEnd(ANIM_ATK_AIRCOMBO, false, 0.5f);


	//공중 트랙공격 (이동키 + 공격키)
	if (m_Moveset.m_Down_Battle_Jump_TrackAttack)
	{
		m_Moveset.m_Down_Battle_Jump_TrackAttack = false;

		m_isFirst_JumpAtk = false;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}

		m_pModelCom->Set_Animation(ANIM_ATK_AIRTRACK);
		JumpStop(0.3);
		Set_FallingStatus(2.8f, 0.01f);
	}
	Ground_Animation_Play(50, 51);
	Go_Straight_Constant(dTimeDelta, 50, 4.f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 51, 4.f * m_fScaleChange, 0.2f * m_fScaleChange); // Down

	m_pModelCom->Set_EarlyEnd(51, true, 0.6f);
}

void CPlayer_Tanjiro::Animation_Control_Battle_Attack(_double dTimeDelta)
{
	_int iCurAnimIndex = m_pModelCom->Get_iCurrentAnimIndex();

	//m_pModelCom->Set_LinearDuration(ANIM_BATTLE_IDLE, 0.1f);

	// 콤보공격
	if (m_Moveset.m_Down_Battle_Combo)
	{
		m_Moveset.m_Down_Battle_Combo = false;
		m_isComboing = true;

		//m_pModelCom->Set_LinearDuration(ANIM_BATTLE_IDLE, 0.5f);

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}

		//첫 애니메이션 설정
		if (m_pModelCom->Get_Combo_Doing() == false)
		{
			m_pModelCom->Set_Combo_Doing(true);
			m_pModelCom->Set_Animation(ANIM_ATK_COMBO);
		}
		//아닐경우, 다음 콤보로 진행
		else
		{
			m_pModelCom->Set_Combo_Trigger(true);

			if (21 == iCurAnimIndex)
			{
				m_pModelCom->Set_EarlyEnd(21, true, 0.4f);
			}
			if (22 == iCurAnimIndex)
			{
				m_pModelCom->Set_EarlyEnd(22, true, 0.4f);
			}
			if (23 == iCurAnimIndex)
			{
				m_pModelCom->Set_EarlyEnd(23, true, 0.55f);
			}

			if (25 == iCurAnimIndex) // 분기 노말
			{
				m_pModelCom->Set_EarlyEnd(25, true, 0.99f);
			}


			//콤보 분기 설정
			if (23 == iCurAnimIndex)
			{
				// 아래콤보 s콤보
				if (m_Moveset.m_Down_Battle_Combo_Down)
				{
					m_pModelCom->Set_Combo_Another(24);
					m_pModelCom->Set_EarlyEnd(24, true, 0.99f);
				}
				// 위콤보 w콤보
				else if (m_Moveset.m_Down_Battle_Combo_Up)
				{
					m_pModelCom->Set_Combo_Another(26);
					m_pModelCom->Set_EarlyEnd(26, true, 0.99f);

					m_isCan_AirDash = true;
				}

				m_pModelCom->Set_AnimisFinish(25);

			}
		}
	}
	m_pModelCom->Set_EarlyEnd(21, false, 0.4f);
	m_pModelCom->Set_EarlyEnd(22, false, 0.4f);
	m_pModelCom->Set_EarlyEnd(23, false, 0.55f);

	m_pModelCom->Set_EarlyEnd(25, false, 0.99f);

	// 공격 모션별 전진이동 제어 (Timedelta, 애니메이션인덱스,  초기화속도,  감속도)
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO, 3.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.3f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 22, 3.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.16f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 23, 4.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.11f * m_fScaleChange);
	//분기
	Go_Straight_Deceleration(dTimeDelta, 24, 3.2f * m_fScaleChange * m_fAtk_Move_Ratio, 0.05f * m_fScaleChange); // Down
	Go_Straight_Deceleration(dTimeDelta, 25, 5.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.35f * m_fScaleChange); // Normal
	Go_Straight_Deceleration(dTimeDelta, 26, 3.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.29f * m_fScaleChange); // Up

	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_BATTLE_IDLE)
	{
		m_isComboing = false;
	}



	//각성상태 일때 + 노말히트마지막
	if (m_Moveset.m_iAwaken == 1 || m_Moveset.m_iAwaken == 2)
	{
		if (m_pModelCom->Get_iCurrentAnimIndex() == 25)
		{
			m_pModelCom->Set_EarlyEnd(25, true, 0.76f);

			m_dDelay_Surge += dTimeDelta;
			if (m_dDelay_Surge > 0.5f)
			{
				m_isCan_Surge = true;
			}
		}
		else
		{
			m_isCan_Surge = false;
		}
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() != 25)
	{
		m_isCan_Surge = false;
		m_Moveset.m_Down_Battle_Combo_Surge = false;
	}


	if (m_Moveset.m_Down_Battle_Combo_Surge)
	{
		m_Moveset.m_Down_Battle_Combo_Surge = false;
		m_isCan_Surge = false;
		m_dDelay_Surge = 0.0;

		m_pModelCom->Set_Animation(27);
		m_dDelay_Surge_Attack = 0.0;

		m_isReset_Atk_MoveControl = true;
	}
	if (m_pModelCom->Get_iCurrentAnimIndex() == 27)
	{
		m_dDelay_Surge_Attack += dTimeDelta;
		if (m_dDelay_Surge_Attack > 0.6f)
		{
			Go_Straight_Deceleration(dTimeDelta, 27, 3.0f, 0.04f);
		}
	}
	else
		m_dDelay_Surge_Attack = 0.0;


	//서지 히트시
	if (m_isHit_SurgeCutScene)
	{

		CCameraManager::GetInstance()->Set_Is_Cut_In_On(true);
		CCameraManager::GetInstance()->Set_Cut_In_Finish_Type(CCamera_Free::TANJIRO_EXECUTION);


		m_isHit_SurgeCutScene = false;

		m_pModelCom->Set_Animation(28);
		m_isReset_Atk_MoveControl = true;

		_tchar szSound[MAX_PATH] = TEXT("Tanjiro_Surge.mp3");
		Play_Sound_Channel(szSound, CSoundMgr::PLAYER_VOICE, 0.9f);
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == 28)
	{
		m_dDelay_SurgeCutScene += dTimeDelta;
		if (0.5f < m_dDelay_SurgeCutScene)
		{
			Go_Straight_Deceleration(dTimeDelta, 28, 4.5f, 0.1f);
		}

		/*
		if (0.5f < m_dDelay_SurgeCutScene && m_dDelay_SurgeCutScene <= 0.7f)
		{
			m_pTransformCom->Go_Left(dTimeDelta, m_pNavigationCom[m_eCurNavi]);
		}
		else if (0.7f < m_dDelay_SurgeCutScene && m_dDelay_SurgeCutScene <= 1.0f)
		{
			m_pTransformCom->Go_Right(dTimeDelta, m_pNavigationCom[m_eCurNavi]);
		}*/

	}
	else
		m_dDelay_SurgeCutScene = 0.0;
}

void CPlayer_Tanjiro::Animation_Control_Battle_Charge(_double dTimeDelta)
{
	if (m_Moveset.m_Down_Battle_Charge)
	{
		m_Moveset.m_Down_Battle_Charge = false;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}

		m_pModelCom->Set_Animation(ANIM_ATK_CHARGE);
	}


	if (m_Moveset.m_Up_Battle_Charge && m_pModelCom->Get_iCurrentAnimIndex() == 32)
	{
		m_Moveset.m_Up_Battle_Charge = false;


		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}

		m_pModelCom->Set_Animation(33);
	}
	else if (m_Moveset.m_Up_Battle_Charge)
	{
		m_Moveset.m_Up_Battle_Charge = false;

		m_pModelCom->Set_Animation(ANIM_BATTLE_IDLE);
	}
	Go_Straight_Deceleration(dTimeDelta, 33, 3.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.03f * m_fScaleChange);
}

void CPlayer_Tanjiro::Animation_Control_Battle_Skill(_double dTimeDelta)
{
	if (m_Moveset.m_iAwaken < 2)
	{
		if (m_isCan_Mp_Skill == false)
		{
			m_Moveset.m_Down_Skill_Normal = false;
			m_Moveset.m_Down_Skill_Move = false;
			m_Moveset.m_Down_Skill_Guard = false;
		}
	}

	//스킬_0
	if (m_Moveset.m_Down_Skill_Normal)
	{
		m_Moveset.m_Down_Skill_Normal = false;
		m_dDelay_CanSkill = 0.0;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}

		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_NORMAL);
		Jumping(2.6f * m_fScaleChange, 0.18f * m_fScaleChange);

		Use_Mp_Skill();

		_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Skill_Normal.mp3");
		Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, 0.8f);

		_tchar szSoundFile1[MAX_PATH] = TEXT("water_01.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::SKILL_0, 0.7f);

		_tchar szSoundFile2[MAX_PATH] = TEXT("hit_sword_04.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::SKILL_1, 0.4f);
	}

	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_SKILL_NORMAL, 2.0f * m_fScaleChange, 0.07f * m_fScaleChange);



	//스킬_1 : 이동키 + I키
	if (m_Moveset.m_Down_Skill_Move)
	{
		m_Moveset.m_Down_Skill_Move = false;
		m_dDelay_CanSkill = 0.0;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_MOVE);
		Jumping(0.3f * m_fScaleChange, 0.07f * m_fScaleChange);

		m_pModelCom->Get_Animation()->Set_EarlyEnd(true);

		Use_Mp_Skill();

		_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Skill_Move.mp3");
		Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, 0.6f);

		_tchar szSoundFile1[MAX_PATH] = TEXT("water_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::SKILL_0, 0.7f);

		_tchar szSoundFile2[MAX_PATH] = TEXT("hit_sword_L.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::SKILL_1, 0.4f);
	}
	Go_Straight_Constant(dTimeDelta, ANIM_ATK_SKILL_MOVE, 1.5f * m_fScaleChange * m_fAtk_Move_Ratio);
	Go_Straight_Constant(dTimeDelta, 40, 1.5f * m_fScaleChange * m_fAtk_Move_Ratio);
	Go_Straight_Deceleration(dTimeDelta, 41, 1.5f * m_fScaleChange * m_fAtk_Move_Ratio, 0.07f * m_fScaleChange);


	//스킬_2 : 가드키 + I키
	if (m_Moveset.m_Down_Skill_Guard)
	{
		m_Moveset.m_Down_Skill_Guard = false;
		m_dDelay_CanSkill = 0.0;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}

		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_GUARD);
		//Jumping(1.25f * m_fScaleChange, 0.02f * m_fScaleChange);
		Jumping(2.0f * m_fScaleChange, 0.04f * m_fScaleChange);

		Use_Mp_Skill();

		_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Skill_Up.mp3");
		Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, 0.8f);

		_tchar szSoundFile1[MAX_PATH] = TEXT("water_03.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::SKILL_0, 0.7f);

		_tchar szSoundFile2[MAX_PATH] = TEXT("hit_sword_06.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::SKILL_1, 0.4f);
	}
	Ground_Animation_Play(36, 37);
	//Ground_Animation_Play(37, 86);
	//Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_SKILL_GUARD, 5.f * m_fScaleChange, 0.25f * m_fScaleChange);
}

void CPlayer_Tanjiro::Animation_Control_Battle_Guard(_double dTimeDelta)
{
	//가드 시작
	if (m_Moveset.m_Down_Battle_Guard)
	{
		m_Moveset.m_Down_Battle_Guard = false;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		//m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		//m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
		m_pModelCom->Set_Animation(ANIM_BATTLE_GUARD);
	}

	//가드 도중


	//가드 내리기
	if (m_Moveset.m_Up_Battle_Guard)
	{
		m_Moveset.m_Up_Battle_Guard = false;

		m_pModelCom->Set_Animation(65);
	}


	//잡기 ( O키 가드키 + J키 공격키)
	if (m_Moveset.m_Down_Battle_Throw)
	{
		m_Moveset.m_Down_Battle_Throw = false;

		m_isThrowing = true;
		m_pModelCom->Set_Animation(ANIM_ATK_THROW);
	}
	if (m_isThrowing && m_pModelCom->Get_iCurrentAnimIndex() == ANIM_BATTLE_IDLE && m_Moveset.m_State_Battle_Guard)
	{
		m_isMaintain_Guard = true;
	}

	//잡기 ( O키 가드키 + 이동키)
	if (m_Moveset.m_Down_Battle_Push)
	{
		m_Moveset.m_Down_Battle_Push = false;

		m_isThrowing = true;
		m_pModelCom->Set_Animation(ANIM_BATTLE_GUARD_PUSH);
	}
	if (m_isThrowing && m_pModelCom->Get_iCurrentAnimIndex() == ANIM_BATTLE_IDLE && m_Moveset.m_State_Battle_Guard)
	{
		m_isMaintain_Guard = true;
	}

}

void CPlayer_Tanjiro::Animation_Control_Battle_Dash(_double dTimeDelta)
{
	if (m_Moveset.m_Down_Battle_Dash)
	{
		m_Moveset.m_Down_Battle_Dash = false;

		m_dDelay_Dash = 0.0;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		_float4 PlayerPos;
		XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (m_LockOnPos.y > PlayerPos.y + 0.1f && m_isCan_AirDash)
		{
			m_isCan_AirDash = false;
			m_dDelay_Can_AirDash = 0.0;
			m_isAirDashing = true;
			m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP); // 공중콤보용 대시
			Jumping(1.0f, 0.055f);
		}
		else
			m_pModelCom->Set_Animation(ANIM_BATTLE_DASH);

		_tchar szRun_0[MAX_PATH] = TEXT("awk_eff_wind.ogg");
		Play_Sound_Channel(szRun_0, CSoundMgr::PLAYER_JUMP, 0.4f);
	}
	Go_Straight_Constant(dTimeDelta, 80, 4.5f * m_fScaleChange);
	if (m_pModelCom->Get_iCurrentAnimIndex() == 80)
	{
		Play_Sound_Move(dTimeDelta, 0.1f); // 0.1
	}


	if (m_isAirDashing)
	{
		Get_LockOn_MonPos();
		if (Get_Distance_To_LockOnPos() > 0.5f)
		{
			m_pTransformCom->Go_Dir(dTimeDelta * 4.0f, Get_Dir_To_LockOnPos());
		}
		else
		{
			m_isAirDashing = false;
		}
	}
	Ground_Animation_Play(85, 86);


	//공중콤보영 대시

	if (m_pModelCom->Get_iCurrentAnimIndex() == 80)
	{
		if (Get_Distance_To_LockOnPos() < 1.0f)
		{
			m_pModelCom->Set_Animation(81);
		}

		m_dDelay_Dash += dTimeDelta;
		if (m_dDelay_Dash > 2.0f)
		{
			m_pModelCom->Set_Animation(81);
		}
	}
	Go_Straight_Deceleration(dTimeDelta, 81, 3.0f * m_fScaleChange, 0.03f);



	if (m_Moveset.m_Down_Battle_Step)
	{
		m_Moveset.m_Down_Battle_Step = false;

		m_pTransformCom->Set_Look(m_vLook);
		//m_pTransformCom->LerpVector(XMLoadFloat4(&m_vLook), 0.8f);
		if (m_isForward)
			m_pModelCom->Set_Animation(ANIM_BATTLE_STEP_F);
		else if (m_isBack)
			m_pModelCom->Set_Animation(ANIM_BATTLE_STEP_B);
		else
		{
			if (m_isLeft)
			{
				//콤보 첫 애니메이션 설정
				if (m_pModelCom->Get_Combo_Doing() == false)
				{
					m_pModelCom->Set_Combo_Doing(true);
					m_pModelCom->Set_Animation(ANIM_BATTLE_STEP_L);

					m_pModelCom->Set_EarlyEnd(ANIM_BATTLE_STEP_L, false, 0.35f);
				}
				//아닐경우, 다음 콤보로 진행
				else
				{
					m_pModelCom->Set_Combo_Trigger(true);

					m_pModelCom->Set_EarlyEnd(ANIM_BATTLE_STEP_L, true, 0.35f);
					//Jumping(3.0f * m_fScaleChange, 0.25f * m_fScaleChange);

					m_dDelay_DoubleStep = 0.0;
				}
			}
			else if (m_isRight)
			{
				//콤보 첫 애니메이션 설정
				if (m_pModelCom->Get_Combo_Doing() == false)
				{
					m_pModelCom->Set_Combo_Doing(true);
					m_pModelCom->Set_Animation(ANIM_BATTLE_STEP_R);

					m_pModelCom->Set_EarlyEnd(ANIM_BATTLE_STEP_R, false, 0.35f);
				}
				//아닐경우, 다음 콤보로 진행
				else
				{
					m_pModelCom->Set_Combo_Trigger(true);

					m_pModelCom->Set_EarlyEnd(ANIM_BATTLE_STEP_R, true, 0.35f);
					//Jumping(1.0f * m_fScaleChange, 0.06f * m_fScaleChange);

					m_dDelay_DoubleStep = 0.0;
				}
			}
		}


	}


	_vector vDir = XMLoadFloat4(&m_Moveset.m_Input_Dir);
	_float4 fDir;
	XMStoreFloat4(&fDir, -vDir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_F, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_B, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_L, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_R, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);

	//더블스텝
	Go_Dir_Deceleration(dTimeDelta, 98, 5.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, 100, 5.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);

	if (m_pModelCom->Get_iCurrentAnimIndex() == 98 || m_pModelCom->Get_iCurrentAnimIndex() == 100)
	{
		m_dDelay_DoubleStep += dTimeDelta;
	}

}

void CPlayer_Tanjiro::Animation_Control_Battle_Awaken(_double dTimeDelta)
{
	if (m_Moveset.m_Down_Battle_Awaken)
	{
		m_Moveset.m_Down_Battle_Awaken = false;

		if (m_Moveset.m_iAwaken == 1)
		{
			m_pModelCom->Set_Animation(ANIM_BATTLE_AWAKEN);

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Awkaen_0.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, 0.9f);

			_vector vPlayerDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
			Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(5.5f, 5.5f, 5.5f), _float3(0.f, 0.0f, 0.0f), 0.7,
				CAtkCollider::TYPE_BIG, vPlayerDir, 0.0f);
		}
		else if (m_Moveset.m_iAwaken == 2)
		{
			m_pModelCom->Set_Animation(ANIM_BATTLE_AWAKEN_COMPLETE_CUTSCENE);

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Awaken_Breath.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, 0.6f);
			m_dSound_Awaken_Complete = 0.0;
			m_isSound_Awaken = true;
		}
	}
	if (m_isSound_Awaken)
	{
		m_dSound_Awaken_Complete += dTimeDelta;
		if (m_dSound_Awaken_Complete > 0.2f)
		{
			m_dSound_Awaken_Complete = 0.0;
			m_isSound_Awaken = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Awaken_Complete.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, 0.9f);
		}
	}
}

void CPlayer_Tanjiro::Animation_Control_Battle_Special(_double dTimeDelta)
{
	if (m_Moveset.m_Down_Battle_Special)
	{
		m_Moveset.m_Down_Battle_Special = false;

		m_isFirst_SpecialReady = true;

		m_pModelCom->Set_Animation(ANIM_ATK_SPECIAL_READY);
		m_dTime_Special_Ready = 0.0;
	}
	m_dTime_Special_Ready += dTimeDelta;
	if (m_dTime_Special_Ready > 1.5f)
		Go_Straight_Deceleration(dTimeDelta, 109, 4.0f * m_fScaleChange, 0.23f * m_fScaleChange);

	//Go_Straight_Constant(dTimeDelta, 108, 2.7f);
	if (m_pModelCom->Get_iCurrentAnimIndex() == 108 || m_pModelCom->Get_iCurrentAnimIndex() == 109)
	{
		if (m_isFirst_SpecialReady)
		{
			m_isFirst_SpecialReady = false;
			if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
			{
				if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
					m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
			}
		}


		if (m_isSpecialHit)
		{
			m_isSpecialHit = false;
			m_isFirst_Special_Jump = true;

			m_pModelCom->Set_Animation(ANIM_ATK_SPECIAL_CUTSCENE);
			m_dTime_Special_CutScene = 0.0;
		}
	}
	m_dTime_Special_CutScene += dTimeDelta;

	Go_Straight_Constant(dTimeDelta, ANIM_ATK_SPECIAL_CUTSCENE, 1.0f * m_fScaleChange);
	if (0.90f <= m_dTime_Special_CutScene)
	{
		Go_Left_Deceleration(dTimeDelta, ANIM_ATK_SPECIAL_CUTSCENE, 10.0f * m_fScaleChange, 0.1f * m_fScaleChange);
	}
	else if (0.65f <= m_dTime_Special_CutScene)
	{
		Go_Right_Deceleration(dTimeDelta, ANIM_ATK_SPECIAL_CUTSCENE, 10.0f * m_fScaleChange, 0.1f * m_fScaleChange);
	}
	else if (0.35f <= m_dTime_Special_CutScene)
	{
		Go_Left_Deceleration(dTimeDelta, ANIM_ATK_SPECIAL_CUTSCENE, 10.0f * m_fScaleChange, 0.1f * m_fScaleChange);
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == 103 && m_isFirst_Special_Jump)
	{
		m_isFirst_Special_Jump = false;
		Jumping(2.65f * m_fScaleChange, 0.025f * m_fScaleChange);
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == 106 && m_isSecond_Special_Jump)
	{
		m_isSecond_Special_Jump = false;
		Jumping(1.0f * m_fScaleChange, 0.08f * m_fScaleChange);
	}
	Go_Straight_Deceleration(-dTimeDelta, 106, 1.0f * m_fScaleChange, 0.01f * m_fScaleChange);
}

void CPlayer_Tanjiro::Animation_Control_Battle_Dmg(_double dTimeDelta)
{
	_float4 AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
	_vector vAtkDir = XMLoadFloat4(&AtkDir);
	_float4 reverseAtkDir;
	XMStoreFloat4(&reverseAtkDir, -vAtkDir);


#pragma region GuardHit
	if (m_isGuardHit)
	{
		m_isGuardHit = false;

		m_pTransformCom->Set_Look(reverseAtkDir);

		if (m_iGuardHit_Index == 0)
		{
			m_iGuardHit_Index++;
			m_pModelCom->Set_Animation(ANIM_BATTLE_GUARD_HIT_SMALL);
		}
		else if (m_iGuardHit_Index == 1)
		{
			m_iGuardHit_Index = 0;
			m_pModelCom->Set_Animation(ANIM_BATTLE_GUARD_HIT_BIG);
		}

		CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
		EffectWorldDesc.fScale = 1.4f;
		EffectWorldDesc.vPosition.y += 0.4f;
		CEffectPlayer::Get_Instance()->Play("Effect_Guard", m_pTransformCom, &EffectWorldDesc);
		CEffectPlayer::Get_Instance()->Play("Effect_Guard", m_pTransformCom, &EffectWorldDesc);

		Play_Sound_Metal(0.5);
	}
#pragma endregion


#pragma region Dmg_Small
	if (m_Moveset.m_Down_Dmg_Small || m_Moveset.m_Down_Dmg_ConnectSmall)
	{
		if (m_Moveset.m_Down_Dmg_Small)
		{
			m_Moveset.m_Down_Dmg_Small = false;
			m_isConnectHitting = false;
		}
		else if (m_Moveset.m_Down_Dmg_ConnectSmall)
		{
			m_Moveset.m_Down_Dmg_ConnectSmall = false;
			m_isConnectHitting = true;
		}

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isSkilling == false)
		{
			//m_pTransformCom->Set_Look(reverseAtkDir);
			m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
			if (m_isJumpOn)
			{
				Jumping(0.12f, 0.030f);

				if (m_iSmallHit_Index == 0)
				{
					m_pModelCom->Set_Animation(ANIM_FALL);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 1)
				{
					m_pModelCom->Set_Animation(ANIM_DMG_AIR_SMALL_CONNECT_0);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 2)
				{
					m_pModelCom->Set_Animation(ANIM_DMG_AIR_SMALL_CONNECT_1);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 3)
				{
					m_pModelCom->Set_Animation(ANIM_DMG_AIR_SMALL_CONNECT_2);
					m_iSmallHit_Index = 0;
				}
			}
			else
			{
				if (m_iSmallHit_Index == 0)
				{
					m_pModelCom->Set_Animation(139);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 1)
				{
					m_pModelCom->Set_Animation(140);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 2)
				{
					m_pModelCom->Set_Animation(141);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 3)
				{
					m_pModelCom->Set_Animation(142);
					m_iSmallHit_Index = 0;
				}
			}
		}
		Play_Sound_Dmg(0, 0.8f);
	}
	if (m_isConnectHitting == false)
	{
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL, 1.5f * m_fDmg_Move_Ratio, 0.01f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 139, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 140, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 141, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 142, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
	}
#pragma endregion



#pragma region Dmg_Big
	if (m_Moveset.m_Down_Dmg_Big)
	{
		m_Moveset.m_Down_Dmg_Big = false;

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isSkilling == false)
		{
			m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
			if (m_isJumpOn)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_BLOW);
			}
			else
			{
				m_pModelCom->Set_Animation(ANIM_DMG_BIG);
			}
		}
		Play_Sound_Dmg(1, 0.8f);
	}
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_BIG, 2.0f, 0.035f, AtkDir);

#pragma endregion


#pragma region Dmg_Blow
	if (m_Moveset.m_Down_Dmg_Blow)
	{
		m_Moveset.m_Down_Dmg_Blow = false;

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isSkilling == false)
		{
			m_pModelCom->Set_Animation(ANIM_DMG_BLOW);
			//m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
			m_pTransformCom->Set_Look(reverseAtkDir);
			Jumping(1.2f, 0.05f);
		}
		Play_Sound_Dmg(2, 0.8f);
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_BLOW, 2.5f * m_fDmg_Move_Ratio, AtkDir);
	Go_Dir_Constant(dTimeDelta, 120, 2.5f * m_fDmg_Move_Ratio, AtkDir);
	Ground_Animation_Play(120, 121);

#pragma endregion


#pragma region Dmg_BigBlow
	if (m_Moveset.m_Down_Dmg_BigBlow)
	{
		m_Moveset.m_Down_Dmg_BigBlow = false;

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isSkilling == false)
		{
			m_pModelCom->Set_Animation(ANIM_DMG_SPIN);
			m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);

			Jumping(1.2f, 0.05f);
		}
		Play_Sound_Dmg(2, 0.8f);
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_SPIN, 3.0f * m_fDmg_Move_Ratio, AtkDir);
	Go_Dir_Constant(dTimeDelta, 132, 3.0f * m_fDmg_Move_Ratio, AtkDir);
	Go_Dir_Deceleration(dTimeDelta, 133, 3.0f * m_fDmg_Move_Ratio, 0.1f, AtkDir);
	Ground_Animation_Play(132, 133);

#pragma endregion


#pragma region Dmg_Upper
	if (m_Moveset.m_Down_Dmg_Upper)
	{
		m_Moveset.m_Down_Dmg_Upper = false;

		m_isConnectHitting = false;

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isSkilling == false)
		{
			m_pModelCom->Set_Animation(ANIM_FALL);
			//m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
			m_pTransformCom->Set_Look(reverseAtkDir);

			Jumping(1.7f, 0.03f);
		}
		Play_Sound_Dmg(1, 0.8f);
	}

	if (m_isConnectHitting == false)
	{
		if (m_isSwampUpper == false)
		{
			Go_Dir_Constant(dTimeDelta, ANIM_FALL, 0.2f * m_fDmg_Move_Ratio, AtkDir);
			Go_Dir_Constant(dTimeDelta, 125, 0.2f * m_fDmg_Move_Ratio, AtkDir);
		}
		else // 스왐프 어퍼 히트할때,
		{
			Go_Dir_Constant(dTimeDelta, ANIM_FALL, 0.45f * m_fDmg_Move_Ratio, AtkDir);
			Go_Dir_Constant(dTimeDelta, 125, 0.45f * m_fDmg_Move_Ratio, AtkDir);
		}
	}
	Ground_Animation_Play(125, 126);



	if (m_Moveset.m_Down_Dmg_SwampUpper)
	{
		m_Moveset.m_Down_Dmg_SwampUpper = false;
		m_isConnectHitting = false;

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isSkilling == false)
		{
			m_pModelCom->Set_Animation(ANIM_FALL);
			//m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
			m_pTransformCom->Set_Look(reverseAtkDir);

			Jumping(2.0f, 0.075f);
			m_isSwampUpper = true;
		}

		Play_Sound_Dmg(1, 0.8f);
	}

#pragma endregion


#pragma region Dmg_SwampBind

	m_dDelay_SwampHit_Again += dTimeDelta;
	if (m_Moveset.m_Down_Dmg_Swamp)
	{
		m_Moveset.m_Down_Dmg_Swamp = false;

		if (m_dDelay_SwampHit_Again < 2.0)
		{
			// 그냥 건너뜀
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_DMG_SWAMPBIND);

			m_isSwampHit = true;
			m_fLand_Y = m_pNavigationCom[m_eCurNavi]->Compute_Height(m_pTransformCom) - 1.5f;
			Jumping(0.01f, 0.025f);
			m_dSwampHit = 0.0;
			m_isFirst_SwampHit = true;
		}
		Play_Sound_Dmg(1, 0.8f);

		_tchar szSoundFile[MAX_PATH] = TEXT("st_swamp01.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::SKILL_EFFECT, 0.8f);
	}

	if (m_isSwampHit)
	{
		m_dSwampHit += dTimeDelta;
		if (m_dSwampHit > 0.4f)
		{
			if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_DMG_SWAMPBIND)
			{
				//m_isSwamp_Escape = true;
				//m_pModelCom->Set_Animation(ANIM_BATTLE_IDLE);
				if (m_isFirst_SwampHit)
				{
					m_isFirst_SwampHit = false;
					Jumping(0.01f, 0.01f);
				}
			}

			_float4 Pos;
			XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			if (Pos.y <= m_fLand_Y + 0.1f)
			{
				if (m_isFirst_SwampUi)
				{
					m_isFirst_SwampUi = false;

					CBattle_UI_Manager::GetInstance()->Set_Timing_On(true);
				}
			}


			if (m_pNavigationCom[m_eCurNavi]->Compute_Height(m_pTransformCom) > Pos.y)
			{
				//m_fLand_Y += 0.01f;
				_int ak = 47;
			}
			else
			{
				m_dSwampHit = 0.0;
				m_isSwampHit = false;
				//m_isSwamp_Escape = false;
				m_isFirst_SwampUi = true;
				m_isFirst_SwampHit = true;
			}
		}

		Create_SwampWaterParticleEffect(dTimeDelta);

	}
	else
	{
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (Pos.y <= m_fLand_Y + 0.1f)
		{
			m_isSwamp_Escape = false;
		}
	}
#pragma endregion


#pragma region Dmg_Web
	if (m_Moveset.m_Down_Dmg_Web)
	{
		m_Moveset.m_Down_Dmg_Web = false;

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		m_isConnectHitting = true;

		if (m_iSmallHit_Index == 0)
		{
			m_pModelCom->Set_Animation(139);
			m_iSmallHit_Index++;
		}
		else if (m_iSmallHit_Index == 1)
		{
			m_pModelCom->Set_Animation(140);
			m_iSmallHit_Index++;
		}
		else if (m_iSmallHit_Index == 2)
		{
			m_pModelCom->Set_Animation(141);
			m_iSmallHit_Index++;
		}
		else if (m_iSmallHit_Index == 3)
		{
			m_pModelCom->Set_Animation(142);
			m_iSmallHit_Index = 0;
		}

		Play_Sound_Dmg(1, 0.8);
	}
#pragma endregion


	if (m_Moveset.m_Down_GetUp)
	{
		m_Moveset.m_Down_GetUp = false;
		m_Moveset.m_Down_Dmg_Blow = false;

		m_pModelCom->Set_Animation(ANIM_DOWN_GETUP);
	}


	if (m_Moveset.m_Down_GetUp_Move)
	{
		m_Moveset.m_Down_GetUp_Move = false;
		m_Moveset.m_Down_Dmg_Blow = false;

		m_pModelCom->Set_Animation(ANIM_DOWN_GETUP_MOVE);
		//m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
	}
	//Go_Straight_Constant(dTimeDelta, 138, 2.0f);
	Go_Straight_Deceleration(dTimeDelta, 138, 3.0f * m_fDmg_Move_Ratio, 0.03f);
	m_pModelCom->Set_EarlyEnd(ANIM_DOWN_GETUP_MOVE, true);


	//Go_Straight_Deceleration(dTimeDelta, 138, 1.0f, 0.01f);

}

void CPlayer_Tanjiro::Animation_Control_Adventure_Move(_double dTimeDelta)
{
	if (m_Moveset.m_isRestrict_Adventure == false)
	{
		//무빙제한시 리턴
		if (m_Moveset.m_isRestrict_Move)
		{
			return;
		}
		//무빙제한이 풀릴시 이동 설정, 버그수정
		if (m_Moveset.m_isPressing_While_Restrict)
		{
			m_Moveset.m_isPressing_While_Restrict = false;
			m_Moveset.m_Down_Battle_Run = true;
		}

		m_pModelCom->Set_LinearDuration(ANIM_ADV_STEALTH_IDLE, 0.1f);
		m_pModelCom->Set_LinearDuration(ANIM_ADV_STEALTH_WALK, 0.1f);
		m_pModelCom->Set_LinearDuration(145, 0.0001f);
		m_pModelCom->Set_LinearDuration(146, 0.15f);

		m_pModelCom->Set_EarlyEnd(ANIM_ADV_STEALTH_IDLE, false, 0.9999f);
		m_pModelCom->Set_EarlyEnd(ANIM_ADV_STEALTH_WALK, false, 0.9999f);
		m_pModelCom->Set_EarlyEnd(145, false, 0.9999f);
		m_pModelCom->Set_EarlyEnd(146, false, 0.9999f);

		//무빙키입력들
		if (m_Moveset.m_Down_Battle_Run)
		{
			m_Moveset.m_Down_Battle_Run = false;

			if (m_isStealthMode)
				m_pModelCom->Set_Animation(ANIM_ADV_STEALTH_WALK);//144 145 146
			else
				m_pModelCom->Set_Animation(ANIM_ADV_RUN);
		}

		if (m_Moveset.m_State_Battle_Run)
		{
			//m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
			m_fMove_Speed = 2.0f;

			m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.17f);


			if (m_isCanNavi)
			{
				if (m_isStealthMode)
				{
					//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange * 0.35f, m_pNavigationCom[m_eCurNavi]);

					Go_Straight_Constant(dTimeDelta, ANIM_ADV_STEALTH_WALK, m_fMove_Speed * m_fScaleChange * 0.35f);
					Go_Straight_Constant(dTimeDelta, 145, m_fMove_Speed * m_fScaleChange * 0.35f);
				}
				else
				{
					//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange * 0.7f, m_pNavigationCom[m_eCurNavi]);

					Go_Straight_Constant(dTimeDelta, ANIM_ADV_RUN, m_fMove_Speed * m_fScaleChange * 0.7f);
					//Go_Straight_Constant(dTimeDelta, 10, m_fMove_Speed * m_fScaleChange * 0.7f);
				}
			}
			else
			{
				if (m_isStealthMode)
				{
					//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange * 0.35f);

					Go_Straight_Constant(dTimeDelta, ANIM_ADV_STEALTH_WALK, m_fMove_Speed * m_fScaleChange * 0.35f, true);
					Go_Straight_Constant(dTimeDelta, 145, m_fMove_Speed * m_fScaleChange * 0.35f, true);
				}
				else
				{
					//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange * 0.7f);

					Go_Straight_Constant(dTimeDelta, ANIM_ADV_RUN, m_fMove_Speed * m_fScaleChange * 0.7f, true);
				}
			}
			//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed);

			/*
			if (m_pTransformCom->LerpVector_Get_End(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.3f))
			{
				if (m_isCanNavi)
					m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange * 0.7f, m_pNavigationCom[m_eCurNavi]);
				else
					m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange * 0.7f);
				//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed);
			}
			*/
		}

		if (m_Moveset.m_Up_Battle_Run)
		{
			m_Moveset.m_Up_Battle_Run = false;

			if (m_isStealthMode)
				m_pModelCom->Set_Animation(146);
			else
				m_pModelCom->Set_Animation(ANIM_ADV_RUN_END);
		}
		Go_Straight_Deceleration(dTimeDelta, ANIM_ADV_RUN_END, m_fMove_Speed * m_fScaleChange * 0.7f, 0.18f);
		Go_Straight_Deceleration(dTimeDelta, 146, m_fMove_Speed * m_fScaleChange * 0.35f, 0.18f);


		//잠입모드 공격시
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		if (pGameInstance->Get_DIKeyDown(DIK_J))
		{
			m_pModelCom->Set_Animation(ANIM_ATK_COMBO);
			m_pSword->Set_SwordIn(false);
			CMonsterManager::GetInstance()->Set_StealthAttack(true);

			_float4 Dir;
			XMStoreFloat4(&Dir, XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)));
			CMonsterManager::GetInstance()->Set_DirStealthAtk(Dir);
		}

		if (CMonsterManager::GetInstance()->Get_StealthEnd_BattleStart())
		{
			CMonsterManager::GetInstance()->Set_StealthEnd_BattleStart(false);

			m_bChangePositionTrigger[CHANGE_POSITON_HOUSE_1A] = true;
			m_dChangePositionAccTime = 0.0;

			CFadeManager::GetInstance()->Set_Fade_Color(true);
			CFadeManager::GetInstance()->Set_Fade_OutIn(true, 1.f);
			CFadeManager::GetInstance()->Set_Is_House_Monster_Encounter(true);
		}



		if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_ADV_RUN)
			Play_Sound_Move(dTimeDelta, 0.38f);
		else if(m_pModelCom->Get_iCurrentAnimIndex() == ANIM_ADV_STEALTH_WALK || m_pModelCom->Get_iCurrentAnimIndex() == 145)
			Play_Sound_Move(dTimeDelta, 0.5f);

		Safe_Release(pGameInstance);
	}


	//잠입모드 발각시
	if (CMonsterManager::GetInstance()->Get_PlayerBack())
	{
		CMonsterManager::GetInstance()->Set_PlayerBack(false);

		m_isPlayerBack_Tanjiro = true;
		m_dDelay_PlayerBack_Tanjiro = 0.0;

		m_Moveset.m_isRestrict_Adventure = true;
		m_pModelCom->Set_Animation(ANIM_DMG_BIG);
	}
	_float4 AtkDir = CMonsterManager::GetInstance()->Get_DirStealthAtk();
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_BIG, 1.3f, 0.03f, AtkDir);


	if (m_isPlayerBack_Tanjiro)
	{
		m_dDelay_PlayerBack_Tanjiro += dTimeDelta;
		if (m_dDelay_PlayerBack_Tanjiro > 2.5)
		{
			m_isPlayerBack_Tanjiro = false;
			m_dDelay_PlayerBack_Tanjiro = 0.0;

			m_iResetIndex = CMonsterManager::GetInstance()->Get_ResetIndex_Player();
			m_Moveset.m_isRestrict_Adventure = false;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_ResetPos[m_iResetIndex]));

			m_pNavigationCom[m_eCurNavi]->Set_CurIndex(0);

			if (m_iResetIndex == 0)
			{
				_float4 PlayerDir = { 0.0f, 0.0f , 1.0f, 0.0f };
				XMStoreFloat4(&PlayerDir, XMVector4Normalize(_vector{ 1.0f, 0.0f, 0.0f, 0.0f }));
				m_pTransformCom->Set_Look(PlayerDir);
				m_pModelCom->Set_Animation(ANIM_ADV_STEALTH_IDLE); // 
			}
			else if (m_iResetIndex == 1)
			{
				_float4 PlayerDir = { 0.0f, 0.0f , 1.0f, 0.0f };
				XMStoreFloat4(&PlayerDir, XMVector4Normalize(_vector{ 0.0f, 0.0f, 1.0f, 0.0f }));
				m_pTransformCom->Set_Look(PlayerDir);
				m_pModelCom->Set_Animation(ANIM_ADV_STEALTH_IDLE); // 
			}
			else if (m_iResetIndex == 2)
			{
				_float4 PlayerDir = { 0.0f, 0.0f , 1.0f, 0.0f };
				XMStoreFloat4(&PlayerDir, XMVector4Normalize(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }));
				m_pTransformCom->Set_Look(PlayerDir);
				m_pModelCom->Set_Animation(ANIM_ADV_STEALTH_IDLE); // 
			}
			else if (m_iResetIndex == 3)
			{
				_float4 PlayerDir = { 0.0f, 0.0f , 1.0f, 0.0f };
				XMStoreFloat4(&PlayerDir, XMVector4Normalize(_vector{ 0.3f, 0.0f, -1.0f, 0.0f }));
				m_pTransformCom->Set_Look(PlayerDir);
				m_pModelCom->Set_Animation(ANIM_ADV_STEALTH_IDLE); // 
			}

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			pGameInstance->Time_Slow(0.3, 0.4);
			Safe_Release(pGameInstance);
		}
	}
}

void CPlayer_Tanjiro::Animation_Control_Adventure_Act(_double dTimeDelta)
{
	_float4 m_ReverseDir;
	XMStoreFloat4(&m_ReverseDir, -XMLoadFloat4(&m_Dir_ScondJump_Box));

	if (m_Moveset.m_Down_ADV_Jump_To_Object)
	{
		m_Moveset.m_Down_ADV_Jump_To_Object = false;

		m_pModelCom->Set_Animation(ANIM_ADV_JUMP);

		m_pModelCom->Set_LinearDuration(2, 0.001f);
		m_pModelCom->Set_LinearDuration(3, 0.001f);
		m_pModelCom->Set_LinearDuration(4, 0.001f);


		//m_isBoxJumping = true;
		//떨어지는
		if (m_isPlayerStatus_OnRoof)
			Jumping(1.0f, 0.077f);			// 처음 점프 // 파워 , 감속도
		//올라가는
		else
			Jumping(1.55f, 0.067f);
		m_isFirst_Jump2_To_Box = true;
		m_dDelay_BoxJump = 0.0;

		if (m_isPlayerStatus_OnRoof == true)
		{
			m_isLand_Roof = false;
		}
		m_vTanjrioToBoxDir = m_vPlayerToBoxDir;

		if (m_isPlayerStatus_OnRoof == false)
		{
			//m_pTransformCom->Set_Look(m_vPlayerToBoxDir);
			m_pTransformCom->LerpVector(XMLoadFloat4(&m_vPlayerToBoxDir), 0.8f);
		}
		else
		{
			// 지붕에 올라가있을때, dl
			//m_pTransformCom->Set_Look(m_ReverseDir);
			m_pTransformCom->LerpVector(XMLoadFloat4(&m_ReverseDir), 0.8f);

			m_eCurNavi = m_eNextNavi;
		}

		_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Small_Hue.mp3");
		Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, 0.85f);

	}

	if (0.1f < m_fDistanceTo_Box)
	{
		//올라갈때
		if (m_isPlayerStatus_OnRoof == false)
		{
			Go_Straight_Constant(dTimeDelta, ANIM_ADV_JUMP, 0.6f, true);
			Go_Straight_Constant(dTimeDelta, 2, 0.6f, true);
		}
		//내려갈때 
		else
		{
			Go_Straight_Constant(dTimeDelta, ANIM_ADV_JUMP, 0.4f);
			Go_Straight_Constant(dTimeDelta, 2, 0.4f);
		}
	}
	/*enum NAVI_TYPE { NAVI_VILLAGE_MAINROAD1,NAVI_VILLAGE_MAINROAD2, NAVI_VILLAGE_INSIDEWALL1,
	NAVI_VILLAGE_INSIDEWALL2 , NAVI_VILLAGE_ROOF , NAVI_VILLAGE_WALL , NAVI_VILLAGE_BATTLE,
	NAVI_HOUSE_0_0,NAVI_HOUSE_1_0,NAVI_HOUSE_1_1,NAVI_HOUSE_2_0,NAVI_HOUSE_3_0,NAVI_HOUSE_4_0,
	NAVI_TRAIN, NAVI_ACAZA, NAVI_END };
	*/
	if (m_isPlayerStatus_OnRoof == false)
	{
		if (NAVI_VILLAGE_WALL == m_eNextNavi)
		{
			Go_Dir_Constant(dTimeDelta, 3, 0.3f, m_Dir_ScondJump_Box);
			Go_Dir_Constant(dTimeDelta, 85, 0.3f, m_Dir_ScondJump_Box);
		}
		else
		{
			Go_Dir_Constant(dTimeDelta, 3, 0.65f, m_Dir_ScondJump_Box, true);
			Go_Dir_Constant(dTimeDelta, 85, 0.65f, m_Dir_ScondJump_Box, true);
		}
	}
	else
	{
		Go_Dir_Constant(dTimeDelta, 3, 0.65f, m_ReverseDir);
		Go_Dir_Constant(dTimeDelta, 85, 0.65f, m_ReverseDir);
	}


	if (m_pModelCom->Get_iCurrentAnimIndex() == 3 && m_isPlayerStatus_OnRoof == false)
	{
		if (m_isFirst_Jump2_To_Box)
		{
			m_isFirst_Jump2_To_Box = false;

			//m_pTransformCom->Set_Look(m_Dir_ScondJump_Box);
			m_pTransformCom->LerpVector(XMLoadFloat4(&m_Dir_ScondJump_Box), 0.8f);
			//떨어지는
			if (m_isPlayerStatus_OnRoof)
				Jumping(1.1f, 0.077f);		// 두번째 올라갈때 점프(땅)
			//올라가는
			else
				Jumping(2.15f, 0.087f);

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Small_Hue.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, 0.85f);
		}

		m_dDelay_BoxJump += dTimeDelta;
		if (m_dDelay_BoxJump > 0.35f)
		{
			if (m_isCan_Jump_RoofOn)
			{
				m_isLand_Roof = true;
			}
			else
			{
				m_isLand_Roof = false;

				//지붕 X  점프가 가장 고점일때임.
				m_eCurNavi = m_eNextNavi;
			}
		}

		
	}
	else if (m_pModelCom->Get_iCurrentAnimIndex() == 3 && m_isPlayerStatus_OnRoof == true)
	{
		if (m_isFirst_Jump2_To_Box)
		{
			m_isFirst_Jump2_To_Box = false;
			//떨어지는
			if (m_isPlayerStatus_OnRoof)
				Jumping(1.1f, 0.087f); // 지붕에서 내려갈때 두번째 점프
			//올라가는
			else
				Jumping(2.45f, 0.087f);

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Small_Hue.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, 0.85f);
		}

	}

	Ground_Animation_Play(85, 4);
	if (m_pModelCom->Get_iCurrentAnimIndex() == 4)
	{
		if (m_isLand_Roof)
			m_isPlayerStatus_OnRoof = true;
		else
			m_isPlayerStatus_OnRoof = false;
	}

}

void CPlayer_Tanjiro::Player_Change(_double dTimeDelta)
{
	if (CPlayerManager::GetInstance()->Get_First_Player_Change())
	{
		CPlayerManager::GetInstance()->Set_First_Player_Change(false);

		m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP);
		m_isJump_Move = false;
		m_dDelay_Player_Change = 0.0;
		CPlayerManager::GetInstance()->Set_First_Setting_Status(true);

		CPlayerManager::GetInstance()->Set_Hp(m_StatusDesc.fHp);
		CPlayerManager::GetInstance()->Set_Mp(m_StatusDesc.fMp);
		CPlayerManager::GetInstance()->Set_Special_Cnt(m_StatusDesc.iSpecial_Cnt);
		CPlayerManager::GetInstance()->Set_Special(m_StatusDesc.fSpecial);
		CPlayerManager::GetInstance()->Set_Support(m_StatusDesc.fSupport);

		CPlayerManager::GetInstance()->Set_Swaping_Pos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	}

	m_dDelay_Player_Change += dTimeDelta;

	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_BATTLE_JUMP || iCurAnim == 84 || iCurAnim == 85 || iCurAnim == 86)
	{
		if (m_dDelay_Player_Change < 0.9)
		{
			m_pTransformCom->Go_Up(dTimeDelta * 7.0f);

			_float4 SwappingPos = CPlayerManager::GetInstance()->Get_Swaping_Pos();
			_float4 MyPos;
			XMStoreFloat4(&MyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			SwappingPos.y = MyPos.y;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&SwappingPos));
		}
		else
			m_isSwap_OnSky = true;

		m_isJumpOn = true;
	}

	if (m_isSwap_OnSky)
	{
		_float4 AnotherPos = CPlayerManager::GetInstance()->Get_PlayerPos_Change();
		_float4 CurPos;
		XMStoreFloat4(&CurPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		AnotherPos.y = CurPos.y;

		AnotherPos.y = 13.0f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&AnotherPos));
	}

}

void CPlayer_Tanjiro::Moving_Restrict()
{
	_int iCurAnimIndex = m_pModelCom->Get_iCurrentAnimIndex();


	//히트시 제한 + 기상, 다운
	if (ANIM_DMG_BLOW == iCurAnimIndex || 120 == iCurAnimIndex || 121 == iCurAnimIndex
		|| ANIM_DMG_SPIN == iCurAnimIndex || 132 == iCurAnimIndex || 133 == iCurAnimIndex
		|| ANIM_FALL == iCurAnimIndex || 125 == iCurAnimIndex || 126 == iCurAnimIndex
		|| ANIM_DMG_SMALL == iCurAnimIndex || ANIM_DMG_SMALL_RETURN == iCurAnimIndex || ANIM_DMG_BIG_RETURN == iCurAnimIndex || ANIM_DMG_BIG == iCurAnimIndex
		|| ANIM_DOWN == iCurAnimIndex || ANIM_DOWN_GETUP_MOVE == iCurAnimIndex || 138 == iCurAnimIndex
		|| ANIM_DOWN_GETUP == iCurAnimIndex || 135 == iCurAnimIndex
		|| 139 == iCurAnimIndex || 140 == iCurAnimIndex || 141 == iCurAnimIndex || 142 == iCurAnimIndex || ANIM_DMG_SWAMPBIND == iCurAnimIndex
		|| ANIM_DMG_AIR_SMALL_CONNECT_0 == iCurAnimIndex || ANIM_DMG_AIR_SMALL_CONNECT_1 == iCurAnimIndex || ANIM_DMG_AIR_SMALL_CONNECT_2 == iCurAnimIndex
		)
	{
		m_Moveset.m_isHitMotion = true;

		m_isAirDashing = false;

		m_Moveset.m_State_Battle_Guard = false;

		//다운상태
		if (ANIM_DOWN == iCurAnimIndex)
		{
			m_Moveset.m_isDownMotion = true;
			m_Moveset.m_Down_Dmg_Blow = false;

			m_Moveset.m_isGetUpMotion = false;

			m_isSwampUpper = false;
		}

		//겟업 상태
		if (ANIM_DOWN_GETUP == iCurAnimIndex || 135 == iCurAnimIndex || ANIM_DMG_BIG_RETURN == iCurAnimIndex
			|| ANIM_DOWN_GETUP_MOVE == iCurAnimIndex || 138 == iCurAnimIndex || ANIM_DMG_SMALL_RETURN == iCurAnimIndex)
		{
			m_Moveset.m_isGetUpMotion = true;
		}

		//스왐프 바인드
		if (ANIM_DMG_SWAMPBIND == iCurAnimIndex)
		{
			m_Moveset.m_isDownMotion = true;
			m_isSwampBinding = true;

		}
	}
	//콤보공격시 무빙제한
	else if (ANIM_ATK_COMBO == iCurAnimIndex
		|| 22 == iCurAnimIndex || 23 == iCurAnimIndex
		|| 24 == iCurAnimIndex || 25 == iCurAnimIndex || 26 == iCurAnimIndex
		|| 27 == iCurAnimIndex || 28 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;

		m_isCan_GuardCancel = true;

		if (27 == iCurAnimIndex || 28 == iCurAnimIndex)
		{
			m_Moveset.m_isRestrict_KeyInput = true;
			m_Moveset.m_isRestrict_Throw = true;
			m_Moveset.m_isRestrict_Jump = true;
			m_Moveset.m_isRestrict_Charge = true;
			m_Moveset.m_isRestrict_Step = true;
			m_Moveset.m_isRestrict_Dash = true;
		}
	}
	//점프 콤보공격시 제한
	else if (ANIM_ATK_AIRCOMBO == iCurAnimIndex || 30 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Jump = true;
		m_Moveset.m_isRestrict_JumpCombo = true;
	}
	//차지공격 시 무빙제한
	else if (ANIM_ATK_CHARGE == iCurAnimIndex || 32 == iCurAnimIndex || 33 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Charge = true;
	}
	//스킬공격 시 무빙제한
	else if (ANIM_ATK_SKILL_GUARD == iCurAnimIndex || 35 == iCurAnimIndex || 36 == iCurAnimIndex || 37 == iCurAnimIndex
		|| ANIM_ATK_SKILL_MOVE == iCurAnimIndex || 39 == iCurAnimIndex || 40 == iCurAnimIndex || 41 == iCurAnimIndex
		|| ANIM_ATK_SKILL_NORMAL == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		_bool bMonster_PushAway = CMonsterManager::GetInstance()->Get_Monster_PushAway();

		m_isSkilling = true;

		if (bMonster_PushAway == true)
			m_isSkilling = false;
	}
	//잡기 공격 시 제한
	else if (ANIM_ATK_THROW == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Throw = true;
	}
	//점프 트랙 (이동키 + J키)공격 시 제한
	else if (ANIM_ATK_AIRTRACK == iCurAnimIndex || 50 == iCurAnimIndex || 51 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Jump = true;
	}
	//Awaken
	else if (ANIM_BATTLE_AWAKEN == iCurAnimIndex || ANIM_BATTLE_AWAKEN_COMPLETE_CUTSCENE == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Jump = true;
		m_Moveset.m_isRestrict_Charge = true;
		m_Moveset.m_isRestrict_Step = true;
		m_Moveset.m_isRestrict_Dash = true;

		if (ANIM_BATTLE_AWAKEN == iCurAnimIndex)
			m_isSkilling = true;
	}
	//Special
	else if (ANIM_ATK_SPECIAL_CUTSCENE == iCurAnimIndex || 102 == iCurAnimIndex || 103 == iCurAnimIndex || 104 == iCurAnimIndex || 105 == iCurAnimIndex || 106 == iCurAnimIndex
		|| ANIM_ATK_SPECIAL_READY == iCurAnimIndex || 108 == iCurAnimIndex || 109 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Jump = true;
		m_Moveset.m_isRestrict_Charge = true;
		m_Moveset.m_isRestrict_Step = true;
		m_Moveset.m_isRestrict_Dash = true;
		m_Moveset.m_isRestrict_Special = true;
	}
	//가드 시 제한
	else if (ANIM_BATTLE_GUARD == iCurAnimIndex || 64 == iCurAnimIndex || 65 == iCurAnimIndex
		|| ANIM_BATTLE_GUARD_HIT_BIG == iCurAnimIndex || ANIM_BATTLE_GUARD_HIT_SMALL == iCurAnimIndex || ANIM_BATTLE_GUARD_PUSH == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isHitMotion = false;
	}
	//대시 시 제한
	else if (ANIM_BATTLE_DASH == iCurAnimIndex || 80 == iCurAnimIndex /* || 81 == iCurAnimIndex */)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Dash = true;
	}
	//점프 시 무빙제한
	else if (ANIM_BATTLE_JUMP == iCurAnimIndex
		|| 84 == iCurAnimIndex || 85 == iCurAnimIndex)
	{
		if (m_ePlayerState == PLAYER_BATTLE)
		{
			m_Moveset.m_isRestrict_Move = true;

			if (m_isSwamp_Escape)
			{
				if (ANIM_BATTLE_JUMP == iCurAnimIndex)
					m_Moveset.m_isRestrict_Jump = true;
				else
					m_Moveset.m_isRestrict_Jump = false;
			}
			else
			{
				m_Moveset.m_isRestrict_Jump = true;
			}
		}
	}
	//스텝 시 제한
	else if (ANIM_BATTLE_STEP_AB == iCurAnimIndex || ANIM_BATTLE_STEP_AF == iCurAnimIndex || ANIM_BATTLE_STEP_AL == iCurAnimIndex || ANIM_BATTLE_STEP_AR == iCurAnimIndex
		|| ANIM_BATTLE_STEP_B == iCurAnimIndex || ANIM_BATTLE_STEP_F == iCurAnimIndex || ANIM_BATTLE_STEP_L == iCurAnimIndex || ANIM_BATTLE_STEP_R == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Step = true;
	}
	//더블스텝 시 제한
	else if (98 == iCurAnimIndex || 100 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;

		if (m_dDelay_DoubleStep < 0.35f)
			m_Moveset.m_isRestrict_DoubleStep = true;
		else
			m_Moveset.m_isRestrict_DoubleStep = false;
	}
	//어드벤처 모드 런, 기본
	else if (ANIM_ADV_IDLE == iCurAnimIndex || ANIM_ADV_RUN == iCurAnimIndex || ANIM_ADV_RUN_END == iCurAnimIndex
		|| ANIM_ADV_STEALTH_IDLE == iCurAnimIndex || ANIM_ADV_STEALTH_WALK == iCurAnimIndex || 145 == iCurAnimIndex || 146 == iCurAnimIndex)
	{
		m_pSword->Set_SwordIn(true);
		m_pSword->Set_SwordIn(true);

		m_Moveset.m_isRestrict_Adventure = false;
	}
	//어드벤처 모드 행동
	else if (ANIM_ADV_JUMP == iCurAnimIndex || 2 == iCurAnimIndex || 3 == iCurAnimIndex || 4 == iCurAnimIndex)
	{
		m_pSword->Set_SwordIn(true);
		m_pSword->Set_SwordIn(true);

		m_Moveset.m_isRestrict_Adventure = true;
	}
	//제한 해제d
	else
	{
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

		m_pSword->Set_SwordIn(false);
		m_pSword->Set_SwordIn(false);

		m_isSkilling = false;

		m_isSwampBinding = false;

		m_isCan_GuardCancel = false;
	}
}

void CPlayer_Tanjiro::Event_Tanjiro_Death(_double dTimeDelta)
{
	if (m_isFirst_Event_Death)
	{
		m_isFirst_Event_Death = false;

		m_pModelCom->Set_Animation(ANIM_GBLOW);

		_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_0_Spak.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);

		Set_CharacterDialog(10.3f, TEXT("[탄지로]"), TEXT("끄아아아아악... "));
	}

	Go_Backward_Deceleration(dTimeDelta, ANIM_GBLOW, 2.5f, 0.055f);

	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_DOWN || iCurAnim == ANIM_BATTLE_IDLE || iCurAnim == ANIM_DOWN_GETUP)
	{
		m_dDelay_TanjiroDead += dTimeDelta;

		if (m_dDelay_TanjiroDead > 1.0f && m_isFirst_Dead_0)
		{
			m_isFirst_Dead_0 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_1.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);

			Set_CharacterDialog(45.0f, TEXT("[탄지로]"), TEXT("몸이.....  움직이지 않아..... "));
		}
		else if (m_dDelay_TanjiroDead > 4.5f && m_isFirst_Dead_1)
		{
			m_isFirst_Dead_1 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_2.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);
		}
		else if (m_dDelay_TanjiroDead > 7.5f && m_isFirst_Dead_2)
		{
			m_isFirst_Dead_2 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_3_Uroko.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);

			Set_CharacterDialog(45.0f, TEXT("[우로코다키]"), TEXT("왜 그러고 있지? 탄지로!"), TEXT("넌 뭘 위해 여기있는거냐?"));
		}
		else if (m_dDelay_TanjiroDead > 9.0f && m_isFirst_Dead_3)
		{
			m_isFirst_Dead_3 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_4_Uroko.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);

		}
		else if (m_dDelay_TanjiroDead > 13.0f && m_isFirst_Dead_4)
		{
			m_isFirst_Dead_4 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_5_Uroko.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);

			Set_CharacterDialog(45.0f, TEXT("[우로코다키]"), TEXT("그러고 있으면 지금까지 해온 모든게 의미없어 진다!!!"), TEXT("일어나거라! 정신 차려! 마지막까지 포기하지 마라!"));

		}
		else if (m_dDelay_TanjiroDead > 16.0f && m_isFirst_Dead_5)
		{
			m_isFirst_Dead_5 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_6_Uroko.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);
		}
		else if (m_dDelay_TanjiroDead > 19.0f && m_isFirst_Dead_6)
		{
			m_isFirst_Dead_6 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_7_Uroko.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);
		}
		else if (m_dDelay_TanjiroDead > 23.0f && m_isFirst_Dead_7)
		{
			m_isFirst_Dead_7 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_8.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);

			Set_CharacterDialog(45.0f, TEXT("[탄지로]"), TEXT("몸이 너무 아파.... 분해... 하지만 할수밖에 없어!!!"), TEXT("집중해야해! 호흡을 가다듬어야 해!!"));
		}//완료
		else if (m_dDelay_TanjiroDead > 32.5f && m_isFirst_Dead_8)
		{
			m_isFirst_Dead_8 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_9.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);
		}
		else if (m_dDelay_TanjiroDead > 37.0f && m_isFirst_Dead_9)
		{
			m_isFirst_Dead_9 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_10_Gihap.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_1, 0.7f);

			Set_CharacterDialog(45.0f, TEXT("[탄지로]"), TEXT("으아아아아아아!!!"), TEXT("난 절대로 포기하지 않아!"));

			m_pModelCom->Set_Animation(ANIM_DOWN_GETUP);
		}
		else if (m_dDelay_TanjiroDead > 40.0f && m_isFirst_Dead_10)
		{
			m_isFirst_Dead_10 = false;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Death_11.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_DEATH_0, 0.75f);
		}
		

		

	}
	if (m_dDelay_TanjiroDead > 37.0f)
	{
		m_StatusDesc.fHp_Max = 300.0f;
		m_StatusDesc.fHp += 0.8f;

		if (m_StatusDesc.fHp >= m_StatusDesc.fHp_Max)
		{
			m_StatusDesc.fHp = m_StatusDesc.fHp_Max;
			m_isPlayerTanjiroDead = false;
		}
	}

}

void CPlayer_Tanjiro::Smell_Detection(_double dTimeDelta)
{
	if (m_bSmell_Detection == true)
	{
		m_dSmellTime += dTimeDelta;
		if (m_dSmellTime > 3.0)
		{
			m_bSmell_Detection = false;
			m_dSmellTime = 0.0;
			m_pRendererCom->Set_GrayScale_On(false);
		}
	}
}

void CPlayer_Tanjiro::Create_SwampWaterParticleEffect(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_dWaterEffectAccTime += dTimeDelta;

	if (m_dWaterEffectAccTime > 0.1)
	{
		CEffectW::EFFECTWDESC EffectWDesc;
		EffectWDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		EffectWDesc.eEffectWType = CEffectW_Manager::EFFECT_SWAMPWATER;
		EffectWDesc.iNumX = 4; EffectWDesc.iNumY = 4;

		EffectWDesc.vStartPosX = { -0.4f,0.4f };  EffectWDesc.vStartPosZ = { -0.4f,0.4f };
		EffectWDesc.vFrameSpeed = { 0.03f , 0.05f };
		EffectWDesc.vStartSizeX = { 0.7f , 1.1f }; EffectWDesc.vStartSizeY = { 1.1f , 1.5f };
		EffectWDesc.vSpeedX = { -2.0f , 2.0f }; EffectWDesc.vSpeedY = { 3.5f , 6.5f };
		EffectWDesc.vStartFrame = { 0.f ,5.f };
		EffectWDesc.fGravity = { 2.f };


		for (_uint i = 0; i < 5; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_SWAMPWATER, &EffectWDesc);

		m_dWaterEffectAccTime = 0.0;
	}

	Safe_Release(pGameInstance);
}

void CPlayer_Tanjiro::Play_Sound_Atk(_int iType, _float vol)
{
	//small
	if (iType == 0)
	{
		if (m_iSound_Atk_Small == 0)
		{
			m_iSound_Atk_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Small_Se.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Atk_Small == 1)
		{
			m_iSound_Atk_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Small_Kya.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
		else if (m_iSound_Atk_Small == 2)
		{
			m_iSound_Atk_Small = 0;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Small_Hue.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
	}
	//medium
	else if (iType == 1)
	{
		if (m_iSound_Atk_Medium == 0)
		{
			m_iSound_Atk_Medium++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Medium_Thiiiaaa.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
		else if (m_iSound_Atk_Medium == 1)
		{
			m_iSound_Atk_Medium++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Medium_Doriya.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Atk_Medium == 2)
		{
			m_iSound_Atk_Medium++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Medium_Tiiiaa.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
		else if (m_iSound_Atk_Medium == 3)
		{
			m_iSound_Atk_Medium++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Medium_Korea.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Atk_Medium == 4)
		{
			m_iSound_Atk_Medium = 0;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Medium_Urya.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
	}
	//big
	else if (iType == 2)
	{
		if (m_iSound_Atk_Big == 0)
		{
			m_iSound_Atk_Big++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Big_Doda.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Atk_Big == 1)
		{
			m_iSound_Atk_Big++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Big_Seiya.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
		else if (m_iSound_Atk_Big == 2)
		{
			m_iSound_Atk_Big++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Big_Thiaa.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Atk_Big == 3)
		{
			m_iSound_Atk_Big = 0;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Shout_Big_Yosha.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}

	}
}

void CPlayer_Tanjiro::Play_Sound_Dmg(_int iType, _float vol)
{
	//small
	if (iType == 0)
	{
		if (m_iSound_Dmg_Small == 0)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Small_0.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Dmg_Small == 1)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Small_1.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
		else if (m_iSound_Dmg_Small == 2)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Small_2.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Dmg_Small == 3)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Small_3.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
		else if (m_iSound_Dmg_Small == 4)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Small_4.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Dmg_Small == 5)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Small_6.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
		else if (m_iSound_Dmg_Small == 6)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Small_6.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Dmg_Small == 7)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Small_7.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
		else if (m_iSound_Dmg_Small == 8)
		{
			m_iSound_Dmg_Small = 0;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Small_8.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
	}
	// Medium
	else if (iType == 1)
	{
		if (m_iSound_Dmg_Medium == 0)
		{
			m_iSound_Dmg_Medium++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Medium_0.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
		else if (m_iSound_Dmg_Medium == 1)
		{
			m_iSound_Dmg_Medium++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Medium_1.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Dmg_Medium == 2)
		{
			m_iSound_Dmg_Medium = 0;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Medium_2.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
	}
	// Big
	else if (iType == 2)
	{
		if (m_iSound_Dmg_Big == 0)
		{
			m_iSound_Dmg_Big++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Big_0.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
		else if (m_iSound_Dmg_Big == 1)
		{
			m_iSound_Dmg_Big++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Big_1.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE_SUB, vol);
		}
		else if (m_iSound_Dmg_Big == 2)
		{
			m_iSound_Dmg_Big = 0;

			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Dmg_Big_2.mp3");
			Play_Sound_Channel(szSoundFile, CSoundMgr::PLAYER_VOICE, vol);
		}
	}
}

HRESULT CPlayer_Tanjiro::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tanjiro"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CPlayer_Tanjiro");
		return E_FAIL;
	}


	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CPlayer_Tanjiro");
		return E_FAIL;
	}

	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CPlayer_Tanjiro");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CPlayer_Tanjiro");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CPlayer_Tanjiro");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.f, 1.f, 1.f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CPlayer_Tanjiro");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Tanjiro::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;


	// OutlineThickness
	if (m_Moveset.m_iAwaken != 0 || m_isSkilling == true)
	{
		m_fOutlineThickness = 2.0f;
	}
	else
		m_fOutlineThickness = 1.5f;


	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineThickness", &m_fOutlineThickness, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineFaceThickness", &m_fOutlineFaceThickness, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fFar2", &m_fFar2, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CPlayer_Tanjiro* CPlayer_Tanjiro::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Tanjiro* pInstance = new CPlayer_Tanjiro(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Tanjiro");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Tanjiro::Clone(void* pArg)
{
	CPlayer_Tanjiro* pInstance = new CPlayer_Tanjiro(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Tanjiro");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Tanjiro::Free()
{
	Safe_Release(m_pSword);
	Safe_Release(m_pSwordHome);

	__super::Free();
}
