#include "pch.h"
#include "..\Public\Monster_StealthZako.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "MonsterManager.h"
#include "Fade_Manager.h"

#include "Player.h"
#include "PlayerManager.h"
#include "World_UI_Hp.h"

CMonster_StealthZako::CMonster_StealthZako(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CMonster_StealthZako::CMonster_StealthZako(const CMonster_StealthZako& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonster_StealthZako::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_StealthZako::Initialize(void* pArg)
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

	m_pModelCom->Set_LinearDuration(ANIM_WALK_FRONT, 0.3);
	m_pModelCom->Set_LinearDuration(ANIM_RUN, 0.4);

	/*
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

	m_StatusDesc.fHp_Max = 100000.f;
	m_StatusDesc.fHp = 100000.f;


	Safe_Release(pGameInstance);
	*/
	Get_PlayerComponent();
	return S_OK;
}

void CMonster_StealthZako::Tick(_double dTimeDelta)
{
	if (CMonsterManager::GetInstance()->Get_BattleOn() == false)
	{
		__super::Tick(dTimeDelta);

		if (m_isDead)
			return;

		if (m_isDeath_Stealth)
		{
			m_dDelay_Dead_Stealth += dTimeDelta;
			if (m_dDelay_Dead_Stealth > 5.f)
				return;
		}
		Animation_Control(dTimeDelta);

		//애니메이션 처리
		m_pModelCom->Play_Animation(dTimeDelta);
		RootAnimation(dTimeDelta);

		//이벤트 콜
		EventCall_Control(dTimeDelta);

		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this)))
			return;
	}
}

void CMonster_StealthZako::LateTick(_double dTimeDelta)
{
	if (CMonsterManager::GetInstance()->Get_BattleOn() == false)
	{
		__super::LateTick(dTimeDelta);

		Gravity(dTimeDelta);
	}
#ifdef _DEBUG
	/*if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
		return;*/
#endif
}

HRESULT CMonster_StealthZako::Render()
{
	if (CMonsterManager::GetInstance()->Get_BattleOn() == false)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;



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
				m_pShaderCom->Begin(1);

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
	}

	return S_OK;
}

HRESULT CMonster_StealthZako::Render_ShadowDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();


	_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector	vLightEye = XMVectorSet(130.f, 10.f, 140.f, 1.f);
	_vector	vLightAt = XMVectorSet(60.f, 0.f, 60.f, 1.f);
	_vector	vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);


	_matrix      LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4   FloatLightViewMatrix;
	XMStoreFloat4x4(&FloatLightViewMatrix, LightViewMatrix);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix",
		&FloatLightViewMatrix)))
		return E_FAIL;

	_matrix      LightProjMatrix;
	_float4x4   FloatLightProjMatrix;

	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), _float(1280) / _float(720), 0.2f, 300.f);
	XMStoreFloat4x4(&FloatLightProjMatrix, LightProjMatrix);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix",
		&FloatLightProjMatrix)))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderBoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			return E_FAIL;


		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CMonster_StealthZako::EventCall_Control(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_iEvent_Index = 0;
	}

	_vector AtkDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (ANIM_POSE_HOWLING == m_pModelCom->Get_iCurrentAnimIndex())
	{
		m_dDelay_Howling += dTimeDelta;
		if (m_dDelay_Howling > 1.0f && m_isFirst_Howling)
		{
			CEffectPlayer::Get_Instance()->Play("Kyogai_Atk_Push", m_pTransformCom);
			m_dDelay_Howling = 0.0;
			m_isFirst_Howling = false;
		}
	}

	if (EventCallProcess())
	{
		if (ANIM_ATK_CLAWS == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{//0.10
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws", m_pTransformCom);
			}
			
			if (2 == m_iEvent_Index)
			{//0.30
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_Left", m_pTransformCom);
			}
			
			if (4 == m_iEvent_Index)
			{//0.50
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws", m_pTransformCom);
			}
			
			if (6 == m_iEvent_Index)
			{//0.80
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_Left", m_pTransformCom);
			}
			
			if (8 == m_iEvent_Index)
			{//1.30
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.x = 0.2f;
				EffectWorldDesc.fScale = 1.2f;

				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.x = -0.2f;
				CEffectPlayer::Get_Instance()->Play("Zako_Atk_Claws_Left", m_pTransformCom, &EffectWorldDesc);
			}
			
		}

		m_iEvent_Index++;
	}
}

void CMonster_StealthZako::Calculate_SpotIndex()
{
	_vector vMonPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSpotPos = XMLoadFloat4(&m_CharacterDesc.NPCDesc.WalkSpot[(m_iSpot_Index)]);
	_vector vDir = XMVector3Normalize(vSpotPos - vMonPos);
	_float fDistance = Convert::GetLength(vSpotPos - vMonPos);

	m_pTransformCom->LerpVector(vDir, 0.05f);

	if (fDistance < 0.15f)
	{
		if (m_iSpot_Index == 3)
			m_iSpot_Index = 0;
		else
			m_iSpot_Index++;
	}
}

void CMonster_StealthZako::Animation_Control(_double dTimeDelta)
{
	if(!m_isQuestioning && !m_isFinding && !m_isAttacking && !m_isPlayerBack)
		Animation_Control_Move(dTimeDelta);

	Animation_Control_Search(dTimeDelta);
}

void CMonster_StealthZako::Animation_Control_Move(_double dTimeDelta)
{
	//순찰
	Calculate_SpotIndex();

	m_dDelay_SpotWalk += dTimeDelta;
	if (m_dDelay_SpotWalk > m_fTime_Stay)
	{
		m_dDelay_SpotWalk = 0.0;
		m_isFirst_Go = true;

		if (m_isGo)
		{
			m_isGo = false;
			m_fTime_Stay = Random::Generate_Float(4.0f, 7.0f);
		}
		else
		{
			m_isGo = true;
			m_fTime_Stay = Random::Generate_Float(10.0f, 16.0f);
		}
	}

	if (m_isGo)
	{
		if (m_isFirst_Go)
		{
			m_isFirst_Go = false;

			m_pModelCom->Set_Animation(ANIM_WALK_FRONT);
		}
		Go_Straight_Constant(dTimeDelta, ANIM_WALK_FRONT, 0.3f, true);
		Go_Straight_Constant(dTimeDelta, 69, 0.3f, true);
	}
	else
	{
		if (m_isFirst_Go)
		{
			m_isFirst_Go = false;

			m_pModelCom->Set_Animation(70);
		}
	}

}

void CMonster_StealthZako::Animation_Control_Search(_double dTimeDelta)
{


	if (!m_isQuestioning && !m_isFinding && !m_isAttacking && !m_isPlayerBack)
	{
		if (Calculate_Distance() < 8.0f) // 6.0
		{
			_float fAngle = Calculate_Angle(m_pTransformCom->Get_State(CTransform::STATE_LOOK), Calculate_Dir_FixY());

			if (fAngle < 35.0f)
			{
				//Find
				m_isQuestioning = true;
				m_pModelCom->Set_Animation(70);
			}
		}
		m_isFirst_Questioning = true;
	}

	if (m_isSlowMotion)
	{
		if (m_isFirst_SlowMotion)
		{
			m_isFirst_SlowMotion = false;
			m_pRendererCom->Set_GrayScale_On(true);
		}

		m_dDelay_SlowMotion += dTimeDelta;
		if (m_dDelay_SlowMotion > 0.1)
		{
			m_isSlowMotion = false;
			m_dDelay_SlowMotion = 0.0;
			m_pRendererCom->Set_GrayScale_On(false);
		}
	}

	if (m_isQuestioning)
	{
		if (m_isFirst_Questioning)
		{
			m_isFirst_Questioning = false;

			m_pModelCom->Set_Animation(ANIM_POSE_MUT);

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			pGameInstance->Time_Slow(0.1, 0.4);
			Safe_Release(pGameInstance);

			m_isSlowMotion = true;
			m_dDelay_SlowMotion = 0.0;
			m_isFirst_SlowMotion = true;
		}


		//안걸리는거
		_float fAngle = Calculate_Angle(m_pTransformCom->Get_State(CTransform::STATE_LOOK), Calculate_Dir_FixY());
		if (Calculate_Distance() > 6.0f || fAngle > 45.0f)
		{
			m_dDelay_Questioning = 0.0;
			m_isQuestioning = false;
			m_isFinding = false;

			m_isGo = false;
			m_dDelay_SpotWalk = 0.0;
			m_fTime_Stay = 1.5f;
		}

		//걸리는거
		m_dDelay_Questioning += dTimeDelta;
		if (m_dDelay_Questioning > 0.9f)
		{
			m_dDelay_Questioning = 0.0;
			m_isQuestioning = false;
			m_isFinding = true;
		}

		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.05f);
	}

	if (m_isFinding)
	{
		if (m_isFirst_Finding)
		{
			m_isFirst_Finding = false;

			m_pModelCom->Set_Animation(ANIM_POSE_HOWLING);
			m_dDelay_Howling = 0.0;
			m_isFirst_Howling = true;

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			pGameInstance->Time_Slow(0.15, 0.7);
			Safe_Release(pGameInstance);
		}

		if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_IDLE)
			m_pModelCom->Set_Animation(ANIM_RUN);
		
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.3f);

		if(Calculate_Distance() > 1.f)
			Go_Straight_Constant(dTimeDelta, ANIM_RUN, 1.50f, true);
		else
		{
			m_isFinding = false;
			m_isAttacking = true;

			m_isFirst_Finding = true;
			m_isFirst_Attacking = true;
		}
	}

	if (m_isAttacking)
	{
		if (m_isFirst_Attacking)
		{
			m_isFirst_Attacking = false;

			m_pModelCom->Set_Animation(ANIM_ATK_CLAWS);

			CFadeManager::GetInstance()->Set_Fade_OutIn_Basic(true, 2.5f);
		}

		m_dDelay_Attacking += dTimeDelta;
		if (m_dDelay_Attacking > 0.1f)
		{
			m_dDelay_Attacking = 0.0;

			m_isAttacking = false;

			m_isPlayerBack = true;
			m_dDelay_PlayerBack = 0.0;
			CMonsterManager::GetInstance()->Set_PlayerBack(true);
			CMonsterManager::GetInstance()->Set_ZenitsuBack(true);
			_float4 Dir;
			XMStoreFloat4(&Dir, Calculate_Dir());
			CMonsterManager::GetInstance()->Set_DirStealthAtk(Dir);


		}
	}

	if (m_isPlayerBack)
	{
		m_dDelay_PlayerBack += dTimeDelta;
		if (m_dDelay_PlayerBack > 2.5f)
		{
			m_dDelay_PlayerBack = 0.0;
			m_isPlayerBack = false;

			//_vector{15.5f, 0.05f, 27.94f, 1.0f}
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_WorldInfo.vPosition));
			m_isGo = false;
			m_dDelay_SpotWalk = 0.0;
			m_fTime_Stay = 1.5f;
		}
	}

	//히트시
	if (m_CharacterDesc.NPCDesc.eNPC == NPC_QUEST) //배틀 몹
	{
		if (CMonsterManager::GetInstance()->Get_StealthAttack())
		{
			CMonsterManager::GetInstance()->Set_StealthAttack(false);

			if (Calculate_Distance() < 1.4f)
			{
				m_isBattleStart_Stealth = true;
				m_dDelay_BattleStart_Stealth = 0.0;
				m_pModelCom->Set_Animation(ANIM_DMG_BIG_BACK);

				CGameInstance* pGameInstance = CGameInstance::GetInstance();
				Safe_AddRef(pGameInstance);
				pGameInstance->Time_Slow(0.3, 0.3);
				Safe_Release(pGameInstance);
			}
		}
	}
	
	_float4 HitDir = CMonsterManager::GetInstance()->Get_DirStealthAtk();
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_BIG_BACK, 1.3f, 0.09f, HitDir, true);

	//배틀 스타트
	if (m_isBattleStart_Stealth)
	{
		m_dDelay_BattleStart_Stealth += dTimeDelta;

		if (m_dDelay_BattleStart_Stealth > 0.5f)
		{
			m_dDelay_BattleStart_Stealth = 0.0;
			m_isBattleStart_Stealth = false;

			m_isDeath_Stealth = true;

			//배틀스타트
			CMonsterManager::GetInstance()->Set_StealthEnd_BattleStart(true);
			CMonsterManager::GetInstance()->Set_StealthEnd_BattleStart_Fade(true);
		}
	}
}

HRESULT CMonster_StealthZako::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Zako_0"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CMonster_StealthZako_0");
		return E_FAIL;
	}

	

	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CMonster_StealthZako");
		return E_FAIL;
	}


	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CMonster_StealthZako");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CMonster_StealthZako");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CMonster_StealthZako");
		return E_FAIL;
	}

	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.0f , 1.0f , 1.0f );
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CMonster_StealthZako");
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

HRESULT CMonster_StealthZako::SetUp_ShaderResources()
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

CMonster_StealthZako* CMonster_StealthZako::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_StealthZako* pInstance = new CMonster_StealthZako(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_StealthZako");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_StealthZako::Clone(void* pArg)
{
	CMonster_StealthZako* pInstance = new CMonster_StealthZako(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_StealthZako");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_StealthZako::Free()
{
	
	__super::Free();
}
