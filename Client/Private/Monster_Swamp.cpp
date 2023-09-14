#include "pch.h"
#include "..\Public\Monster_Swamp.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "MonsterManager.h"

#include "Player.h"
#include "PlayerManager.h"

#include "SwampManager.h"
#include "SwampShot.h"

#include "Swamp.h"

#include "Fade_Manager.h"
#include "WaterParticleEffect.h"
#include "Swamp_AlertRect.h"
#include "GroundSmoke.h"

CMonster_Swamp::CMonster_Swamp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CMonster_Swamp::CMonster_Swamp(const CMonster_Swamp& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonster_Swamp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Swamp::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components_Model()))
		return E_FAIL;

	if (FAILED(Read_Animation_Control_File("SwampHorn3.bin")))
	{
		MSG_BOX("Failed to AnimData Read : SwampHorn3");
		return E_FAIL;
	}

	m_pTransformCom->Scaling(_float3{ m_fScale, m_fScale, m_fScale });


	m_pModelCom->Set_Animation(ANIM_IDLE);


	m_ScrewPos[0] = { 430.18f, 3.35f, 320.6f, 1.0f };
	m_ScrewPos[1] = { 419.84f, 3.35f, 294.57f, 1.0f };
	m_ScrewPos[2] = { 423.05f, 3.35f, 320.9f, 1.0f };
	m_ScrewPos[3] = { 430.24f, 3.35f, 294.36f, 1.0f };
	m_ScrewPos[4] = { 424.16f, 3.35f, 312.7f, 1.0f };
	m_ScrewPos[5] = { 419.02f, 3.35f, 308.9f, 1.0f };
	m_ScrewPos[6] = { 430.5f, 3.35f, 308.3f, 1.0f };
	m_ScrewPos[7] = { 420.01f, 3.35f, 325.34f, 1.0f };
	m_ScrewPos[8] = { 426.0f, 3.35f, 301.7f, 1.0f };
	m_ScrewPos[9] = { 423.14f, 3.35f, 294.6f, 1.0f };

	if (m_CharacterDesc.SwampHorn == 1)
	{
		m_iIndex_Swamping = 2;
	}

	// 늪 세팅
	Create_MySwampEffect();

	return S_OK;
}

void CMonster_Swamp::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
		if (pPlayer->Get_CurNaviMesh() != CLandObject::NAVI_VILLAGE_BATTLE)
		{
			Safe_Release(pGameInstance);
			return;
		}
	}
	Safe_Release(pGameInstance);

	if (true == m_isDead)
		return;

	if (CMonsterManager::GetInstance()->Get_BattleOn_Swamp())
	{
		//d
		__super::Tick(dTimeDelta);
		if (m_bTanjiroAwake == false && m_bZenitsuAwake == false && m_bTanjiroSurge == false)
		{
			if (m_isFirst_BattleOn)
			{
				m_isFirst_BattleOn = false;
				CMonsterManager::GetInstance()->Set_BattleOn(true);
			}

			Trigger();
			Animation_Control(dTimeDelta);

			//애니메이션 처리
			m_pModelCom->Play_Animation(dTimeDelta);
			RootAnimation(dTimeDelta);

			//이벤트 콜
			EventCall_Control(dTimeDelta);
		}
	}


}

void CMonster_Swamp::LateTick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
		if (pPlayer->Get_CurNaviMesh() != CLandObject::NAVI_VILLAGE_BATTLE)
		{
			Safe_Release(pGameInstance);
			return;
		}
	}

	Safe_Release(pGameInstance);

	if (CMonsterManager::GetInstance()->Get_BattleOn_Swamp())
	{

		__super::LateTick(dTimeDelta);
		if (m_bTanjiroAwake == false && m_bZenitsuAwake == false && m_bTanjiroSurge == false)
			Gravity(dTimeDelta);
	}

#ifdef _DEBUG
	/*if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
		return;*/
#endif
}

HRESULT CMonster_Swamp::Render()
{
	if (CMonsterManager::GetInstance()->Get_BattleOn_Swamp())
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

	}
	return S_OK;
}

HRESULT CMonster_Swamp::Render_ShadowDepth()
{
	if (FAILED(__super::Render_ShadowDepth()))
		return E_FAIL;

	return S_OK;
}

void CMonster_Swamp::EventCall_Control(_double dTimeDelta)
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

		CGameInstance* pGameInstnace = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstnace);
		_uint iCurIdx = pGameInstnace->Get_CurLevelIdx();


		if (3 == m_pModelCom->Get_iCurrentAnimIndex()) //jumpstomp 준비자세
		{
			if (0 == m_iEvent_Index)	// 0.23초
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_JUMP);
			}
		}

		if (5 == m_pModelCom->Get_iCurrentAnimIndex()) //jumpstomp
		{
			if (0 == m_iEvent_Index)	// 0초
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.4f;
				CEffectPlayer::Get_Instance()->Play("Swamp_Atk_5", m_pTransformCom, &EffectWorldDesc);
			}

			else if (1 == m_iEvent_Index) // 0.17
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.7f), 0.4,
					CAtkCollider::TYPE_BLOW, AtkDir, 7.2f);

				CEffectPlayer::Get_Instance()->Play("Swamp_AtkParticle_Kulbam", m_pTransformCom);

				Create_GroundSmoke(CGroundSmoke::SMOKE_DASHLAND);
			}
		}

		if (7 == m_pModelCom->Get_iCurrentAnimIndex()) //screw
		{
			if (0 == m_iEvent_Index)	// 0초
				m_pMySwamp->Set_Pattern(CSwamp::PATTERN_THROWAWAY);
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 1.7f), 0.4, CAtkCollider::TYPE_SMALL, AtkDir, 0.5f);
				Create_WaterParticleEffect(4);
			}
			else if (2 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 1.7f), 0.4, CAtkCollider::TYPE_SMALL, AtkDir, 0.5f);
				Create_WaterParticleEffect(4);
			}
			else if (3 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 1.7f), 0.4, CAtkCollider::TYPE_SMALL, AtkDir, 0.5f);
				Create_WaterParticleEffect(4);
			}
			else if (4 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 1.7f), 0.4, CAtkCollider::TYPE_SMALL, AtkDir, 0.5f);
				Create_WaterParticleEffect(4);
			}
			else if (5 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 1.7f), 0.4, CAtkCollider::TYPE_SMALL, AtkDir, 0.5f);
				Create_WaterParticleEffect(4);
			}
			else if (6 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 1.7f), 0.4, CAtkCollider::TYPE_SMALL, AtkDir, 0.5f);
				Create_WaterParticleEffect(4);
			}
			else if (7 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 1.7f), 0.4, CAtkCollider::TYPE_SMALL, AtkDir, 0.5f);
				Create_WaterParticleEffect(4);
			}
			else if (8 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 1.7f), 0.4, CAtkCollider::TYPE_SMALL, AtkDir, 0.5f);
				Create_WaterParticleEffect(4);
			}
			else if (9 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.8f, 0.8f, 0.8f), _float3(0.f, 1.0f, 1.7f), 0.8, CAtkCollider::TYPE_BIGBLOW, AtkDir, 4.0f);
				Create_WaterParticleEffect(4);
			}

			else if (10 == m_iEvent_Index)
			{
				Create_WaterParticleEffect(4);
			}

			else if (11 == m_iEvent_Index)
			{
				Create_WaterParticleEffect(4);
			}

			else if (12 == m_iEvent_Index)
			{
				Create_WaterParticleEffect(4);
			}
		}

		if (8 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0
			{
				Create_MySwampEffect();	// 기존 늪 버리고 새늪으로

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.4f;
				CEffectPlayer::Get_Instance()->Play("Swamp_Land", m_pTransformCom, &EffectWorldDesc);
			}
		}

		if (ANIM_ATK_CROSS == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += 0.8f;

				CEffectPlayer::Get_Instance()->Play("Swamp_Atk_10", m_pTransformCom, &EffectWorldDesc);
			}
			else if (1 == m_iEvent_Index)	// 0.38
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.8f), 0.2,
					CAtkCollider::TYPE_BLOW, AtkDir, 3.0f);
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(1.f, 1.0f, 1.8f), 0.2,
					CAtkCollider::TYPE_BLOW, AtkDir, 3.0f);
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(-1.f, 1.0f, 1.8f), 0.2,
					CAtkCollider::TYPE_BLOW, AtkDir, 3.0f);
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(-1.f, 2.5f, 0.1f), 0.2,
					CAtkCollider::TYPE_BLOW, AtkDir, 3.0f);


				CEffectPlayer::Get_Instance()->Play("Swamp_AtkParticle", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Swamp_AtkParticle2", m_pTransformCom);
			}
			else if (2 == m_iEvent_Index) // 0.47
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_DASHLAND);
			}
		}


		if (ANIM_ATK_COMBO == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0초
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.4f;
				CEffectPlayer::Get_Instance()->Play("Swamp_Atk_11", m_pTransformCom, &EffectWorldDesc);

			}

			if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.8f), 0.4,
					CAtkCollider::TYPE_SMALL, AtkDir, 3.0f);

				CEffectPlayer::Get_Instance()->Play("Swamp_AtkParticle", m_pTransformCom);
			}
		}
		else if (12 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.0
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.4f;
				CEffectPlayer::Get_Instance()->Play("Swamp_Atk_12", m_pTransformCom, &EffectWorldDesc);
			}

			if (1 == m_iEvent_Index) // 0.3
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.8f), 0.4,
					CAtkCollider::TYPE_SMALL, AtkDir, 3.0f);

				CEffectPlayer::Get_Instance()->Play("Swamp_AtkParticle2", m_pTransformCom);
			}
		}
		else if (13 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.0f;
				CEffectPlayer::Get_Instance()->Play("Swamp_Atk_13", m_pTransformCom, &EffectWorldDesc);

				Create_GroundSmoke(CGroundSmoke::SMOKE_JUMP);
			}

			if (1 == m_iEvent_Index)	// 0.45
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.8f), 0.4,
					CAtkCollider::TYPE_BIG, AtkDir, 8.0f);

				_vector vPlusPos = m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 2.5f;

				Create_GroundSmoke(CGroundSmoke::SMOKE_SMESHSPREAD, vPlusPos);
				Create_GroundSmoke(CGroundSmoke::SMOKE_UPDOWN, vPlusPos);

				Create_StoneParticle(CStoneParticle::STONE_SWAMP, vPlusPos);
				Create_SmeshStone(vPlusPos);
				Camera_Shake();

				CEffectPlayer::Get_Instance()->Play("Swamp_AtkParticle3", m_pTransformCom);
			}
		}

		if (ANIM_ATK_SWAMP_SWIM == m_pModelCom->Get_iCurrentAnimIndex())	// 노천탕 만들기
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				m_pMySwamp->Set_Pattern(CSwamp::PATTERN_BIG);
			}
		}

		if (ANIM_ATK_SHOT == m_pModelCom->Get_iCurrentAnimIndex())	// 18 (늪 함정 만들기)
		{
			if (0 == m_iEvent_Index)	// 0.2
			{
				Create_LandSwampEffect();
			}
		}

		if (ANIM_ATK_SHORYU_TO_SWAMP_0 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.0
			{
				m_pMySwamp->Set_Pattern(CSwamp::PATTERN_SHORYU);

				CEffectPlayer::Get_Instance()->Play("Swamp_Atk_22", m_pTransformCom);

				CEffectPlayer::Get_Instance()->Play("Swamp_AtkParticle_Upper", m_pTransformCom);
			}

			if (1 == m_iEvent_Index)	// 0.41
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 1.5f, 0.3f), 1.5,
					CAtkCollider::TYPE_HEKIREKI, AtkDir, 7.0f);

				CEffectPlayer::Get_Instance()->Play("Swamp_AtkParticle4", m_pTransformCom);
			}
		}
		else if (ANIM_ATK_SHORYU_TO_SWAMP_0 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 1.5f, 0.3f), 1.5,
					CAtkCollider::TYPE_HEKIREKI, AtkDir, 7.0f);
			}
		}

		if (ANIM_ATK_SHORYU_TO_SWAMP_1 == m_pModelCom->Get_iCurrentAnimIndex())  // 24
		{
			if (0 == m_iEvent_Index) // 0.0
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.4f;
				EffectWorldDesc.vPosition.y += 0.3f;
				CEffectPlayer::Get_Instance()->Play("Swamp_Atk_24", m_pTransformCom, &EffectWorldDesc);

				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 1.5f, 0.3f), 1.5,
					CAtkCollider::TYPE_UPPER, AtkDir, 7.0f);

				CEffectPlayer::Get_Instance()->Play("Swamp_AtkParticle_Upper", m_pTransformCom);
			}

			else if (1 == m_iEvent_Index) // 0.23
			{
				m_pMySwamp->Set_Pattern(CSwamp::PATTERN_THROWAWAY);
				Create_MySwampEffect();
			}
		}

		if (ANIM_STEP_SWAMPIN == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.4
			{
				m_pMySwamp->Set_Pattern(CSwamp::PATTERN_STEPIN);
			}
		}

		if (ANIM_STEP_SWAMPOUT == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.22
			{
				m_pMySwamp->Set_Pattern(CSwamp::PATTERN_STEPOUT);
			}
		}

		if (ANIM_ATK_SHORYU_TO_IDLE == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.45
			{
				m_pMySwamp->Set_Pattern(CSwamp::PATTERN_SHORYU);
			}
		}

		if (ANIM_STEP_B == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.4f;
				CEffectPlayer::Get_Instance()->Play("Step_Effect", m_pTransformCom, &EffectWorldDesc);
			}
		}

		if (ANIM_STEP_F == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.4f;
				CEffectPlayer::Get_Instance()->Play("Step_Effect", m_pTransformCom, &EffectWorldDesc);
			}
		}

		if (ANIM_STEP_L == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.4f;
				CEffectPlayer::Get_Instance()->Play("Step_Effect", m_pTransformCom, &EffectWorldDesc);
			}
		}

		if (ANIM_STEP_R == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_SIDESTEP);
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.4f;
				CEffectPlayer::Get_Instance()->Play("Step_Effect", m_pTransformCom, &EffectWorldDesc);
			}
		}

		if (ANIM_DMG_FALL_END == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_FALLDOWN);

				Play_FallDownEffect();
			}
		}

		if (ANIM_RUN == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0초
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
			else if (0 == m_iEvent_Index) // 0.2초
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
		}

		if (ANIM_RUN_END == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.03초
			{
				Create_GroundSmoke(CGroundSmoke::SMOKE_RUN);
			}
		}

		Safe_Release(pGameInstnace);

#pragma endregion
		m_iEvent_Index++;
	}
}

void CMonster_Swamp::Trigger()
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
			if (m_isNonHitState)
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Spin(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);
			}
			else
			{
				m_eCurState = STATE_HIT;
			}
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
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);
	}
}

void CMonster_Swamp::Animation_Control(_double dTimeDelta)
{
	if (m_isDeath_Motion)
	{
		m_bMonsterDead = true;
		m_fDeadTime += (_float)dTimeDelta;

		if (m_fDeadTime > 4.0f)
			m_isDead = true;
	}
	else
	{
		Navigation_Y_Control(dTimeDelta);

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

void CMonster_Swamp::Animation_Control_Idle(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();


	m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.05f);

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD7))
	{
		m_eCurState = STATE_ATTACK;
		m_eCurPattern = PATTERN_COMBO;
	}

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD8))
	{
		m_eCurState = STATE_ATTACK;
		m_eCurPattern = PATTERN_SHORYU;
	}

	/*PATTERN_JUMPSTOMP = 0, PATTERN_SWAMP_SCREW = 1, PATTERN_SWAMP_IN = 2,
		PATTERN_COMBO = 3, PATTERN_SHORYU = 4, PATTERN_TELESHORYU = 5,
		PATTERN_SHOTSWAMP = 6, PATTERN_BIGSWAMP = 7, PATTERN_RAGE_PIOHYO = 8,
		PATTERN_STEP = 9, PATTERN_RAGE_DUDUGE = 10, PATTERN_RAGE_DUDUGE_ATK = 11,*/

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD9))
	{
		CSwampManager::GetInstance()->Set_Dmg(10.0f);
		m_eCurState = STATE_ATTACK;
		m_eCurPattern = PATTERN_TELESHORYU;
	}

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD4))
	{
		m_eCurState = STATE_ATTACK;
		m_eCurPattern = PATTERN_SWAMP_IN;
	}

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD5))
	{
		m_eCurState = STATE_ATTACK;
		m_eCurPattern = PATTERN_JUMPSTOMP;
	}

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD6))
	{
		m_eCurState = STATE_ATTACK;
		m_eCurPattern = PATTERN_SWAMP_SCREW;
	}

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD1))
	{
		m_eCurState = STATE_ATTACK;
		m_eCurPattern = PATTERN_SHOTSWAMP;
	}

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD2))
	{
		m_eCurState = STATE_ATTACK;
		m_eCurPattern = PATTERN_BIGSWAMP;
	}

	if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD3))
	{
		m_eCurState = STATE_ATTACK;
		m_eCurPattern = PATTERN_BIGSWAMP;
	}

	//레이지 모드
	if (CSwampManager::GetInstance()->Get_Hp_Phase() == 2)
	{
		m_dCooltime_Atk_Pattern += dTimeDelta;

		if (m_isSwamping == false)
		{
			if (m_iIndex_Rage == 0)
			{
				if (m_dCooltime_Atk_Pattern > 1.7f)
				{
					m_dCooltime_Atk_Pattern = 0.0;
					m_iIndex_Rage++;

					m_eCurState = STATE_ATTACK;
					m_eCurPattern = PATTERN_RAGE_PIOHYO;
				}
			}
			else if (m_iIndex_Rage == 1)
			{
				if (m_dCooltime_Atk_Pattern > 0.8f)
				{
					m_dCooltime_Atk_Pattern = 0.0;
					m_iIndex_Rage++;

					m_eCurState = STATE_ATTACK;
					m_eCurPattern = PATTERN_SHOTSWAMP;
					m_iIndex_SwampShot = 4;
				}
			}
			else if (m_iIndex_Rage == 2)
			{
				if (m_iCnt_Duduge == 4)
				{
					if (m_dCooltime_Atk_Pattern > 3.f)
					{
						m_dCooltime_Atk_Pattern = 0.0;
						//m_iIndex_Rage++;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_SWAMP_IN;
					}
				}
				else
				{
					if (m_dCooltime_Atk_Pattern > 0.2f)
					{
						m_dCooltime_Atk_Pattern = 0.0;
						//m_iIndex_Rage++;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_SWAMP_IN;
					}
				}
			}
		}
		else
		{
			m_iIndex_Rage = 2;
			if (m_iIndex_Rage == 2)
			{
				if (m_dCooltime_Atk_Pattern > 0.1f)
				{
					m_dCooltime_Atk_Pattern = 0.0;

					m_eCurState = STATE_ATTACK;

					if (m_iCnt_Duduge < 4)
					{
						m_iCnt_Duduge++;

						m_eCurPattern = PATTERN_RAGE_DUDUGE;
					}
					else if (m_iCnt_Duduge == 4)
					{
						m_iCnt_Duduge = 0;

						m_eCurPattern = PATTERN_RAGE_DUDUGE_ATK;
					}
				}
			}

		}

	}
	//일반 모드
	else
	{
		//메인 상태
		if (m_CharacterDesc.SwampHorn == CSwampManager::GetInstance()->Get_Phase1_MainIndex())
		{
			m_dCooltime_Atk_Pattern += dTimeDelta;

			if (m_isSwamping == false)
			{
				Animation_Control_Walk(dTimeDelta);

				//실제 패턴
				if (m_iIndex_Normal == 0)
				{
					if (m_dCooltime_Atk_Pattern > 1.0f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_STEP;

						m_iIndex_Normal++;
					}
				}
				else if (m_iIndex_Normal == 1)
				{
					if (m_dCooltime_Atk_Pattern > 0.1f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_JUMPSTOMP;

						m_iIndex_Normal++;
					}
				}
				else if (m_iIndex_Normal == 2)
				{
					if (m_dCooltime_Atk_Pattern > 4.1f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_SHOTSWAMP;

						m_iIndex_Normal++;
					}
				}
				else if (m_iIndex_Normal == 3)
				{
					if (m_dCooltime_Atk_Pattern > 0.1f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_STEP;

						m_iIndex_Normal++;
					}
				}
				else if (m_iIndex_Normal == 4)
				{
					if (m_dCooltime_Atk_Pattern > 4.4f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_COMBO;

						m_iIndex_Normal++;
					}
				}
				else if (m_iIndex_Normal == 5)
				{
					if (m_dCooltime_Atk_Pattern > 0.1f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_SWAMP_IN;

						m_iIndex_Normal++;
					}
				}
				else if (m_iIndex_Normal == 6)
				{
					if (m_dCooltime_Atk_Pattern > 5.4f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_COMBO;

						m_iIndex_Normal++;
					}
				}
				else if (m_iIndex_Normal == 7)
				{
					if (m_dCooltime_Atk_Pattern > 0.1f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_STEP;

						m_iIndex_Normal++;
					}
				}
				else if (m_iIndex_Normal == 8)
				{
					if (m_dCooltime_Atk_Pattern > 0.1f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_SHOTSWAMP;

						m_iIndex_Normal++;
					}
				}
				else if (m_iIndex_Normal == 9)
				{
					if (m_dCooltime_Atk_Pattern > 0.1f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_SHOTSWAMP;

						m_iIndex_Normal++;
					}
				}
				else if (m_iIndex_Normal == 10)
				{
					if (m_dCooltime_Atk_Pattern > 0.1f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_SWAMP_IN;

						m_iIndex_Normal = 0;
					}
				}
			}
			else
			{
				//실제 패턴
				if (m_iIndex_Swamping == 0)
				{
					if (m_dCooltime_Atk_Pattern > 0.85f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_SHORYU;

						m_iIndex_Swamping++;
					}
				}
				else if (m_iIndex_Swamping == 1)
				{
					if (m_dCooltime_Atk_Pattern > 0.1f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_TELESHORYU;

						m_iIndex_Swamping++;
					}
				}
				else if (m_iIndex_Swamping == 2)
				{
					if (m_dCooltime_Atk_Pattern > 0.3f)
					{
						m_dCooltime_Atk_Pattern = 0.0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_BIGSWAMP;

						m_iIndex_Swamping = 0;
					}
				}
			}
		}
		//서브 상태
		else
		{
			m_dCooltime_Atk_Pattern += dTimeDelta;

			if (m_isSwamping == false)
			{
				if (m_dCooltime_Atk_Pattern > 0.4f)
				{
					m_dCooltime_Atk_Pattern = 0.0;

					m_eCurState = STATE_ATTACK;
					m_eCurPattern = PATTERN_SWAMP_IN;
				}
			}
			else
			{
				if (m_iIndex_Swamping == 0)
				{
					if (m_dCooltime_Atk_Pattern > 0.4f)
					{
						m_dCooltime_Atk_Pattern = 0.0;
						m_iIndex_Swamping++;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_SWAMP_SCREW;
					}
				}
				else if (m_iIndex_Swamping == 1)
				{
					if (m_dCooltime_Atk_Pattern > 0.5f)
					{
						m_dCooltime_Atk_Pattern = 0.0;
						m_iIndex_Swamping++;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_SHORYU;
					}
				}
				else if (m_iIndex_Swamping == 2)
				{
					if (m_dCooltime_Atk_Pattern > 0.4f)
					{
						m_dCooltime_Atk_Pattern = 0.0;
						m_iIndex_Swamping = 0;

						m_eCurState = STATE_ATTACK;
						m_eCurPattern = PATTERN_TELESHORYU;
					}
				}
			}
		}
	}

	Safe_Release(pGameInstance);
}


void CMonster_Swamp::Navigation_Y_Control(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();
	//네비 타는거
	if (iCurAnim == ANIM_SWAMP_IN)
	{
		m_isNavi_Y_Off = true;
		m_fLand_Y = m_pNavigationCom[m_eCurNavi]->Compute_Height(m_pTransformCom) - 1.4f;
		m_isSwamping = true;

		m_isNonHitState = true;
	}
	else if (iCurAnim == ANIM_SWAMP_IDLE_IN || iCurAnim == ANIM_ATK_SHORYU_TO_SWAMP_0 || iCurAnim == ANIM_ATK_SHORYU_TO_SWAMP_1)
	{
		m_isNavi_Y_Off = true;
		m_fLand_Y = m_pNavigationCom[m_eCurNavi]->Compute_Height(m_pTransformCom) - 1.4f;
		m_isSwamping = true;

		m_isNonHitState = true;
	}
	else if (iCurAnim == ANIM_ATK_SWAMP_SWIM || iCurAnim == 16 || iCurAnim == 17)
	{
		m_isNavi_Y_Off = true;
		m_isSwamping = true;

		if (m_fLand_Y < m_pNavigationCom[m_eCurNavi]->Compute_Height(m_pTransformCom) - 1.0f)
		{
			m_fLand_Y += 0.02f;
		}

		m_isNonHitState = true;
	}
	else if (iCurAnim == ANIM_SWAMP_IDLE)
	{
		m_isNavi_Y_Off = true;
		m_isSwamping = true;

		if (m_fLand_Y < m_pNavigationCom[m_eCurNavi]->Compute_Height(m_pTransformCom) - 1.0f)
		{
			m_fLand_Y += 0.02f;
		}

		m_isNonHitState = true;
	}
	//넌히트만을 위한 곳
	else if (iCurAnim == ANIM_ATK_SWAMP_SCREW || iCurAnim == 7 || iCurAnim == ANIM_ANGRY_CUTSCENE)
	{
		m_isNavi_Y_Off = false;
		m_isSwamping = false;

		m_isNonHitState = true;
	}
	else
	{
		m_isNavi_Y_Off = false;
		m_isSwamping = false;

		m_isNonHitState = false;
	}
}

void CMonster_Swamp::Animation_Control_Attack(_double dTimeDelta, _int AttackIndex)
{
	switch (AttackIndex)
	{
	case 0: //PATTERN_JUMPSTOMP
		Animation_Control_JumpStomp(dTimeDelta);
		break;
	case 1: // PATTERN_SWAMP_SCREW
		Animation_Control_SwampScrew(dTimeDelta);
		break;
	case 2: //Pattern_Swamp_In
		Animation_Control_Swamp_In(dTimeDelta);
		break;
	case 3:	//PATTERN_COMBO
		Animation_Control_Combo(dTimeDelta);
		break;
	case 4: //PATTERN_SHORYU
		Animation_Control_Shoryu(dTimeDelta);
		break;
	case 5: // pattern_Teleport_Shoryu
		Animation_Control_Teleport_Shoryu(dTimeDelta);
		break;
	case 6: // PATTERN_SHOTSWAMP
		Animation_Control_ShotSwamp(dTimeDelta);
		break;
	case 7: //PATTERN_BIGSWAMP
		Animation_Control_BigSwamp(dTimeDelta);
		break;
	case 8: //pioHyo 
		Animation_Control_Piohyo(dTimeDelta);
		break;
	case 9: //Step
		Animation_Control_Step(dTimeDelta);
		break;
	case 10: //Rage_DUDUGE
		Animation_Control_Rage_Duduge(dTimeDelta);
		break;
	case 11: // Rage_Duduge_atk
		Animation_Control_Rage_Duduge_Atk(dTimeDelta);
		break;
	default:
		break;
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_IDLE || m_pModelCom->Get_iCurrentAnimIndex() == ANIM_SWAMP_IDLE)
	{
		m_eCurState = STATE_IDLE;



		m_isAtkFinish = true;

		m_isFrist_Atk_Pattern = true;
		m_isFirst_Atk_0 = true;

		m_isOff_Dash = false;
	}
}

void CMonster_Swamp::Animation_Control_JumpStomp(_double dTimeDelta)
{
	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_isFirst_Dash = true;
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (m_isOff_Dash == false)
	{
		if (Animation_Control_Dash(dTimeDelta, 3.0f))
			m_isOff_Dash = true;
	}
	else
	{
		if (m_isFirst_Atk_0)
		{
			m_isFirst_Atk_0 = false;

			m_pModelCom->Set_Animation(ANIM_ATK_JUMPSTOMP);
		}
		Go_Straight_Constant(dTimeDelta, 4, 1.1f);
		Go_Straight_Deceleration(dTimeDelta, 5, 1.1f, 0.35f);
	}
}

void CMonster_Swamp::Animation_Control_Combo(_double dTimeDelta)
{
	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_isFirst_Dash = true;
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (m_isOff_Dash == false)
	{
		if (Animation_Control_Dash(dTimeDelta, 1.5f))
			m_isOff_Dash = true;
	}
	else
	{
		if (m_isFirst_Atk_0)
		{
			m_isFirst_Atk_0 = false;

			m_pModelCom->Set_Animation(ANIM_ATK_COMBO);
		}
		Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO, 2.0f, 0.6f);
		Go_Straight_Deceleration(dTimeDelta, 12, 2.0f, 0.6f);
		Go_Straight_Deceleration(dTimeDelta, 13, 2.0f, 0.6f);
		Go_Straight_Deceleration(dTimeDelta, 14, 2.0f, 0.6f);
		m_pModelCom->Set_EarlyEnd(ANIM_ATK_COMBO, true, 0.6f);
		m_pModelCom->Set_EarlyEnd(12, true, 0.6f);
		m_pModelCom->Set_EarlyEnd(13, true, 0.6f);

		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.02f);
	}
}

void CMonster_Swamp::Animation_Control_ShotSwamp(_double dTimeDelta)
{
	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_pModelCom->Set_Animation(ANIM_ATK_SHOT);
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_ATK_SHOT)
	{
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.2f);

		if (m_isFirst_Atk_0)
		{
			m_isFirst_Atk_0 = false;

			if (m_iIndex_SwampShot == 0)
			{
				Swamp_Create(1, 0); // 0:싱글, 1:쿼드, 2:큰장판, 3:스왐핑 4:두더쥐
				m_iIndex_SwampShot++;
			}
			else if (m_iIndex_SwampShot == 1)
			{
				Swamp_Create(4, 1);
				m_iIndex_SwampShot++;
			}
			else if (m_iIndex_SwampShot == 2)
			{
				Swamp_Create(1, 0);
				m_iIndex_SwampShot++;
			}
			else if (m_iIndex_SwampShot == 3)
			{
				Swamp_Create(4, 1);
				m_iIndex_SwampShot = 0;
			}
			else if (m_iIndex_SwampShot == 4)
			{
				Swamp_Create(10, 4);
				m_iIndex_SwampShot = 0;
			}
		}
	}
}

void CMonster_Swamp::Animation_Control_BigSwamp(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_pModelCom->Set_Animation(ANIM_SWAMP_IDLE_IN);
		Jumping(0.01f, 0.01f);

		if (m_CharacterDesc.SwampHorn == 1)
			m_iScrewPosIndex = rand() % 10;
		if (m_CharacterDesc.SwampHorn == 2)
			m_iScrewPosIndex = rand() % 9;
		if (m_CharacterDesc.SwampHorn == 3)
			m_iScrewPosIndex = rand() % 8;
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_SWAMP_IN)
	{
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.1f);

		if (0.4f < Calculate_Distance_From_Pos(m_ScrewPos[m_iScrewPosIndex]))
		{
			Go_Dir_Constant(dTimeDelta, ANIM_SWAMP_IN, 1.5f, Calculate_Dir_From_Pos(m_ScrewPos[m_iScrewPosIndex]));

			Create_SwampWaterEffect(dTimeDelta);
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_ATK_SWAMP_SWIM);
			XMStoreFloat4(&m_SaveDir, Calculate_Dir());
			Swamp_Create(1, 2);
		}
	}
	Safe_Release(pGameInstance);
}

void CMonster_Swamp::Animation_Control_Shoryu(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_pModelCom->Set_Animation(ANIM_SWAMP_IDLE_IN);
		Jumping(0.01f, 0.01f);
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_SWAMP_IN)
	{
		_float4 PlayerPos;
		XMStoreFloat4(&PlayerPos, Calculate_PlayerPos());
		if (1.f < Calculate_Distance_From_Pos(PlayerPos))
		{
			_float4 Dir;
			XMStoreFloat4(&Dir, Calculate_Dir_FixY());
			Go_Dir_Constant(dTimeDelta, ANIM_SWAMP_IN, 2.0f, Dir);

			Create_SwampWaterEffect(dTimeDelta);
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_ATK_SHORYU_TO_SWAMP_0);
			Jumping(1.1f, 0.03f);
			m_pModelCom->Set_EarlyEnd(ANIM_ATK_SHORYU_TO_SWAMP_0, true, 0.7f);
			m_fLand_Y = m_pNavigationCom[m_eCurNavi]->Compute_Height(m_pTransformCom) - 4.0f;
		}
	}

	if (iCurAnim == ANIM_JUMP_IDLE)
	{
		//m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.1f);
		_float4 Dir;
		XMStoreFloat4(&Dir, Calculate_Dir_FixY());
		m_pTransformCom->Set_Look(Dir);

		if (m_isFirst_Atk_0)
		{
			m_isFirst_Atk_0 = false;

			m_pModelCom->Set_Animation(ANIM_ATK_CROSS);
			Set_FallingStatus(0.0f, 0.04f);
		}
	}
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_CROSS, 4.0f, 0.25f);

	Safe_Release(pGameInstance);

}

void CMonster_Swamp::Animation_Control_Teleport_Shoryu(_double dTimeDelta)
{
	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_pModelCom->Set_Animation(ANIM_SWAMP_IDLE_IN);
		Jumping(0.01f, 0.01f);
		m_dDelay_Atk = 0.0;

		m_dDelay_Teleporting = 0.0;
		m_isFirst_Teleporting = true;
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	_float4 MyPos;
	XMStoreFloat4(&MyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (iCurAnim == ANIM_SWAMP_IN && m_isFirst_Teleporting && MyPos.y <= m_fLand_Y)
	{
		m_isFirst_Teleporting = false;

		_float4 PlayerPos;
		XMStoreFloat4(&PlayerPos, Calculate_PlayerPos());

		_vector Dir = -Calculate_Dir_FixY();

		PlayerPos.y = MyPos.y;

		_vector vPlayerPos = XMLoadFloat4(&PlayerPos);

		vPlayerPos = vPlayerPos + Dir * 1.7f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
		m_isTeleporting = true;

		m_pMySwamp->Set_Pattern(CSwamp::PATTERN_THROWAWAY_NOWATEREEFCT);

		Create_MySwampEffect();
	}

	if (m_isTeleporting && iCurAnim == ANIM_SWAMP_IN)
	{
		m_dDelay_Teleporting += dTimeDelta;

		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.9f);

		if (!m_bMakeNewSwamp)
		{
			if (m_dDelay_Teleporting > 0.1f)
			{
				m_pMySwamp->Set_Pattern(CSwamp::PATTERN_TELEPORT);

				m_bMakeNewSwamp = true;
			}
		}

		if (m_dDelay_Teleporting > 0.6f)
		{
			m_pModelCom->Set_Animation(ANIM_ATK_SHORYU_TO_IDLE);
			m_isTeleporting = false;
			m_dDelay_Teleporting = 0.0;

			m_bMakeNewSwamp = false;
		}
	}

	if (iCurAnim == ANIM_ATK_SHORYU_TO_IDLE)
	{
		m_dDelay_Atk += dTimeDelta;
		if (m_isFirst_Atk_0 && m_dDelay_Atk > 0.1f)
		{
			m_isFirst_Atk_0 = false;

			Jumping(4.2f, 0.65f);


		}
	}
}

void CMonster_Swamp::Animation_Control_SwampScrew(_double dTimeDelta)
{
	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_pModelCom->Set_Animation(ANIM_SWAMP_IDLE_IN);
		Jumping(0.01f, 0.01f);

		if (m_CharacterDesc.SwampHorn == 1)
			m_iScrewPosIndex = rand() % 10;
		if (m_CharacterDesc.SwampHorn == 2)
			m_iScrewPosIndex = rand() % 9;
		if (m_CharacterDesc.SwampHorn == 3)
			m_iScrewPosIndex = rand() % 8;
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_SWAMP_IN)
	{
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.1f);

		if (0.4f < Calculate_Distance_From_Pos(m_ScrewPos[m_iScrewPosIndex]))
		{
			Go_Dir_Constant(dTimeDelta, ANIM_SWAMP_IN, 1.5f, Calculate_Dir_From_Pos(m_ScrewPos[m_iScrewPosIndex]));

			Create_SwampWaterEffect(dTimeDelta);
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_ATK_SWAMP_SCREW);
		}
	}
	//Go_Dir_Constant(dTimeDelta, 7, 2.6f, m_SaveDir);
	//Go_Dir_Deceleration(dTimeDelta, 8, 2.6f, 0.15f, m_SaveDir);
	Go_Straight_Constant(dTimeDelta, 7, 2.6f);
	Go_Straight_Deceleration(dTimeDelta, 8, 2.6f, 0.15f);


	if (iCurAnim == ANIM_ATK_SWAMP_SCREW)
	{
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.9f);

		Create_SwampAlertRect();

		if (!m_bSetDir)
		{
			m_dSetDirAccTime += dTimeDelta;

			//if (m_dSetDirAccTime > 0.8)
			//{
			XMStoreFloat4(&m_SaveDir, Calculate_Dir());
			m_bSetDir = true;
			m_dSetDirAccTime = 0.0;
			m_bLandEffect[0] = false;
			m_bLandEffect[1] = false;
			m_bLandEffect[2] = false;
			//}
		}
	}
	else if (iCurAnim == 8)
	{
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.05f);

		m_dSetDirAccTime += dTimeDelta;

		if (m_dSetDirAccTime > 0.00)
		{
			if (!m_bLandEffect[0])
			{
				Create_LandSwampEffect();

				m_bLandEffect[0] = true;
			}
		}

		/*if (m_dSetDirAccTime > 0.04)
		{
			if (!m_bLandEffect[1])
			{
				Create_LandSwampEffect();

				m_bLandEffect[1] = true;
			}
		}*/

		if (m_dSetDirAccTime > 0.07)
		{
			if (!m_bLandEffect[2])
			{
				Create_LandSwampEffect();

				m_bLandEffect[2] = true;

				m_dSetDirAccTime = 0.0;
			}
		}

		m_bMakeSwampAlertRect = false;

		m_bSetDir = false;
	}
}

void CMonster_Swamp::Animation_Control_Swamp_In(_double dTimeDelta)
{
	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_pModelCom->Set_Animation(ANIM_STEP_SWAMPIN);
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_SWAMP_IDLE)
		m_fLand_Y = m_pNavigationCom[m_eCurNavi]->Compute_Height(m_pTransformCom) - 2.0f;
}

void CMonster_Swamp::Animation_Control_Piohyo(_double dTimeDelta)
{
	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_pModelCom->Set_Animation(ANIM_ANGRY_CUTSCENE);
	}
}

void CMonster_Swamp::Animation_Control_Rage_Duduge(_double dTimeDelta)
{
	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_pModelCom->Set_Animation(ANIM_SWAMP_IDLE_IN);
		Jumping(0.01f, 0.01f);
		m_pModelCom->Set_EarlyEnd(ANIM_SWAMP_IDLE_IN, true, 0.1f);
		m_dDelay_Rage_Duduge = 0.0;

		m_iDudugeIndex = rand() % 10;
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();


	m_dDelay_Rage_Duduge += dTimeDelta;

	if (iCurAnim == ANIM_SWAMP_IN && m_dDelay_Rage_Duduge > 0.25f)
	{
		m_dDelay_Rage_Duduge = 0.0;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_ScrewPos[m_iDudugeIndex]));
		m_pModelCom->Set_Animation(ANIM_ATK_SHORYU_TO_IDLE);
	}
	_float4 Dir;
	XMStoreFloat4(&Dir, Calculate_Dir_FixY());
	m_pTransformCom->Set_Look(Dir);
}

void CMonster_Swamp::Animation_Control_Rage_Duduge_Atk(_double dTimeDelta)
{
	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_pModelCom->Set_Animation(ANIM_SWAMP_IDLE_IN);
		Jumping(0.01f, 0.01f);
		m_pModelCom->Set_EarlyEnd(ANIM_SWAMP_IDLE_IN, true, 0.1f);
		m_dDelay_Rage_Duduge = 0.0;
		m_isDuduge_Atk = false;
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_SWAMP_IN)
	{
		if (m_isDuduge_Atk)
		{
			m_isDuduge_Atk = false;

			_int index = rand() % 10;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_ScrewPos[index]));
			m_pModelCom->Set_Animation(ANIM_ATK_SHORYU_TO_IDLE);
		}
		else
		{
			m_isDuduge_Atk = true;

			_float4 PlayerPos;
			XMStoreFloat4(&PlayerPos, Calculate_PlayerPos());

			_float4 MyPos;
			XMStoreFloat4(&MyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			_vector Dir = -Calculate_Dir_FixY();

			PlayerPos.y = MyPos.y;

			_vector vPlayerPos = XMLoadFloat4(&PlayerPos);

			vPlayerPos = vPlayerPos + Dir * 1.7f;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

			m_pModelCom->Set_Animation(ANIM_ATK_SHORYU_TO_SWAMP_1);
			Jumping(3.2f, 0.15f);
		}
	}
	_float4 Dir;
	XMStoreFloat4(&Dir, Calculate_Dir_FixY());
	m_pTransformCom->Set_Look(Dir);
}

void CMonster_Swamp::Animation_Control_Step(_double dTimeDelta)
{
	if (m_isFrist_Atk_Pattern)
	{
		m_isFrist_Atk_Pattern = false;

		m_pModelCom->Set_Animation(ANIM_STEP_B);
		Jumping(0.01f, 0.01f);

		if (m_isStepLeft)
			m_isStepLeft = false;
		else
			m_isStepLeft = true;
	}
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	Go_Backward_Deceleration(dTimeDelta, ANIM_STEP_B, 2.0f, 0.1f);

	if (iCurAnim == ANIM_STEP_L && m_isStepLeft == false)
	{
		m_pModelCom->Set_Animation(ANIM_STEP_R);
		Go_Left_Deceleration(dTimeDelta, ANIM_STEP_L, 1.8f, 0.2f);
	}
	if (iCurAnim == ANIM_STEP_R)
	{
		Go_Right_Deceleration(dTimeDelta, ANIM_STEP_R, 1.8f, 0.2f);
	}
}

void CMonster_Swamp::Animation_Control_Walk(_double dTimeDelta)
{
	_float fDistance = Calculate_Distance();

	m_dDelay_Walk += dTimeDelta;

	if (fDistance < 4.0f)
	{
		if (m_isFirst_Walk_0 && m_dDelay_Walk > 1.0f)
		{
			m_isFirst_Walk_0 = false;
			m_isFirst_Walk_1 = true;

			m_pModelCom->Set_Animation(ANIM_WALK_B);
			m_dDelay_Walk = 0.0;
			m_isWalk_Back = true;
		}
	}
	else if (4.0f <= fDistance && fDistance < 10.0f)
	{
		if (m_isFirst_Walk_1 && m_dDelay_Walk > 1.0f)
		{
			m_isFirst_Walk_0 = true;
			m_isFirst_Walk_1 = false;

			if (m_isWalk_Back)
				m_pModelCom->Set_Animation(ANIM_WALK_B_END);
			else
				m_pModelCom->Set_Animation(ANIM_WALK_F_END);
			m_dDelay_Walk = 0.0;
		}

		if (m_dDelay_Walk > 3.0)
		{
			if (m_isFirst_Walk_Side)
			{
				m_isFirst_Walk_Side = false;

				if (m_isWalk_Left)
				{
					m_isWalk_Left = false;
					m_pModelCom->Set_Animation(ANIM_WALK_L);
				}
				else
				{
					m_isWalk_Left = true;
					m_pModelCom->Set_Animation(ANIM_WALK_R);
				}
			}
		}
	}
	else if (10.0f <= fDistance)
	{
		if (m_isFirst_Walk_0 && m_dDelay_Walk > 1.0f)
		{
			m_isFirst_Walk_0 = false;
			m_isFirst_Walk_1 = true;

			m_pModelCom->Set_Animation(ANIM_WALK_F);
			m_dDelay_Walk = 0.0;
			m_isWalk_Back = false;
		}
	}
	Go_Straight_Constant(dTimeDelta, ANIM_WALK_F, 0.6f);
	Go_Straight_Constant(dTimeDelta, 63, 0.6f);
	Go_Backward_Constant(dTimeDelta, ANIM_WALK_B, 0.6f);
	Go_Backward_Constant(dTimeDelta, 60, 0.6f);

	_float4		Dir;
	XMStoreFloat4(&Dir, Calculate_Dir_Cross());
	_float4		reverseDir;
	XMStoreFloat4(&reverseDir, -Calculate_Dir_Cross());
	Go_Dir_Constant(dTimeDelta, ANIM_WALK_L, 0.45f, Dir);
	Go_Dir_Constant(dTimeDelta, 66, 0.45f, Dir);
	Go_Dir_Constant(dTimeDelta, ANIM_WALK_R, 0.45f, reverseDir);
	Go_Dir_Constant(dTimeDelta, 69, 0.45f, reverseDir);
}

_bool CMonster_Swamp::Animation_Control_Dash(_double dTimeDelta, _float fDistance)
{
	if (m_isFirst_Dash)
	{
		m_isFirst_Dash = false;

		m_pModelCom->Set_Animation(ANIM_RUN);
	}

	if (fDistance < Calculate_Distance())
	{
		Go_Straight_Constant(dTimeDelta, 40, 2.3f);
	}
	else if (Calculate_Distance() <= fDistance)
	{
		m_pModelCom->Set_Animation(ANIM_RUN_END);
		return true;
	}

	m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.2f);

	return false;
}

void CMonster_Swamp::Animation_Control_Hit(_double dTimeDelta)
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

			Play_HitEffect();
			CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);

			/*CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
			EffectWorldDesc.vPosition.y += 0.5f;
			CEffectPlayer::Get_Instance()->Play("Hit_Particle0", m_pTransformCom , &EffectWorldDesc);*/
		}
		else if (m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
			m_isConnectHitting = true;

			Play_HitEffect();
			CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
			/*CEffectPlayer::Get_Instance()->Play("Hit_Particle0", m_pTransformCom);*/

		}

		m_dDelay_ComboChain = 1.0;
		pPlayer->Set_Hit_Success(true); // 플레이어가 맞았따
		//m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		CSwampManager::GetInstance()->Set_Dmg(m_pColliderCom[COLL_SPHERE]->Get_fDamage());
		m_isSwamp_Deathing = true;

		if (m_isJumpOn)
		{
			m_pModelCom->Set_Animation(ANIM_DMG_FALL);
			Jumping(0.3f, 0.030f);
			m_dDelay_ComboChain = 6.0;
		}
		else
		{
			if (m_iSmallHit_Index == 0)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_SMALL_F);
				m_iSmallHit_Index++;
			}
			else if (m_iSmallHit_Index == 1)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_SMALL_L);
				m_iSmallHit_Index++;
			}
			else if (m_iSmallHit_Index == 2)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_SMALL_R);
				m_iSmallHit_Index++;
			}
			else if (m_iSmallHit_Index == 3)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_SMALL_U);
				m_iSmallHit_Index = 0;
			}
		}
	}
	if (m_isConnectHitting == false)
	{
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_F, 0.8f, 0.04f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_L, 0.8f, 0.04f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_R, 0.8f, 0.04f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_U, 0.8f, 0.04f, AtkDir);
	}


#pragma endregion


#pragma region Hit_Big
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Big())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);

		pPlayer->Set_Hit_Success(true);
		//m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		CSwampManager::GetInstance()->Set_Dmg(m_pColliderCom[COLL_SPHERE]->Get_fDamage());
		m_isSwamp_Deathing = true;

		if (m_isJumpOn)
		{
			m_pModelCom->Set_Animation(ANIM_DMG_BLOW);
			m_dDelay_ComboChain = 4.0;
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_DMG_BIG);
			m_dDelay_ComboChain = 1.7;
		}

		Play_HitEffect();
		CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);

		//pGameInstance->Time_Slow(0.3, 0.15);
	}
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_BIG, 1.6f, 0.05f, AtkDir);
#pragma endregion


#pragma region Hit_Upper
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Upper())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);

		m_dDelay_ComboChain = 6.0;
		pPlayer->Set_Hit_Success(true);
		//m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		CSwampManager::GetInstance()->Set_Dmg(m_pColliderCom[COLL_SPHERE]->Get_fDamage());
		m_isSwamp_Deathing = true;

		m_pModelCom->Set_Animation(ANIM_DMG_FALL);
		Jumping(1.75f, 0.03f); // 1.75

		Play_HitEffect();
		CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);

		//pGameInstance->Time_Slow(0.23, 0.3);
	}

	//어퍼시 수직상승 여부
	if (m_isStrictUpper == false)
	{
		Go_Dir_Constant(dTimeDelta, ANIM_DMG_FALL, 0.5f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 104, 0.5f, 0.01f, AtkDir);
	}

	if (m_isBounding)
	{
		Ground_Animation_Play(104, ANIM_DMG_BOUND);
	}
	else
		Ground_Animation_Play(104, ANIM_DMG_FALL_END);
#pragma endregion


#pragma region Hit_Bound
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Bound())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Bound(false);

		m_dDelay_ComboChain = 15.0;
		m_isBounding = true;
		pPlayer->Set_Hit_Success(true);
		//m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		CSwampManager::GetInstance()->Set_Dmg(m_pColliderCom[COLL_SPHERE]->Get_fDamage());
		m_isSwamp_Deathing = true;

		if (m_isJumpOn)
		{
			m_pModelCom->Set_Animation(ANIM_DMG_FALL);
			Set_FallingStatus(3.0f, 0.0f);
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_DMG_BOUND);
			//pGameInstance->Time_Slow(0.15, 0.1);
		}

		Play_HitEffect();
		CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_BOUND, 0.3f, AtkDir);
	Go_Dir_Constant(dTimeDelta, 91, 0.3f, AtkDir);


	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_DMG_BOUND && m_isBounding)
	{
		if (m_isFirst_Anim)
		{
			m_isFirst_Anim = false;
			m_isBounding = false;

			Jumping(2.25f, 0.05f); // 1.85
			//pGameInstance->Time_Slow(0.25, 0.5);
		}

		Play_HitEffect();
		CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);

	}
#pragma endregion


#pragma region Hit_Blow
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);

		m_dDelay_ComboChain = 2.5;
		pPlayer->Set_Hit_Success(true);
		//m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		CSwampManager::GetInstance()->Set_Dmg(m_pColliderCom[COLL_SPHERE]->Get_fDamage());
		m_isSwamp_Deathing = true;

		m_pModelCom->Set_Animation(ANIM_DMG_BLOW);
		Jumping(1.4f, 0.05f); // 1.1f

		Play_HitEffect();
		CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);

		//pGameInstance->Time_Slow(0.6, 0.4);
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_BLOW, 2.5f, AtkDir);
	Go_Dir_Constant(dTimeDelta, 86, 2.5f, AtkDir);
	Ground_Animation_Play(86, 87);
#pragma endregion



#pragma region Hit_CutScene
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_CutScene())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_CutScene(false);

		pPlayer->Set_Hit_SurgeCutScene(true);
		pPlayer->Set_Hit_Success(true);
		//m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		CSwampManager::GetInstance()->Set_Dmg(m_pColliderCom[COLL_SPHERE]->Get_fDamage());
		m_isSwamp_Deathing = true;

		m_dDelay_ComboChain = 5.5;

		m_pModelCom->Set_Animation(ANIM_DEATH);

		m_isSurging = true;

		//pGameInstance->Time_Slow(0.08, 0.15);
	}
#pragma endregion


#pragma region Hit_HEKIREKI
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Hekireki())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);

		m_dDelay_ComboChain = 5.5;
		pPlayer->Set_Hit_Success(true);
		pPlayer->Set_Hit_Success_Hekireki(true);
		//m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		CSwampManager::GetInstance()->Set_Dmg(m_pColliderCom[COLL_SPHERE]->Get_fDamage());
		m_isSwamp_Deathing = true;

		m_pModelCom->Set_Animation(ANIM_DMG_FALL);
		m_isStrictUpper = true;

		if (m_isJumpOn == false)
		{
			Jumping(2.05f, 0.03f); // 1.75
		}
		else
		{
			Jumping(1.0f, 0.030f); // 0.75
		}

		Play_HitEffect();
		CEffectPlayer::Get_Instance()->Play("Hit_Particle_Up", m_pTransformCom);

		pGameInstance->Time_Slow(0.2, 0.1);
	}
#pragma endregion



#pragma region Death_Motion
	if (m_isSwamp_Deathing)
	{
		m_isSwamp_Deathing = false;

		if (CSwampManager::GetInstance()->Get_Hp_Phase() == 0)
		{
			if (CSwampManager::GetInstance()->Get_Hp() <= CSwampManager::GetInstance()->Get_Hp_Max() * 0.666f)
			{
				m_pModelCom->Set_Animation(ANIM_DEATH);
				m_isDeath_Motion = true;
				CSwampManager::GetInstance()->Set_Hp_Phase_Up();

				CSwampManager::GetInstance()->Set_Dead(m_CharacterDesc.SwampHorn);
				CSwampManager::GetInstance()->Set_Phase1_MainIndex(CSwampManager::GetInstance()->Find_Alive());

				pGameInstance->Time_Slow(0.6, 0.3);
			}
		}
		else if (CSwampManager::GetInstance()->Get_Hp_Phase() == 1)
		{
			if (CSwampManager::GetInstance()->Get_Hp() <= CSwampManager::GetInstance()->Get_Hp_Max() * 0.333f)
			{
				m_pModelCom->Set_Animation(ANIM_DEATH);
				m_isDeath_Motion = true;
				CSwampManager::GetInstance()->Set_Hp_Phase_Up();

				CSwampManager::GetInstance()->Set_Dead(m_CharacterDesc.SwampHorn);
				CSwampManager::GetInstance()->Set_Phase1_MainIndex(CSwampManager::GetInstance()->Find_Alive());

				pGameInstance->Time_Slow(0.6, 0.3);
			}
		}
		else if (CSwampManager::GetInstance()->Get_Hp_Phase() == 2)
		{
			if (CSwampManager::GetInstance()->Get_Hp() <= 0.0f)
			{
				m_pModelCom->Set_Animation(ANIM_DEATH);
				m_isDeath_Motion = true;

				CMonsterManager::GetInstance()->Set_BattleOn(false);

				pGameInstance->Time_Slow(0.6, 0.3);
			}
		}
	}
#pragma endregion


	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	m_dDelay_ComboChain -= dTimeDelta;
	if (m_dDelay_ComboChain <= 0.0f)
	{
		m_dDelay_ComboChain = 0.0;

		m_isFirst_Anim = true;

		m_eCurState = STATE_IDLE;

	}

	if (iCurAnim == ANIM_DOWN_IDLE || iCurAnim == ANIM_DEATH || iCurAnim == ANIM_DMG_FALL_END)
	{
		m_dDelay_ComboChain = 0.0;

		m_isFirst_Anim = true;

		m_eCurState = STATE_DOWN;
	}

	Safe_Release(pGameInstance);
}

void CMonster_Swamp::Animation_Control_Down(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();



	if (iCurAnim == ANIM_DEATH && m_StatusDesc.fHp <= 0.0f)
	{
		m_isDeath_Motion = true;


	}


	if (iCurAnim == ANIM_DOWN_IDLE || iCurAnim == ANIM_DEATH_IDLE)
	{
		m_dDelay_Down += dTimeDelta;
		if (m_dDelay_Down > 1.7f)
		{
			m_dDelay_Down = 0.0;

			if (iCurAnim == ANIM_DEATH_IDLE)
				m_pModelCom->Set_Animation(ANIM_DEATH_GETUP);
			else
				m_pModelCom->Set_Animation(ANIM_DOWN_GETUP);
		}

		m_isSurging = false;
	}

	if (iCurAnim == ANIM_IDLE)
	{
		m_eCurState = STATE_IDLE;
	}

}

void CMonster_Swamp::Swamp_Create(_int iNumSwamp, _int iType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	CSwampShot::SHOTDESC ShotDesc;

	XMStoreFloat4(&ShotDesc.MonsterPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	ShotDesc.iType = iType; // 0:싱글, 1:쿼드, 2:큰장판, 3:스왐핑

	if (iType == 0)
	{
		_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector Dir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		Pos = Pos + Dir * 1.65f;

		XMStoreFloat4(&ShotDesc.WorldInfo.vPosition, Pos);
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Shot"), TEXT("Prototype_GameObject_SwampShot"), &ShotDesc)))
		{
			MSG_BOX("Failed to Add_GameObject : SwampShot");
			return;
		}
	}
	else if (iType == 1)
	{
		for (_int i = 0; i < iNumSwamp; i++)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vDir = XMVector4Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			_vector vUp = { 0.0f, 1.0f, 0.0f, 0.0f };
			_vector crossLeft = XMVector3Cross(vDir, vUp);

			vPos = vPos + vDir * 1.65f;
			if (i == 1)
			{
				vPos = vPos + crossLeft * 1.7f;
			}
			else if (i == 2)
			{
				vPos = vPos - crossLeft * 1.7f;
			}
			else if (i == 3)
			{
				vPos = vPos + crossLeft * 3.4f;
			}

			XMStoreFloat4(&ShotDesc.WorldInfo.vPosition, vPos);
			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Shot"), TEXT("Prototype_GameObject_SwampShot"), &ShotDesc)))
			{
				MSG_BOX("Failed to Add_GameObject : SwampShot");
				return;
			}
		}
	}
	else if (iType == 2)
	{
		_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&ShotDesc.WorldInfo.vPosition, Pos);
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Shot"), TEXT("Prototype_GameObject_SwampShot"), &ShotDesc)))
		{
			MSG_BOX("Failed to Add_GameObject : SwampShot");
			return;
		}
	}
	else if (iType == 3)
	{
		_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&ShotDesc.WorldInfo.vPosition, Pos);
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Shot"), TEXT("Prototype_GameObject_SwampShot"), &ShotDesc)))
		{
			MSG_BOX("Failed to Add_GameObject : SwampShot");
			return;
		}
	}
	else if (iType == 4)
	{
		for (_int i = 0; i < iNumSwamp; i++)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			XMStoreFloat4(&ShotDesc.WorldInfo.vPosition, vPos);

			ShotDesc.iDududgeIndex = i;

			if (FAILED(pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Shot"), TEXT("Prototype_GameObject_SwampShot"), &ShotDesc)))
			{
				MSG_BOX("Failed to Add_GameObject : SwampShot");
				return;
			}
		}
	}
	Safe_Release(pGameInstance);
}

void CMonster_Swamp::Create_MySwampEffect()
{
	// 늪 세팅
	CSwamp::EFFECTDESC EffectDesc;
	EffectDesc.pOwner = this;
	EffectDesc.pTransform = m_pTransformCom;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iCurLevelIdx = pGameInstance->Get_CurLevelIdx();

	if (FAILED(pGameInstance->Add_GameObject(iCurLevelIdx, TEXT("Layer_Swamp"), TEXT("Prototype_GameObject_Swamp"), &EffectDesc)))
	{
		Safe_Release(pGameInstance);
		return;
	}

	m_pMySwamp = dynamic_cast<CSwamp*>(pGameInstance->Get_GameObject(iCurLevelIdx, TEXT("Layer_Swamp"), (_uint)pGameInstance->Get_GameObject_ListSize(iCurLevelIdx, TEXT("Layer_Swamp")) - 1));
	if (nullptr == m_pMySwamp)
	{
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
}

void CMonster_Swamp::Create_SwampWaterEffect(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_dWaterEffectAccTime += dTimeDelta;
	m_dRemainSwampEffectAccTime += dTimeDelta;

	if (m_dWaterEffectAccTime > 0.09)
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

		for (_uint i = 0; i < 5; ++i)
			CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_SWAMPWATER, &EffectWDesc);

		m_dWaterEffectAccTime = 0.0;
	}

	if (m_dRemainSwampEffectAccTime > 0.11)
	{
		CSwamp::EFFECTDESC EffectSwampDesc;
		EffectSwampDesc.eType = CSwamp::TYPE_REMAIN;
		EffectSwampDesc.pTransform = m_pTransformCom;
		pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Swamp"), &EffectSwampDesc);

		m_dRemainSwampEffectAccTime = 0.0;
	}

	Safe_Release(pGameInstance);
}

void CMonster_Swamp::Create_WaterParticleEffect(_uint iNum)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CEffectW::EFFECTWDESC EffectWDesc;
	EffectWDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.1f, 0.f, 0.f);
	EffectWDesc.eEffectWType = CEffectW_Manager::EFFECT_SWAMPWATER;
	EffectWDesc.iNumX = 4; EffectWDesc.iNumY = 4;

	EffectWDesc.vStartPosX = { -0.4f,0.4f };  EffectWDesc.vStartPosZ = { -0.4f,0.4f };
	EffectWDesc.vFrameSpeed = { 0.03f , 0.05f };
	EffectWDesc.vStartSizeX = { 0.7f , 1.0f }; EffectWDesc.vStartSizeY = { 0.7f , 1.0f };
	EffectWDesc.vSpeedX = { -2.0f , 2.0f }; EffectWDesc.vSpeedY = { 3.5f , 6.5f };
	EffectWDesc.vStartFrame = { 0.f ,5.f };
	EffectWDesc.fGravity = { 2.f };

	for (_uint i = 0; i < 5; ++i)
		CEffectW_Manager::Get_Instance()->Play(CEffectW_Manager::EFFECT_SWAMPWATER, &EffectWDesc);

	Safe_Release(pGameInstance);
}

void CMonster_Swamp::Create_LandSwampEffect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSwamp::EFFECTDESC EffectSwampDesc;
	EffectSwampDesc.eType = CSwamp::TYPE_LAND;
	EffectSwampDesc.pTransform = m_pTransformCom;
	pGameInstance->Add_GameObject(LEVEL_VILLAGE, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Swamp"), &EffectSwampDesc);

	Safe_Release(pGameInstance);
}

void CMonster_Swamp::Create_SwampAlertRect()
{
	if (!m_bMakeSwampAlertRect)
	{
		CSwamp_AlertRect::EFFECTDESC EffectDesc;
		EffectDesc.pOwnerTransform = m_pTransformCom;
		EffectDesc.vDir = m_SaveDir;

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Swamp_AlertRect"), &EffectDesc);

		Safe_Release(pGameInstance);

		m_bMakeSwampAlertRect = true;
	}
}

HRESULT CMonster_Swamp::Add_Components()
{
	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CMonster_Swamp");
		return E_FAIL;
	}


	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CMonster_Swamp");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CMonster_Swamp");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CMonster_Swamp");
		return E_FAIL;
	}

	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.0f, 1.0f, 1.0f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CMonster_Swamp");
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

HRESULT CMonster_Swamp::Add_Components_Model()
{
	/* for.Com_Model */
	if (m_CharacterDesc.SwampHorn == 1)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Swamp_Horn1"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSG_BOX("Failed to Add_Com_Model : CMonster_Swamp_0");
			return E_FAIL;
		}
	}
	else if (m_CharacterDesc.SwampHorn == 2)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Swamp_Horn2"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSG_BOX("Failed to Add_Com_Model : CMonster_Swamp_0");
			return E_FAIL;
		}
	}
	else if (m_CharacterDesc.SwampHorn == 3)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Swamp_Horn3"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		{
			MSG_BOX("Failed to Add_Com_Model : CMonster_Swamp_0");
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CMonster_Swamp::SetUp_ShaderResources()
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

CMonster_Swamp* CMonster_Swamp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Swamp* pInstance = new CMonster_Swamp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Swamp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Swamp::Clone(void* pArg)
{
	CMonster_Swamp* pInstance = new CMonster_Swamp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Swamp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Swamp::Free()
{

	__super::Free();
}
