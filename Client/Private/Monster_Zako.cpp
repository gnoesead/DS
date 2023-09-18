#include "pch.h"
#include "..\Public\Monster_Zako.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "MonsterManager.h"

#include "Player.h"
#include "PlayerManager.h"
#include "World_UI_Hp.h"

#include "Fade_Manager.h"

CMonster_Zako::CMonster_Zako(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CMonster_Zako::CMonster_Zako(const CMonster_Zako& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonster_Zako::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Zako::Initialize(void* pArg)
{

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_pModelCom->Set_Animation(ANIM_IDLE);

	if (FAILED(Read_Animation_Control_File("Zako_0.bin")))
	{
		MSG_BOX("Failed to AnimData Read : Zako_0");
		return E_FAIL;
	}

	m_pTransformCom->Scaling(_float3{ m_fScale, m_fScale, m_fScale });


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Monster_Hp
	CWorld_UI_Hp::UIDESC UIDesc3;

	ZeroMemory(&UIDesc3, sizeof UIDesc3);
	UIDesc3.m_Is_Reverse = false;
	UIDesc3.m_Type = 0;
	UIDesc3.m_pMonster = this;
	UIDesc3.m_Up_Mount = 1.7f;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_Battle_UI"), TEXT("Prototype_GameObject_World_UI_Hp"), &UIDesc3))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc3, sizeof UIDesc3);

	UIDesc3.m_Is_Reverse = false;
	UIDesc3.m_Type = 1;
	UIDesc3.m_pMonster = this;
	UIDesc3.m_Up_Mount = 1.7f;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_Battle_UI"), TEXT("Prototype_GameObject_World_UI_Hp"), &UIDesc3))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc3, sizeof UIDesc3);

	UIDesc3.m_Is_Reverse = false;
	UIDesc3.m_Type = 2;
	UIDesc3.m_pMonster = this;
	UIDesc3.m_Up_Mount = 1.7f;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_Battle_UI"), TEXT("Prototype_GameObject_World_UI_Hp"), &UIDesc3))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc3, sizeof UIDesc3);

	UIDesc3.m_Is_Reverse = false;
	UIDesc3.m_Type = 3;
	UIDesc3.m_pMonster = this;
	UIDesc3.m_Up_Mount = 1.7f;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Boss_Battle_UI"), TEXT("Prototype_GameObject_World_UI_Hp"), &UIDesc3))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	m_StatusDesc.fHp_Max = 100.f;
	m_StatusDesc.fHp = 100.f;

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_GAMEPLAY)
	{
		m_isCan_Tutorial = true;
		m_StatusDesc.fHp_Max = 100000.f;
		m_StatusDesc.fHp = 100000.f;
	}

	Safe_Release(pGameInstance);

	if (m_CharacterDesc.NPCDesc.eNPC == NPC_QUEST)
	{
		m_iAttackIndex = 0; // 2,5
		m_SoundChr = CSoundMgr::MONSTER_VOICE;
	}
	else if (m_CharacterDesc.NPCDesc.eNPC == NPC_TALK)
	{
		m_iAttackIndex = 2; // 2,5
		m_SoundChr = CSoundMgr::MONSTER_VOICE_SUB;
	}
	else if (m_CharacterDesc.NPCDesc.eNPC == NPC_WALKTALK)
	{
		m_iAttackIndex = 5; // 2,5
		m_SoundChr = CSoundMgr::MONSTER_VOICE_SUB2;
	}
	m_pModelCom->Set_LinearDuration(ANIM_IDLE, 0.6);

	return S_OK;
}

void CMonster_Zako::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
		if (pPlayer->Get_CurNaviMesh() != CLandObject::NAVI_HOUSE_2_0)
		{
			Safe_Release(pGameInstance);
			return;
		}
	}

	Safe_Release(pGameInstance);

	__super::Tick(dTimeDelta);

	if (true == m_isDead)
		return;
	if (m_bTanjiroAwake == false && m_bZenitsuAwake == false && m_bTanjiroSurge == false)
	{
		if (CFadeManager::GetInstance()->Get_Is_House_Monster_Battle_Start() || m_isCan_Tutorial)
		{
			if (m_isFirst_BattleOn)
			{
				m_isFirst_BattleOn = false;
				CMonsterManager::GetInstance()->Set_BattleOn(true);
				CMonsterManager::GetInstance()->Set_StealthEnd_Delete(true);
			}

			Trigger();
			if (CPlayerManager::GetInstance()->Get_Slow() == false)
			{
				Animation_Control(dTimeDelta);
			}


		}

		//¾Ö´Ï¸ÞÀÌ¼Ç Ã³¸®
		if (CPlayerManager::GetInstance()->Get_Slow() == false)
		{
			m_pModelCom->Play_Animation(dTimeDelta);
		}
		RootAnimation(dTimeDelta);

		//ÀÌº¥Æ® ÄÝ
		EventCall_Control(dTimeDelta);

		Dialog_Update(dTimeDelta);
	}

}

void CMonster_Zako::LateTick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
		if (pPlayer->Get_CurNaviMesh() != CLandObject::NAVI_HOUSE_2_0)
		{
			Safe_Release(pGameInstance);
			return;
		}
	}

	Safe_Release(pGameInstance);

	__super::LateTick(dTimeDelta);
	if (m_bTanjiroAwake == false && m_bZenitsuAwake == false && m_bTanjiroSurge == false)
	{
		if (CPlayerManager::GetInstance()->Get_Slow() == false)
		{
			Gravity(dTimeDelta);
		}
	}
#ifdef _DEBUG
	/*if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
		return;*/
#endif
}

HRESULT CMonster_Zako::Render()
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
				m_pShaderCom->Begin(1);

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

HRESULT CMonster_Zako::Render_ShadowDepth()
{
	if (FAILED(__super::Render_ShadowDepth()))
		return E_FAIL;

	return S_OK;
}

void CMonster_Zako::EventCall_Control(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_iEvent_Index = 0;
	}

	_vector AtkDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (EventCallProcess())
	{
#pragma region Attack
		if (0 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0.3
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_Down", m_pTransformCom);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_D", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_05.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.7f);
			}
			if (1 == m_iEvent_Index)
			{//0.39
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_SMALL, AtkDir, 2.0f);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_05.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.55f);
			}
		}

		if (1 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0.55
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Slam", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_05.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_2, 0.7f);
			}
			if (1 == m_iEvent_Index)
			{//0.64
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_BIG, AtkDir, 2.0f);
			}
		}

		if (ANIM_ATK_CLAWS == m_pModelCom->Get_iCurrentAnimIndex())
		{
			CEffectPlayer::EFFECTWORLDDESC EffectDesc;
			EffectDesc.fScale = 0.85f;

			if (0 == m_iEvent_Index)
			{//0.10
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_R", m_pTransformCom, &EffectDesc);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_R", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.55f);
			}
			if (1 == m_iEvent_Index)
			{//0.17
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_CONNECTSMALL, AtkDir, 1.5f);
			}
			if (2 == m_iEvent_Index)
			{//0.30
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_L", m_pTransformCom, &EffectDesc);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_L", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_S.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.55f);
			}
			if (3 == m_iEvent_Index)
			{//0.35
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_CONNECTSMALL, AtkDir, 1.5f);
			}
			if (4 == m_iEvent_Index)
			{//0.50
				CEffectPlayer::Get_Instance()->Play("Zako_ Atk_Claws_R", m_pTransformCom, &EffectDesc);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_R", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.55f);
			}
			if (5 == m_iEvent_Index)
			{//0.61
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_CONNECTSMALL, AtkDir, 1.5f);
			}
			if (6 == m_iEvent_Index)
			{//0.80
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_L", m_pTransformCom, &EffectDesc);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_L", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("hit_sword_01.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.55f);
			}
			if (7 == m_iEvent_Index)
			{//0.84
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_CONNECTSMALL, AtkDir, 1.5f);
			}
			if (8 == m_iEvent_Index)
			{//1.30
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.x = 0.2f;
				EffectWorldDesc.fScale = 1.2f;

				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_R", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.x = -0.2f;
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_L", m_pTransformCom, &EffectWorldDesc);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_R", m_pTransformCom);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_L", m_pTransformCom);

				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_BIG, AtkDir, 3.5f);


				_tchar szSoundFile[MAX_PATH] = TEXT("st_sword04.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.55f);

				_tchar szSoundFile1[MAX_PATH] = TEXT("hit_sword_S.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_2, 0.55f);
			}
		}

		if (9 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0
				CEffectPlayer::EFFECTWORLDDESC EffectDesc;
				EffectDesc.fScale = 0.9f;
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_L", m_pTransformCom, &EffectDesc);

				_tchar szSoundFile1[MAX_PATH] = TEXT("st_slash01.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_2, 0.55f);
			}
			if (1 == m_iEvent_Index)
			{//0.1
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_BIG, AtkDir, 4.0f);
			}
		}

		if (12 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			//if (0 == m_iEvent_Index)
			//{//0.5
			//}
			if (1 == m_iEvent_Index)
			{//0.65
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_KickDown", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_04.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.55f);
			}
		}
		if (13 == m_pModelCom->Get_iCurrentAnimIndex()) // ±æ°Ô
		{
			if (0 == m_iEvent_Index)
			{//0.02
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.7f, 1.7f, 1.7f), _float3(0.f, 0.0f, 0.0f), 1.5,
					CAtkCollider::TYPE_BLOW, AtkDir, 4.5f);
			}
		}
		if (14 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0
				CEffectPlayer::Get_Instance()->Play("Akaza_Shockwave_XYZ_Small", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("awk_eff_wind.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.55f);

				_tchar szSoundFile1[MAX_PATH] = TEXT("hit_firel_01.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_2, 0.55f);
			}
		}

		if (18 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0.15
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_SpinKick", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("awk_eff_wind_1.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.55f);
			}
		}

		if (19 == m_pModelCom->Get_iCurrentAnimIndex()) // ±æ°Ô
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.7f, 1.7f, 1.7f), _float3(0.f, 0.0f, 0.0f), 1.5,
					CAtkCollider::TYPE_BLOW, AtkDir, 4.6f);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_19.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.55f);
			}
		}

		if (24 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0.1
				CEffectPlayer::EFFECTWORLDDESC EffectDesc;
				EffectDesc.fScale = 0.8f;
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_Up_L", m_pTransformCom, &EffectDesc);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_R", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_04.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.55f);

				_tchar szSoundFile1[MAX_PATH] = TEXT("st_sword04.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_1, 0.55f);
			}
			if (1 == m_iEvent_Index)
			{//0.24
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_SMALL, AtkDir, 2.0f);
			}
		}

		if (25 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0.1
				CEffectPlayer::EFFECTWORLDDESC EffectDesc;
				EffectDesc.fScale = 0.8f;
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_Up_R", m_pTransformCom, &EffectDesc);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_L", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_05.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.55f);

				_tchar szSoundFile1[MAX_PATH] = TEXT("st_sword04.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_1, 0.55f);
			}
			if (1 == m_iEvent_Index)
			{//0.28
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_SMALL, AtkDir, 2.5f);
			}
		}

		if (26 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0.4
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_BLOW, AtkDir, 3.5f);
			}
			if (1 == m_iEvent_Index)
			{//0.5
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Slam", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_05.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.55f);

				_tchar szSoundFile1[MAX_PATH] = TEXT("hit_sword_L.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_1, 0.55f);
			}
		}

		if (30 == m_pModelCom->Get_iCurrentAnimIndex()) // ±æ°Ô
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.7f, 1.7f, 1.7f), _float3(0.f, 0.0f, 0.0f), 1.5,
					CAtkCollider::TYPE_BLOW, AtkDir, 4.0f);

				_tchar szSoundFile[MAX_PATH] = TEXT("awk_eff_wind_1.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.55f);
			}
		}
		if (31 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0.05
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.x = 0.2f;
				EffectWorldDesc.fScale = 1.2f;

				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_R", m_pTransformCom, &EffectWorldDesc);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_R", m_pTransformCom);

				EffectWorldDesc.vPosition.x = -0.2f;
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_L", m_pTransformCom, &EffectWorldDesc);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_L", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("st_sword03.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.55f);

				_tchar szSoundFile1[MAX_PATH] = TEXT("st_sword04.ogg");
				Play_Sound_Channel(szSoundFile1, CSoundMgr::MONSTER_EFFECT_2, 0.55f);
			}
		}

		if (38 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0.45
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Spin", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Spin_Wind", m_pTransformCom);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_Spin", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_09.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_0, 0.55f);
			}
			if (1 == m_iEvent_Index)
			{//0.59
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_SMALL, AtkDir, 1.5f);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_04.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.55f);
			}
			if (2 == m_iEvent_Index)
			{//0.9
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Spin", m_pTransformCom);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_Spin", m_pTransformCom);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_05.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_2, 0.55f);
			}
			if (3 == m_iEvent_Index)
			{//0.99
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_SMALL, AtkDir, 2.0f);
			}
			if (4 == m_iEvent_Index)
			{//1.9
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_Down", m_pTransformCom);

				CEffectPlayer::Get_Instance()->Play("Zako_Claws_Particle_D", m_pTransformCom);
			}
			if (5 == m_iEvent_Index)
			{//1.960
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_BLOW, AtkDir, 3.5f);
			}
		}

		if (44 == m_pModelCom->Get_iCurrentAnimIndex()) // ±æ°Ô
		{
			if (0 == m_iEvent_Index)
			{//0
				CEffectPlayer::EFFECTWORLDDESC EffectDesc;
				EffectDesc.fScale = 0.7f;
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_SpinPunch", m_pTransformCom, &EffectDesc);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_09.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.85f);
			}
			if (1 == m_iEvent_Index)
			{//0.02
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.7f, 1.7f, 1.7f), _float3(0.f, 0.0f, 0.0f), 0.4,
					CAtkCollider::TYPE_SMALL, AtkDir, 3.0f);

				_tchar szSoundFile[MAX_PATH] = TEXT("swing_19.ogg");
				Play_Sound_Channel(szSoundFile, CSoundMgr::MONSTER_EFFECT_1, 0.55f);
			}
		}

		if (45 == m_pModelCom->Get_iCurrentAnimIndex())	// °ø°ÝÇÏ°í ÂøÁö
		{
			if (0 == m_iEvent_Index)
			{// 0.62
				Create_GroundSmoke(CGroundSmoke::SMOKE_DASHLAND);
			}
		}

		if (52 == m_pModelCom->Get_iCurrentAnimIndex())	// Á¡ÇÁ
		{
			if (0 == m_iEvent_Index)
			{// 0.07
				Create_GroundSmoke(CGroundSmoke::SMOKE_JUMP);
			}
		}

		if (55 == m_pModelCom->Get_iCurrentAnimIndex())	// Á¡ÇÁ ÂøÁö
		{
			if (0 == m_iEvent_Index)
			{// 0.00
				Create_GroundSmoke(CGroundSmoke::SMOKE_DASHLAND);
			}
		}

		if (57 == m_pModelCom->Get_iCurrentAnimIndex())	// Run
		{
			if (0 == m_iEvent_Index)
			{// 0.00
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
			else if (1 == m_iEvent_Index)
			{// 0.20
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
			else if (2 == m_iEvent_Index)
			{// 0.44
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
		}

		if (58 == m_pModelCom->Get_iCurrentAnimIndex())	// Run End
		{
			if (0 == m_iEvent_Index)
			{// 0.10
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
		}

		if (ANIM_STEP_BACK == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{// 0.00
				Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);
			}
		}

		if (ANIM_STEP_FRONT == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{// 0.00
				Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);
			}
		}

		if (ANIM_STEP_LEFT == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{// 0.00
				Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);
			}
		}

		if (ANIM_STEP_RIGHT == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{// 0.4
				Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);
			}
		}

		if (93 == m_pModelCom->Get_iCurrentAnimIndex()) // Blow Fall
		{
			if (0 == m_iEvent_Index)
			{// 0.04
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_FallDownEffect();
				Play_Sound_BodyFall();
			}
		}

		if (94 == m_pModelCom->Get_iCurrentAnimIndex()) // Blow µ¥±¸¸£¸£
		{
			if (0 == m_iEvent_Index)
			{// 0.25
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_Sound_BodyFall();
			}
			else if (1 == m_iEvent_Index)
			{// 0.52
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
			}
			else if (2 == m_iEvent_Index)
			{// 0.96
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
			}
			else if (3 == m_iEvent_Index)
			{// 1.75
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_Sound_BodyFall();
			}
		}

		if (95 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{// 0.45
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_FallDownEffect();
				Play_Sound_BodyFall();
			}
		}

		if (97 == m_pModelCom->Get_iCurrentAnimIndex()) // BoundFall
		{
			if (0 == m_iEvent_Index)
			{// 0.04
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_FallDownEffect();
				Play_Sound_BodyFall();
			}
		}


		if (112 == m_pModelCom->Get_iCurrentAnimIndex()) // Fall
		{
			if (0 == m_iEvent_Index)
			{// 0.05
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_FallDownEffect();
				Play_Sound_BodyFall();
			}
		}


		if (118 == m_pModelCom->Get_iCurrentAnimIndex()) // SpinFall
		{
			if (0 == m_iEvent_Index)
			{// 0.00
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);
				Play_FallDownEffect();
				Play_Sound_BodyFall();
			}
		}

#pragma endregion
		m_iEvent_Index++;
	}
}

void CMonster_Zako::Trigger()
{
	//Hit_Trigger
	if (m_eCurState != STATE_DOWN)
	{
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Big()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Spin()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Upper()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Hekireki())
		{
			m_eCurState = STATE_HIT;
		}
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
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 1));

			pPlayer->Set_Hit_Success_Hekireki(true);

			Safe_Release(pGameInstance);

			m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);
		}
		else
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);
		}
	}
}

void CMonster_Zako::Animation_Control(_double dTimeDelta)
{
	if (m_isDeath_Motion)
	{
		m_bMonsterDead = true;
		m_fDeadTime += (_float)dTimeDelta;

		m_dDeadParticleAccTime += dTimeDelta;
		m_dDeadSmokeAccTime += dTimeDelta;

		if (m_fDeadTime > 1.8f && m_fDeadTime < 7.5f)
		{
			if (m_fDeadTime > 2.2f)
			{
				if (m_dDeadParticleAccTime > 1.4)
				{
					m_dDeadParticleAccTime = 0.0;
					CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
					EffectWorldDesc.vPosition.x += Random::Generate_Float(-0.3f, 0.3f);
					EffectWorldDesc.vPosition.z += Random::Generate_Float(-0.3f, 0.3f);
					CEffectPlayer::Get_Instance()->Play("Death_Particle", m_pTransformCom, &EffectWorldDesc);
				}
			}

			if (m_dDeadSmokeAccTime > 1.0)
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_DEAD_NORMAL);
				m_dDeadSmokeAccTime = 0.0;
			}
		}

		if (m_fDeadTime > 10.0f) // Á×´Â ½Ã°£ Á¶Àý ÇØµµ µÊ
			m_isDead = true;

	}
	else
	{
		if (m_eCurState == STATE_DOWN)
			Animation_Control_Down(dTimeDelta);
		else if (m_eCurState == STATE_HIT)
			Animation_Control_Hit(dTimeDelta);
		else if (m_eCurState == STATE_IDLE)
			Animation_Control_Idle(dTimeDelta);
		else if (m_eCurState == STATE_ATTACK)
			Animation_Control_Attack(dTimeDelta, m_eCurPattern);
	}
}

void CMonster_Zako::Animation_Control_Idle(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.05f);
	_float fDistance = Calculate_Distance();

	m_dDelay_Move += dTimeDelta;
	//¾Õ µÚ ÀÌµ¿
	if (fDistance < 4.5f)
	{
		if (m_isFirst_Move_0 && m_dDelay_Move > 1.0f)
		{
			m_isFirst_Move_0 = false;
			m_isFirst_Move_1 = true;

			m_pModelCom->Set_Animation(ANIM_WALK_BACK);
			m_dDelay_Move = 0.0;
		}
	}
	else if (4.5f <= fDistance && fDistance < 7.0f)
	{
		if (m_isFirst_Move_1 && m_dDelay_Move > 1.0f)
		{
			m_isFirst_Move_0 = true;
			m_isFirst_Move_1 = false;

			m_dDelay_Move = 0.0;
			m_pModelCom->Set_Animation(ANIM_IDLE);
		}

		//¿· ÀÌµ¿
		m_dCoolTime_SideMove += dTimeDelta;
		if (3.7f < m_dCoolTime_SideMove)
		{
			m_dCoolTime_SideMove = 0.0;

			if (m_isSideMoveOn)
			{
				m_isSideMoveOn = false;
				m_pModelCom->Set_Animation(ANIM_IDLE);
			}
			else
			{
				m_isSideMoveOn = true;
				m_isFirst_SideMove = true;
			}

			if (m_isSideMoveOn)
			{
				if (m_isSideMove_Left)
					m_isSideMove_Left = false;
				else
					m_isSideMove_Left = true;
			}
		}

		if (m_isSideMoveOn)
		{
			if (m_isFirst_SideMove)
			{
				m_isFirst_SideMove = false;

				if (m_isSideMove_Left)
					m_pModelCom->Set_Animation(ANIM_WALK_LEFT);
				else
					m_pModelCom->Set_Animation(ANIM_WALK_RIGHT);
			}

			if (m_isSideMove_Left)
			{
				Go_Left_Constant(dTimeDelta, ANIM_WALK_LEFT, 0.3f);
				Go_Left_Constant(dTimeDelta, 72, 0.3f);
			}
			else
			{
				Go_Right_Constant(dTimeDelta, ANIM_WALK_RIGHT, 0.3f);
				Go_Right_Constant(dTimeDelta, 75, 0.3f);
			}
		}
	}
	else if (7.0f <= fDistance)
	{
		if (m_isFirst_Move_0 && m_dDelay_Move > 1.0f)
		{
			m_isFirst_Move_0 = false;
			m_isFirst_Move_1 = true;

			m_pModelCom->Set_Animation(ANIM_WALK_FRONT);
			m_dDelay_Move = 0.0;
		}
	}
	Go_Backward_Constant(dTimeDelta, ANIM_WALK_BACK, 0.3f);
	Go_Backward_Constant(dTimeDelta, 66, 0.3f);
	Go_Straight_Constant(dTimeDelta, ANIM_WALK_FRONT, 0.3f);
	Go_Straight_Constant(dTimeDelta, 69, 0.3f);

	//»ç¿îµå
	/*if (iCurAnim == ANIM_WALK_LEFT || iCurAnim == ANIM_WALK_RIGHT || iCurAnim == ANIM_WALK_BACK || iCurAnim == ANIM_WALK_FRONT
		|| iCurAnim == 72 || iCurAnim == 75 || iCurAnim == 66 || iCurAnim == 69)
	{
		Play_Sound_Move(dTimeDelta, 0.65f);
	}*/


	Idle_ATK_Pattern_Controler(dTimeDelta);
}

void CMonster_Zako::Idle_ATK_Pattern_Controler(_double dTimeDelta)
{
	// Attack CoolTime
	if (m_isCoolTime_On)
		m_dCoolTime_AtkPattern += dTimeDelta;

#pragma region Cheat
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	/*enum PATTERN { PATTERN_CLAWS, PATTERN_TACKLE, PATTERN_SPINKICK, PATTERN_JUMPKICK,
		PATTERN_BUTTERFLY, PATTERN_CLAWCROSS,
		PATTERN_SPINMOVE, PATTERN_MOVE,
		PATTERN_END };*/

	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL))
	{
		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
			m_iAttackIndex = 10;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_CLAWS;
		}
		else if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			m_iAttackIndex = 10;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_TACKLE;
		}
		else if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			m_iAttackIndex = 10;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_SPINKICK;
		}
		else if (pGameInstance->Get_DIKeyDown(DIK_4))
		{
			m_iAttackIndex = 10;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_JUMPKICK;
		}
	}
	
	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT))
	{
		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
			m_iAttackIndex = 10;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_BUTTERFLY;
		}
		else if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			m_iAttackIndex = 10;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_CLAWCROSS;
		}
		else if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			m_iAttackIndex = 10;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_SPINMOVE;
		}
		else if (pGameInstance->Get_DIKeyDown(DIK_4))
		{
			m_iAttackIndex = 10;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_MOVE;
		}
	}
	
	Safe_Release(pGameInstance);
#pragma endregion


	if (m_iAttackIndex == 0)
	{
		if (5.1f < m_dCoolTime_AtkPattern)
		{
			m_dCoolTime_AtkPattern = 0.0;

			m_isCoolTime_On = false;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_CLAWS;
		}
	}
	if (m_iAttackIndex == 1 && m_isAtkFinish)
	{
		m_isAtkFinish = false;
		m_eCurState = STATE_ATTACK;
		m_isFirst_AtkPattern = true;

		m_eCurPattern = PATTERN_SPINKICK;
	}


	if (m_iAttackIndex == 2 && m_isAtkFinish)
	{
		m_isCoolTime_On = true;
		m_isAtkFinish = false;
	}
	if (m_iAttackIndex == 2)
	{
		if (7.1f < m_dCoolTime_AtkPattern)
		{
			m_dCoolTime_AtkPattern = 0.0;
			m_isCoolTime_On = false;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_SPINMOVE;
		}
	}
	if (m_iAttackIndex == 3 && m_isAtkFinish)
	{
		m_isAtkFinish = false;
		m_eCurState = STATE_ATTACK;
		m_isFirst_AtkPattern = true;

		m_eCurPattern = PATTERN_CLAWCROSS;
	}
	if (m_iAttackIndex == 4 && m_isAtkFinish)
	{
		m_isAtkFinish = false;
		m_eCurState = STATE_ATTACK;
		m_isFirst_AtkPattern = true;

		m_eCurPattern = PATTERN_BUTTERFLY;
	}



	if (m_iAttackIndex == 5 && m_isAtkFinish)
	{
		m_isCoolTime_On = true;
		m_isAtkFinish = false;
	}
	if (m_iAttackIndex == 5)
	{
		if (7.1f < m_dCoolTime_AtkPattern)
		{
			m_dCoolTime_AtkPattern = 0.0;
			m_isCoolTime_On = false;

			m_eCurState = STATE_ATTACK;
			m_isFirst_AtkPattern = true;

			m_eCurPattern = PATTERN_JUMPKICK;
		}
	}
	if (m_iAttackIndex == 6 && m_isAtkFinish)
	{
		m_isAtkFinish = false;
		m_eCurState = STATE_ATTACK;
		m_isFirst_AtkPattern = true;

		m_eCurPattern = PATTERN_CLAWS;
	}
	if (m_iAttackIndex == 7 && m_isAtkFinish)
	{
		m_isAtkFinish = false;
		m_eCurState = STATE_ATTACK;
		m_isFirst_AtkPattern = true;

		m_eCurPattern = PATTERN_TACKLE;
	}
	if (m_iAttackIndex == 8 && m_isAtkFinish)
	{
		m_isAtkFinish = false;
		m_eCurState = STATE_ATTACK;
		m_isFirst_AtkPattern = true;

		m_eCurPattern = PATTERN_CLAWCROSS;
	}
	if (m_iAttackIndex == 9 && m_isAtkFinish)
	{
		m_isAtkFinish = false;

		m_iAttackIndex = 0;
		m_isCoolTime_On = true;
	}
}

void CMonster_Zako::Animation_Control_Attack(_double dTimeDelta, _int AttackIndex)
{
	switch (AttackIndex)
	{
	case 0: //ANIM_ATK_CLAWS
		Animation_Control_Attack_Claws(dTimeDelta);
		break;
	case 1: // ANIM_ATK_TACKLE
		Animation_Control_Attack_Tackle(dTimeDelta);
		break;
	case 2: // ANIM_ATK_SPINKICK
		Animation_Control_Attack_Spinkick(dTimeDelta);
		break;
	case 3: // ANIM_ATK_JUMPKICK
		Animation_Control_Attack_Jumpkick(dTimeDelta);
		break;
	case 4: // ANIM_ATK_BUTTERFLY
		Animation_Control_Attack_ButterFly(dTimeDelta);
		break;
	case 5: // ANIM_ATK_CLAWCROSS
		Animation_Control_Attack_Cross(dTimeDelta);
		break;
	case 6: // ANIM_ATK_SpinMove
		Animation_Control_Attack_SpinMove(dTimeDelta);
		break;
	case 7: // Dash

		break;
	default:
		break;
	}


	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_IDLE)
	{
		m_eCurState = STATE_IDLE;
		m_isFirst_Move_0 = true;
		m_isFirst_Move_1 = true;

		m_isAtkFinish = true;
		m_iAttackIndex++;
	}
}

void CMonster_Zako::Animation_Control_Attack_Claws(_double dTimeDelta)
{
	if (m_isFirst_AtkPattern)
	{
		m_isFirst_AtkPattern = false;

		if (2.5f < Calculate_Distance())
		{
			m_pModelCom->Set_Animation(ANIM_RUN);
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_ATK_CLAWS);
		}
		m_isFirst_ATK_0 = true;

		_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Shout_Hoooooggggh.mp3");
		if(!m_bDialog)
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.5f);
		else
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.1f);
	}

	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	//´Þ¸®´Â µµÁß
	if (iCurAnim == ANIM_RUN)
	{
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.2f);
		if (2.5f < Calculate_Distance())
			Go_Straight_Constant(dTimeDelta, ANIM_RUN, 1.6f);
		else
		{
			if (m_isFirst_ATK_0)
			{
				m_isFirst_ATK_0 = false;

				m_pModelCom->Set_Animation(58);
			}
		}

		//Play_Sound_Move(dTimeDelta, 0.14f);
	}
	if (iCurAnim == 58)
	{
		if (m_pModelCom->Get_AnimFinish(58))
			m_pModelCom->Set_Animation(ANIM_ATK_CLAWS);
	}

}

void CMonster_Zako::Animation_Control_Attack_Tackle(_double dTimeDelta)
{
	if (m_isFirst_AtkPattern)
	{
		m_isFirst_AtkPattern = false;

		if (3.5f < Calculate_Distance())
			m_pModelCom->Set_Animation(ANIM_ATK_TACKLE);
		else
			m_pModelCom->Set_Animation(ANIM_ATK_CLAW_FROG);

		m_isFirst_ATK_0 = true;

		_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Talk_WasiniwaKatene.mp3");
		if (!m_bDialog)
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.5f);
		else
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.1f);
	}

	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_ATK_TACKLE)
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.35f);

	if (iCurAnim == 8)
	{
		if (4.5f < Calculate_Distance())
			m_pModelCom->Set_Animation(9);
	}
	Go_Straight_Constant(dTimeDelta, 8, 1.5f);

	Go_Straight_Deceleration(dTimeDelta, 9, 1.5f, 0.015f);

	if (iCurAnim == 10)
	{
		if (m_pModelCom->Get_AnimFinish(10))
		{
			m_pModelCom->Set_Animation(ANIM_ATK_CLAW_FROG);
		}
	}

	if (iCurAnim == ANIM_ATK_CLAW_FROG || iCurAnim == 23 || iCurAnim == 24 || iCurAnim == 25 || iCurAnim == 26)
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.05f);

	Go_Straight_Deceleration(dTimeDelta, 26, 2.0f, 0.03f);
}

void CMonster_Zako::Animation_Control_Attack_Spinkick(_double dTimeDelta)
{
	if (m_isFirst_AtkPattern)
	{
		m_isFirst_AtkPattern = false;

		m_pModelCom->Set_Animation(ANIM_ATK_SPINKICK);

		m_isFirst_ATK_0 = true;

		_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Talk_ZankokuniKorositeYarojo.mp3");
		if (!m_bDialog)
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.5f);
		else
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.1f);
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_ATK_SPINKICK || iCurAnim == 17)
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.35f);

	Go_Straight_Constant(dTimeDelta, 18, 2.0f);
	Go_Straight_Constant(dTimeDelta, 19, 2.0f);
	Go_Straight_Deceleration(dTimeDelta, 20, 2.0f, 0.034f);
}

void CMonster_Zako::Animation_Control_Attack_Jumpkick(_double dTimeDelta)
{
	if (m_isFirst_AtkPattern)
	{
		m_isFirst_AtkPattern = false;

		m_pModelCom->Set_Animation(ANIM_ATK_JUMPKICK);

		m_isFirst_ATK_0 = true;
		m_isFirst_ATK_1 = true;
		m_isTime_ATK_0 = false;
		m_dTime_ATK_0 = 0.0;

		_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Talk_UHahahahaha.mp3");

		if (!m_bDialog)
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.5f);
		else
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.1f);
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_ATK_JUMPKICK || iCurAnim == 12)
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.35f);

	if (iCurAnim == 12 && m_isFirst_ATK_0)
	{
		m_isFirst_ATK_0 = false;

		Jumping(2.0f, 0.03f);
	}
	Go_Straight_Deceleration(dTimeDelta, 12, 2.0f, 0.04f);


	if (iCurAnim == 13 && m_isFirst_ATK_1)
	{
		m_isFirst_ATK_1 = false;

		JumpStop(0.2f);
		Set_FallingStatus(4.0f, 0.0f);

		m_isTime_ATK_0 = true;
		m_dTime_ATK_0 = 0.0;
	}

	if (m_isTime_ATK_0)
		m_dTime_ATK_0 += dTimeDelta;

	if (m_dTime_ATK_0 <= 0.2f)
	{
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.55f);
	}
	else if (0.2f < m_dTime_ATK_0)
	{
		Go_Straight_Constant(dTimeDelta, 13, 4.5f);
	}

	Ground_Animation_Play(13, 14);
}

void CMonster_Zako::Animation_Control_Attack_ButterFly(_double dTimeDelta)
{
	if (m_isFirst_AtkPattern)
	{
		m_isFirst_AtkPattern = false;

		m_pModelCom->Set_Animation(ANIM_ATK_BUTTERFLY);

		m_isFirst_ATK_0 = true;
		m_isFirst_ATK_1 = true;

		m_isTime_ATK_0 = false;
		m_dTime_ATK_0 = 0.0;

		_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Talk_Hehehehe.mp3");
		if (!m_bDialog)
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.5f);
		else
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.1f);
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_ATK_BUTTERFLY || iCurAnim == 28 || iCurAnim == 29)
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.1f);


	if (iCurAnim == 28 && m_isFirst_ATK_0)
	{
		m_isFirst_ATK_0 = false;

		Jumping(2.0f, 0.037f);
	}
	Go_Backward_Deceleration(dTimeDelta, 28, 3.5f, 0.037f);

	if (iCurAnim == 29 && m_isFirst_ATK_1)
	{
		m_isFirst_ATK_1 = false;

		JumpStop(0.4f);
	}


	Go_Straight_Constant(dTimeDelta, 30, 4.0f);
	Go_Straight_Deceleration(dTimeDelta, 31, 4.0f, 0.075f);


}

void CMonster_Zako::Animation_Control_Attack_Cross(_double dTimeDelta)
{
	if (m_isFirst_AtkPattern)
	{
		m_isFirst_AtkPattern = false;

		m_pModelCom->Set_Animation(ANIM_ATK_CLAW_CROSS);

		m_isFirst_ATK_0 = true;
		m_isFirst_ATK_1 = true;

		m_isTime_ATK_0 = true;
		m_dTime_ATK_0 = 0.0;

		_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Shout_Sirumonoga.mp3");
		if (!m_bDialog)
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.5f);
		else
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.1f);
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_ATK_CLAW_CROSS)
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.05f);

	if (m_isTime_ATK_0)
	{
		m_dTime_ATK_0 += dTimeDelta;

		if (0.45f <= m_dTime_ATK_0 && m_dTime_ATK_0 < 1.85f)
		{
			Go_Straight_Constant(dTimeDelta, ANIM_ATK_CLAW_CROSS, 0.8f);
		}
	}

}

void CMonster_Zako::Animation_Control_Attack_SpinMove(_double dTimeDelta)
{
	if (m_isFirst_AtkPattern)
	{
		m_isFirst_AtkPattern = false;

		m_pModelCom->Set_Animation(ANIM_ATK_SPINPUNCH);

		m_isFirst_ATK_0 = true;
		m_isFirst_ATK_1 = true;

		m_isTime_ATK_0 = false;
		m_dTime_ATK_0 = 0.0;

		_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Shout_Hue.mp3");
		if (!m_bDialog)
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.5f);
		else
			Play_Sound_Channel(szSoundFile, m_SoundChr, 0.1f);
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_ATK_SPINPUNCH)
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.3f);

	if (iCurAnim == 44 && m_isFirst_ATK_0)
	{
		m_isFirst_ATK_0 = false;

		Jumping(1.5f, 0.07f);
	}

	if (iCurAnim == 44)
	{
		m_dTime_ATK_0 += dTimeDelta;
		if (0.1f < m_dTime_ATK_0)
			Ground_Animation_Play(44, 45);
	}

	Go_Straight_Constant(dTimeDelta, 44, 1.8f);
	Go_Straight_Deceleration(dTimeDelta, 45, 1.8f, 0.045f);
}

void CMonster_Zako::Animation_Control_Hit(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_int PlayerIndex = CPlayerManager::GetInstance()->Get_PlayerIndex();
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), PlayerIndex));

	_float4 AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();

	AtkDir.y = 0.0f;
	XMStoreFloat4(&AtkDir, XMVector4Normalize(XMLoadFloat4(&AtkDir)));

	m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.05f);

#pragma region Hit_Small
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small() || m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
	{
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
			m_isConnectHitting = false;

			if (PlayerIndex == 0) {
				Play_HitEffect();
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.3f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.7f;
				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
			else {
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.3f;

				int n = Random::Generate_Int(0, 2);

				if (n == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_Hit_Small_1", m_pTransformCom, &EffectWorldDesc);
				else if (n == 1)
					CEffectPlayer::Get_Instance()->Play("Zen_Hit_Small_2", m_pTransformCom, &EffectWorldDesc);
				else if (n == 2)
					CEffectPlayer::Get_Instance()->Play("Zen_Hit_Small_3", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.7f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
		}
		else if (m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
			m_isConnectHitting = true;

			if (PlayerIndex == 0) {
				Play_HitEffect();
				CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.3f;
				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.7f;
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

				EffectWorldDesc.fScale = 1.f;
				EffectWorldDesc.vPosition.y += 0.3f;

				int n = Random::Generate_Int(0, 2);

				if (n == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_Hit_Small_1", m_pTransformCom, &EffectWorldDesc);
				else if (n == 1)
					CEffectPlayer::Get_Instance()->Play("Zen_Hit_Small_2", m_pTransformCom, &EffectWorldDesc);
				else if (n == 2)
					CEffectPlayer::Get_Instance()->Play("Zen_Hit_Small_3", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.y -= 2.5f;
				EffectWorldDesc.vPosition.z -= 0.7f;

				CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
			}
		}

		m_dDelay_ComboChain = 1.0;
		pPlayer->Set_Hit_Success(true); // ÇÃ·¹ÀÌ¾î°¡ ¸Â¾Òµû
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isJumpOn)
		{
			m_pModelCom->Set_Animation(ANIM_FALL);
			Jumping(0.3f, 0.030f);
			m_dDelay_ComboChain = 6.0;
		}
		else
		{
			if (m_iSmallHit_Index == 0)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_SMALL_FRONT);
				m_iSmallHit_Index++;
			}
			else if (m_iSmallHit_Index == 1)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_SMALL_LEFT);
				m_iSmallHit_Index++;
			}
			else if (m_iSmallHit_Index == 2)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_SMALL_RIGHT);
				m_iSmallHit_Index = 0;
			}
		}
		Play_Sound_Dmg(0, 0.9f);
	}
	if (m_isConnectHitting == false)
	{
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_FRONT, 1.1f, 0.04f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_LEFT, 1.1f, 0.04f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_RIGHT, 1.1f, 0.04f, AtkDir);
	}


#pragma endregion


#pragma region Hit_Big
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Big())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);

		pPlayer->Set_Hit_Success(true);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();


		if (m_isJumpOn)
		{
			m_pModelCom->Set_Animation(ANIM_DMG_BLOW);
			m_dDelay_ComboChain = 4.0;
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_DMG_BIG_FRONT);
			m_dDelay_ComboChain = 1.7;
		}

		if (PlayerIndex == 0) {
			Play_HitEffect();
			CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
			CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
			EffectWorldDesc.vPosition.y += 0.3f;
			EffectWorldDesc.vPosition.y -= 2.5f;
			EffectWorldDesc.vPosition.z -= 0.7f;
			CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
		}
		else {
			CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

			EffectWorldDesc.fScale = 1.f;
			EffectWorldDesc.vPosition.y += 0.3f;
			CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);

		}
		//pGameInstance->Time_Slow(0.3, 0.15);
		Play_Sound_Dmg(0, 0.9f);
	}
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_BIG_FRONT, 2.0f, 0.05f, AtkDir);
#pragma endregion


#pragma region Hit_Upper
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Upper())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);

		m_dDelay_ComboChain = 6.0;
		pPlayer->Set_Hit_Success(true);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		m_pModelCom->Set_Animation(ANIM_FALL);
		Jumping(1.5f, 0.035f); // 1.5

		if (PlayerIndex == 0) {
			Play_HitEffect();
			CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
		}
		else {
			CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

			EffectWorldDesc.fScale = 1.f;
			EffectWorldDesc.vPosition.y += 0.3f;
			CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);

		}
		//pGameInstance->Time_Slow(0.23, 0.3);
		Play_Sound_Dmg(0, 0.9f);
	}

	//¾îÆÛ½Ã ¼öÁ÷»ó½Â ¿©ºÎ
	if (m_isStrictUpper == false)
	{
		Go_Dir_Constant(dTimeDelta, ANIM_FALL, 0.35f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 111, 0.35f, 0.01f, AtkDir);
	}

	if (m_isBounding)
	{
		Ground_Animation_Play(111, ANIM_DMG_BOUND);
	}
	else
		Ground_Animation_Play(111, 112);
#pragma endregion


#pragma region Hit_Bound
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Bound())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Bound(false);

		m_dDelay_ComboChain = 15.0;
		m_isBounding = true;
		pPlayer->Set_Hit_Success(true);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		m_isFirst_Anim = true;
		if (m_isJumpOn)
		{
			m_pModelCom->Set_Animation(ANIM_FALL);
			Set_FallingStatus(3.0f, 0.0f);
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_DMG_BOUND);
			//pGameInstance->Time_Slow(0.15, 0.1);
		}

		if (PlayerIndex == 0) {
			Play_HitEffect();
			CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
			CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
			EffectWorldDesc.vPosition.y += 0.3f;
			EffectWorldDesc.vPosition.y -= 2.5f;
			EffectWorldDesc.vPosition.z -= 0.7f;
			CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
		}
		else {
			CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

			
			EffectWorldDesc.fScale = 1.f;
			EffectWorldDesc.vPosition.y += 0.3f;
			CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);

		
		}

		Play_Sound_Dmg(1, 0.9f);
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_BOUND, 0.3f, AtkDir);
	Go_Dir_Constant(dTimeDelta, 97, 0.3f, AtkDir);

	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_DMG_BOUND && m_isBounding)
	{
		if (m_isFirst_Anim)
		{
			m_isFirst_Anim = false;
			m_isBounding = false;

			Jumping(2.25f, 0.065f);//2.0

			//pGameInstance->Time_Slow(0.25, 0.1);
		}
	}
#pragma endregion


#pragma region Hit_Blow
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);

		m_dDelay_ComboChain = 2.5;
		pPlayer->Set_Hit_Success(true);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		m_pModelCom->Set_Animation(ANIM_DMG_BLOW);
		Jumping(1.4f, 0.05f); //1.2

		if (PlayerIndex == 0) {
			Play_HitEffect();
			CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
			CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
			EffectWorldDesc.vPosition.y += 0.3f;
			EffectWorldDesc.vPosition.y -= 2.5f;
			EffectWorldDesc.vPosition.z -= 0.7f;
			CEffectPlayer::Get_Instance()->Play("Zen_Hit_Particle", m_pTransformCom, &EffectWorldDesc);
		}
		else {
			CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

			EffectWorldDesc.fScale = 1.f;
			EffectWorldDesc.vPosition.y += 0.3f;
			CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);

			
		}
		//pGameInstance->Time_Slow(0.6, 0.4);

		Play_Sound_Dmg(1, 0.9f);
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_BLOW, 2.5f, AtkDir);
	Go_Dir_Constant(dTimeDelta, 92, 2.5f, AtkDir);
	Ground_Animation_Play(92, 93);
#pragma endregion



#pragma region Hit_CutScene
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_CutScene())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_CutScene(false);

		pPlayer->Set_Hit_SurgeCutScene(true);
		pPlayer->Set_Hit_Success(true);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		m_dDelay_ComboChain = 5.5;

		m_pModelCom->Set_Animation(ANIM_DEATH);

		//Play_HitEffect();

		m_isSurging = true;


		Play_Sound_Dmg(1, 0.9f);
	}
#pragma endregion


#pragma region Hit_HEKIREKI
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Hekireki())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);

		m_dDelay_ComboChain = 5.5;
		pPlayer->Set_Hit_Success(true);
		pPlayer->Set_Hit_Success_Hekireki(true);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();


		m_isStrictUpper = true;


		m_isHekireki_Hit = true;
		m_dHekireki_Hit = 0.0;



		CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;

		

		EffectWorldDesc.fScale = 1.f;
		EffectWorldDesc.vPosition.y += 0.3f;
		CEffectPlayer::Get_Instance()->Play("Zen_Big_Hit", m_pTransformCom, &EffectWorldDesc);
		
		
		if (m_isJumpOn == false)
		{
			Jumping(1.5f, 0.035f); // 1.85
		}
		else
		{
			Jumping(1.05f, 0.030f); // 0.85
		}
		if (m_iHekirekiHit_Index == 0)
		{
			m_pModelCom->Set_Animation(ANIM_DMG_SPIN);
			m_iHekirekiHit_Index++;
		}
		else if (m_iHekirekiHit_Index == 1)
		{
			m_pModelCom->Set_Animation(ANIM_FALL);
			m_iHekirekiHit_Index = 0;
		}

		//pGameInstance->Time_Slow(0.2, 0.1);
		Play_Sound_Dmg(0, 0.9f);
	}


	//Go_Dir_Constant(dTimeDelta, ANIM_DMG_SPIN, 0.2f, AtkDir);
	//Go_Dir_Constant(dTimeDelta, 117, 0.3f, AtkDir);
	if (m_isHekireki_Hit)
	{
		m_dHekireki_Hit += dTimeDelta;
		if (m_dHekireki_Hit > 0.09f) // 0.15f
		{
			/*if (m_isJumpOn == false)
			{
				Jumping(1.85f, 0.03f);
			}
			else
			{
				Jumping(0.85f, 0.030f);
			}

			if (m_iHekirekiHit_Index == 0)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_SPIN);
				m_iHekirekiHit_Index++;
			}
			else if (m_iHekirekiHit_Index == 1)
			{
				m_pModelCom->Set_Animation(ANIM_FALL);
				m_iHekirekiHit_Index = 0;
			}*/

			m_isHekireki_Hit = false;
		}
	}
	if (m_dHekireki_Hit >= 0.088f)
	{
		Ground_Animation_Play(117, 118);
	}

#pragma endregion



#pragma region Death_Motion
	if (m_StatusDesc.fHp <= 0.0f)
	{
		m_pModelCom->Set_Animation(ANIM_DEATH);

		if (m_isFirst_Death_For_Stealth)
		{
			m_isFirst_Death_For_Stealth = false;

			CMonsterManager::GetInstance()->Plus_ThreeCnt();

			pGameInstance->Time_Slow(0.6, 0.3);
		}
	}
#pragma endregion


	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	m_dDelay_ComboChain -= dTimeDelta;
	if (m_dDelay_ComboChain <= 0.0f)
	{
		m_dDelay_ComboChain = 0.0;

		m_isFirst_Move_0 = true;
		m_isFirst_Move_1 = true;
		m_isCoolTime_On = true;

		m_isFirst_Anim = true;

		m_eCurState = STATE_IDLE;

		if (m_iAttackIndex != 10)
		{
			_int i = rand() % 3;
			if (i == 0)
				m_iAttackIndex = 0;
			else if (i == 1)
				m_iAttackIndex = 2;
			else if (i == 2)
				m_iAttackIndex = 5;
		}
	}

	if (iCurAnim == ANIM_DOWN_IDLE || iCurAnim == ANIM_DEATH || iCurAnim == 112) //112´Â fall¸¶Áö¸· ¸ð¼Ç
	{
		m_dDelay_ComboChain = 0.0;

		m_isFirst_Move_0 = true;
		m_isFirst_Move_1 = true;
		m_isCoolTime_On = true;

		m_isFirst_Anim = true;

		m_eCurState = STATE_DOWN;

		if (m_iAttackIndex != 10)
		{
			_int i = rand() % 3;
			if (i == 0)
				m_iAttackIndex = 0;
			else if (i == 1)
				m_iAttackIndex = 2;
			else if (i == 2)
				m_iAttackIndex = 5;
		}


	}

	Safe_Release(pGameInstance);
}

void CMonster_Zako::Animation_Control_Down(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_DEATH && m_StatusDesc.fHp <= 0.0f)
	{
		m_isDeath_Motion = true;
	}


	if (iCurAnim == ANIM_DOWN_IDLE || iCurAnim == ANIM_DEATH_IDLE)
	{
		m_dDelay_Down += dTimeDelta;
		if (m_dDelay_Down > 0.6f)
		{
			m_dDelay_Down = 0.0;

			if (iCurAnim == ANIM_DEATH_IDLE)
				m_pModelCom->Set_Animation(ANIM_DEATH_GETUP);
			else
				m_pModelCom->Set_Animation(ANIM_DOWN_GETUP_MOVE);
		}
		m_isSurging = false;
	}

	if (iCurAnim == ANIM_IDLE)
	{
		m_eCurState = STATE_IDLE;
	}

}

void CMonster_Zako::Play_Sound_Dmg(_int iType, _float vol)
{
	//small
	if (iType == 0)
	{
		if (m_iSound_Dmg_Small == 0)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Small_Chae.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
		else if (m_iSound_Dmg_Small == 1)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Small_Keh.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
		else if (m_iSound_Dmg_Small == 2)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Small_Choa.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
		else if (m_iSound_Dmg_Small == 3)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Small_Gk.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
		else if (m_iSound_Dmg_Small == 4)
		{
			m_iSound_Dmg_Small++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Small_DDeuk.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
		else if (m_iSound_Dmg_Small == 5)
		{
			m_iSound_Dmg_Small = 0;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Small_Ggowak.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
	}
	// Medium
	else if (iType == 1)
	{
		if (m_iSound_Dmg_Medium == 0)
		{
			m_iSound_Dmg_Medium++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Middle_Juuaak.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
		else if (m_iSound_Dmg_Medium == 1)
		{
			m_iSound_Dmg_Medium++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Middle_Zzum.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
		else if (m_iSound_Dmg_Medium == 2)
		{
			m_iSound_Dmg_Medium++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Middle_Uuummm.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
		else if (m_iSound_Dmg_Medium == 3)
		{
			m_iSound_Dmg_Medium = 0;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Middle_Yaaghk.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
	}
	// Big
	else if (iType == 2)
	{
		if (m_iSound_Dmg_Big == 0)
		{
			m_iSound_Dmg_Big++;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Big_Euak.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
		else if (m_iSound_Dmg_Big == 1)
		{
			m_iSound_Dmg_Big = 0;

			_tchar szSoundFile[MAX_PATH] = TEXT("Zako_Dmg_Big_Kkyahak.mp3");
			Play_Sound_Channel(szSoundFile, m_SoundChr, vol);
		}
	}
}

void CMonster_Zako::Dialog_Update(_double dTimeDelta)
{
	m_dDialogAccTime += dTimeDelta;

	if (Event_Time(dTimeDelta, 5.f, m_dDialogAccTime))
	{
		Set_CharacterDialog(4.f, TEXT("[Ç÷±Í]"), TEXT("¹¹¾ß,³× ³à¼®! À¸ÇÏÇÏ!"));
		_tchar szSoundFile[MAX_PATH] = TEXT("V_MAIN04#385.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 1.f);
	}
	else if (Event_Time(dTimeDelta, 9.5f, m_dDialogAccTime))
	{
		Set_CharacterDialog(3.f, TEXT("[¾Æ°¡Ã÷¸¶ Á¨ÀÌÃ÷]"), TEXT("½¬ÀÌÀÌÀÌÀÍ"));
		_tchar szSoundFile[MAX_PATH] = TEXT("V_MAIN04#386.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 1.f);
	}
	else if (Event_Time(dTimeDelta, 13.f, m_dDialogAccTime))
	{
		Set_CharacterDialog(7.f, TEXT("[Ç÷±Í]"), TEXT("(...ÀÌ ³à¼®, ÀÚ´Â Ã´ÇÏ´Â°Ô ¾Æ´Ï¶ó"),TEXT("ÁøÂ¥·Î °ô¾Æ¶³¾îÁ³ÀÝ¾Æ)"));
		_tchar szSoundFile[MAX_PATH] = TEXT("V_MAIN04#387.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 1.f);
	}
	else if (Event_Time(dTimeDelta, 20.5f, m_dDialogAccTime))
	{
		Set_CharacterDialog(6.f, TEXT("[Ç÷±Í]"), TEXT("(±×·±µ¥µµ... ³¯·ÆÇØ!"), TEXT("°¡, °­ÇØ!)"));
		_tchar szSoundFile[MAX_PATH] = TEXT("V_MAIN04#388.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 1.f);
	}
	else if (Event_Time(dTimeDelta, 27.5f, m_dDialogAccTime))
	{
		Set_CharacterDialog(5.f, TEXT("[¾Æ°¡Ã÷¸¶ Á¨ÀÌÃ÷]"), TEXT("½¬ÀÌÀÌÀÌÀÌÀÌÀÌÀÌÀÍ"));
		_tchar szSoundFile[MAX_PATH] = TEXT("V_MAIN04#389.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 1.f);
	}
	else if (Event_Time(dTimeDelta, 33.f, m_dDialogAccTime))
	{
		Set_CharacterDialog(2.f, TEXT("[Ç÷±Í]"), TEXT("Á¨Àå! ½Ã²ô·¯¿ö!"));
		_tchar szSoundFile[MAX_PATH] = TEXT("V_MAIN04#390.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 1.f);
	}
	else if (Event_Time(dTimeDelta, 35.5f, m_dDialogAccTime))
	{
		Set_CharacterDialog(5.f, TEXT("[Ç÷±Í]"), TEXT("±×¸¸ÇØ! È£Èí ¼Ò¸®°¡ ±Í¿¡ °Å½½¸°´Ù°í!"));
		_tchar szSoundFile[MAX_PATH] = TEXT("V_MAIN04#391.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 1.f);

	}
	else if (Event_Time(dTimeDelta, 41.f, m_dDialogAccTime))
	{
		Set_CharacterDialog(3.f, TEXT("[¾Æ°¡Ã÷¸¶ Á¨ÀÌÃ÷]"), TEXT("(Ä®³¡¿¡ È£ÈíÀ» ÁýÁßÇÑ´Ù..)"));
		_tchar szSoundFile[MAX_PATH] = TEXT("V_BATTLE#412.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 1.f);

	}
	else if (Event_Time(dTimeDelta, 44.5f, m_dDialogAccTime))
	{
		Set_CharacterDialog(5.f, TEXT("[¾Æ°¡Ã÷¸¶ Á¨ÀÌÃ÷]"), TEXT("(ÀÌ Ç÷±Í¸¦ Á×¿©¾ß ³ª¿Í Ä£±¸¸¦ ÁöÅ³¼ö ÀÖ´Ù)"));
		_tchar szSoundFile[MAX_PATH] = TEXT("V_BATTLE#415.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 1.f);

	}
	else if (Event_Time(dTimeDelta, 50.f, m_dDialogAccTime))
	{
		Set_CharacterDialog(5.f, TEXT("[¾Æ°¡Ã÷¸¶ Á¨ÀÌÃ÷]"), TEXT("³» Ä£±¸¸¦ ÁöÅ°±âÀ§ÇØ, ³Ê¸¦ ¹°¸®Ä¡°Ú´Ù!"));
		_tchar szSoundFile[MAX_PATH] = TEXT("V_BATTLE#487.ogg");
		Play_Sound_Channel(szSoundFile, CSoundMgr::CHARACTER_DIALOG, 1.f);

		m_bDialog = false;
	}
	
}

HRESULT CMonster_Zako::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Zako_0"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CMonster_Zako_0");
		return E_FAIL;
	}



	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CMonster_Zako");
		return E_FAIL;
	}


	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CMonster_Zako");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CMonster_Zako");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CMonster_Zako");
		return E_FAIL;
	}

	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.0f, 1.0f, 1.0f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CMonster_Zako");
		return E_FAIL;
	}

	m_CharacterDesc.NaviDesc.iCurrentIndex = 0;

	/* for.Com_Navigation_Acaza*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Acaza"),
		TEXT("Com_Navigation_Acaza"), (CComponent**)&m_pNavigationCom[NAVI_ACAZA], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Acaza: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Village_Battle*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_Battle"),
		TEXT("Com_Navigation_Village_Battle"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_BATTLE], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_Battle: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_House_2_0*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_2_0"),
		TEXT("Com_Navigation_House_2_0"), (CComponent**)&m_pNavigationCom[NAVI_HOUSE_2_0], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_House_2_0: CPlayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_Zako::SetUp_ShaderResources()
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
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineThickness", &m_fOutlineThickness, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineFaceThickness", &m_fOutlineFaceThickness, sizeof(_float))))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

CMonster_Zako* CMonster_Zako::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Zako* pInstance = new CMonster_Zako(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Zako");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Zako::Clone(void* pArg)
{
	CMonster_Zako* pInstance = new CMonster_Zako(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Zako");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Zako::Free()
{

	__super::Free();
}
