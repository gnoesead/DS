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

#include "Camera_Manager.h"


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

	Safe_Release(pGameInstance);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 150.f,0.f,150.f,1.f });
	m_pTransformCom->Set_Look(_float4{0.0f, 0.0f, -1.0f, 0.0f});

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
	if (pGameInstance->Get_DIKeyDown(DIK_X))
	{
		m_pRendererCom->Set_GrayScale();
	}
	Safe_Release(pGameInstance);

	if (true == m_isDead)
		return;

	Key_Input_PlayerChange(dTimeDelta);
	//playerswap
	if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 0) //탄지로
	{
		Player_Change_Setting_Status(dTimeDelta);
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


		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this)))
			return;
	}

	_float4 TestPos;
	XMStoreFloat4(&TestPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_int ak = 47;
}

void CPlayer_Tanjiro::LateTick(_double dTimeDelta)
{
	if (m_isSwap_OnSky == false)
	{
		m_pSword->LateTick(dTimeDelta);
		m_pSwordHome->LateTick(dTimeDelta);
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
#pragma endregion
	}
	return S_OK;
}

HRESULT CPlayer_Tanjiro::Render_ShadowDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	

	_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector   vLightEye = vPlayerPos + XMVectorSet(-5.f, 10.f, -5.f, 1.f);
	_vector   vLightAt = vPlayerPos;
	//_vector   vLightAt = XMVectorSet(60.f, 0.f, 60.f, 1.f);
	_vector   vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);



	_matrix      LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);
	_float4x4   FloatLightViewMatrix;
	XMStoreFloat4x4(&FloatLightViewMatrix, LightViewMatrix);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix",
		&FloatLightViewMatrix)))
		return E_FAIL;

	_matrix      LightProjMatrix;
	_float4x4   FloatLightProjMatrix;

	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(30.f), _float(1280) / _float(720), 0.2f, 300.f);
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


void CPlayer_Tanjiro::EventCall_Control(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0.0)
	{
		m_iEvent_Index = 0;
	}

	_vector vPlayerDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

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

				//CEffectPlayer::Get_Instance()->Play("ATK_Combo_Up", m_pTransformCom);
			}
			else if (1 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
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

			}
			else if (1 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
			}
			else if (2 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
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
			}
			else if (1 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
			}
		}

		if (24 == m_pModelCom->Get_iCurrentAnimIndex()) //Combo_Down
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Down", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo4_Down", m_pTransformCom);
			}
			if (1 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BIG, vPlayerDir, 2.0f);
			}
			else if (2 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
			}
			else if (3 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.5f), 0.1,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 2.0f);
			}
		}
		if (25 == m_pModelCom->Get_iCurrentAnimIndex()) // Combo_Normal
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Normal", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo4", m_pTransformCom);
			}
			else if (1 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BIG, vPlayerDir, 2.0f);
			}
		}
		if (26 == m_pModelCom->Get_iCurrentAnimIndex()) //Combo_Up
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_BasicCombo4_Up", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo4_Up", m_pTransformCom);
			}
			if (1 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_UPPER, vPlayerDir, 2.0f);
			}
		}
		if (27 == m_pModelCom->Get_iCurrentAnimIndex()) //Combo_Surge_Attack
		{
			if (0 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.5,
					CAtkCollider::TYPE_CUTSCENE, vPlayerDir, 9.0f);
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
			}
			if (1 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.0f);
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
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 2.0f);
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
				

				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super1", m_pTransformCom);
			}
			if (1 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			if (2 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			if (3 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			if (4 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			if (5 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, vDIr, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 10.0f);
			}
		}


		if (ANIM_ATK_SKILL_MOVE == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CBattle_UI_Manager::GetInstance()->Set_Player_Type(0);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(1);
				
				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super2", m_pTransformCom);
			}
		}
		if (40 == m_pModelCom->Get_iCurrentAnimIndex()) // SKILL_MOVE 도중
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			if (2 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			if (3 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			if (4 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
		}
		if (41 == m_pModelCom->Get_iCurrentAnimIndex()) // SKILL_MOVE 도중
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.0f), 0.1,
					CAtkCollider::TYPE_BIG, vPlayerDir, 10.0f);
			}
			
		}


		if (ANIM_ATK_SKILL_GUARD == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CBattle_UI_Manager::GetInstance()->Set_Player_Type(0);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(2);
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(5.5f, 5.5f, 5.5f), _float3(0.f, 0.0f, 0.0f), 0.3,
					CAtkCollider::TYPE_UPPER, vPlayerDir, 15.0f);

				CEffectPlayer::Get_Instance()->Play("Tanjiro_Super3", m_pTransformCom);
			}
			
			
		}
#pragma endregion


#pragma region Air_Attack
		if (ANIM_ATK_AIRTRACK == m_pModelCom->Get_iCurrentAnimIndex())
		{
			
		}
		if (50 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 0.0f, 0.5f), 0.6,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.0f);
			}
		}

#pragma endregion



#pragma region Dash_Tackle
		if (ANIM_BATTLE_DASH == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.5f, 1.5f), 5.0,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.0f);
			}
		}
		
#pragma endregion


#pragma region Charge_Attack
		if (33 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Tanjiro_Tilt", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeTilt", m_pTransformCom);
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.5f, 1.5f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 2.0f);
			}
			else if (2 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.5f, 1.5f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 2.0f);
			}
			else if (3 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.5f, 1.5f), 0.1,
					CAtkCollider::TYPE_BLOW, vPlayerDir, 2.0f);
			}

		}

#pragma endregion

		m_iEvent_Index++;
	}
}

void CPlayer_Tanjiro::Animation_Control(_double dTimeDelta)
{
	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_DIKeyDown(DIK_Z))
		m_isBattleStart = true;
	Safe_Release(pGameInstance);

	if (m_isBattleStart)
	{
		m_dDelay_BattleStart += dTimeDelta;
		if (m_dDelay_BattleStart > 1.5f)
		{
			m_pModelCom->Set_Animation(ANIM_BATTLESTART);
			m_isBattleStart = false;
			m_dDelay_BattleStart = 0.0;
		}
	}


	Moving_Restrict();

	if (m_ePlayerState == PLAYER_BATTLE)
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
		//m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.5f);
		m_fMove_Speed = 2.0f;

		if (m_pModelCom->Get_iCurrentAnimIndex() == 88)
		{
			if (m_isCanNavi)
				m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange, m_pNavigationCom[m_eCurNavi]);
			else
				m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange);
		}
		//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed);
	}

	if (m_Moveset.m_Up_Battle_Run)
	{
		m_Moveset.m_Up_Battle_Run = false;
		m_pModelCom->Set_Animation(ANIM_BATTLE_RUN_END);
	}
	Go_Straight_Deceleration(dTimeDelta, ANIM_BATTLE_RUN_END, m_fMove_Speed * m_fScaleChange, 0.15f);
}

void CPlayer_Tanjiro::Animation_Control_Battle_Jump(_double dTimeDelta)
{
	if (m_Moveset.m_Down_Battle_JumpMove)
	{
		//m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
		m_Moveset.m_Down_Battle_JumpMove = false;
		m_isJump_Move = true;

		m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP);
		Jumping(4.5f * m_fScaleChange, 0.2f * m_fScaleChange);

		m_pModelCom->Set_EarlyEnd(85, true);
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
		Jumping(4.5f * m_fScaleChange, 0.2f * m_fScaleChange);

		m_pModelCom->Set_EarlyEnd(85, true);
	}
	


	//공중 공격 콤보
	if (m_Moveset.m_Down_Battle_Jump_Attack)
	{
		m_Moveset.m_Down_Battle_Jump_Attack = false;
		m_isJump_Move = false;

		if(Get_LockOn_MonPos())
			m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		
		//콤보 첫 애니메이션 설정
		if (m_pModelCom->Get_Combo_Doing() == false)
		{
			m_pModelCom->Set_Combo_Doing(true);
			m_pModelCom->Set_Animation(ANIM_ATK_AIRCOMBO);

			JumpStop(0.3);
		}
		//아닐경우, 다음 콤보로 진행
		else
		{
			m_pModelCom->Set_Combo_Trigger(true);

			m_pModelCom->Set_EarlyEnd(30, true);
			JumpStop(0.65);
			m_isFirst_JumpAtk = false;
		}
	}


	//공중 트랙공격 (이동키 + 공격키)
	if (m_Moveset.m_Down_Battle_Jump_TrackAttack)
	{
		m_Moveset.m_Down_Battle_Jump_TrackAttack = false;

		m_isFirst_JumpAtk = false;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}

		m_pModelCom->Set_Animation(ANIM_ATK_AIRTRACK);
		JumpStop(0.3);
		Set_FallingStatus(3.0f, 0.0f);
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
			if (Get_LockOn_MonPos())
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

			if(21 == iCurAnimIndex)
				m_pModelCom->Set_EarlyEnd(21, true, 0.5f);
			if (22 == iCurAnimIndex)
				m_pModelCom->Set_EarlyEnd(22, true, 0.5f);
			if (23 == iCurAnimIndex)
				m_pModelCom->Set_EarlyEnd(23, true, 0.5f);
			if (25 == iCurAnimIndex)
				m_pModelCom->Set_EarlyEnd(25, true, 0.99f);

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
	m_pModelCom->Set_EarlyEnd(21, false, 0.5f);
	m_pModelCom->Set_EarlyEnd(22, false, 0.5f);
	m_pModelCom->Set_EarlyEnd(23, false, 0.5f);

	m_pModelCom->Set_EarlyEnd(25, false, 0.99f);

	// 공격 모션별 전진이동 제어 (Timedelta, 애니메이션인덱스,  초기화속도,  감속도)
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO, 3.0f * m_fScaleChange, 0.3f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 22, 3.0f * m_fScaleChange, 0.16f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 23, 4.0f * m_fScaleChange, 0.11f * m_fScaleChange);
	//분기
	Go_Straight_Deceleration(dTimeDelta, 24, 3.2f * m_fScaleChange, 0.05f * m_fScaleChange); // Down
	Go_Straight_Deceleration(dTimeDelta, 25, 5.0f * m_fScaleChange, 0.35f * m_fScaleChange); // Normal
	Go_Straight_Deceleration(dTimeDelta, 26, 3.0f * m_fScaleChange, 0.29f * m_fScaleChange); // Up

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
		m_isHit_SurgeCutScene = false;

		m_pModelCom->Set_Animation(28);
		m_isReset_Atk_MoveControl = true;
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == 28)
	{
		m_dDelay_SurgeCutScene += dTimeDelta;
		if (0.5f < m_dDelay_SurgeCutScene)
		{
			Go_Straight_Deceleration(dTimeDelta, 28, 4.5f, 0.1f);
		}

		if (0.5f < m_dDelay_SurgeCutScene && m_dDelay_SurgeCutScene <= 1.0f)
		{
			m_pTransformCom->Go_Left(dTimeDelta, m_pNavigationCom[m_eCurNavi]);
		}
		else if (1.0f < m_dDelay_SurgeCutScene && m_dDelay_SurgeCutScene <= 1.5f)
		{
			m_pTransformCom->Go_Right(dTimeDelta, m_pNavigationCom[m_eCurNavi]);
		}
		
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
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(ANIM_ATK_CHARGE);
	}


	if (m_Moveset.m_Up_Battle_Charge && m_dDelay_Charge > 1.0f)
	{
		m_Moveset.m_Up_Battle_Charge = false;
		m_dDelay_Charge = 0.0;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(33);
	}
	else if (m_Moveset.m_Up_Battle_Charge && m_dDelay_Charge <= 1.0f)
	{
		m_Moveset.m_Up_Battle_Charge = false;
		m_dDelay_Charge = 0.0;

		m_pModelCom->Set_Animation(ANIM_BATTLE_IDLE);
	}
	Go_Straight_Deceleration(dTimeDelta, 33, 3.0f * m_fScaleChange, 0.03f * m_fScaleChange);
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

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_NORMAL);
		Jumping(4.0f * m_fScaleChange, 0.18f * m_fScaleChange);

		Use_Mp_Skill();
		
	}
	
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_SKILL_NORMAL, 3.0f * m_fScaleChange, 0.07f * m_fScaleChange);
		
	

	//스킬_1 : 이동키 + I키
	if (m_Moveset.m_Down_Skill_Move)
	{
		m_Moveset.m_Down_Skill_Move = false;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_MOVE);
		Jumping(0.3f * m_fScaleChange, 0.07f * m_fScaleChange);

		m_pModelCom->Get_Animation()->Set_EarlyEnd(true);

		Use_Mp_Skill();
	}
	Go_Straight_Constant(dTimeDelta, ANIM_ATK_SKILL_MOVE, 1.5f * m_fScaleChange);
	Go_Straight_Constant(dTimeDelta, 40, 1.5f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 41, 1.5f * m_fScaleChange, 0.07f * m_fScaleChange);


	//스킬_2 : 가드키 + I키
	if (m_Moveset.m_Down_Skill_Guard)
	{
		m_Moveset.m_Down_Skill_Guard = false;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}

		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_GUARD);
		Jumping(3.0f * m_fScaleChange, 0.05f * m_fScaleChange);

		Use_Mp_Skill();
	}
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
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		//m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
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
			if (Get_LockOn_MonPos())
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
	}
	Go_Straight_Constant(dTimeDelta, 80, 3.0f * m_fScaleChange);

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

		//m_pTransformCom->Set_Look(m_vLook);
		m_pTransformCom->LerpVector(XMLoadFloat4(&m_vLook), 0.8f);
		if(m_isForward)
			m_pModelCom->Set_Animation(ANIM_BATTLE_STEP_F);
		else if(m_isBack)
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

					m_pModelCom->Set_EarlyEnd(ANIM_BATTLE_STEP_L, true);
				}
				//아닐경우, 다음 콤보로 진행
				else
				{
					m_pModelCom->Set_Combo_Trigger(true);
					//Jumping(3.0f * m_fScaleChange, 0.25f * m_fScaleChange);
				}
			}
			else if (m_isRight)
			{
				//콤보 첫 애니메이션 설정
				if (m_pModelCom->Get_Combo_Doing() == false)
				{
					m_pModelCom->Set_Combo_Doing(true);
					m_pModelCom->Set_Animation(ANIM_BATTLE_STEP_R);
					
					m_pModelCom->Set_EarlyEnd(ANIM_BATTLE_STEP_R, true);
				}
				//아닐경우, 다음 콤보로 진행
				else
				{
					m_pModelCom->Set_Combo_Trigger(true);
					//Jumping(1.0f * m_fScaleChange, 0.06f * m_fScaleChange);
				}
			}
		}
	}
	_vector vDir = XMLoadFloat4(&m_Moveset.m_Input_Dir);
	_float4 fDir;
	XMStoreFloat4(&fDir, -vDir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_F, 4.5f * m_fScaleChange, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_B, 4.5f * m_fScaleChange, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_L, 4.5f * m_fScaleChange, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_R, 4.5f * m_fScaleChange, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);

	//더블스텝
	Go_Dir_Deceleration(dTimeDelta, 98, 5.0f * m_fScaleChange, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, 100, 5.0f * m_fScaleChange, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
}

void CPlayer_Tanjiro::Animation_Control_Battle_Awaken(_double dTimeDelta)
{
	if (m_Moveset.m_Down_Battle_Awaken)
	{
		m_Moveset.m_Down_Battle_Awaken = false;

		if (m_Moveset.m_iAwaken == 1)
		{
			m_pModelCom->Set_Animation(ANIM_BATTLE_AWAKEN);
		}
		else if (m_Moveset.m_iAwaken == 2)
		{
			m_pModelCom->Set_Animation(ANIM_BATTLE_AWAKEN_COMPLETE_CUTSCENE);
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
	if(m_dTime_Special_Ready >1.5f)
		Go_Straight_Deceleration(dTimeDelta, 109, 4.0f * m_fScaleChange, 0.23f * m_fScaleChange);
	
	//Go_Straight_Constant(dTimeDelta, 108, 2.7f);
	if (m_pModelCom->Get_iCurrentAnimIndex() == 108 || m_pModelCom->Get_iCurrentAnimIndex() == 109)
	{
		if (m_isFirst_SpecialReady)
		{
			m_isFirst_SpecialReady = false;
			if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
			{
				if (Get_LockOn_MonPos())
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
	else if (0.35f <= m_dTime_Special_CutScene )
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

		//m_pTransformCom->Set_Look(reverseAtkDir);
		m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

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
	if (m_isConnectHitting == false)
	{
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL, 1.5f, 0.01f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 139, 1.0f, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 140, 1.0f, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 141, 1.0f, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 142, 1.0f, 0.015f, AtkDir);
	}
#pragma endregion



#pragma region Dmg_Big
	if (m_Moveset.m_Down_Dmg_Big)
	{
		m_Moveset.m_Down_Dmg_Big = false;

		m_pModelCom->Set_Animation(ANIM_DMG_BIG);
		m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

	}
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_BIG, 2.0f, 0.035f,  AtkDir);
	
#pragma endregion


#pragma region Dmg_Blow
	if (m_Moveset.m_Down_Dmg_Blow)
	{
		m_Moveset.m_Down_Dmg_Blow = false;

		m_pModelCom->Set_Animation(ANIM_DMG_BLOW);
		m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		Jumping(1.2f, 0.05f);
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_BLOW, 2.5f, AtkDir);
	Go_Dir_Constant(dTimeDelta, 120, 2.5f, AtkDir);
	Ground_Animation_Play(120, 121);

#pragma endregion


#pragma region Dmg_BigBlow
	if (m_Moveset.m_Down_Dmg_BigBlow)
	{
		m_Moveset.m_Down_Dmg_BigBlow = false;

		m_pModelCom->Set_Animation(ANIM_DMG_SPIN);
		m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		Jumping(1.2f, 0.05f);
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_SPIN, 3.0f, AtkDir);
	Go_Dir_Constant(dTimeDelta, 132, 3.0f, AtkDir);
	Go_Dir_Deceleration(dTimeDelta, 133, 3.0f, 0.1f, AtkDir);
	Ground_Animation_Play(132, 133);

#pragma endregion


#pragma region Dmg_Upper
	if (m_Moveset.m_Down_Dmg_Upper)
	{
		m_Moveset.m_Down_Dmg_Upper = false;

		m_pModelCom->Set_Animation(ANIM_FALL);
		m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		Jumping(1.7f, 0.03f);
	}
	Go_Dir_Constant(dTimeDelta, ANIM_FALL, 0.5f, AtkDir);
	Go_Dir_Constant(dTimeDelta, 125, 0.5f, AtkDir);
	Ground_Animation_Play(125, 126);
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
	Go_Straight_Deceleration(dTimeDelta, 138, 3.0f, 0.03f);
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


		//무빙키입력들
		if (m_Moveset.m_Down_Battle_Run)
		{
			m_Moveset.m_Down_Battle_Run = false;
			m_pModelCom->Set_Animation(ANIM_ADV_RUN);
		}

		if (m_Moveset.m_State_Battle_Run)
		{
			//m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
			m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.3f);
			m_fMove_Speed = 2.0f;

			if (m_isCanNavi)
				m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange * 0.7f, m_pNavigationCom[m_eCurNavi]);
			else
				m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange * 0.7f);
			//m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed);
		}

		if (m_Moveset.m_Up_Battle_Run)
		{
			m_Moveset.m_Up_Battle_Run = false;
			m_pModelCom->Set_Animation(ANIM_ADV_IDLE);
		}
		Go_Straight_Deceleration(dTimeDelta, ANIM_ADV_RUN_END, m_fMove_Speed * m_fScaleChange * 0.7f, 0.18f);
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
		
		//m_isBoxJumping = true;
		//떨어지는
		if (m_isPlayerStatus_OnRoof)
			Jumping(1.0f, 0.07f);			// 처음 점프 // 파워 , 감속도
		//올라가는
		else
			Jumping(1.55f, 0.06f);			
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
	}

	if (0.1f < m_fDistanceTo_Box)
	{
		//올라갈때
		if (m_isPlayerStatus_OnRoof == false)
		{
			Go_Straight_Constant(dTimeDelta, ANIM_ADV_JUMP, 0.6f , true);
			Go_Straight_Constant(dTimeDelta, 2, 0.6f, true);
		}
		//내려갈때 
		else
		{
			Go_Straight_Constant(dTimeDelta, ANIM_ADV_JUMP, 0.4f);
			Go_Straight_Constant(dTimeDelta, 2, 0.4f);
		}
	}

	if (m_isPlayerStatus_OnRoof == false)
	{
		if (NAVI_VILLAGE_WALL == m_eNextNavi)
		{
			Go_Dir_Constant(dTimeDelta, 3, 0.3f, m_Dir_ScondJump_Box);
			Go_Dir_Constant(dTimeDelta, 85, 0.3f, m_Dir_ScondJump_Box);
		}
		else
		{
			Go_Dir_Constant(dTimeDelta, 3, 0.65f, m_Dir_ScondJump_Box , true);
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
			if(m_isPlayerStatus_OnRoof)
				Jumping(1.1f, 0.07f);		// 두번째 올라갈때 점프(땅)
			//올라가는
			else
				Jumping(2.15f, 0.08f);
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
				Jumping(1.1f, 0.08f); // 지붕에서 내려갈때 두번째 점프
			//올라가는
			else
				Jumping(2.45f, 0.08f);
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
		if (m_dDelay_Player_Change < 1.5)
		{
			m_pTransformCom->Go_Up(dTimeDelta * 5.0f);

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
		|| 139 == iCurAnimIndex || 140 == iCurAnimIndex || 141 == iCurAnimIndex || 142 == iCurAnimIndex)
	{
		m_Moveset.m_isHitMotion = true;
		
		//다운상태
		if (ANIM_DOWN == iCurAnimIndex)
		{
			m_Moveset.m_isDownMotion = true;
			m_Moveset.m_Down_Dmg_Blow = false;
		}

		//겟업 상태
		if (ANIM_DOWN_GETUP == iCurAnimIndex || 135 == iCurAnimIndex || ANIM_DMG_BIG_RETURN == iCurAnimIndex
			|| ANIM_DOWN_GETUP_MOVE == iCurAnimIndex || 138 == iCurAnimIndex || ANIM_DMG_SMALL_RETURN == iCurAnimIndex)
		{
			m_Moveset.m_isGetUpMotion = true;
		}
	}
	//콤보공격시 무빙제한
	else if (ANIM_ATK_COMBO == iCurAnimIndex
		|| 22 == iCurAnimIndex || 23 == iCurAnimIndex
		|| 24 == iCurAnimIndex || 25 == iCurAnimIndex || 26 == iCurAnimIndex
		|| 27 == iCurAnimIndex || 28 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;

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
	else if (ANIM_ATK_CHARGE == iCurAnimIndex || 32 == iCurAnimIndex || 33 == iCurAnimIndex )
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Jump = true;
		m_Moveset.m_isRestrict_Charge = true;
	}
	//스킬공격 시 무빙제한
	else if (ANIM_ATK_SKILL_GUARD == iCurAnimIndex || 35 == iCurAnimIndex || 36 == iCurAnimIndex || 37 == iCurAnimIndex
		|| ANIM_ATK_SKILL_MOVE == iCurAnimIndex || 39 == iCurAnimIndex || 40 == iCurAnimIndex || 41 == iCurAnimIndex
		|| ANIM_ATK_SKILL_NORMAL == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
	} 
	//잡기 공격 시 제한
	else if (ANIM_ATK_THROW == iCurAnimIndex )
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
	else if (ANIM_BATTLE_AWAKEN == iCurAnimIndex || ANIM_BATTLE_AWAKEN_COMPLETE_CUTSCENE == iCurAnimIndex )
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Jump = true;
		m_Moveset.m_isRestrict_Charge = true;
		m_Moveset.m_isRestrict_Step = true;
		m_Moveset.m_isRestrict_Dash = true;
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
	}
	//대시 시 제한
	else if (ANIM_BATTLE_DASH == iCurAnimIndex || 80 == iCurAnimIndex /* || 81 == iCurAnimIndex */ )
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Dash = true;
	}
	//점프 시 무빙제한
	else if (ANIM_BATTLE_JUMP == iCurAnimIndex
		|| 84 == iCurAnimIndex || 85 == iCurAnimIndex )
	{
		if (m_ePlayerState == PLAYER_BATTLE)
		{
			m_Moveset.m_isRestrict_Move = true;
			m_Moveset.m_isRestrict_Jump = true;
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
	else if ( 98 == iCurAnimIndex || 100 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		
		m_Moveset.m_isRestrict_DoubleStep = true;
	}
	//어드벤처 모드 런, 기본
	else if (ANIM_ADV_IDLE == iCurAnimIndex || ANIM_ADV_RUN == iCurAnimIndex || ANIM_ADV_RUN_END == iCurAnimIndex)
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
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineThickness", &m_fOutlineThickness, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineFaceThickness", &m_fOutlineFaceThickness, sizeof(_float))))
		return E_FAIL;

	// 슈퍼아머 상태 넣어주셈
	/*if (FAILED(m_pShaderCom->SetUp_RawValue("g_bSuperArmor", &m_bAwake, sizeof(_bool))))
		return E_FAIL;*/



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
