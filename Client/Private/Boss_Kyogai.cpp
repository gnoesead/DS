#include "pch.h"
#include "Boss_Kyogai.h"
#include "GameInstance.h"
#include "SoundMgr.h"
#include "RotationMapObject.h"
#include "EffectPlayer.h"
#include "Player.h"
#include "PlayerManager.h"
#include "AtkCollManager.h"
#include "AlertCircle.h"
#include "AlertRect.h"
#include "RoomSmoke.h"
#include "Camera_Manager.h"
#include "Camera_Free.h"
#include "Fade_Manager.h"
#include "MonsterManager.h"
#include "Battle_UI_Manager.h"


CBoss_Kyogai::CBoss_Kyogai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CBoss_Kyogai::CBoss_Kyogai(const CBoss_Kyogai& rhs)
	: CMonster(rhs)
{
}

HRESULT CBoss_Kyogai::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Kyogai::Initialize(void* pArg)
{

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Read_Animation_Control_File("Kyogai.bin")))
	{
		MSG_BOX("Failed to AnimData Read : Kyogai");
		return E_FAIL;
	}

	//Get_PlayerComponent();

	m_eCurAnimIndex = ANIM_IDLE;
	m_eCurstate = STATE_INTERACT;
	m_eCurPhase = BEGIN;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(123.f, 0.f, 126.f, 1.f));
	m_eCurNavi = NAVI_HOUSE_4_0;

	//m_eCurNavi = NAVI_ACAZA;
	m_StatusDesc.fHp = 200.f;
	m_StatusDesc.fHp_Max = 200.f;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CAurora::EFFECTDESC AuroraDesc;
	AuroraDesc.pTransform = m_pTransformCom;
	AuroraDesc.pGameObject = this;
	AuroraDesc.eType = CAurora::TYPE_LOCAL;
	_uint iCurIdx = pGameInstance->Get_CurLevelIdx();
	AuroraDesc.eCharacter = CAurora::CHARACTER_KYOGAI;

	AuroraDesc.eColor = CAurora::COLOR_RED;
	for (_uint i = 0; i < 20; ++i)
		pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_Effect_Aurora"), TEXT("Prototype_GameObject_Aurora"), &AuroraDesc);

	AuroraDesc.eColor = CAurora::COLOR_BLUE;
	for (_uint i = 0; i < 20; ++i)
		pGameInstance->Add_GameObject(iCurIdx, TEXT("Layer_Effect_Aurora"), TEXT("Prototype_GameObject_Aurora"), &AuroraDesc);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CBoss_Kyogai::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
		if (pPlayer->Get_CurNaviMesh() != CLandObject::NAVI_HOUSE_4_0)
		{
			Safe_Release(pGameInstance);
			return;
		}
	}

	Safe_Release(pGameInstance);

	__super::Tick(dTimeDelta);

	if (true == m_isDead)
		return;


	if (m_bTanjiroAwake == false && m_bZenitsuAwake == false)
	{
		Debug_State(dTimeDelta);
		Update_Hit_Messenger(dTimeDelta);
		Update_Trigger(dTimeDelta);
		Update_State(dTimeDelta);

		if (m_bStopAnim == false)
		{
			m_pModelCom->Set_Animation(m_eCurAnimIndex);
			m_pModelCom->Play_Animation_For_Boss(dTimeDelta);
		}


		EventCall_Control(dTimeDelta);
	}

	Dialog_Update(dTimeDelta);

	FastBook_Update(dTimeDelta);
}

void CBoss_Kyogai::LateTick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
		if (pPlayer->Get_CurNaviMesh() != CLandObject::NAVI_HOUSE_4_0)
		{
			Safe_Release(pGameInstance);
			return;
		}
	}

	Safe_Release(pGameInstance);

	__super::LateTick(dTimeDelta);
	if (m_bTanjiroAwake == false && m_bZenitsuAwake == false)
	{
		Update_AnimIndex(m_eCurAnimIndex);
		if (m_bTurn == false)
			Gravity(dTimeDelta);
	}
}

HRESULT CBoss_Kyogai::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	if (m_bMonsterDead == false)
	{
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
				if (m_bSuperArmor == false)
					m_pShaderCom->Begin(1);
				else
					m_pShaderCom->Begin(4);
			}

			m_pModelCom->Render(m_iMeshNum);
		}
	}
	// Default Render
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderBoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;

		if (m_bMonsterDead == true)
			m_pShaderCom->Begin(8);
		else
			m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}



	return S_OK;
}

HRESULT CBoss_Kyogai::Render_ShadowDepth()
{
	if (FAILED(__super::Render_ShadowDepth()))
		return E_FAIL;
	return S_OK;
}
//#ifdef _DEBUG
void CBoss_Kyogai::Debug_State(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_F2))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(114.f, 0.f, 117.f, 1.f));
		m_iTriggerCnt = 0;
		m_dTimeAcc = 0.0;
		m_eCurstate = STATE_INTERACT;
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE))
	{
		if (pGameInstance->Get_DIKeyDown(DIK_DELETE))
		{
			m_StatusDesc.fHp -= 20.f;
		}
		if (pGameInstance->Get_DIKeyDown(DIK_INSERT))
		{
			m_StatusDesc.fHp += 20.f;
		}

		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
			Trigger_AtkCmb();

		}
		if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			Trigger_AtkCmb2();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			Trigger_StompKick();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_4))
		{
			Trigger_AtkStep();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{
			Trigger_Awake();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_6))
		{
			Trigger_AtkPunch();
		}

	}
	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT))
	{

		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
			Trigger_LinkerCmb();

		}
		if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			Trigger_AtkSkCmb();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			Trigger_JumpStep();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_4))
		{
			Trigger_Awake_AtkskCmb();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{
			Trigger_AtkCmb2();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_6))
		{
			pGameInstance->Time_Slow(0.5, 0.2);
		}
	}
	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL))
	{

		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
			Trigger_AtkSk();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			Trigger_AtkPangPang();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			Trigger_Hit_Dead();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_4))
		{

		}
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{

		}
		if (pGameInstance->Get_DIKeyDown(DIK_6))
		{

		}
	}
	Safe_Release(pGameInstance);
}
//#endif //_DEBUG

void CBoss_Kyogai::EventCall_Control(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_iEvent_Index = 0;
	}

	if (EventCallProcess())
	{
		_vector vMonsterDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vDir = Calculate_Dir_ZeroY();
		_vector vPlayerDir = m_pPlayerTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vRandomDir = Random_Dir(vPlayerDir, 0.f, 0.f, 0.f, 360.f);
		_vector vVerticalDir = Vertical_Dir();
		_vector vHorizonDir = HorizonDir();

		_double dLifeTime = 0.20;
		_double dLongLifeTime = 1.0;
		_double dSpeed = 5.0;

		if (0 == m_pModelCom->Get_iCurrentAnimIndex()) // 컷신
		{
			if (0 == m_iEvent_Index) // 0.1
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.5f;
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Effect0", m_pTransformCom, &EffectWorldDesc);

				Set_CharacterDialog(3.f, TEXT("[쿄우가이]"), TEXT("소생은 강하다..!!"));
				_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Gaebang_Talk.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
			}

			else if (1 == m_iEvent_Index) // 0.2 
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle", m_pTransformCom);
			}
			else if (2 == m_iEvent_Index) // 0.34
			{
				m_isAuroraOn[0] = true;
			}
			else if (3 == m_iEvent_Index) // 0.7
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle", m_pTransformCom);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.7f;
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle2", m_pTransformCom, &EffectWorldDesc);
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle2", m_pTransformCom, &EffectWorldDesc);
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle2", m_pTransformCom, &EffectWorldDesc);
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle2", m_pTransformCom, &EffectWorldDesc);
				Camera_Shake(1.0, 40);
				m_pRendererCom->Set_RadialBlur();
			}
			else if (4 == m_iEvent_Index) // 1.2
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle", m_pTransformCom);
			}
			else if (5 == m_iEvent_Index) // 1.7
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle", m_pTransformCom);
				m_pRendererCom->Set_RadialBlur();

			}
			else if (6 == m_iEvent_Index) // 2.2
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Explosion", m_pTransformCom);
				Camera_Shake(1.6, 150);
				m_pRendererCom->Set_RadialBlur();
				Create_GroundSmoke(CGroundSmoke::SMOKE_KYOGAI_KICKDOWN);
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle3", m_pTransformCom);

			}
			else if (7 == m_iEvent_Index) // 2.7
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle", m_pTransformCom);

			}
			else if (8 == m_iEvent_Index) // 3.2
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_CutScene_Particle", m_pTransformCom);
				m_pRendererCom->Set_RadialBlur();
			}
		}


#pragma region AWAKE_ComboPunch
		if (ANIM_ATKCMB2 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.8f;
				EffectWorldDesc.vPosition.z += 0.5f;
				EffectWorldDesc.fScale = 1.2f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_1_1", m_pTransformCom, &EffectWorldDesc);

			}
			else if (1 == m_iEvent_Index) // 0.15
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("tsuzumi_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_3, 0.7f);
			}
			else if (2 == m_iEvent_Index)	// 0.25
			{
				dLongLifeTime = 6.0;
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

				CAtkCollider* pAtkCollider = dynamic_cast<CAtkCollider*> (pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_MonsterAtk"), (_uint)pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("Layer_MonsterAtk")) - 1));

				Create_AlertRect(BLADE_FIVE_RANDOM);
				Create_BladeEffect(BLADE_FIVE_RANDOM, vRandomDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

				
				_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_1, 0.1f);
				_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
				Play_Sound_Channel(szSoundFile2, CSoundMgr::MONSTER_EFFECT_2, 0.1f);

			}
			else if (3 == m_iEvent_Index) // 1.10
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("tsuzumi_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_3, 0.7f);
			}
			else if (4 == m_iEvent_Index)	// 1.20
			{
				dLongLifeTime = 6.0;

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

				Create_AlertRect(BLADE_THREE_RANDOM);
				Create_BladeEffect(BLADE_THREE_RANDOM, vRandomDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

				_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_4, 0.1f);
				_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
				Play_Sound_Channel(szSoundFile2, CSoundMgr::MONSTER_EFFECT_5, 0.1f);
			}
		}

		if (ANIM_ATKCMB3 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			switch (m_eCurPhase)
			{
			case CBoss_Kyogai::PHASE_1:
			{
				if (0 == m_iEvent_Index)	// 0.0
				{
					CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
					EffectWorldDesc.vPosition.y += 0.8f;
					EffectWorldDesc.vPosition.z += 0.5f;
					EffectWorldDesc.fScale = 1.2f;

					CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_1_2", m_pTransformCom, &EffectWorldDesc);
				}
				else if (1 == m_iEvent_Index) // 0.13
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("tsuzumi_01.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_3, 0.7f);
				}
				else if (2 == m_iEvent_Index)	// 0.25
				{
					dLongLifeTime = 3.0;
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

					Create_AlertRect(BLADE_THREE_RANDOM);
					Create_BladeEffect(BLADE_THREE_RANDOM, vRandomDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

					_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
					Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_1, 0.15f);
					_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
					Play_Sound_Channel(szSoundFile2, CSoundMgr::MONSTER_EFFECT_2, 0.15f);
				}
			}
			break;
			case CBoss_Kyogai::PHASE_2:
			{
				if (0 == m_iEvent_Index)	// 0.0
				{
					CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
					EffectWorldDesc.vPosition.y += 0.8f;
					EffectWorldDesc.vPosition.z += 0.5f;
					EffectWorldDesc.fScale = 1.2f;

					CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_1_2", m_pTransformCom, &EffectWorldDesc);
				}
				if (1 == m_iEvent_Index)	// 0.25
				{
					dLongLifeTime = 5.0;
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
						CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET, "Kyogai_BladeAtk");
					Create_AlertRect(BLADE_ONE_RANDOM);

					vVerticalDir = Rotation_Dir(vVerticalDir, -20.f);

					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
						CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET, "Kyogai_BladeAtk");
					Create_AlertRect(BLADE_ONE_RANDOM);

					vVerticalDir = Rotation_Dir(vVerticalDir, 40.f);

					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
						CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET, "Kyogai_BladeAtk");
					Create_AlertRect(BLADE_ONE_RANDOM);

					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
						CAtkCollider::TYPE_SMALL, -vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET, "Kyogai_BladeAtk");
					Create_AlertRect(BLADE_ONE_RANDOM);

					vVerticalDir = Rotation_Dir(vVerticalDir, 40.f);

					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
						CAtkCollider::TYPE_SMALL, -vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET, "Kyogai_BladeAtk");
					Create_AlertRect(BLADE_ONE_RANDOM);

					_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
					Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_1, 0.15f);
					_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
					Play_Sound_Channel(szSoundFile2, CSoundMgr::MONSTER_EFFECT_2, 0.15f);
				}
			}
			break;
			default:
				break;
			}

		}


		if (ANIM_ATKCMB_02LOOP == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.09
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("tsuzumi_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.7f);
			}

			else if (1 == m_iEvent_Index)	// 0.19
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.8f;
				EffectWorldDesc.vPosition.z += 0.5f;
				EffectWorldDesc.fScale = 1.2f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_2", m_pTransformCom, &EffectWorldDesc);

				dLongLifeTime = 4.0;

				if (m_iLoopCount == 0)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(8.5f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vVerticalDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

					Create_AlertRect(BLADE_FIVE_RANDOM, vVerticalDir, 7.f);
					Create_BladeEffect(BLADE_FIVE_RANDOM, vVerticalDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.5f);

					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(-8.5f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vVerticalDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

					Create_AlertRect(BLADE_FIVE_RANDOM, vVerticalDir, -7.f);
					Create_BladeEffect(BLADE_FIVE_RANDOM, vVerticalDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, -8.5f);
				}
				if (m_iLoopCount == 1)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vVerticalDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

					Create_AlertRect(BLADE_FIVE_RANDOM, vVerticalDir);
					Create_BladeEffect(BLADE_FIVE_RANDOM, vVerticalDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

					Create_AlertRect(BLADE_FIVE_RANDOM, vHorizonDir);
					Create_BladeEffect(BLADE_FIVE_RANDOM, vHorizonDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);
				}
				if (m_iLoopCount == 2)
				{
					vHorizonDir = Rotation_Dir(vHorizonDir, 20.f, 0.f);
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

					Create_AlertRect(BLADE_ONE_RANDOM, vHorizonDir);
					Create_BladeEffect(BLADE_ONE_RANDOM, vHorizonDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

				}
				if (m_iLoopCount == 3)
				{
					vHorizonDir = Rotation_Dir(vHorizonDir, 60.f, 0.f);
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

					Create_AlertRect(BLADE_ONE_RANDOM, vHorizonDir);
					Create_BladeEffect(BLADE_ONE_RANDOM, vHorizonDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

				}

				_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_1, 0.1f);
				_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
				Play_Sound_Channel(szSoundFile2, CSoundMgr::MONSTER_EFFECT_2, 0.1f);
			}

			else if (2 == m_iEvent_Index) // 0.52
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("tsuzumi_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_3, 0.7f);
			}

			else if (3 == m_iEvent_Index)	// 0.62
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.8f;
				EffectWorldDesc.vPosition.z += 0.5f;
				EffectWorldDesc.fScale = 1.2f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_2", m_pTransformCom, &EffectWorldDesc);
				dLongLifeTime = 3.0;
				/*if (m_iLoopCount == 0)
				{
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(8.5f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

					Create_AlertRect(BLADE_FIVE_RANDOM, vHorizonDir, 8.5f);
					Create_BladeEffect(BLADE_FIVE_RANDOM, vHorizonDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.5f);

					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(-8.5f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

					Create_AlertRect(BLADE_FIVE_RANDOM, vHorizonDir, -8.5f);
					Create_BladeEffect(BLADE_FIVE_RANDOM, vHorizonDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, -8.5f);
				}*/
				/*if (m_iLoopCount == 1)
				{

					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

					Create_AlertRect(BLADE_ONE_RANDOM, vHorizonDir);
					Create_BladeEffect(BLADE_ONE_RANDOM, vHorizonDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

				}
				if (m_iLoopCount == 2)
				{
					vHorizonDir = Rotation_Dir(vHorizonDir, 40.f, 0.f);
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_CONNECTSMALL, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

					Create_AlertRect(BLADE_ONE_RANDOM, vHorizonDir);
					Create_BladeEffect(BLADE_ONE_RANDOM, vHorizonDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

				}
				if (m_iLoopCount == 3)
				{
					vHorizonDir = Rotation_Dir(vHorizonDir, 80.f, 0.f);
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_CONNECTSMALL, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

					Create_AlertRect(BLADE_ONE_RANDOM, vHorizonDir);
					Create_BladeEffect(BLADE_ONE_RANDOM, vHorizonDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

				}*/
			}

		}

		if (ANIM_ATKSK_RF == m_pModelCom->Get_iCurrentAnimIndex() || ANIM_ATKSK_LF == m_pModelCom->Get_iCurrentAnimIndex() ||
			ANIM_ATKSK_RB == m_pModelCom->Get_iCurrentAnimIndex() || ANIM_ATKSK_LB == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.40  or 0.45
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("ui_taishihyo_stamp_LW.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::KYOGAI_BOOK_0, 0.5f);

				
			}
			else if (1 == m_iEvent_Index)	// 0.5 or 0.55
			{
				CAlertCircle::EFFECTDESC EffectDesc;
				EffectDesc.pOwnerTransform = m_pTransformCom;
				EffectDesc.iType = CAlertCircle::TYPE_ROOMCHANGE;

				pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertCircle"), &EffectDesc, false);

				if (ANIM_ATKSK_LF == m_pModelCom->Get_iCurrentAnimIndex())
				{
					CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_22", m_pTransformCom);

					_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_12.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_2, 0.7f);

					if (PART_NONE == m_ePart)
					{
						m_ePart = PART_1;
					}
				}

				if (ANIM_ATKSK_RF == m_pModelCom->Get_iCurrentAnimIndex())
				{
					CEffectPlayer::EFFECTWORLDDESC EffectDesc;
					EffectDesc.vPosition.x += 0.6f;
					EffectDesc.vPosition.y += 0.5f;
					CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_22", m_pTransformCom, &EffectDesc);

					_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_AradaTasi.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_2, 1.f);
				}
			}
		}
		if (ANIM_KICKDOWN == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.5
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("hit_firel_01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.8f);
			}

			else if (1 == m_iEvent_Index)	// 0.6
			{
				//CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_11", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(8.0f, 8.0f, 8.0f), _float3(0.f, 1.0f, 2.f), dLifeTime,
					CAtkCollider::TYPE_UPPER, vMonsterDir, m_fBigDmg);

				CCameraManager::GetInstance()->Camera_Shake(1.0, 200);
				m_pRendererCom->Set_RadialBlur();

				CEffectPlayer::Get_Instance()->Play("Kyogai_Explosion_Particle", m_pTransformCom);
				Create_GroundSmoke(CGroundSmoke::SMOKE_KYOGAI_KICKDOWN);
			}
			else if (2 == m_iEvent_Index)	// 0.67
			{
				m_pRendererCom->Set_RadialBlur();
			}

		}

		if (ANIM_STOMPKICK == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.34
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("teoni_foot_01W.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.2f);
			}

			else if (1 == m_iEvent_Index)	// 1.1
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.7f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_11", m_pTransformCom, &EffectWorldDesc);

				CAlertCircle::EFFECTDESC EffectDesc;
				EffectDesc.pOwnerTransform = m_pTransformCom;
				EffectDesc.iType = CAlertCircle::TYPE_KICKDOWN;

				pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertCircle"), &EffectDesc, false);
			}

			else if (2 == m_iEvent_Index) // 1.55
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("hit_firel_01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.6f);
			}
			else if (3 == m_iEvent_Index) // 1.65
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(8.0f, 8.0f, 8.0f), _float3(0.f, 1.0f, 2.0f), dLifeTime,
					CAtkCollider::TYPE_UPPER, vMonsterDir, m_fBigDmg);

				CCameraManager::GetInstance()->Camera_Shake(1.0 , 200);
				m_pRendererCom->Set_RadialBlur();
				Create_GroundSmoke(CGroundSmoke::SMOKE_KYOGAI_KICKDOWN);
			}

			else if (4 == m_iEvent_Index) // 1.73
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_Explosion_Particle", m_pTransformCom);
				m_pRendererCom->Set_RadialBlur();
			}


		}
		if (ANIM_PUSHAWAY == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.0
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(10.0f, 10.0f, 10.0f), _float3(0.f, 5.f, 0.0f), dLongLifeTime,
					CAtkCollider::TYPE_BIG, vMonsterDir, 0.f);

				//CEffectPlayer::Get_Instance()->Play("Kyogai_Atk_Push", m_pTransformCom);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 2.5f;
				CEffectPlayer::Get_Instance()->Play("Kyogai_Push", m_pTransformCom, &EffectWorldDesc);
				Create_GroundSmoke(CGroundSmoke::SMOKE_KYOGAI_PUSH);
				Camera_Shake(1.0);
				CEffectPlayer::Get_Instance()->Play("Kyogai_Explosion_Particle", m_pTransformCom);
				m_pRendererCom->Set_RadialBlur();

				if (m_StatusDesc.fHp > 0.f)
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Scream.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.6f);
				}
				else
				{
					Play_Sound_Dmg(1, 0.9f);
				}
				_tchar szSoundFile2[MAX_PATH] = TEXT("amb_treeBreak_divided03.ogg");
				Play_Sound_Channel(szSoundFile2, CSoundMgr::MONSTER_EFFECT_1, 0.6f);
				_tchar szSoundFile3[MAX_PATH] = TEXT("hit_firel_01.ogg");
				Play_Sound_Channel(szSoundFile3, CSoundMgr::MONSTER_EFFECT_2, 0.6f);

				CMonsterManager::GetInstance()->Set_Monster_PushAway(true);
			}
			else if (1 == m_iEvent_Index) // 0.8
			{
				m_pRendererCom->Set_RadialBlur();
				CMonsterManager::GetInstance()->Set_Monster_PushAway(false);
			}

		}
		if (ANIM_ATKCMBW05 == m_pModelCom->Get_iCurrentAnimIndex()) // 앞발 내밀고 배꼽 팡팡 
		{

			if (0 == m_iEvent_Index)	// 0.0
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.8f;
				EffectWorldDesc.vPosition.z += 0.5f;
				EffectWorldDesc.fScale = 1.3f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_12", m_pTransformCom, &EffectWorldDesc);

			}
			else if (1 == m_iEvent_Index)
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("tsuzumi_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.5f);

				_tchar szSoundFile2[MAX_PATH] = TEXT("Kyogai_Simusik.ogg");
				Play_Sound_Channel(szSoundFile2, CSoundMgr::MONSTER_EFFECT_1, 0.2f);

				_tchar szSoundFile3[MAX_PATH] = TEXT("swingL_07.ogg");
				Play_Sound_Channel(szSoundFile3, CSoundMgr::MONSTER_EFFECT_3, 0.5f);
			}
			else if (2 == m_iEvent_Index) 
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("tsuzumi_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_2, 0.5f);
				Camera_Shake();
			}
			else if (3 == m_iEvent_Index) // 0.95
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 3.5f), dLifeTime,
					CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fBigDmg);

				m_pRendererCom->Set_RadialBlur();
			}
			else if (4 == m_iEvent_Index) // 1.05
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 2.5f), dLifeTime,
					CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg);

				m_pRendererCom->Set_RadialBlur();
			}
		}

		if (ANIM_ATKPUNCH == m_pModelCom->Get_iCurrentAnimIndex())
		{

			if (0 == m_iEvent_Index) // 1.15
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("swingL_01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.6f);
			}

			else if (1 == m_iEvent_Index) // 1.30
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.6f;
				EffectWorldDesc.vPosition.z += 0.5f;
				EffectWorldDesc.fScale = 2.0f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_9_2", m_pTransformCom, &EffectWorldDesc);


				Create_GroundSmoke(CGroundSmoke::SMOKE_DASHLAND);

			}

			else if (2 == m_iEvent_Index) // 1.50
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.8f;
				EffectWorldDesc.vPosition.z += 0.5f;
				EffectWorldDesc.fScale = 3.0f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_9", m_pTransformCom, &EffectWorldDesc);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.f), dLifeTime,
					CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);

				CEffectPlayer::Get_Instance()->Play("Kyogai_Swing_Particle1", m_pTransformCom);
				Camera_Shake();
			}

		}

		if (ANIM_ATKPUNCH2 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.15
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("swingL_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.6f);
			}

			else if (1 == m_iEvent_Index) // 0.2
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.6f;
				EffectWorldDesc.vPosition.z += 0.5f;
				EffectWorldDesc.fScale = 2.0f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_10_2", m_pTransformCom, &EffectWorldDesc);

				Create_GroundSmoke(CGroundSmoke::SMOKE_DASHLAND);
			}

			else if (2 == m_iEvent_Index) // 0.4
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.8f;
				EffectWorldDesc.vPosition.z += 0.5f;
				EffectWorldDesc.fScale = 3.0f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_10", m_pTransformCom, &EffectWorldDesc);
				CEffectPlayer::Get_Instance()->Play("Kyogai_Swing_Particle2", m_pTransformCom);

			}

			else if (3 == m_iEvent_Index) // 0.5
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.f), dLifeTime,
					CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg);

				Camera_Shake();
			}
		}

		if (ANIM_ATKSTEP_BACK == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Jump_Hueh.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_SUBEFFECT_0, 0.9f); 
				_tchar szSoundFile2[MAX_PATH] = TEXT("swingL_01.ogg");
				Play_Sound_Channel(szSoundFile2, CSoundMgr::MONSTER_SUBEFFECT_1, 0.7f);
			}

			if (1 == m_iEvent_Index) // 0.7
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("tsuzumi_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_2, 0.7f);
			}

			if (2 == m_iEvent_Index)	// 0.77
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.4f;
				EffectWorldDesc.vPosition.z += 0.f;
				EffectWorldDesc.fScale = 1.f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkBack", m_pTransformCom, &EffectWorldDesc);

				_tchar szSoundFile[MAX_PATH] = TEXT("aura_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_3, 0.2f);
			
			}
			switch (m_eCurPhase)
			{
			case CBoss_Kyogai::PHASE_1:
			{
				if (3 == m_iEvent_Index)	// 1.0
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_4, 0.2f);

					dLongLifeTime = 5.f;

					if (m_iAtkStepTypeNum == 1) // 일직선
					{
						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

						Create_AlertRect(BLADE_THREE_FRONT, vMonsterDir);
						Create_BladeEffect(BLADE_THREE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);
					}
					if (m_iAtkStepTypeNum == 2) // 삼각
					{
						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);
						Create_AlertRect(BLADE_THREE_FRONT, vMonsterDir);
						Create_BladeEffect(BLADE_THREE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

						vMonsterDir = Rotation_Dir(vMonsterDir, 10);
						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(1.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

						Create_AlertRect(BLADE_THREE_FRONT, vMonsterDir);
						Create_BladeEffect(BLADE_THREE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

						vMonsterDir = Rotation_Dir(vMonsterDir, -20);
						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(-1.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

						Create_AlertRect(BLADE_THREE_FRONT, vMonsterDir);
						Create_BladeEffect(BLADE_THREE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

					}

					if (m_iAtkStepTypeNum == 3) // 랜덤 두곳
					{
						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

						Create_AlertRect(BLADE_THREE_RANDOM);
						Create_BladeEffect(BLADE_THREE_RANDOM, vRandomDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

						vRandomDir = Random_Dir(vDir, 0.f, 0.f, 0.f, 360.f);

						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

						Create_AlertRect(BLADE_THREE_RANDOM);
						Create_BladeEffect(BLADE_THREE_RANDOM, vRandomDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);
					}
				}
			}
			break;
			case CBoss_Kyogai::PHASE_2:
			{
				if (3 == m_iEvent_Index)	// 1.0
				{
					_tchar szSoundFile[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
					Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_4, 0.2f);


					dLongLifeTime = 5.f;

					if (m_iAtkStepTypeNum == 1) // 일직선
					{
						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

						Create_AlertRect(BLADE_FIVE_FRONT, vMonsterDir);
						Create_BladeEffect(BLADE_FIVE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);
					}
					if (m_iAtkStepTypeNum == 2) // 삼각
					{
						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);
						Create_AlertRect(BLADE_FIVE_FRONT, vMonsterDir);
						Create_BladeEffect(BLADE_FIVE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

						vMonsterDir = Rotation_Dir(vMonsterDir, 10);
						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(1.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

						Create_AlertRect(BLADE_FIVE_FRONT, vMonsterDir);
						Create_BladeEffect(BLADE_FIVE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

						vMonsterDir = Rotation_Dir(vMonsterDir, -20);
						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(-1.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

						Create_AlertRect(BLADE_THREE_FRONT, vMonsterDir);
						Create_BladeEffect(BLADE_THREE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

					}

					if (m_iAtkStepTypeNum == 3) // 랜덤 두곳
					{
						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

						Create_AlertRect(BLADE_THREE_RANDOM);
						Create_BladeEffect(BLADE_THREE_RANDOM, vRandomDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

						vRandomDir = Random_Dir(vDir, 0.f, 0.f, 0.f, 360.f);

						Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
							CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

						Create_AlertRect(BLADE_THREE_RANDOM);
						Create_BladeEffect(BLADE_THREE_RANDOM, vRandomDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);
					}
				}
			}
			break;

			}
		}

		if (47 == m_pModelCom->Get_iCurrentAnimIndex())	// 뛰댕기기
		{
			if (0 == m_iEvent_Index)	// 0.07
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("swing_12.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_SIDESTEP, 0.4f);
			}
			else if (1 == m_iEvent_Index)	// 0.6
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("st_landing01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_LAND, 0.9f);
			}
		}

		if (ANIM_ATKSTEP_FRONT == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.4f;
				EffectWorldDesc.vPosition.z += 0.f;
				EffectWorldDesc.fScale = 1.f;

				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkStepB", m_pTransformCom, &EffectWorldDesc);

				_tchar szSoundFile[MAX_PATH] = TEXT("swingL_01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_SIDESTEP, 0.7f);
			}
			if (1 == m_iEvent_Index)	// 1.0
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("aura_02.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.2f);
				_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
				Play_Sound_Channel(szSoundFile2, CSoundMgr::MONSTER_EFFECT_2, 0.2f);

				dLongLifeTime = 5.f;

				if (m_iAtkStepTypeNum == 1) // 일직선
				{
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

					Create_AlertRect(BLADE_THREE_FRONT, vMonsterDir);
					Create_BladeEffect(BLADE_THREE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);
				}
				if (m_iAtkStepTypeNum == 2) // 삼각
				{
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);
					Create_AlertRect(BLADE_THREE_FRONT, vMonsterDir);
					Create_BladeEffect(BLADE_THREE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

					vMonsterDir = Rotation_Dir(vMonsterDir, 10);
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(1.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

					Create_AlertRect(BLADE_THREE_FRONT, vMonsterDir);
					Create_BladeEffect(BLADE_THREE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

					vMonsterDir = Rotation_Dir(vMonsterDir, -20);
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(-1.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);

					Create_AlertRect(BLADE_THREE_FRONT, vMonsterDir);
					Create_BladeEffect(BLADE_THREE_FRONT, vMonsterDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_DELAY_BULLET);
				}

				if (m_iAtkStepTypeNum == 3) // 랜덤 두곳
				{
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

					Create_AlertRect(BLADE_THREE_RANDOM);
					Create_BladeEffect(BLADE_THREE_RANDOM, vRandomDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

					vRandomDir = Random_Dir(vDir, 0.f, 0.f, 0.f, 360.f);

					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

					Create_AlertRect(BLADE_THREE_RANDOM);
					Create_BladeEffect(BLADE_THREE_RANDOM, vRandomDir, dLongLifeTime, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);
				}
			}
		}

		if (ANIM_ROOMCHANGE_READY == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Yeeeeah.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_SPECIAL, 0.7f);
			}
		}

		if (ANIM_ROOMCHANGE_START == m_pModelCom->Get_iCurrentAnimIndex())
		{
			CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
			EffectWorldDesc.vPosition.y += 1.f;
			EffectWorldDesc.fScale = 2.1f;
			dLongLifeTime = 3.0;

			if (0 == m_iEvent_Index)	// 0.0
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_Atk_26", m_pTransformCom, &EffectWorldDesc);
			}

			if (1 == m_iEvent_Index)	// 0.28
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_Atk_26", m_pTransformCom, &EffectWorldDesc);

			}

			if (2 == m_iEvent_Index)	// 0.56
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_Atk_26", m_pTransformCom, &EffectWorldDesc);

			}

			if (3 == m_iEvent_Index)	// 0.84
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_Atk_26", m_pTransformCom, &EffectWorldDesc);

			}
		}

		if (ANIM_SPIN_END == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0
			{
				Play_FallDownEffect();
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_Sound_BodyFall();
			}
		}

		if (66 == m_pModelCom->Get_iCurrentAnimIndex()) // DEAD
		{
			if (0 == m_iEvent_Index) // 1.85
			{
				Play_FallDownEffect();
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				_tchar szSoundFile[MAX_PATH] = TEXT("bodyfall_03.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_FALLDOWN, 0.7f);
			}
		}


		m_iEvent_Index++;
	}

	Safe_Release(pGameInstance);
}


void CBoss_Kyogai::Update_Hit_Messenger(_double dTimeDelta)
{
	if (m_bNoDmg == false /*&& m_bStart == true*/)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		_int PlayerIndex = CPlayerManager::GetInstance()->Get_PlayerIndex();
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), PlayerIndex));

		_float4 AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
		AtkDir.y = 0.f;

		_float3 vKyogaiOffset = { 0.f , 0.5f, 0.f };

		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small() || m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
		{
			if (m_bSuperArmor == false)
			{
				if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small())
				{
					m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
					Trigger_Hit_Small();
				}
				if (m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
				{
					m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
					Trigger_Hit_ConnectSmall();
				}
				if (true == m_isJumpOn)
					Jumping(0.2f, 0.030f);
			}
			else
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
			}
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

			if (PlayerIndex == 0) {
				Play_HitEffect(vKyogaiOffset);
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.4f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.0f;
				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.6f;

				int n = Random::Generate_Int(0, 2);

				if (n == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_Hit_Small_1", m_pTransformCom, &EffectWorldDesc);
				else if (n == 1)
					CEffectPlayer::Get_Instance()->Play("Zen_Hit_Small_2", m_pTransformCom, &EffectWorldDesc);
				else if (n == 2)
					CEffectPlayer::Get_Instance()->Play("Zen_Hit_Small_3", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}

			if(!m_bSuperArmor)
				Play_Sound_Dmg(0, 0.7f);
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow())
		{
			if (m_bSuperArmor == false)
			{
				m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
				Trigger_Hit_Blow();
			}
			else
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);


			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

			if (PlayerIndex == 0) {
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect(vKyogaiOffset);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.4f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.0f;
				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.f;
				EffectWorldDesc.vPosition.y += 0.4f;
				CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}

			if (!m_bSuperArmor)
				Play_Sound_Dmg(1, 0.9f);
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Upper())
		{
			if (m_bSuperArmor == false)
			{
				m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
				Trigger_Hit_Small();
			}
			else
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);

			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

			if (PlayerIndex == 0) {
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect(vKyogaiOffset);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.4f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.0f;
				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.f;
				EffectWorldDesc.vPosition.y += 0.4f;
				CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}

			if (!m_bSuperArmor)
				Play_Sound_Dmg(0, 0.7f);
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Big())
		{
			if (m_bSuperArmor == false)
			{
				m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
				Trigger_Hit_Blow();
			}
			else
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);

			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

			if (PlayerIndex == 0) {
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect(vKyogaiOffset);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.4f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.0f;
				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.f;
				EffectWorldDesc.vPosition.y += 0.4f;
				CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}

			if (!m_bSuperArmor)
				Play_Sound_Dmg(2, 0.7f);
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Bound())
		{
			if (m_bSuperArmor == false)
			{
				m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Bound(false);
				Trigger_Hit_Small();
			}
			else
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Bound(false);
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

			if (PlayerIndex == 0) {
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect(vKyogaiOffset);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.4f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.0f;
				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.f;
				EffectWorldDesc.vPosition.y += 0.4f;
				CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}

			//Set_FallingStatus(3.0f, 0.0f);

			if (!m_bSuperArmor)
				Play_Sound_Dmg(0, 0.9f);
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_CutScene())
		{
			m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
			Trigger_Hit_CutScene();
			pPlayer->Set_Hit_SurgeCutScene(true);
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

			if (PlayerIndex == 0) {
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect(vKyogaiOffset);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.f;
				EffectWorldDesc.vPosition.y += 0.4f;
				CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			if (!m_bSuperArmor)
				Play_Sound_Dmg(2, 0.7f);
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Hekireki())
		{
			if (m_bSuperArmor == false)
			{
				//m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
				Trigger_Hit_Hekireki();
			}
			else
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);

			pPlayer->Set_Hit_Success(true);
			pPlayer->Set_Hit_Success_Hekireki(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

			if (PlayerIndex == 0) {
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				Play_HitEffect(vKyogaiOffset);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.3f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.7f;
				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.f;
				EffectWorldDesc.vPosition.y += 0.4f;
				CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 1.f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}

			if (!m_bSuperArmor)
				Play_Sound_Dmg(2, 0.7f);
		}
		Safe_Release(pGameInstance);
	}
	else
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Spin(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Hekireki())
			m_pPlayer_Zenitsu->Set_Hit_Success_Hekireki(true);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);

	}
}

void CBoss_Kyogai::Update_AnimIndex(_uint iAnimIndex)
{
	if (m_eCurAnimIndex != m_ePreAnimIndex)
	{
		m_ePreAnimIndex = m_eCurAnimIndex;
	}
}

void CBoss_Kyogai::Update_Trigger(_double dTimeDelta)
{

	switch (m_eCurPhase)
	{
	case CBoss_Kyogai::BEGIN:
		Update_Begin(dTimeDelta);
		break;
	case CBoss_Kyogai::PHASE_1:
		Update_Phase_1(dTimeDelta);
		break;
	case CBoss_Kyogai::PHASE_2:
		Update_Phase_2(dTimeDelta);
		break;
	}

}

void CBoss_Kyogai::Update_State(_double dTimeDelta)
{

	switch (m_eCurstate)
	{
	case CBoss_Kyogai::STATE_INTERACT:
		Update_Interact(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_BEGIN:
		Update_Begin(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_NEXTPHASE:
		Update_NextPhase(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_AWAKE:
		Update_Awake(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_GUARD:
		Update_Guard(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_JUMP_STEP:
		Update_JumpStep(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKCMB:
		Update_AtkCmb(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKCMB2:
		Update_AtkCmb2(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKSTEP:
		Update_AtkStep(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_STOMPKICK:
		Update_StompKick(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKPUNCH:
		Update_AtkPunch(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_LINKERCMB:
		Update_LinkerCmb(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKSKCMB:
		Update_AtkSkCmb(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_AWAKE_ROOMCHANGE:
		Update_Awake_AtkskCmb(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKSK:
		Update_AtkSk(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKPANGPANG:
		Update_AtkPangPang(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_HIT_SMALL:
		Update_Hit_Small(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_HIT_BIG:
		Update_Hit_Big(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_HIT_BLOW:
		Update_Hit_Blow(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_GETUP:
		Update_Hit_GetUp(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_HIT_CUTSCENE:
		Update_Hit_CutScene(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_HIT_HEKIREKI:
		Update_Hit_Hekireki(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_HIT_DEAD:
		Update_Hit_Dead(dTimeDelta);
		break;

	}

}

void CBoss_Kyogai::Update_Interact(_double dTimeDelta)
{

	if (m_eCurPhase == PHASE_1)
	{
		Trigger_Interact_Phase_1(dTimeDelta);
	}
	if (m_eCurPhase == PHASE_2)
	{
		Trigger_Interact_Phase_2(dTimeDelta);
	}

}

void CBoss_Kyogai::Update_Begin(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_IDLE;
	_float fDistance = Calculate_Distance();
	if (CFadeManager::GetInstance()->Get_Is_House_Boss_Battle_Start())
	{
		m_dTriggerTime = 0.0;
		m_eCurPhase = PHASE_1;
		m_iTriggerCnt = 1;
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();

		CMonsterManager::GetInstance()->Set_BattleOn(true);
		CMonsterManager::GetInstance()->Set_Kyogai_On(true);
	}
}

void CBoss_Kyogai::Update_Phase_1(_double dTimeDelta)
{
	if (m_StatusDesc.fHp <= 0.f && m_bNoDmg == false)
	{
		m_StatusDesc.fHp = 0.f;
		m_bTrigger = false;
		m_bPatternStart = false;
		m_bNoDmg = true;
		m_iTriggerCnt = 8;
		m_dTriggerTime = 0.0;

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		pGameInstance->Time_Slow(1.0, 0.1);
		Safe_Release(pGameInstance);
	}
	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) <= 0.5f && m_bFirstAwake == false)
	{
		m_bFirstAwake = true;
		m_bTrigger = false;
		m_iTriggerCnt = 4;
		m_dTriggerTime = 0.0;

	}

	if (m_bTrigger == false)
	{

		switch (m_iTriggerCnt)
		{
		case 1:
			Trigger_Interact();
			break;
		case 2:
			Trigger_AtkStep();
			break;
		case 3:
			Trigger_JumpStep();
			break;
		case 4:
			Trigger_AtkSkCmb();
			break;
		case 5:
			Trigger_AtkPunch();
			break;
		case 6:
			Trigger_StompKick();
			break;
		case 7:
			Trigger_LinkerCmb();
			break;
		case 8:
			Trigger_NextPhase();
			break;

		}

	}

}

void CBoss_Kyogai::Update_Phase_2(_double dTimeDelta)
{
	// 개방하고 25초 유지 m_bAwake가 활성되면 25초 후에 다시 false
	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) >= 1.f && m_bSecondAwake == false)
	{
		m_bFirstAwake = false;
		m_bSecondAwake = true;
		m_bAwake = true;
		m_bTrigger = false;
		m_iTriggerCnt = 1;
		m_dAwakeTime = 0.0;

		m_bPatternStart = false;
		m_dTriggerTime = 0.0;
	}
	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) <= 0.3f && m_bFirstAwake == false)
	{
		m_bFirstAwake = true;
		m_bTrigger = false;
		m_bPatternStart = false;

		m_iTriggerCnt = 9;
		m_dTriggerTime = 0.0;

	}
	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) <= 0.0f)
	{
		m_bNoDmg = true;
		m_StatusDesc.fHp = 0.f;
		if (m_bDead_Trigger == false)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			pGameInstance->Time_Slow(1.0, 0.1);
			Safe_Release(pGameInstance);

			m_bDead_Trigger = true;
			Trigger_Hit_Dead();
		}
	}

	if (m_bAwake == true)
	{
		m_dAwakeTime += dTimeDelta;
		if (65.5 < m_dAwakeTime && m_dAwakeTime <= 65.5 + dTimeDelta)
			m_pRendererCom->Set_Invert();
		if (m_dAwakeTime > 66.0)
		{
			m_dAwakeTime = 0.0;
			m_pRendererCom->Set_Invert();
			m_bAwake = false;

			m_isAuroraOn[0] = false;
		}
	}

	if (m_bTrigger == false)
	{
		if (m_bAwake == false)
		{
			switch (m_iTriggerCnt)
			{
			case 1:
				Trigger_Interact();
				break;
			case 2:
				Trigger_AtkStep();
				break;
			case 3:
				Trigger_JumpStep();
				break;
			case 4:
				Trigger_AtkSk();
				break;
			case 5:
				Trigger_AtkPunch();
				break;
			case 6:
				Trigger_StompKick();
				break;
			case 7:
				Trigger_AtkCmb2();
				break;
			case 8:
				Trigger_LinkerCmb();
				break;
			case 9:
				Trigger_AtkSkCmb();
				break;
			}
		}
		if (m_bAwake == true)
		{

			switch (m_iTriggerCnt)
			{
			case 1:
				Trigger_Awake();
				break;
			case 2:
				Trigger_Awake_AtkskCmb();
				break;
			}
		}

	}
}

void CBoss_Kyogai::Trigger_Interact_Phase_1(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_IDLE;

	if (m_bPatternStart == false)
	{
		m_dTriggerTime += dTimeDelta;
		if (Event_Time(dTimeDelta, 0.2, m_dTriggerTime))
		{
			m_dTriggerTime = 0.0;
			m_bTrigger = false;
			m_iTriggerCnt++;

			if (m_iTriggerCnt >= 8)
			{
				m_iTriggerCnt = 1;
				m_bPatternStart = true;
			}
		}

	}
	if (m_bPatternStart == true)
	{
		_float fDistance = Calculate_Distance();
		m_dTriggerTime += dTimeDelta;
		if (Event_Time(dTimeDelta, 0.2, m_dTriggerTime))
		{
			m_dTriggerTime = 0.0;
			m_bTrigger = false;
			m_iRandomPatternNum = Random::Generate_Int(2, 7);

			if (fDistance > 7.f) //멀 때
			{
				m_iTriggerCnt = 3;

			}
			if (fDistance <= 7.f) // 가까울 때
			{
				if (m_iRandomPatternNum == 2)
				{
					m_iTriggerCnt = 2;
				}
				if (m_iRandomPatternNum == 4)
				{
					m_iTriggerCnt = 5;
				}
				if (m_iRandomPatternNum == 5)
				{
					m_iTriggerCnt = 6;
				}
				if (m_iRandomPatternNum == 6)
				{
					m_iTriggerCnt = 7;
				}
			}
		}

	}




}

void CBoss_Kyogai::Trigger_Interact_Phase_2(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_IDLE;

	if (m_bAwake == false)
	{
		if (m_bPatternStart == false)
		{
			m_dTriggerTime += dTimeDelta;
			if (Event_Time(dTimeDelta, 0.2, m_dTriggerTime))
			{
				m_dTriggerTime = 0.0;
				m_bTrigger = false;
				m_iTriggerCnt++;

				if (m_iTriggerCnt >= 9)
				{
					m_iTriggerCnt = 1;
					m_dTriggerTime = 0.0;
					m_bPatternStart = true;
				}
			}
		}
		if (m_bPatternStart == true)
		{
			_float fDistance = Calculate_Distance();
			m_dTriggerTime += dTimeDelta;
			if (Event_Time(dTimeDelta, 0.20, m_dTriggerTime))
			{
				m_dTriggerTime = 0.0;
				m_bTrigger = false;

				m_iRandomPatternNum = Random::Generate_Int(2, 8);

				if (fDistance > 7.f) //멀 때
				{
					m_iTriggerCnt = 3;
				}
				if (fDistance <= 7.f) // 가까울 때
				{
					if (m_iRandomPatternNum == 2)
					{
						m_iTriggerCnt = 2;
					}
					if (m_iRandomPatternNum == 3)
					{
						m_iTriggerCnt = 4;
					}
					if (m_iRandomPatternNum == 4)
					{
						m_iTriggerCnt = 5;
					}
					if (m_iRandomPatternNum == 5)
					{
						m_iTriggerCnt = 6;
					}
					if (m_iRandomPatternNum == 6)
					{
						m_iTriggerCnt = 7;
					}
					if (m_iRandomPatternNum == 7)
					{
						m_iTriggerCnt = 8;
					}
				}
			}

		}
	}

}




void CBoss_Kyogai::Trigger_Interact()
{
	m_bTrigger = true;
	m_bSuperArmor = false;
	m_bNoDmg = false;
	m_bHit = false;
	m_dTriggerTime = 0.0;
	m_pRendererCom->Set_BloomRatio();
	m_eCurstate = STATE_INTERACT;
}

void CBoss_Kyogai::Trigger_Begin()
{
	m_bTrigger = true;
	m_eCurstate = STATE_INTERACT;
}


void CBoss_Kyogai::Trigger_Guard()
{
	m_bTrigger = true;
	m_eCurstate = STATE_INTERACT;
}

void CBoss_Kyogai::Trigger_PushAway()
{
	m_bTrigger = true;
	m_eCurstate = STATE_INTERACT;
}

void CBoss_Kyogai::Trigger_NextPhase()
{
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_dTimeAcc = 0.0;
	m_eCurstate = STATE_NEXTPHASE;
}

void CBoss_Kyogai::Trigger_Heal()
{
	m_bTrigger = true;
	m_eCurstate = STATE_INTERACT;
}

void CBoss_Kyogai::Trigger_Awake()
{
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_eCurstate = STATE_AWAKE;
}

void CBoss_Kyogai::Trigger_JumpStep()
{
	m_vDir = Calculate_Dir_FixY();
	m_vDir = Random_Dir(m_vDir, 0.f, 0.f, -50.f, 50.f);

	m_pTransformCom->LerpVector(m_vDir, 1.0f);

	m_bTrigger = true;
	m_bSuperArmor = true;
	m_bAnimFinish = false;
	m_eCurstate = STATE_JUMP_STEP;

}

void CBoss_Kyogai::Trigger_AtkCmb()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKCMB;
	m_bAnimFinish = false;
	m_bSuperArmor = true;
}

void CBoss_Kyogai::Trigger_AtkCmb2()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKCMB2;
	m_bAnimFinish = false;
	m_bSuperArmor = true;
	m_iLoopCount = 0;
}

void CBoss_Kyogai::Trigger_AtkStep()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKSTEP;
	m_bAnimFinish = false;
	m_bSuperArmor = true;

	if (m_bAtkStepType == false)
		m_iAtkStepTypeNum++;

	if (m_iAtkStepTypeNum > 3)
		m_bAtkStepType = true;

	if (m_bAtkStepType == true)
		m_iAtkStepTypeNum = Random::Generate_Int(1, 3);

	if (Check_Distance(2.f) == true)
		m_iRandomDirNum = 0;
	else
		m_iRandomDirNum = 1;
}

void CBoss_Kyogai::Trigger_StompKick()
{
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_bTrigger = true;
	m_eCurstate = STATE_STOMPKICK;
	m_bAnimFinish = false;
	m_bSuperArmor = true;
	m_bMove = false;
}

void CBoss_Kyogai::Trigger_AtkPunch()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKPUNCH;
	m_bAnimFinish = false;
	m_bSuperArmor = true;
}

void CBoss_Kyogai::Trigger_LinkerCmb()
{
	m_vDir = Calculate_Dir_FixY();
	m_vDir = Random_Dir(m_vDir, 0.f, 0.f, -50.f, 50.f);

	m_pTransformCom->LerpVector(m_vDir, 1.0f);

	m_bTrigger = true;
	m_eCurstate = STATE_LINKERCMB;
	m_bAnimFinish = false;
	m_bSuperArmor = true;
	m_bMove = false;
	m_iLinkerNum = Random::Generate_Int(1, 2);
}

void CBoss_Kyogai::Trigger_AtkSkCmb()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKSKCMB;
	m_bAnimFinish = false;
	m_bAnimFinish2 = false;
	m_bAnimFinish3 = false;
	m_bSuperArmor = true;
	m_bTurn = false;

	m_iLoopCount = 0;
	m_dTurnTime = 0.0;
	m_dReturnTime = 0.0;
	m_dTimeAcc = 0.0;
}

void CBoss_Kyogai::Trigger_Awake_AtkskCmb()
{
	m_bTrigger = true;
	m_eCurstate = STATE_AWAKE_ROOMCHANGE;
	m_bAnimFinish = false;
	m_bSuperArmor = true;
	m_dTurnTime = 0.0;
	m_dTimeAcc = 0.0;
}

void CBoss_Kyogai::Trigger_AtkSk()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKSK;
	m_bAnimFinish = false;
	m_iLoopCount = 0;
}

void CBoss_Kyogai::Trigger_AtkPangPang()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKPANGPANG;
	m_bAnimFinish = false;
	m_bSuperArmor = true;
}

void CBoss_Kyogai::Trigger_Hit_Small()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
	m_isConnectHitting = false;
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_iSmallHit_Index++;
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_BASETURN_L);
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_BASETURN_R);
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_HIT);
	m_pModelCom->Get_AnimFinish(ANIM_BASETURN_L);
	m_pModelCom->Get_AnimFinish(ANIM_BASETURN_R);

	m_eCurstate = STATE_HIT_SMALL;
}

void CBoss_Kyogai::Trigger_Hit_ConnectSmall()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
	m_isConnectHitting = true;
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_iSmallHit_Index++;
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_BASETURN_L);
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_BASETURN_R);
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_HIT);
	m_pModelCom->Get_AnimFinish(ANIM_BASETURN_L);
	m_pModelCom->Get_AnimFinish(ANIM_BASETURN_R);
	m_eCurstate = STATE_HIT_SMALL;
}

void CBoss_Kyogai::Trigger_Hit_Upper()
{
}

void CBoss_Kyogai::Trigger_Hit_Big()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_eCurstate = STATE_HIT_BIG;
}

void CBoss_Kyogai::Trigger_Hit_Blow()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_pModelCom->Set_AnimisFinish(ANIM_SPIN);
	m_pModelCom->Set_AnimisFinish(ANIM_SPIN_LOOP);
	m_pModelCom->Set_AnimisFinish((ANIM_SPIN_END));
	m_eCurstate = STATE_HIT_BLOW;
}

void CBoss_Kyogai::Trigger_Hit_CutScene()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_CutScene(false);
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_bSuperArmor = false;
	m_bNoDmg = true;

	m_eCurstate = STATE_HIT_CUTSCENE;
}

void CBoss_Kyogai::Trigger_Hit_GetUp()
{
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_bNoDmg = true;
	m_bAir_Motion = false;

	m_eCurstate = STATE_GETUP;
}

void CBoss_Kyogai::Trigger_Hit_Hekireki()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_dTimeAcc = 0.0;
	m_eCurstate = STATE_HIT_HEKIREKI;
}

void CBoss_Kyogai::Trigger_Hit_Dead()
{
	m_pModelCom->Set_AnimisFinish(ANIM_DEATH);
	m_pModelCom->Set_AnimisFinish(ANIM_DEATH_GETUP);

	m_bNoDmg = true;
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_bMonsterDead = false;
	m_bStopAnim = false;
	m_fDeadTime = 0.0F;

	Play_Sound_Dmg(2, 0.7f);

	m_eCurstate = STATE_HIT_DEAD;
}

void CBoss_Kyogai::Update_Guard(_double dTimeDelta)
{

}


void CBoss_Kyogai::Update_NextPhase(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_PUSHAWAY;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_PUSHAWAY))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_PUSHAWAY);
		m_eCurAnimIndex = ANIM_HIT_DOWN_0;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_DOWN_0))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_DOWN_0);
		m_eCurAnimIndex = ANIM_HIT_DOWN_1;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_DOWN_1))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_DOWN_1);
		m_eCurAnimIndex = ANIM_HIT_DOWN_2;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_DOWN_2))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_DOWN_2);
		m_eCurAnimIndex = ANIM_HIT_RETURN;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_RETURN))
	{
		m_eCurAnimIndex = ANIM_IDLE;

		m_dTimeAcc += dTimeDelta;
		if (m_dTimeAcc > 2.0)
		{
			m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN);
			m_eCurPhase = PHASE_2;
			m_iTriggerCnt = 1;
			Trigger_Interact();
			CFadeManager::GetInstance()->Set_Is_House_Boss_Battle_Start(true);

			CSoundMgr::Get_Instance()->StopSound(CSoundMgr::BGM);
			_tchar szBgm[MAX_PATH] = TEXT("BGM_House_Boss_1.mp3");
			CSoundMgr::Get_Instance()->PlayBGM(szBgm, 0.6f);
		}

		if (m_StatusDesc.fHp <= m_StatusDesc.fHp_Max)
		{
			if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) >= 0.97)
				m_StatusDesc.fHp = m_StatusDesc.fHp_Max;
			else
				m_StatusDesc.fHp = m_StatusDesc.fHp_Max * (_float)m_dTimeAcc * 0.5f;
		}
	}
}

void CBoss_Kyogai::Update_Awake(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_PUSHAWAY;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_PUSHAWAY))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_PUSHAWAY);
		m_eCurAnimIndex = ANIM_AWAKE;
		CCameraManager::GetInstance()->Set_Is_Cut_In_On(true);
		CCameraManager::GetInstance()->Set_Cut_In_Finish_Type(CCamera_Free::KYOGAI_AWAKE);
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_AWAKE);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Awake_AtkskCmb();
	}
	Pos_FixY();
}

void CBoss_Kyogai::Update_JumpStep(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_STEP_FRONT2;
		Jumping(1.5f, 0.065f);
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_STEP_FRONT2))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_STEP_FRONT2);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
	if (m_pModelCom->Get_AnimRatio(ANIM_STEP_FRONT2, 0.01) && !m_pModelCom->Get_AnimRatio(ANIM_STEP_FRONT2, 0.538))
	{
		Go_Dir_Constant(dTimeDelta, ANIM_STEP_FRONT2, 3.f, Convert::ToFloat4(m_vDir));
	}

	_vector vDir = Calculate_Dir_FixY();
	m_pTransformCom->LerpVector(vDir, 0.3f);
}

void CBoss_Kyogai::Update_AtkCmb(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_ATKCMB1;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB1))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB1);
		m_eCurAnimIndex = ANIM_ATKCMB2;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB2))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB2);
		m_eCurAnimIndex = ANIM_ATKCMB3;
		m_bSuperArmor = false;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB3))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB3);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
}

void CBoss_Kyogai::Update_AtkCmb2(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_ATKCMB_01READY;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB_01READY))
	{

		m_eCurAnimIndex = ANIM_ATKCMB_02LOOP;
	}
	if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKCMB_02LOOP, 0.95, dTimeDelta))
		m_iLoopCount++;

	if (m_iLoopCount >= 4)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB_01READY);

		if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB_02LOOP))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB_02LOOP);
			m_eCurAnimIndex = ANIM_ATKCMB_03END;
		}

		if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB_03END))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB_03END);
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
		}
	}
}

void CBoss_Kyogai::Update_AtkStep(_double dTimeDelta)
{
	if (m_iRandomDirNum == 1)
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_ATKSTEP_BACK;
			Jumping(1.f, 0.03f);
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_ATKSTEP_BACK))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKSTEP_BACK);
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
		}
		if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKSTEP_BACK, 0.32, dTimeDelta))
		{
			_vector vDir = Calculate_Dir_FixY();
			m_pTransformCom->LerpVector(vDir, 0.9f);
			JumpStop(0.4);
		}
	}
	else
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_ATKSTEP_FRONT;
			Jumping(1.f, 0.03f);
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_ATKSTEP_FRONT))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKSTEP_FRONT);
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
		}
		if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKSTEP_FRONT, 0.32, dTimeDelta))
		{
			_vector vDir = Calculate_Dir_FixY();
			m_pTransformCom->LerpVector(vDir, 0.9f);
			JumpStop(0.4);
		}
	}
	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_ATKSTEP_BACK, 3.f, 0.01, 0.16);
	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_ATKSTEP_FRONT, 3.f, 0.01, 0.16);
}

void CBoss_Kyogai::Update_StompKick(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_STOMPKICK;
	}

	if (Check_Player_Y() == true)
	{
		if (m_pModelCom->Check_PickAnimRatio(ANIM_STOMPKICK, 0.87, dTimeDelta))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_STOMPKICK);
			m_eCurAnimIndex = ANIM_IDLE;

			Trigger_AtkPangPang();
		}
	}
	else
	{
		if (m_pModelCom->Get_AnimFinish(ANIM_STOMPKICK))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_STOMPKICK);
			m_eCurAnimIndex = ANIM_IDLE;

			Trigger_Interact();
		}
	}
	if (m_pModelCom->Check_PickAnimRatio(ANIM_STOMPKICK, 0.380, dTimeDelta))
	{
		m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		Jumping(1.f, 0.06f);
	}
	if (Check_Distance_FixY(2.f) == true)
		m_bMove = true;

	if (Check_Distance_FixY(2.f) == false && m_bMove == false)
		Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_STOMPKICK, 1.f, 0.38, 0.56);
}

void CBoss_Kyogai::Update_AtkPunch(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_ATKPUNCH;
	}
	//if (m_pModelCom->Get_AnimFinish(ANIM_ATKPUNCH))
	if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKPUNCH, 0.70, dTimeDelta))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKPUNCH);
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_ATKPUNCH);
		m_eCurAnimIndex = ANIM_ATKPUNCH2;

	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKPUNCH2))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKPUNCH2);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}

	_vector vDir = Calculate_Dir_FixY();
	m_pTransformCom->LerpVector(vDir, 0.7f);
	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_ATKPUNCH, 1.5f, 0.50, 0.60);
}

void CBoss_Kyogai::Update_LinkerCmb(_double dTimeDelta)
{
	//거리가 멀다면 스텝으로 다가가 주먹 발차기 스텝북
	if (Check_Distance_FixY(2.f) == true)
		m_bMove = true;

	if (false == Check_Distance_FixY(2.f) && false == m_bMove)
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_STEP_FRONT2;
			Jumping(1.5f, 0.065f);
		}
		if (m_pModelCom->Get_AnimRatio(ANIM_STEP_FRONT2, 0.01) && !m_pModelCom->Get_AnimRatio(ANIM_STEP_FRONT2, 0.538))
		{
			Go_Dir_Constant(dTimeDelta, ANIM_STEP_FRONT2, 3.f, Convert::ToFloat4(m_vDir));
		}

		_vector vDir = Calculate_Dir_FixY();
		m_pTransformCom->LerpVector(vDir, 0.3f);
	}
	//if(m_bMove == true)
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_ATKPUNCH;
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_STEP_FRONT2))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_STEP_FRONT2);
			m_eCurAnimIndex = ANIM_ATKPUNCH;

		}
		if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKPUNCH, 0.70, dTimeDelta))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKPUNCH);
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_ATKPUNCH);
			m_eCurAnimIndex = ANIM_ATKPUNCH2;

		}

		if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKPUNCH2, 0.540, dTimeDelta))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKPUNCH2);
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_ATKPUNCH2);
			if (m_iLinkerNum == 1)
			{
				m_eCurAnimIndex = ANIM_KICKDOWN;

				CGameInstance* pGameInstance = CGameInstance::GetInstance();
				Safe_AddRef(pGameInstance);

				CAlertCircle::EFFECTDESC EffectDesc;
				EffectDesc.pOwnerTransform = m_pTransformCom;
				EffectDesc.iType = CAlertCircle::TYPE_KICKDOWN;

				pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertCircle"), &EffectDesc, false);
				Safe_Release(pGameInstance);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.7f;
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_11", m_pTransformCom, &EffectWorldDesc);
			}
			if (m_iLinkerNum == 2)
			{
				m_eCurAnimIndex = ANIM_ATKCMBW05;
			}
		}

		//if (m_pModelCom->Get_AnimFinish(ANIM_STOMPKICK))
		if (m_pModelCom->Check_PickAnimRatio(ANIM_KICKDOWN, 0.850, dTimeDelta) || m_pModelCom->Check_PickAnimRatio(ANIM_ATKCMBW05, 0.850, dTimeDelta))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_KICKDOWN);
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_KICKDOWN);
			m_pModelCom->Set_AnimisFinish(ANIM_ATKCMBW05);
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_ATKCMBW05);
			Trigger_AtkStep();
		}

		if (m_pModelCom->Check_PickAnimRatio(ANIM_KICKDOWN, 0.130, dTimeDelta))
		{
			m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
			Jumping(0.9f, 0.06f);
		}

		_vector vDir = Calculate_Dir_FixY();
		m_pTransformCom->LerpVector(vDir, 0.7f);
		Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_ATKPUNCH, 1.5f, 0.50, 0.60);
		Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_KICKDOWN, 1.2f, 0.13, 0.39);
	}
}

void CBoss_Kyogai::Update_AtkSkCmb(_double dTimeDelta)
{
	switch (m_eCurPhase)
	{
	case CBoss_Kyogai::PHASE_1:
		Update_RoomChange(dTimeDelta);
		break;
	case CBoss_Kyogai::PHASE_2:
		Update_RoomChange_2(dTimeDelta);
		break;
	}
	CMonsterManager::GetInstance()->Set_RoomTurn(m_bTurn);
}

void CBoss_Kyogai::Update_Awake_AtkskCmb(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_ROOMCHANGE_READY;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ROOMCHANGE_READY))
	{
		m_bTurn = true;
		m_dTurnTime += dTimeDelta;
		if (m_dTurnTime < 56.5)
		{
			m_eCurAnimIndex = ANIM_ROOMCHANGE_START;
			m_bFastBook = true;
		}

		if (m_dTurnTime >= 56.5)
		{
			if (m_pModelCom->Get_AnimFinish(ANIM_ROOMCHANGE_START))
			{
				m_bTurn = false;
				m_pModelCom->Set_AnimisFinish(ANIM_ROOMCHANGE_READY);
				m_pModelCom->Set_AnimisFinish(ANIM_ROOMCHANGE_START);
				m_eCurAnimIndex = ANIM_ROOMCHANGE_END;
				m_bFastBook = false;
			}
		}
		CMonsterManager::GetInstance()->Set_RoomTurn(m_bTurn);
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_ROOMCHANGE_END))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ROOMCHANGE_END);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_AtkSk();
		CCameraManager::GetInstance()->Set_Is_Battle_LockFree(false);
	}

	if (Event_Time(dTimeDelta, 0.5, m_dTurnTime)) // 맨처음
	{
		m_StatusDesc.fHp -= m_StatusDesc.fHp_Max * 0.05f;
		m_bTurnRB = true;
	}
	if (Event_Time(dTimeDelta, 6.0, m_dTurnTime)) // 첫방
	{
		m_StatusDesc.fHp -= m_StatusDesc.fHp_Max * 0.05f;
		m_bTurnRB = true;
	}
	if (Event_Time(dTimeDelta, 11.5, m_dTurnTime))
	{
		m_StatusDesc.fHp -= m_StatusDesc.fHp_Max * 0.05f;
		m_bTurnRF = true;
	}

	if (Event_Time(dTimeDelta, 16.5, m_dTurnTime))
	{
		Set_CharacterDialog(2.f, TEXT("[카마도 탄지로]"), TEXT("(으아아아악!)"));
		_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_45_Tanjiro.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
	}

	if (Event_Time(dTimeDelta, 19.5, m_dTurnTime))
	{
		Set_CharacterDialog(8.f, TEXT("[카마도 탄지로]"), TEXT("(엄청난 회전 속도야!! 손톱의 개수가 늘었어!!"), TEXT("큰일이다, 엄청난 기술이야!)"));
		_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_46_Tanjiro.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
	}

	if (Event_Time(dTimeDelta, 28.0, m_dTurnTime))
	{
		Set_CharacterDialog(8.f, TEXT("[쿄우가이]"), TEXT("끈질긴 버러지가! 잘도 피하는군!"));
		_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_27.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
	}


	if (Event_Time(dTimeDelta, 24.0, m_dTurnTime))
	{
		m_StatusDesc.fHp -= m_StatusDesc.fHp_Max * 0.05f;
		m_bTurnLF = true;
	}
	if (Event_Time(dTimeDelta, 29.5, m_dTurnTime))
	{
		m_StatusDesc.fHp -= m_StatusDesc.fHp_Max * 0.05f;
		m_bTurnRB = true;
	}
	if (Event_Time(dTimeDelta, 35.0, m_dTurnTime))
	{
		m_StatusDesc.fHp -= m_StatusDesc.fHp_Max * 0.05f;
		m_bTurnRF = true;
	}
	if (Event_Time(dTimeDelta, 40.5, m_dTurnTime))
	{
		m_StatusDesc.fHp -= m_StatusDesc.fHp_Max * 0.05f;
		m_bTurnLF = true;
	}
	if (Event_Time(dTimeDelta, 53.0, m_dTurnTime)) // 맨 마지막
	{
		m_bTurnRB = true;
	}




	_vector vMonsterLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vMonstervRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	Rotation_Bullet(dTimeDelta, 4.0, vMonsterLook, CAtkCollider::TYPE_KYOGAI_BULLET);
	Grid_Bullet(dTimeDelta, 9.5, vMonsterLook);
	Liar_Bullet(dTimeDelta, 15.0, vMonsterLook);
	Rotation_Bullet(dTimeDelta, 27.5, vMonsterLook, CAtkCollider::TYPE_KYOGAI_BULLET);
	Grid_Bullet(dTimeDelta, 33.0, vMonsterLook);
	Grid_Bullet(dTimeDelta, 38.5, vMonstervRight); // 이 룩이 문제됨 
	//Liar_Bullet(dTimeDelta, 44.0, vMonsterLook);
	Wave_Bullet(dTimeDelta, 44.0, vMonsterLook);

	Turn_Trigger(dTimeDelta);
	TurnRoom();


}

void CBoss_Kyogai::Update_AtkSk(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_ATKCMB3;
	}
	if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKCMB3, 0.95, dTimeDelta))
		m_iLoopCount++;
	if (m_iLoopCount >= 2)
	{
		if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB3))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB3);
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
		}

	}
}

void CBoss_Kyogai::Update_AtkPangPang(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_ATKCMBW05;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMBW05))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMBW05);
		m_eCurAnimIndex = ANIM_IDLE;

		Trigger_Interact();
	}
}


void CBoss_Kyogai::Update_Hit_Small(_double dTimeDelta)
{

	if (m_isJumpOn == false && m_bAir_Motion == false)
	{
		if (false == m_bAnimFinish)
		{
			if (m_iSmallHit_Index == 1)
				m_eCurAnimIndex = ANIM_BASETURN_L;

			if (m_iSmallHit_Index == 2)
				m_eCurAnimIndex = ANIM_BASETURN_R;

			if (m_iSmallHit_Index == 3)
				m_eCurAnimIndex = ANIM_HIT;

			if (m_iSmallHit_Index >= 4)
				m_iSmallHit_Index = 1;



		}
		if ((m_pModelCom->Get_AnimFinish(ANIM_BASETURN_L) || m_pModelCom->Get_AnimFinish(ANIM_BASETURN_R) || m_pModelCom->Get_AnimFinish(ANIM_HIT)))
		{
			m_bAnimFinish = true;
			m_pModelCom->Set_AnimisFinish(ANIM_BASETURN_L);
			m_pModelCom->Set_AnimisFinish(ANIM_BASETURN_R);
			m_pModelCom->Set_AnimisFinish(ANIM_HIT);
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
			//m_eCurAnimIndex = ANIM_BASETURN_L;
		}
	}
	if (m_isConnectHitting == false)
	{
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_BASETURN_L, 0.3f, 0.0, 0.7);
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_BASETURN_R, 0.3f, 0.0, 0.7);
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT, 0.3f, 0.0, 0.7);

	}


	/*if (m_pModelCom->Get_AnimFinish(ANIM_HIT_BLOW_END))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_END);
		m_eCurAnimIndex = ANIM_HIT_GETUP_DIZZY;
		Trigger_Hit_GetUp();
	}*/
}

void CBoss_Kyogai::Update_Hit_Upper(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_Big(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_HIT;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}

	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT, 1.2f, 0.01, 0.85);
}

void CBoss_Kyogai::Update_Hit_Blow(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_SPIN;
		Jumping(1.0f, 0.05f);
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_SPIN))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_SPIN);
		m_eCurAnimIndex = ANIM_SPIN_LOOP;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_SPIN_END))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_SPIN_END);
		m_eCurAnimIndex = ANIM_GETUP_SPIN;
		Trigger_Hit_GetUp();
	}

	Land_Anim_Play(ANIM_SPIN_LOOP, ANIM_SPIN_END);

	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_SPIN, 3.0f);
	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_SPIN_LOOP, 3.0f);
	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_SPIN_END, 1.0f, 0.0, 0.3);
}

void CBoss_Kyogai::Update_Hit_BigBlow(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_Bound(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_Spin(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_CutScene(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_DEATH;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_DEATH))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_DEATH);
		m_eCurAnimIndex = ANIM_DEATH_GETUP;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_DEATH_GETUP))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_DEATH_GETUP);
		m_bNoDmg = false;
		m_eCurAnimIndex = ANIM_STEP_FRONT2;
		Trigger_JumpStep();
	}

}

void CBoss_Kyogai::Update_Hit_GetUp(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_GETUP_SPIN;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_GETUP_SPIN))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_GETUP_SPIN);
		m_eCurAnimIndex = ANIM_STEP_FRONT2;
		Trigger_JumpStep();
	}
	Pos_FixY();

}

void CBoss_Kyogai::Update_Hit_RollGetUp(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_BigGetUp(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_Hekireki(_double dTimeDelta)
{
	m_dTimeAcc += dTimeDelta;
	if (m_dTimeAcc > 0.150)
	{

		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_SPIN;
			if (m_isJumpOn == false)
				Jumping(1.85f, 0.03f);
			else
				Jumping(0.85f, 0.03f);
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_SPIN))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_SPIN);
			m_eCurAnimIndex = ANIM_SPIN_LOOP;
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_SPIN_END))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_SPIN_END);
			m_eCurAnimIndex = ANIM_GETUP_SPIN;
			Trigger_Hit_GetUp();
		}
	}
	if (m_dTimeAcc >= 0.150)
		Land_Anim_Play(ANIM_SPIN_LOOP, ANIM_SPIN_END);

}

void CBoss_Kyogai::Update_Hit_Dead(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_DEATH;
	}

	if (m_pModelCom->Check_PickAnimRatio(ANIM_DEATH, 0.90, dTimeDelta))
	{
		m_bStopAnim = true;
	}

	if (m_bStopAnim == true)
	{
		m_bMonsterDead = true;
		m_fDeadTime += (_float)dTimeDelta;
		m_dDeadParticleAccTime += dTimeDelta;
		m_dDeadSmokeAccTime += dTimeDelta;

		if (Event_Time(dTimeDelta, 4.f, m_fDeadTime))
		{
			Set_CharacterDialog(3.f, TEXT("[쿄우가이]"), TEXT("꼬마... 대답해라....."));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_58.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 7.5f, m_fDeadTime))
		{
			Set_CharacterDialog(6.f, TEXT("[쿄우가이]"), TEXT("소생의 혈귀술은 대단한가?"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_59.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 14.f, m_fDeadTime))
		{
			Set_CharacterDialog(2.f, TEXT("[카마도 탄지로]"), TEXT("....대단했어"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_60_Tanjiro.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 16.5f, m_fDeadTime))
		{
			Set_CharacterDialog(4.f, TEXT("[카마도 탄지로]"), TEXT("하지만 사람을 죽인 일은 용서 못 해"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_61_Tanjiro.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 21.0f, m_fDeadTime))
		{
			Set_CharacterDialog(2.f, TEXT("[쿄우가이]"), TEXT(".....그런가"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_63.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}

		if (m_fDeadTime > 1.8f && m_fDeadTime < 22.5f)
		{
			if (m_fDeadTime > 2.2f)
			{
				if (m_dDeadParticleAccTime > 2.0)
				{
					m_dDeadParticleAccTime = 0.0;
					CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
					EffectWorldDesc.vPosition.x += Random::Generate_Float(-0.5f, 0.5f);
					EffectWorldDesc.vPosition.z += Random::Generate_Float(-0.5f, 0.5f);
					CEffectPlayer::Get_Instance()->Play("Death_Particle", m_pTransformCom, &EffectWorldDesc);
				}
			}

			if (m_dDeadSmokeAccTime > 1.0)
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_DEAD);
				m_dDeadSmokeAccTime = 0.0;
			}
		}

		if (m_fDeadTime > 25.f) // 죽는 시간도 형이 조절해도 됨
			m_isDead = true;

	}
}

void CBoss_Kyogai::Update_RoomChange(_double dTimeDelta)
{
	if (m_bTurn == false)
	{
		if (Check_Distance_FixY(3.f) == true)
		{
			if (m_bAnimFinish == false)
			{
				Pos_FixY();
				m_bAnimFinish = true;
				m_eCurAnimIndex = ANIM_PUSHAWAY;
			}
		}
		else
		{
			if (m_bAnimFinish == false)
			{
				m_bAnimFinish = true;
				m_eCurAnimIndex = ANIM_ATKSK_READY;
			}
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_PUSHAWAY))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_PUSHAWAY);
			m_eCurAnimIndex = ANIM_ATKSK_READY;
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_ATKSK_READY))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKSK_READY);
			m_eCurAnimIndex = ANIM_ATKSK_LOOP;
		}

		if (m_pModelCom->Get_AnimFinish(ANIM_ATKSK_LOOP) && m_bTurn == false)
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKSK_LOOP);
			m_eCurAnimIndex = ANIM_ATKSK_RF;
			m_bTurn = true;
			m_bTurnRF = true;
			m_StatusDesc.fHp -= m_StatusDesc.fHp_Max * 0.05f;
		}
	}
	else
	{
		m_dTurnTime += dTimeDelta;

		Turn_Trigger(dTimeDelta); // 방돌리기
		TurnRoom(); // 쿄우가이 돌리기

		if ((3.0 + dTimeDelta < m_dTurnTime) && m_bTurnRoom == false) // 방 돌리기는 애니메이션 이후 1.5초
		{
			if (m_bAnimFinish2 == false)
			{
				m_bAnimFinish2 = true;
				m_eCurAnimIndex = ANIM_ATKCMB1;

			}
			if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB1))
			{
				CCameraManager::GetInstance()->Set_Is_Battle_LockFree(true);
				m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB1);
				m_eCurAnimIndex = ANIM_ATKCMB2;
			}
			if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB2))
			{
				m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB2);
				m_eCurAnimIndex = ANIM_ATKCMB3;
			}
			if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB3))
			{
				m_eCurAnimIndex = ANIM_IDLE;
				m_dReturnTime += dTimeDelta;
				if (1.0 < m_dReturnTime)
				{
					m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB3);
					m_eCurAnimIndex = ANIM_ATKSK_LF;
					m_bTurnLF = true;
					m_dReturnTime = 0.0;
				}
			}
			if (m_fCurAngleX == 0.f && m_fCurANgleZ == 0.f)
			{
				if (m_bAnimFinish3 == false)
				{
					m_bAnimFinish3 = true;
					m_eCurAnimIndex = ANIM_ATKSK_END;
					CCameraManager::GetInstance()->Set_Is_Battle_LockFree(false);
				}
				if (m_pModelCom->Get_AnimFinish(ANIM_ATKSK_END))
				{
					m_bTurn = false;
					m_dTurnTime = 0.0;
					m_pModelCom->Set_AnimisFinish(ANIM_ATKSK_END);
					m_eCurAnimIndex = ANIM_IDLE;
					Trigger_Interact();
				}

			}
		}


	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKSK_RF) || m_pModelCom->Get_AnimFinish(ANIM_ATKSK_LF))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKSK_RF);
		m_pModelCom->Set_AnimisFinish(ANIM_ATKSK_LF);
		m_eCurAnimIndex = ANIM_ATKSK_LOOP;
	}
}

void CBoss_Kyogai::Update_RoomChange_2(_double dTimeDelta)
{
	if (m_bTurn == false)
	{
		if (Check_Distance_FixY(3.f) == true)
		{
			if (m_bAnimFinish == false)
			{
				Pos_FixY();
				m_bAnimFinish = true;
				m_eCurAnimIndex = ANIM_PUSHAWAY;
			}
		}
		else
		{
			if (m_bAnimFinish == false)
			{
				m_bAnimFinish = true;
				m_eCurAnimIndex = ANIM_ATKSK_READY;
			}
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_PUSHAWAY))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_PUSHAWAY);
			m_eCurAnimIndex = ANIM_ATKSK_READY;
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_ATKSK_READY))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKSK_READY);
			m_eCurAnimIndex = ANIM_ATKSK_LOOP;
		}

		if (m_pModelCom->Get_AnimFinish(ANIM_ATKSK_LOOP) && m_bTurn == false)
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKSK_LOOP);
			m_eCurAnimIndex = ANIM_ATKSK_RF;
			m_bTurn = true;
			m_bTurnRF = true;
			m_StatusDesc.fHp -= m_StatusDesc.fHp_Max * 0.05f;
		}
	}
	else
	{
		m_dTurnTime += dTimeDelta;

		Turn_Trigger(dTimeDelta); // 방돌리기
		TurnRoom(); // 쿄우가이 돌리기

		if ((3.0 + dTimeDelta < m_dTurnTime) && m_bTurnRoom == false) // 방 돌리기는 애니메이션 이후 1.5초
		{
			if (m_bAnimFinish2 == false)
			{
				m_bAnimFinish2 = true;
				m_eCurAnimIndex = ANIM_ATKCMB_01READY;
			}
			if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB_01READY))
			{
				CCameraManager::GetInstance()->Set_Is_Battle_LockFree(true);
				m_eCurAnimIndex = ANIM_ATKCMB_02LOOP;
			}
			if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKCMB_02LOOP, 0.95, dTimeDelta))
			{
				m_iLoopCount++;
				if (m_iLoopCount >= 4 && m_bLoop == true)
				{
					m_iLoopCount = 0;
					m_bLoop = false;
				}
			}
			if (m_iLoopCount >= 4 && m_bLoop == false)
			{
				m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB_01READY);

				if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB_02LOOP))
				{
					m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB_02LOOP);
					m_eCurAnimIndex = ANIM_ATKCMB_03END;
				}

				if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB_03END))
				{
					m_eCurAnimIndex = ANIM_IDLE;
					m_dReturnTime += dTimeDelta;
					if (1.0 < m_dReturnTime)
					{
						m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB_03END);
						m_eCurAnimIndex = ANIM_ATKSK_LF;
						m_bTurnLF = true;
						m_dReturnTime = 0.0;
					}
				}
			}

			if ((m_fCurAngleX == 0.f && m_fCurANgleZ == 0.f) || (m_fCurAngleX == 0.f && m_fCurANgleZ == 360.f))
			{
				if (m_bAnimFinish3 == false)
				{
					m_bAnimFinish3 = true;
					m_eCurAnimIndex = ANIM_ATKSK_END;
					CCameraManager::GetInstance()->Set_Is_Battle_LockFree(false);
				}
				if (m_pModelCom->Get_AnimFinish(ANIM_ATKSK_END))
				{
					m_bTurn = false;
					m_dTurnTime = 0.0;
					m_pModelCom->Set_AnimisFinish(ANIM_ATKSK_END);
					m_eCurAnimIndex = ANIM_IDLE;
					Trigger_Interact();
				}
			}
		}


	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKSK_RF) || m_pModelCom->Get_AnimFinish(ANIM_ATKSK_LF))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKSK_RF);
		m_pModelCom->Set_AnimisFinish(ANIM_ATKSK_LF);
		m_eCurAnimIndex = ANIM_ATKSK_LOOP;
	}
}

void CBoss_Kyogai::Create_AlertRect(BLADETYPE eBladeType, _fvector vDir, _float fMovePos, _bool bLiarColor)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurLevel = pGameInstance->Get_CurLevelIdx();

	CAtkCollider* pAtkCollider = dynamic_cast<CAtkCollider*> (pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_MonsterAtk"), (_uint)pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("Layer_MonsterAtk")) - 1));

	CAlertRect::EFFECTDESC EffectDesc;
	EffectDesc.bLiarColor = bLiarColor;

	for (size_t i = 0; i < 2; i++)
	{
		switch (eBladeType)
		{
		case BLADE_ONE_RANDOM:
		{
			EffectDesc.vLook = pAtkCollider->Get_Dir();

			_vector vNormalVector = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), EffectDesc.vLook));

			EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + vNormalVector * fMovePos;

			pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);
			Create_GroundSmoke(CGroundSmoke::SMOKE_BLADECREATE, EffectDesc.vPos);
		}
		break;

		case BLADE_THREE_RANDOM:
		case BLADE_THREE_FRONT:
		{
			if (eBladeType == BLADE_THREE_RANDOM)
				EffectDesc.vLook = pAtkCollider->Get_Dir();
			else if (BLADE_THREE_FRONT)
				EffectDesc.vLook = vDir;

			_vector vNormalVector = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), EffectDesc.vLook));

			EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + vNormalVector * fMovePos;
			pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);
			Create_GroundSmoke(CGroundSmoke::SMOKE_BLADECREATE, EffectDesc.vPos);


			EffectDesc.vPos = (dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + vNormalVector * 0.8f) + vNormalVector * fMovePos;
			pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);
			Create_GroundSmoke(CGroundSmoke::SMOKE_BLADECREATE, EffectDesc.vPos);


			EffectDesc.vPos = (dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) - vNormalVector * 0.8f) + vNormalVector * fMovePos;
			pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);
			Create_GroundSmoke(CGroundSmoke::SMOKE_BLADECREATE, EffectDesc.vPos);
		}
		break;
		case BLADE_FIVE_RANDOM:
		case BLADE_FIVE_FRONT:
		{
			if (eBladeType == BLADE_FIVE_RANDOM)
				EffectDesc.vLook = pAtkCollider->Get_Dir();
			else if (BLADE_FIVE_FRONT)
				EffectDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			_vector vNormalVector = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), EffectDesc.vLook));

			EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + vNormalVector * fMovePos;
			pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);
			Create_GroundSmoke(CGroundSmoke::SMOKE_BLADECREATE, EffectDesc.vPos);


			EffectDesc.vPos = (dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + vNormalVector * 0.8f) + vNormalVector * fMovePos;
			pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);
			Create_GroundSmoke(CGroundSmoke::SMOKE_BLADECREATE, EffectDesc.vPos);


			EffectDesc.vPos = (dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + vNormalVector * 1.6f) + vNormalVector * fMovePos;
			pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);
			Create_GroundSmoke(CGroundSmoke::SMOKE_BLADECREATE, EffectDesc.vPos);


			EffectDesc.vPos = (dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) - vNormalVector * 0.8f) + vNormalVector * fMovePos;
			pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);
			Create_GroundSmoke(CGroundSmoke::SMOKE_BLADECREATE, EffectDesc.vPos);

			EffectDesc.vPos = (dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) - vNormalVector * 1.6f) + vNormalVector * fMovePos;
			pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false); }
		Create_GroundSmoke(CGroundSmoke::SMOKE_BLADECREATE, EffectDesc.vPos);
		break;
		default:
			break;
		}
	}

	Safe_Release(pGameInstance);
}

void CBoss_Kyogai::Create_BladeEffect(BLADETYPE eBladeType, _fvector vDir, _double dLongLifeTime, _double dSpeed, CAtkCollider::BULLET_TYPE eBulletType, _float fPosX)
{

	CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

	switch (eBladeType)
	{
	case BLADE_ONE_RANDOM:
		EffectWorldDesc.vPosition.x = fPosX + 0.f;
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 0.f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);
		break;
	case BLADE_THREE_RANDOM:
		EffectWorldDesc.vPosition.x = fPosX + (-1.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x - 0.8f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX + 0.f;
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 0.f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX + 1.f;
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 0.8f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);
		break;
	case BLADE_THREE_FRONT:
		EffectWorldDesc.vPosition.x = fPosX + (-1.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x - 0.8f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX + 0.f;

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 0.f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX + 1.f;
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 0.8f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);
		break;
	case BLADE_FIVE_RANDOM:
		EffectWorldDesc.vPosition.x = fPosX - 2.f;

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x - 0.5f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX - 1.f;

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x - 0.25f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX;
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 0.f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX + 1.f;
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 0.25f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX + 2.f;
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 0.5f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);
		break;
	case BLADE_FIVE_FRONT:
		EffectWorldDesc.vPosition.x = fPosX + (-2.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x - 1.6f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX + (-1.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x - 0.8f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX + 0.f;

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 0.f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX + 1.f;
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 0.8f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);

		EffectWorldDesc.vPosition.x = fPosX + 2.f;
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(EffectWorldDesc.vPosition.x + 1.6f, 1.0f, 0.f), dLongLifeTime,
			CAtkCollider::TYPE_EFFECT, vDir, m_fBigDmg, m_pTransformCom, dSpeed, eBulletType, "Kyogai_BladeAtk", &EffectWorldDesc);
		break;
	default:
		break;
	}

}

void CBoss_Kyogai::Turn_Trigger(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iCurLevel = pGameInstance->Get_CurLevelIdx();
	CRotationMapObject* pRotationMapObject = dynamic_cast<CRotationMapObject*>(pGameInstance->Get_GameObject(iCurLevel, TEXT("Layer_MapObject"), 101));

	if (true == m_bTurnRF || true == m_bTurnLF || true == m_bTurnRB || true == m_bTurnLB)
		m_dTimeAcc += dTimeDelta;

	if (Event_Time(dTimeDelta, 1.0, m_dTimeAcc))
	{
		_tchar szSoundFile[MAX_PATH] = TEXT("room_rotation_04.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.3f);
	}

	if (Event_Time(dTimeDelta, 1.5, m_dTimeAcc))
		CBattle_UI_Manager::GetInstance()->Set_Timing_On(true);

	if (Event_Time(dTimeDelta, 1.7, m_dTimeAcc))
	{
		_tchar szSoundFile[MAX_PATH] = TEXT("room_rotation_03.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.5f);

		_tchar szSoundFile2[MAX_PATH] = TEXT("ui_qte_success.ogg");
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::UI);
		CSoundMgr::Get_Instance()->PlaySound(szSoundFile2, CSoundMgr::UI, 0.7f);
	}

	if (Event_Time(dTimeDelta, 2.0, m_dTimeAcc))
	{
		for (_uint i = 0; i < 70; ++i)
		{
			pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_RoomSmoke"));
		}
		CCameraManager::GetInstance()->Camera_Shake(0.5, 200);
		//CBattle_UI_Manager::GetInstance()->Set_Timing_On(true);
	}

	if (true == m_bTurnRF && m_dTimeAcc > 2.5)
	{
		m_dTimeAcc = 0.0;
		m_bTurnRF = false;
		pRotationMapObject->Set_TurnRoomTriggerOn(CRotationMapObject::ROT_X_PLUS);

	}
	if (true == m_bTurnLF && m_dTimeAcc > 2.5)
	{
		m_dTimeAcc = 0.0;
		m_bTurnLF = false;
		pRotationMapObject->Set_TurnRoomTriggerOn(CRotationMapObject::ROT_X_MINUS);

	}
	if (true == m_bTurnRB && m_dTimeAcc > 2.5)
	{
		m_dTimeAcc = 0.0;
		m_bTurnRB = false;
		pRotationMapObject->Set_TurnRoomTriggerOn(CRotationMapObject::ROT_Z_PLUS);

	}
	if (true == m_bTurnLB && m_dTimeAcc > 2.5)
	{
		m_dTimeAcc = 0.0;
		m_bTurnLB = false;
		pRotationMapObject->Set_TurnRoomTriggerOn(CRotationMapObject::ROT_Z_MINUS);

	}

	Safe_Release(pGameInstance);
}

void CBoss_Kyogai::TurnRoom()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iCurLevel = pGameInstance->Get_CurLevelIdx();
	CRotationMapObject* pRotationMapObject = dynamic_cast<CRotationMapObject*>(pGameInstance->Get_GameObject(iCurLevel, TEXT("Layer_MapObject"), 101));

	_vector vRoomPos = pRotationMapObject->Get_RoomPos();
	_matrix		RotationMatrix;
	m_fCurAngleX = pRotationMapObject->Get_RotAngle().x;
	m_fCurANgleZ = pRotationMapObject->Get_RotAngle().z;
	m_bTurnRoom = pRotationMapObject->Get_TurnRoom();
	if (pRotationMapObject->Get_TurnRoom())
	{

		if (m_bPreTurn != pRotationMapObject->Get_TurnRoom()) // 처음 회전한다 했을 때 단 한 번만 들어오는 내용들
		{
			// 정육면체의 원점과 플레이어의 포지션과의 거리를 구한다.
			m_fDist = XMVectorGetX(XMVector3Length(vRoomPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

			// 플레이어가 정육면체의 원점을 바라보는 방향벡터
			m_vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			m_vDir = vRoomPos - m_vPos;

			m_vDir = XMVector3Normalize(m_vDir);
			m_vRotDir = m_vDir;
		}

		// 플레이어가 원점을 바라보는 방향벡터 방향으로 거리만큼 이동한다. -> 정육면체의 원점으로 이동
		m_pTransformCom->Set_Up(m_fDist, m_vDir);

		// 정육면체의 회전하는 만큼 플레이어도 회전한다.
		m_pTransformCom->Rotation(_float3(m_fCurAngleX, 0.f, m_fCurANgleZ));

		// 정육면체의 회전하는 만큼 Dir또한 회전한다.
		m_vDir = m_vRotDir;
		m_vDir = XMVector3Normalize(m_vDir);

		RotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fCurAngleX - (m_fPreAngleX)), XMConvertToRadians(0.f), XMConvertToRadians(m_fCurANgleZ - (m_fPreAngleZ)));

		m_vDir = XMVector3TransformNormal(m_vDir, RotationMatrix);

		// 원점까지 이동하고 회전했다면 같은 값을 빼주면서 뒤로 돌아간다. -> 공전된 듯한 원리
		m_pTransformCom->Set_Up(m_fDist, -m_vDir);

	}

	m_bPreTurn = pRotationMapObject->Get_TurnRoom();
	if (!pRotationMapObject->Get_TurnRoom())
	{
		m_fPreAngleX = pRotationMapObject->Get_RotAngle().x;
		m_fPreAngleZ = pRotationMapObject->Get_RotAngle().z;
	}

	Safe_Release(pGameInstance);
}

void CBoss_Kyogai::Rotation_Bullet(_double dTimeDelta, _double dTime, _fvector vDir, CAtkCollider::BULLET_TYPE eBulletType)
{
	_vector vMonsterDir = vDir;
	_double dSpeed = 4.0;
	////////////////////////////////////////칼날 1개////////////////////////////////////////////////////
	if (dTime < m_dTurnTime && m_dTurnTime <= dTime + dTimeDelta)
	{
		CCameraManager::GetInstance()->Set_Is_Battle_LockFree(true);
		//vMonsterDir = Rotation_Dir(vMonsterDir, 30.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_0, 0.3f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_1, 0.3f);
	}
	if (dTime + 0.1 < m_dTurnTime && m_dTurnTime <= dTime + 0.1 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 30.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);
	}
	if (dTime + 0.2 < m_dTurnTime && m_dTurnTime <= dTime + 0.2 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 60.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);
	}
	if (dTime + 0.3 < m_dTurnTime && m_dTurnTime <= dTime + 0.3 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 90.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_2, 0.3f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_3, 0.3f);
	}
	if (dTime + 0.4 < m_dTurnTime && m_dTurnTime <= dTime + 0.4 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 120.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);
	}
	if (dTime + 0.5 < m_dTurnTime && m_dTurnTime <= dTime + 0.5 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 150.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);
	}
	if (dTime + 0.6 < m_dTurnTime && m_dTurnTime <= dTime + 0.6 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 180.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);
	}
	if (dTime + 0.7 < m_dTurnTime && m_dTurnTime <= dTime + 0.7 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 210.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_4, 0.3f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_5, 0.3f);
	}
	if (dTime + 0.8 < m_dTurnTime && m_dTurnTime <= dTime + 0.8 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 240.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);
	}
	if (dTime + 0.9 < m_dTurnTime && m_dTurnTime <= dTime + 0.9 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 270.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);
	}
	if (dTime + 1.0 < m_dTurnTime && m_dTurnTime <= dTime + 1.0 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 300.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_0, 0.3f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_1, 0.3f);
	}
	if (dTime + 1.1 < m_dTurnTime && m_dTurnTime <= dTime + 1.1 + dTimeDelta)
	{
		vMonsterDir = Rotation_Dir(vMonsterDir, 330.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, eBulletType);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, eBulletType);


	}

}

void CBoss_Kyogai::Grid_Bullet(_double dTimeDelta, _double dTime, _fvector vDir)
{
	_vector vVerticalDir = Vertical_Dir();
	_vector vHorizonDir = HorizonDir();

	_vector vMonsterDir = vDir;
	_double dSpeed = 4.0;
	//////////////////////////////// 칼날 3개 or 5개 형 맘대로 /////////////////////////////////////
	if (dTime < m_dTurnTime && m_dTurnTime <= dTime + dTimeDelta)
	{
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_FIVE_RANDOM, vVerticalDir, 12.f);
		Create_BladeEffect(BLADE_FIVE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_FIVE_RANDOM);
		Create_BladeEffect(BLADE_FIVE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(-12.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_FIVE_RANDOM, vVerticalDir, -12.f);
		Create_BladeEffect(BLADE_FIVE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, -12.f);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_0, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_1, 0.4f);
	}
	if (dTime + 0.2 < m_dTurnTime && m_dTurnTime <= dTime + 0.2 + dTimeDelta)
	{

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_FIVE_RANDOM, vVerticalDir, 12.f);
		Create_BladeEffect(BLADE_FIVE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_FIVE_RANDOM);
		Create_BladeEffect(BLADE_FIVE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(-12.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_FIVE_RANDOM, vHorizonDir, -12.f);
		Create_BladeEffect(BLADE_FIVE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, -12.f);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_2, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_3, 0.4f);
	}

	/////////////////////////////////////// 여기 부터는 칼날 1개짜리 /////////////////////////////////////////////////
	if (dTime + 0.6 < m_dTurnTime && m_dTurnTime <= dTime + 0.6 + dTimeDelta)
	{

		vMonsterDir = Rotation_Dir(vMonsterDir, 20.f, 0.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_4, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_5, 0.4f);

	}
	if (dTime + 0.7 < m_dTurnTime && m_dTurnTime <= dTime + 0.7 + dTimeDelta)
	{

		vMonsterDir = Rotation_Dir(vMonsterDir, 40.f, 0.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

	}
	if (dTime + 0.8 < m_dTurnTime && m_dTurnTime <= dTime + 0.8 + dTimeDelta)
	{

		vMonsterDir = Rotation_Dir(vMonsterDir, 60.f, 0.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_0, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_1, 0.4f);

	}
	if (dTime + 0.9 < m_dTurnTime && m_dTurnTime <= dTime + 0.9 + dTimeDelta)
	{

		vMonsterDir = Rotation_Dir(vMonsterDir, 80.f, 0.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

	}
	if (dTime + 1.0 < m_dTurnTime && m_dTurnTime <= dTime + 1.0 + dTimeDelta)
	{

		vMonsterDir = Rotation_Dir(vMonsterDir, 100.f, 0.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

	}
	if (dTime + 1.10 < m_dTurnTime && m_dTurnTime <= dTime + 1.10 + dTimeDelta)
	{

		vMonsterDir = Rotation_Dir(vMonsterDir, 120.f, 0.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_2, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_3, 0.4f);

	}
	if (dTime + 1.20 < m_dTurnTime && m_dTurnTime <= dTime + 1.20 + dTimeDelta)
	{

		vMonsterDir = Rotation_Dir(vMonsterDir, 140.f, 0.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 0.f), 3.0,
			CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

		Create_AlertRect(BLADE_ONE_RANDOM);
		Create_BladeEffect(BLADE_ONE_RANDOM, vMonsterDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_BULLET);

	}


}

void CBoss_Kyogai::Wave_Bullet(_double dTimeDelta, _double dTime, _fvector vDir)
{
	_vector vGoDir = vDir;
	_vector vVerticalDir = Vertical_Dir();
	_vector vHorizonDir = HorizonDir();
	_int RandomNum = Random::Generate_Int(1, 2);
	// + - / vertical horizon

	_double dSpeed = 2.0;
	_double dLifeTime = 7.0;
	_float fPosX = 1.f;
	_bool	bLiar = false;

	if (dTime < m_dTurnTime && m_dTurnTime <= dTime + dTimeDelta) // 이 패턴은 나아 가는 방향 그대로 표시 해주면 됨
	{
		RandomNum = Random::Generate_Int(1, 2);
		if (RandomNum == 1)
			vGoDir = vVerticalDir;
		else
			vGoDir = vHorizonDir;

		RandomNum = Random::Generate_Int(1, 2);

		if (RandomNum == 1)
			vGoDir *= -1.f;
		RandomNum = Random::Generate_Int(1, 2);
		if (RandomNum == 1)
			bLiar = true;


		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(2.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 2.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 2.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 4.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 8.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 12.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 16.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		fPosX *= -1.f;
		if (bLiar == true)
			bLiar = false;
		else if (bLiar == false)
			bLiar = true;
		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 2.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 2.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 4.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 8.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 12.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 16.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_0, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_1, 0.4f);

	}
	if (dTime + 2.0 < m_dTurnTime && m_dTurnTime <= dTime + 2.0 + dTimeDelta) // 이 패턴은 나아 가는 방향 그대로 표시 해주면 됨
	{
		RandomNum = Random::Generate_Int(1, 2);
		if (RandomNum == 1)
			vGoDir = vVerticalDir;
		else
			vGoDir = vHorizonDir;

		RandomNum = Random::Generate_Int(1, 2);

		if (RandomNum == 1)
			vGoDir *= -1.f;

		RandomNum = Random::Generate_Int(1, 2);
		if (RandomNum == 1)
			bLiar = true;


		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(2.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 2.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 2.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 4.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 8.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 12.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 16.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		fPosX *= -1.f;
		if (bLiar == true)
			bLiar = false;
		else if (bLiar == false)
			bLiar = true;
		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 2.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 2.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 4.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 8.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 12.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 16.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_2, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_3, 0.4f);
	}
	if (dTime + dLifeTime < m_dTurnTime && m_dTurnTime <= dTime + dLifeTime + dTimeDelta) // 이 패턴은 나아 가는 방향 그대로 표시 해주면 됨
	{
		RandomNum = Random::Generate_Int(1, 2);
		if (RandomNum == 1)
			vGoDir = vVerticalDir;
		else
			vGoDir = vHorizonDir;

		RandomNum = Random::Generate_Int(1, 2);

		if (RandomNum == 1)
			vGoDir *= -1.f;

		RandomNum = Random::Generate_Int(1, 2);
		if (RandomNum == 1)
			bLiar = true;


		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(2.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 2.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 2.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 4.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 8.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 12.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 16.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		fPosX *= -1.f;
		if (bLiar == true)
			bLiar = false;
		else if (bLiar == false)
			bLiar = true;
		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 2.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 2.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 4.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 8.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 12.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 16.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_4, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_5, 0.4f);

	}
	if (dTime + 6.0 < m_dTurnTime && m_dTurnTime <= dTime + 6.0 + dTimeDelta) // 이 패턴은 나아 가는 방향 그대로 표시 해주면 됨
	{
		RandomNum = Random::Generate_Int(1, 2);
		if (RandomNum == 1)
			vGoDir = vVerticalDir;
		else
			vGoDir = vHorizonDir;

		RandomNum = Random::Generate_Int(1, 2);

		if (RandomNum == 1)
			vGoDir *= -1.f;

		RandomNum = Random::Generate_Int(1, 2);
		if (RandomNum == 1)
			bLiar = true;


		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(2.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 2.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 2.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 4.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 8.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 12.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 16.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		fPosX *= -1.f;
		if (bLiar == true)
			bLiar = false;
		else if (bLiar == false)
			bLiar = true;
		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 2.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 2.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 4.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 8.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 12.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 16.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_0, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_1, 0.4f);
	}
	if (dTime + 8.0 < m_dTurnTime && m_dTurnTime <= dTime + 8.0 + dTimeDelta) // 이 패턴은 나아 가는 방향 그대로 표시 해주면 됨
	{
		RandomNum = Random::Generate_Int(1, 2);
		if (RandomNum == 1)
			vGoDir = vVerticalDir;
		else
			vGoDir = vHorizonDir;

		RandomNum = Random::Generate_Int(1, 2);

		if (RandomNum == 1)
			vGoDir *= -1.f;

		RandomNum = Random::Generate_Int(1, 2);
		if (RandomNum == 1)
			bLiar = true;


		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(2.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 2.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 2.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 4.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 8.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 12.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), dLifeTime,
			CAtkCollider::TYPE_SMALL, vGoDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 16.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		fPosX *= -1.f;

		if (bLiar == true)
			bLiar = false;
		else if (bLiar == false)
			bLiar = true;
		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 2.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 2.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 4.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 8.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 12.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Create_AlertRect(BLADE_THREE_RANDOM, vGoDir, 16.f * fPosX, bLiar);
		Create_BladeEffect(BLADE_THREE_RANDOM, vGoDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_2, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_3, 0.4f);
	}
}

void CBoss_Kyogai::Liar_Bullet(_double dTimeDelta, _double dTime, _fvector vDir)
{
	_vector vMonsterDir = vDir;
	_vector vVerticalDir = Vertical_Dir();
	_vector vHorizonDir = HorizonDir();

	_double dSpeed = 5.0;
	CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;


	if (dTime < m_dTurnTime && m_dTurnTime <= dTime + dTimeDelta) // 이 패턴은 나아 가는 방향 그대로 표시 해주면 됨
	{
		/*Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET);*/

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, -vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, -vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);


		vVerticalDir = Rotation_Dir(vVerticalDir, 45.f, 0.f);
		vHorizonDir = Rotation_Dir(vHorizonDir, 45.f, 0.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, -vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, -vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_0, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_1, 0.4f);

	}
	if (dTime + 2.0 < m_dTurnTime && m_dTurnTime <= dTime + 2.0 + dTimeDelta) // 이 패턴은 나아 가는 방향 그대로 표시 해주면 됨
	{
		vVerticalDir = Rotation_Dir(vVerticalDir, 30.f, 0.f);
		vHorizonDir = Rotation_Dir(vHorizonDir, 30.f, 0.f);
		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, -vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, -vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		vVerticalDir = Rotation_Dir(vVerticalDir, 30.f, 0.f);
		vHorizonDir = Rotation_Dir(vHorizonDir, 30.f, 0.f);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);


		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, -vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, -vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_LIAR_BULLET, "Kyogai_BladeAtk");

		Create_AlertRect(BLADE_ONE_RANDOM);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_0, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_1, 0.4f);
	}
	if (dTime + 4.0 < m_dTurnTime && m_dTurnTime <= dTime + 4.0 + dTimeDelta) // 이 패턴은 나아 가는 방향 그대로 표시 해주면 됨
	{
		dSpeed = 3.0;
		_int iRandomDir = Random::Generate_Int(1, 2);
		_int iRandomPos = Random::Generate_Int(1, 2);
		_float fPosX = 1.f;

		if (iRandomDir == 1)
			vVerticalDir *= -1;
		else
			vVerticalDir *= 1;

		iRandomDir = Random::Generate_Int(1, 2);

		if (iRandomDir == 1)
			vHorizonDir *= -1;
		else
			vHorizonDir *= 1;

		if (iRandomPos == 1)
			fPosX *= -1.f;
		else
			fPosX = 1.f;

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(0.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);


		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 4.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 8.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 12.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 16.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(0.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 4.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 8.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 12.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 16.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_0, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_1, 0.4f);

	}
	if (dTime + 6.0 < m_dTurnTime && m_dTurnTime <= dTime + 6.0 + dTimeDelta) // 이 패턴은 나아 가는 방향 그대로 표시 해주면 됨
	{
		dSpeed = 3.0;
		_int iRandomDir = Random::Generate_Int(1, 2);
		_int iRandomPos = Random::Generate_Int(1, 2);
		_float fPosX = 1.f;

		if (iRandomDir == 1)
			vVerticalDir *= -1;
		else
			vVerticalDir *= 1;

		iRandomDir = Random::Generate_Int(1, 2);

		if (iRandomDir == 1)
			vHorizonDir *= -1;
		else
			vHorizonDir *= 1;

		if (iRandomPos == 1)
			fPosX *= -1.f;
		else
			fPosX = 1.f;

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(0.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 4.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 8.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 12.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 16.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(0.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 4.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 8.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 12.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 16.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_2, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_3, 0.4f);
	}
	if (dTime + 8.0 < m_dTurnTime && m_dTurnTime <= dTime + 8.0 + dTimeDelta) // 이 패턴은 나아 가는 방향 그대로 표시 해주면 됨
	{
		dSpeed = 3.0;
		_int iRandomDir = Random::Generate_Int(1, 2);
		_int iRandomPos = Random::Generate_Int(1, 2);
		_float fPosX = 1.f;

		if (iRandomDir == 1)
			vVerticalDir *= -1;
		else
			vVerticalDir *= 1;

		iRandomDir = Random::Generate_Int(1, 2);

		if (iRandomDir == 1)
			vHorizonDir *= -1;
		else
			vHorizonDir *= 1;

		if (iRandomPos == 1)
			fPosX *= -1.f;
		else
			fPosX = 1.f;

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(0.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 4.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 8.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 12.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vVerticalDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vVerticalDir, 16.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vVerticalDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(0.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(4.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 4.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 4.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(8.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 8.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 8.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(12.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 12.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 12.f * fPosX);

		Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(2.f, 2.f, 2.f), _float3(16.f * fPosX, 1.0f, 0.f), 4.0,
			CAtkCollider::TYPE_SMALL, vHorizonDir, m_fSmallDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAI_HORIZON_BULLET);

		Create_AlertRect(BLADE_THREE_RANDOM, vHorizonDir, 16.f * fPosX);
		Create_BladeEffect(BLADE_THREE_RANDOM, vHorizonDir, 3.0, dSpeed, CAtkCollider::TYPE_KYOGAI_VERTICAL_BULLET, 16.f * fPosX);

		_tchar szSoundFile1[MAX_PATH] = TEXT("aura_02.ogg");
		Play_Sound_Channel(szSoundFile1, CSoundMgr::KYOGAI_BLADE_4, 0.4f);
		_tchar szSoundFile2[MAX_PATH] = TEXT("cut_finalHit_01.ogg");
		Play_Sound_Channel(szSoundFile2, CSoundMgr::KYOGAI_BLADE_5, 0.4f);
	}

}

_vector CBoss_Kyogai::Vertical_Dir()
{
	_vector vRoomPos = { 126.536f, 0.f, 123.840f, 1.f };
	_vector vVerticalPos = { 143.536f, 0.f, 123.840f, 1.f };

	return XMVector3Normalize(vVerticalPos - vRoomPos);

}

_vector CBoss_Kyogai::HorizonDir()
{
	_vector vRoomPos = { 126.536f, 0.f, 123.840f, 1.f };
	_vector vHorizonPos = { 126.536f, 0.f, 140.840f, 1.f };

	return XMVector3Normalize(vHorizonPos - vRoomPos);
}

void CBoss_Kyogai::Land_Anim_Play(ANIM CurAnim, ANIM LandAnim)
{
	if (m_pModelCom->Get_iCurrentAnimIndex() == CurAnim)
	{
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (Pos.y <= m_fLand_Y)
		{
			m_eCurAnimIndex = LandAnim;

			Pos.y = m_fLand_Y;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));
		}
	}
}

void CBoss_Kyogai::Dialog_Update(_double dTimeDelta)
{
	if (PART_1 == m_ePart)
	{
		m_dDialogAccTime += dTimeDelta;

		if (Event_Time(dTimeDelta, 3.f, m_dDialogAccTime))
		{
			Set_CharacterDialog(4.f, TEXT("[카마도 탄지로]"), TEXT("이번에야말로 널 베겠다!"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_11_Tanjiro.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 7.5f, m_dDialogAccTime))
		{
			Set_CharacterDialog(4.f, TEXT("[쿄우가이]"), TEXT("꺼져라! 버러지 같은 놈!"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_12.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 13.5f, m_dDialogAccTime))
		{
			Set_CharacterDialog(12.f, TEXT("[카마도 탄지로]"), TEXT("(지금 난 상태가 좋지 않아)"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_15_Tanjiro.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 18.0f, m_dDialogAccTime))
		{
			Set_CharacterDialog(12.f, TEXT("[카마도 탄지로]"), TEXT("(간격 안으로 들어가려고 파고들 때") , TEXT("통증이 밀려와 발이 얽히거나 하면.....)"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_16_Tanjiro.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 24.5f, m_dDialogAccTime))
		{
			Set_CharacterDialog(12.f, TEXT("[카마도 탄지로]"), TEXT("(온 몸이 조각나겠지)"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_17_Tanjiro.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 32.f, m_dDialogAccTime))
		{
			Set_CharacterDialog(8.f, TEXT("[쿄우가이]"), TEXT("남의 집에 멋대로 들어와선.."));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_18.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 36.f, m_dDialogAccTime))
		{
			Set_CharacterDialog(8.f, TEXT("[쿄우가이]"), TEXT("갈가리 찢기면서 후회해라, 버러지 녀석!"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_19.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 44.f, m_dDialogAccTime))
		{
			Set_CharacterDialog(5.f, TEXT("[쿄우가이]"), TEXT("소생을 방해하지마라! 성가신 버러지 같으니!"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_14.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 49.5f, m_dDialogAccTime))
		{
			Set_CharacterDialog(8.f, TEXT("[카마도 탄지로]"), TEXT("(저 혈귀의... 손톱 같은 흔적이 생기는 공격!"), TEXT("저게 무서워서 녀석한테 접근할 수가 없어)"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_26_Tanjiro.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 58.f, m_dDialogAccTime))
		{
			Set_CharacterDialog(8.f, TEXT("[쿄우가이]"), TEXT("끈질긴 버러지가! 잘도 피하는군!"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_27.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 66.5f, m_dDialogAccTime))
		{
			Set_CharacterDialog(6.f, TEXT("[쿄우가이]"), TEXT("젠장, 화가 치민다! 빨리 희귀혈을 먹어야 하건만!"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_28.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 73.f, m_dDialogAccTime))
		{
			Set_CharacterDialog(6.f, TEXT("[카마도 탄지로]"), TEXT("(진정해... 똑바로 앞을 향해라!!") , TEXT("자신을 고무시켜라!!)"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_29_Tanjiro.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 80.f, m_dDialogAccTime))
		{
			Set_CharacterDialog(6.f, TEXT("[쿄우가이]"), TEXT("소생은... 희귀혈을 얻어서"), TEXT("십이귀월로 되돌아간다!!"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_35.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		else if (Event_Time(dTimeDelta, 86.5f, m_dDialogAccTime))
		{
			Set_CharacterDialog(3.f, TEXT("[쿄우가이]"), TEXT("꺼져라! 버러지 녀석!"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_Talk_36.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);
		}
		
		if (!m_bFinishVoice && m_eCurPhase == PHASE_1 && (m_StatusDesc.fHp / m_StatusDesc.fHp_Max) <= 0.2f)
		{
			Set_CharacterDialog(4.f, TEXT("[카마도 탄지로]"), TEXT("좋아! 이대로 한 번에 끝낸다..!!"));
			_tchar szSoundFile[MAX_PATH] = TEXT("Tanjiro_Talk_Yosi.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 0.8f);

			m_bFinishVoice = true;
		}
	}
	
}

void CBoss_Kyogai::FastBook_Update(_double dTimeDelta)
{
	if (m_bFastBook)
	{
		m_dFastBoolAccTime += dTimeDelta;

		if (m_dFastBoolAccTime > 0.5)
		{
			static	_uint iIdx = 0;
			m_dFastBoolAccTime = 0.0;


			if (0 == iIdx)
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_FastBook.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::KYOGAI_BOOK_0, 0.3f);
			}
			else if (1 == iIdx)
			{
				_tchar szSoundFile[MAX_PATH] = TEXT("Kyogai_FastBook.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::KYOGAI_BOOK_1, 0.3f);
			}

			++iIdx;

			if (iIdx == 2)
				iIdx = 0;
		}
	}
}

void CBoss_Kyogai::Play_Sound_Dmg(_int iType, _float vol)
{
	//small
	if (iType == 0)
	{
		if (m_iSound_Dmg_Small == 0)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Kyo_Dmg_Small0.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_VOICE, vol);
		}
		else if (m_iSound_Dmg_Small == 1)
		{
			m_iSound_Dmg_Small = 0;

			_tchar szSoundFile[MAX_PATH] = TEXT("Kyo_Dmg_Small1.ogg");
			Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_VOICE_SUB, vol);
		}
	}
	// Medium
	else if (iType == 1)
	{
		_tchar szSoundFile[MAX_PATH] = TEXT("Kyo_Dmg_Medium.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_VOICE, vol);
	}
	// Big
	else if (iType == 2)
	{
		_tchar szSoundFile[MAX_PATH] = TEXT("Kyo_Dmg_Big.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_VOICE, vol);
	}
}

HRESULT CBoss_Kyogai::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Kyogai"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CBoss_Kyogai");
		return E_FAIL;
	}

	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CBoss_Kyogai");
		return E_FAIL;
	}

	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CBoss_Kyogai");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(0.8f, 0.8f, 0.8f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CBoss_Kyogai");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CBoss_Kyogai");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.f, 1.f, 1.f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CBoss_Kyogai");
		return E_FAIL;
	}

	m_CharacterDesc.NaviDesc.iCurrentIndex = 0;
	/* for.Com_Navigation_House_4_0*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_4_0"),
		TEXT("Com_Navigation_House_4_0"), (CComponent**)&m_pNavigationCom[NAVI_HOUSE_4_0], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_House_4_0: CBoss_Kyogai");
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CBoss_Kyogai::SetUp_ShaderResources()
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

	Safe_Release(pGameInstance);
	// OutlineThickness
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineThickness", &m_fOutlineThickness, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineFaceThickness", &m_fOutlineFaceThickness, sizeof(_float))))
		return E_FAIL;

	
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fFar2", &m_fFar2, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

CBoss_Kyogai* CBoss_Kyogai::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Kyogai* pInstance = new CBoss_Kyogai(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_Kyogai");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Kyogai::Clone(void* pArg)
{
	CBoss_Kyogai* pInstance = new CBoss_Kyogai(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Kyogai");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Kyogai::Free()
{
	__super::Free();
}
