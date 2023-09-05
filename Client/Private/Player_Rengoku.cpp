#include "pch.h"
#include "..\Public\Player_Rengoku.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"
#include "DialogManager.h"
#include "MissionManager.h"
#include "Battle_UI_Manager.h"

#include "PlayerManager.h"

#include "Camera_Manager.h"
#include "OptionManager.h"
#include "Camera_Free.h"



CPlayer_Rengoku::CPlayer_Rengoku(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer(pDevice, pContext)
{
}

CPlayer_Rengoku::CPlayer_Rengoku(const CPlayer_Rengoku& rhs)
	: CPlayer(rhs)
{
}

HRESULT CPlayer_Rengoku::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Rengoku::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_pModelCom->Set_Animation(ANIM_IDLE);

	if (FAILED(Read_Animation_Control_File("Rengoku.bin")))
	{
		MSG_BOX("Failed to AnimData Read : Rengoku");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSword::SWORDDESC SwordDesc;
	ZeroMemory(&SwordDesc, sizeof SwordDesc);
	SwordDesc.m_PlayerName = CSword::PLAYER_RENGOKU;
	SwordDesc.pParentTransform = m_pTransformCom;
	SwordDesc.pBone = m_pModelCom->Get_Bone("R_HandCommon_1_Lct");
	m_pSword = dynamic_cast<CSword*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Sword"), &SwordDesc));

	CSwordHome::SWORDDESC SwordHomeDesc;
	ZeroMemory(&SwordHomeDesc, sizeof SwordHomeDesc);
	SwordHomeDesc.m_PlayerName = CSwordHome::PLAYER_RENGOKU;
	SwordHomeDesc.pParentTransform = m_pTransformCom;
	SwordHomeDesc.pBone = m_pModelCom->Get_Bone("L_Weapon_1");
	m_pSwordHome = dynamic_cast<CSwordHome*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_SwordHome"), &SwordHomeDesc));

	Safe_Release(pGameInstance);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 150.f,0.f,150.f,1.f });
	m_pTransformCom->Set_Look(_float4{0.0f, 0.0f, -1.0f, 0.0f});

	m_ePlayerState = PLAYER_BATTLE;
	return S_OK;
}

void CPlayer_Rengoku::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pSword->Tick(dTimeDelta);
	m_pSwordHome->Tick(dTimeDelta);

	if (true == m_isDead)
		return;

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

void CPlayer_Rengoku::LateTick(_double dTimeDelta)
{
	m_pSword->LateTick(dTimeDelta);
	m_pSwordHome->LateTick(dTimeDelta);

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

#ifdef _DEBUG
	if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom[m_eCurNavi])))
		return;
#endif
}

HRESULT CPlayer_Rengoku::Render()
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

	return S_OK;
}

HRESULT CPlayer_Rengoku::Render_ShadowDepth()
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


void CPlayer_Rengoku::EventCall_Control(_double dTimeDelta)
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
/*				if (m_Moveset.m_iAwaken == 0)	*/				
					CEffectPlayer::Get_Instance()->Play("Rengoku_BasicCombo1", m_pTransformCom);
				//else
				//	CEffectPlayer::Get_Instance()->Play("Tanjiro_SurgeCombo1", m_pTransformCom);

				//CEffectPlayer::Get_Instance()->Play("Hit_Spark", m_pTransformCom);
				//CEffectPlayer::Get_Instance()->Play("Hit_Shock", m_pTransformCom);
			}
			if (1 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Rengoku_BasicCombo1_Decal", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Rengoku_BasicCombo1_DecalParticle", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.8f, 1.8f, 1.8f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.5f);
			}
		}
		else if (1 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Rengoku_BasicCombo2", m_pTransformCom);
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.8f, 1.8f, 1.8f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.f);
			}
			else if (2 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.8f, 1.8f, 1.8f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.f);
			}
		}
		else if (2 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Rengoku_BasicCombo3", m_pTransformCom);
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.8f, 1.8f, 1.8f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.5f);
			}
			else if (2 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.8f, 1.8f, 1.8f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 2.f);
			}
		}
		else if (ANIM_ATK_COMBO_DOWN == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Rengoku_BasicCombo4_Down", m_pTransformCom);
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.8f, 1.8f, 1.8f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.5f);
			}
			else if (2 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 2.5f);
			}
		}
		else if (ANIM_ATK_COMBO_NORMAL == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Rengoku_BasicCombo4_Normal", m_pTransformCom);
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 3.5f);
			}
		}
		else if (ANIM_ATK_COMBO_UP == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Rengoku_BasicCombo4_Up", m_pTransformCom);
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.7f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (2 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 2.2f), 0.1,
					CAtkCollider::TYPE_UPPER, vPlayerDir, 2.0f);
			}
		}
		if (ANIM_ATK_COMBO_SURGE == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.6f, 1.6f, 1.6f), _float3(0.f, 1.0f, 1.0f), 1.0,
					CAtkCollider::TYPE_CUTSCENE, vPlayerDir, 1.0f);
			}
		}

#pragma endregion



#pragma region Super_Skill
		if (ANIM_ATK_SKILL_GUARD == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.4f, 2.4f, 2.4f), _float3(0.f, 1.0f, 2.5f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 4.3f);
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.8f, 2.8f, 2.8f), _float3(0.f, 1.0f, 2.5f), 0.3,
					CAtkCollider::TYPE_UPPER, vPlayerDir, 8.5f);
			}
		}

		if (ANIM_ATK_SKILL_MOVE == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.4f, 2.4f, 2.4f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 4.3f);
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.8f, 2.8f, 2.8f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 8.5f);
			}
		}
		if (ANIM_ATK_SKILL_MOVE_AIR == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.8f, 2.8f, 2.8f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 11.3f);
			}
			
		}

		if (ANIM_ATK_SKILL_NORMAL == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				//이펙트용
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.5f);
			}
			else if (2 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(4.0f, 4.0f, 4.0f), _float3(0.f, 1.0f, 3.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (3 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(4.0f, 4.0f, 4.0f), _float3(0.f, 1.0f, 3.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (4 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(4.0f, 4.0f, 4.0f), _float3(0.f, 1.0f, 3.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (5 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(4.0f, 4.0f, 4.0f), _float3(0.f, 1.0f, 3.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (6 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(4.0f, 4.0f, 4.0f), _float3(0.f, 1.0f, 3.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (7 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(4.0f, 4.0f, 4.0f), _float3(0.f, 1.0f, 3.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (8 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(4.0f, 4.0f, 4.0f), _float3(0.f, 1.0f, 3.0f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (9 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(4.5f, 4.5f, 4.5f), _float3(0.f, 1.0f, 3.0f), 0.1,
					CAtkCollider::TYPE_BLOW, vPlayerDir, 11.2f);
			}
		}
		if (ANIM_ATK_SKILL_NORMAL_AIR == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				//이펙트
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.5f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (2 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.5f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (3 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.5f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (4 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.5f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (5 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.5f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (6 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.5f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (7 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.5f), 0.1,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 10.0f);
			}
		}

#pragma endregion



#pragma region Air_Attack
		if (ANIM_ATK_AIR_COMBO == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.8f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
			}
			else if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.3f, 2.3f, 2.3f), _float3(0.f, 1.0f, 1.8f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.5f);
			}
		}
		else if (9 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.6f, 2.6f, 2.6f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 4.5f);
			}
		}

		if (ANIM_ATK_AIRTRACK == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.8f, 1.8f, 1.8f), _float3(0.f, 1.0f, 1.7f), 1.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
			}
		}
#pragma endregion



#pragma region Dash_Tackle
		if (ANIM_DASH_RUN == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.8f, 1.8f, 1.8f), _float3(0.f, 1.0f, 1.7f), 5.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
			}
		}
#pragma endregion



#pragma region Charge_Attack
		if (12 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 1.0f, 1.8f), 0.3,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 8.0f);
			}
		}
#pragma endregion

		m_iEvent_Index++;
	}
}

void CPlayer_Rengoku::Animation_Control(_double dTimeDelta)
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
			m_pModelCom->Set_Animation(ANIM_VSBATTLE_START);
			m_isBattleStart = false;
			m_dDelay_BattleStart = 0.0;
		}
	}


	Moving_Restrict();

	
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

void CPlayer_Rengoku::Animation_Control_Battle_Move(_double dTimeDelta)
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
		m_pModelCom->Set_Animation(ANIM_RUN);
	}

	if (m_Moveset.m_State_Battle_Run)
	{
		m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		//m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.5f);
		m_fMove_Speed = 2.0f;

		if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_RUN )
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
		m_pModelCom->Set_Animation(ANIM_RUN_END);
	}
	Go_Straight_Deceleration(dTimeDelta, ANIM_RUN_END, m_fMove_Speed * m_fScaleChange, 0.15f);
}

void CPlayer_Rengoku::Animation_Control_Battle_Jump(_double dTimeDelta)
{
	if (m_Moveset.m_Down_Battle_JumpMove)
	{
		m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		//m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
		m_Moveset.m_Down_Battle_JumpMove = false;
		m_isJump_Move = true;

		m_pModelCom->Set_Animation(ANIM_JUMP);
		Jumping(4.0f * m_fScaleChange, 0.2f * m_fScaleChange);

		m_pModelCom->Set_EarlyEnd(35, true);
	}
	if (m_isJump_Move)
	{
		Go_Straight_Constant(dTimeDelta, ANIM_JUMP, m_fMove_Speed * 1.2f * m_fScaleChange);
		Go_Straight_Constant(dTimeDelta, 34, m_fMove_Speed * 1.2f * m_fScaleChange);
		Go_Straight_Constant(dTimeDelta, 35, m_fMove_Speed * 1.2f * m_fScaleChange);
		//Go_Straight_Deceleration(dTimeDelta, 86, m_fMove_Speed * 1.2f * m_fScaleChange, 0.36f * m_fScaleChange); // Down
	}
	Ground_Animation_Play(35, 36);
	m_pModelCom->Set_LinearDuration(ANIM_JUMP, 0.001f);
	m_pModelCom->Set_LinearDuration(34, 0.001f);
	m_pModelCom->Set_LinearDuration(35, 0.001f);
	m_pModelCom->Set_LinearDuration(36, 0.001f);


	if (m_Moveset.m_Down_Battle_Jump)
	{
		m_Moveset.m_Down_Battle_Jump = false;
		m_isJump_Move = false;

		m_pModelCom->Set_Animation(ANIM_JUMP);
		Jumping(4.0f * m_fScaleChange, 0.2f * m_fScaleChange);

		m_pModelCom->Set_EarlyEnd(35, true);
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
			m_pModelCom->Set_Animation(ANIM_ATK_AIR_COMBO);
			m_pModelCom->Set_EarlyEnd(ANIM_ATK_AIR_COMBO, true, 0.5f);

			JumpStop(0.6);
		}
		//아닐경우, 다음 콤보로 진행
		else
		{
			m_pModelCom->Set_Combo_Trigger(true);

			m_pModelCom->Set_EarlyEnd(9, true, 0.7f);
			JumpStop(0.85);
			m_isFirst_JumpAtk = false;
		}
	}
	m_pModelCom->Set_EarlyEnd(ANIM_ATK_AIR_COMBO, false, 0.5f);


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
		JumpStop(0.45);
		Set_FallingStatus(3.2f, 0.0f);
	}
	Ground_Animation_Play(25, 26);
	Go_Straight_Constant(dTimeDelta, 25, 4.2f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 26, 4.2f * m_fScaleChange, 0.2f * m_fScaleChange); // Down

	m_pModelCom->Set_EarlyEnd(26, true, 0.6f);
}

void CPlayer_Rengoku::Animation_Control_Battle_Attack(_double dTimeDelta)
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

			if(ANIM_ATK_COMBO == iCurAnimIndex)
				m_pModelCom->Set_EarlyEnd(0, true, 0.25f);
			if (1 == iCurAnimIndex)
				m_pModelCom->Set_EarlyEnd(1, true, 0.33f);
			if (2 == iCurAnimIndex)
				m_pModelCom->Set_EarlyEnd(2, true, 0.50f);
			if (ANIM_ATK_COMBO_NORMAL == iCurAnimIndex)
				m_pModelCom->Set_EarlyEnd(4, true, 0.99f);

			//콤보 분기 설정
			if (2 == iCurAnimIndex)
			{
				// 아래콤보 s콤보
				if (m_Moveset.m_Down_Battle_Combo_Down)
				{
					m_pModelCom->Set_Combo_Another(ANIM_ATK_COMBO_DOWN);
					m_pModelCom->Set_EarlyEnd(ANIM_ATK_COMBO_DOWN, true, 0.99f);
				}
				// 위콤보 w콤보
				else if (m_Moveset.m_Down_Battle_Combo_Up)
				{
					m_pModelCom->Set_Combo_Another(ANIM_ATK_COMBO_UP);
					m_pModelCom->Set_EarlyEnd(ANIM_ATK_COMBO_UP, true, 0.99f);

					m_isCan_AirDash = true;
				}
				
				m_pModelCom->Set_AnimisFinish(ANIM_ATK_COMBO_NORMAL);
				
			}
		}
	}
	m_pModelCom->Set_EarlyEnd(ANIM_ATK_COMBO, false, 0.25f);
	m_pModelCom->Set_EarlyEnd(1, false, 0.33f);
	m_pModelCom->Set_EarlyEnd(2, false, 0.50f);

	m_pModelCom->Set_EarlyEnd(ANIM_ATK_COMBO_NORMAL, false, 0.99f);

	// 공격 모션별 전진이동 제어 (Timedelta, 애니메이션인덱스,  초기화속도,  감속도)
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO, 2.8f * m_fScaleChange * m_fAtk_Move_Ratio, 0.3f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 1, 2.8f * m_fScaleChange * m_fAtk_Move_Ratio, 0.16f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 2, 4.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.08f * m_fScaleChange);
	//분기
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO_DOWN, 3.2f * m_fScaleChange * m_fAtk_Move_Ratio, 0.05f * m_fScaleChange); // Down
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO_NORMAL, 5.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.35f * m_fScaleChange); // Normal
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO_UP, 3.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.29f * m_fScaleChange); // Up

	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_IDLE)
	{
		m_isComboing = false;
	}



	//각성상태 일때 + 노말히트마지막
	if (m_Moveset.m_iAwaken == 1 || m_Moveset.m_iAwaken == 2)
	{
		if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_ATK_COMBO_NORMAL)
		{
			m_pModelCom->Set_EarlyEnd(ANIM_ATK_COMBO_NORMAL, true, 0.76f);

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
	
	if (m_pModelCom->Get_iCurrentAnimIndex() != ANIM_ATK_COMBO_NORMAL)
	{
		m_isCan_Surge = false;
		m_Moveset.m_Down_Battle_Combo_Surge = false;
	}



	if (m_Moveset.m_Down_Battle_Combo_Surge)
	{
		m_Moveset.m_Down_Battle_Combo_Surge = false;
		m_isCan_Surge = false;
		m_dDelay_Surge = 0.0;

		m_pModelCom->Set_Animation(ANIM_ATK_COMBO_SURGE);
		m_dDelay_Surge_Attack = 0.0;
		
		m_isReset_Atk_MoveControl = true;
	}
	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_ATK_COMBO_SURGE)
	{
		m_dDelay_Surge_Attack += dTimeDelta;
		if (m_dDelay_Surge_Attack > 0.6f)
		{
			Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO_SURGE, 3.0f, 0.04f);
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

		m_pModelCom->Set_Animation(ANIM_ATK_COMBO_SURGE_CUTSCENE);
		m_isReset_Atk_MoveControl = true;
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_ATK_COMBO_SURGE_CUTSCENE)
	{
		m_dDelay_SurgeCutScene += dTimeDelta;
		if (0.5f < m_dDelay_SurgeCutScene)
		{
			Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO_SURGE_CUTSCENE, 4.5f, 0.1f);
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

void CPlayer_Rengoku::Animation_Control_Battle_Charge(_double dTimeDelta)
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


	if (m_Moveset.m_Up_Battle_Charge && m_pModelCom->Get_iCurrentAnimIndex() == 11)
	{
		m_Moveset.m_Up_Battle_Charge = false;
		
		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(12);
		m_isReset_Atk_MoveControl = true;
		m_dDelay_Charge_Move = 0.0;
	}
	else if (m_Moveset.m_Up_Battle_Charge )
	{
		m_Moveset.m_Up_Battle_Charge = false;

		m_pModelCom->Set_Animation(ANIM_IDLE);
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == 12)
	{
		m_dDelay_Charge_Move += dTimeDelta;
		if (m_dDelay_Charge_Move > 0.1f)
		{
			Go_Straight_Deceleration(dTimeDelta, 12, 3.3f * m_fScaleChange * m_fAtk_Move_Ratio, 0.035f * m_fScaleChange);
		}
	}
}

void CPlayer_Rengoku::Animation_Control_Battle_Skill(_double dTimeDelta)
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

	//스킬_노말 공중
	if (m_Moveset.m_Down_Skill_Normal && m_isJumpOn)
	{
		m_Moveset.m_Down_Skill_Normal = false;
		m_dDelay_CanSkill = 0.0;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_NORMAL_AIR);
		Jumping(1.2f * m_fScaleChange, 0.09f * m_fScaleChange);

		Use_Mp_Skill();
	}
	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_ATK_SKILL_NORMAL_AIR && m_isAir_Skill_Normal == false && m_isAir_Skill_Normal_1 == false)
	{
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if( Pos.y <= m_fLand_Y)
		{
			m_isAir_Skill_Normal = true;
			m_dDelay_Skill_Normal = 0.0;
		}
		else
		{
			Go_Straight_Constant(dTimeDelta, ANIM_ATK_SKILL_NORMAL_AIR, 1.2f * m_fScaleChange);
		}
	}
	if (m_isAir_Skill_Normal)
	{
		m_dDelay_Skill_Normal += dTimeDelta;
		if (m_dDelay_Skill_Normal > 0.3f)
		{
			m_isAir_Skill_Normal = false;
			m_dDelay_Skill_Normal = 0.0;
			m_isAir_Skill_Normal_1 = true;
			Jumping(1.f, 0.035f);
		}
	}
	if (m_isAir_Skill_Normal_1)
	{
		Go_Backward_Constant(dTimeDelta, ANIM_ATK_SKILL_NORMAL_AIR, 1.3f);

		m_dDelay_Skill_Normal += dTimeDelta;
		if (m_dDelay_Skill_Normal > 0.1f)
		{
			_float4 Pos;
			XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			if (Pos.y <= m_fLand_Y)
			{
				m_isAir_Skill_Normal_1 = false;
				m_dDelay_Skill_Normal = 0.0;
			}
		}
	}

	//스킬_노말 지상
	if (m_Moveset.m_Down_Skill_Normal)
	{
		m_Moveset.m_Down_Skill_Normal = false;
		m_dDelay_CanSkill = 0.0;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_NORMAL);

		Use_Mp_Skill();
	}
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_SKILL_NORMAL, 2.0f * m_fScaleChange, 0.07f * m_fScaleChange);
		
	

	//스킬_1 : 이동키 + I키 (Air)
	if (m_Moveset.m_Down_Skill_Move && m_isJumpOn)
	{
		m_Moveset.m_Down_Skill_Move = false;
		m_dDelay_CanSkill = 0.0;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_MOVE_AIR);
		JumpStop(1.5);
		Set_FallingStatus(0.10f, 0.1f);
		
		Use_Mp_Skill();
	}
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_SKILL_MOVE_AIR, 2.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.02f * m_fScaleChange);
	//Go_Straight_Deceleration(dTimeDelta, 19, 1.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.07f * m_fScaleChange);
	Ground_Animation_Play(19, 36);

	//스킬_1 : 이동키 + I키
	if (m_Moveset.m_Down_Skill_Move)
	{
		m_Moveset.m_Down_Skill_Move = false;
		m_dDelay_CanSkill = 0.0;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_MOVE);
		
		Use_Mp_Skill();
	}
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_SKILL_MOVE, 2.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.02f * m_fScaleChange);
	//Go_Straight_Deceleration(dTimeDelta, 17, 1.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.08f * m_fScaleChange);

	



	//스킬_2 : 가드키 + I키
	if (m_Moveset.m_Down_Skill_Guard)
	{
		m_Moveset.m_Down_Skill_Guard = false;
		m_dDelay_CanSkill = 0.0;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos())
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}

		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_GUARD);

		m_isSkill_Guard = true;
		m_dDelay_Skill_Guard = 0.0;

		Use_Mp_Skill();
	}
	Ground_Animation_Play(14, 15);
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_SKILL_GUARD, 2.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.02f * m_fScaleChange);
	if (m_isSkill_Guard)
	{
		m_dDelay_Skill_Guard += dTimeDelta;
		if (m_dDelay_Skill_Guard > 0.5f)
		{
			m_dDelay_Skill_Guard = 0.0;
			m_isSkill_Guard = false;
			Jumping(1.50f * m_fScaleChange, 0.04f * m_fScaleChange);
		}
	}
}

void CPlayer_Rengoku::Animation_Control_Battle_Guard(_double dTimeDelta)
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
		m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		//m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
		m_pModelCom->Set_Animation(ANIM_GUARD);
	}

	//가드 도중


	//가드 내리기
	if (m_Moveset.m_Up_Battle_Guard)
	{
		m_Moveset.m_Up_Battle_Guard = false;

		m_pModelCom->Set_Animation(47);
	}

	
	//잡기 ( O키 가드키 + J키 공격키)
	if (m_Moveset.m_Down_Battle_Throw)
	{
		m_Moveset.m_Down_Battle_Throw = false;

		m_isThrowing = true;
		//m_pModelCom->Set_Animation(ANIM_ATK_THROW);
	}
	//if (m_isThrowing && m_pModelCom->Get_iCurrentAnimIndex() == ANIM_BATTLE_IDLE && m_Moveset.m_State_Battle_Guard)
	//{
	//	m_isMaintain_Guard = true;
	//}
	
	//잡기 ( O키 가드키 + 이동키)
	if (m_Moveset.m_Down_Battle_Push)
	{
		m_Moveset.m_Down_Battle_Push = false;

		m_isThrowing = true;
		//m_pModelCom->Set_Animation(ANIM_BATTLE_GUARD_PUSH);
	}
	//if (m_isThrowing && m_pModelCom->Get_iCurrentAnimIndex() == ANIM_BATTLE_IDLE && m_Moveset.m_State_Battle_Guard)
	//{
	//	m_isMaintain_Guard = true;
	//}

}

void CPlayer_Rengoku::Animation_Control_Battle_Dash(_double dTimeDelta)
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
			m_pModelCom->Set_Animation(ANIM_JUMP); // 공중콤보용 대시
			Jumping(1.0f, 0.055f);
		}
		else
			m_pModelCom->Set_Animation(ANIM_DASH_RUN);
	}
	Go_Straight_Constant(dTimeDelta, 41, 3.0f * m_fScaleChange);

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
	Ground_Animation_Play(35, 36);


	//공중콤보영 대시

	if (m_pModelCom->Get_iCurrentAnimIndex() == 41)
	{
		if (Get_Distance_To_LockOnPos() < 1.0f)
		{
			m_pModelCom->Set_Animation(42);
		}
		
		m_dDelay_Dash += dTimeDelta;
		if (m_dDelay_Dash > 2.0f)
		{
			m_pModelCom->Set_Animation(42);
		}
	}
	Go_Straight_Deceleration(dTimeDelta, 42, 3.0f * m_fScaleChange, 0.03f);



	if (m_Moveset.m_Down_Battle_Step)
	{
		m_Moveset.m_Down_Battle_Step = false;

		m_pTransformCom->Set_Look(m_vLook);
		//m_pTransformCom->LerpVector(XMLoadFloat4(&m_vLook), 0.8f);
		if(m_isForward)
			m_pModelCom->Set_Animation(ANIM_STEP_F);
		else if(m_isBack)
			m_pModelCom->Set_Animation(ANIM_STEP_B);
		else
		{
			if (m_isLeft)
			{
				//콤보 첫 애니메이션 설정
				if (m_pModelCom->Get_Combo_Doing() == false)
				{
					m_pModelCom->Set_Combo_Doing(true);
					m_pModelCom->Set_Animation(ANIM_STEP_L_0);

					m_pModelCom->Set_EarlyEnd(ANIM_STEP_L_0, true);
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
					m_pModelCom->Set_Animation(ANIM_STEP_R_0);
					
					m_pModelCom->Set_EarlyEnd(ANIM_STEP_R_0, true);
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
	Go_Dir_Deceleration(dTimeDelta, ANIM_STEP_F, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_STEP_B, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_STEP_L_0, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_STEP_R_0, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);

	//더블스텝
	Go_Dir_Deceleration(dTimeDelta, ANIM_STEP_L_1, 5.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_STEP_R_1, 5.5f * m_fScaleChange * m_fMoving_Ratio, 0.15f * m_fScaleChange, m_Moveset.m_Input_Dir);
}

void CPlayer_Rengoku::Animation_Control_Battle_Awaken(_double dTimeDelta)
{
	if (m_Moveset.m_Down_Battle_Awaken)
	{
		m_Moveset.m_Down_Battle_Awaken = false;

		if (m_Moveset.m_iAwaken == 1)
		{
			m_pModelCom->Set_Animation(ANIM_AWAKE);
		}
		else if (m_Moveset.m_iAwaken == 2)
		{
			m_pModelCom->Set_Animation(ANIM_AWAKE_COMPLETE);
		}
	}
}

void CPlayer_Rengoku::Animation_Control_Battle_Special(_double dTimeDelta)
{
	if (m_Moveset.m_Down_Battle_Special)
	{
		m_Moveset.m_Down_Battle_Special = false;

		m_isFirst_SpecialReady = true;

		m_pModelCom->Set_Animation(ANIM_SPECIAL_READY);
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

			m_pModelCom->Set_Animation(ANIM_SPECIAL_CUTSCENE);
			m_dTime_Special_CutScene = 0.0;
		}
	}
	m_dTime_Special_CutScene += dTimeDelta;

	Go_Straight_Constant(dTimeDelta, 113, 1.5f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 114, 1.5f * m_fScaleChange, 0.015f);

}

void CPlayer_Rengoku::Animation_Control_Battle_Dmg(_double dTimeDelta)
{
	_float4 AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
	AtkDir.y = 0.0f;
	_vector vAtkDir = XMVector4Normalize(XMLoadFloat4(&AtkDir));
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
			m_pModelCom->Set_Animation(ANIM_GUARD_HIT_SMALL);
		}
		else if (m_iGuardHit_Index == 1)
		{
			m_iGuardHit_Index = 0;
			m_pModelCom->Set_Animation(ANIM_GUARD_HIT_BIG);
		}
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
					m_pModelCom->Set_Animation(ANIM_DMG_SMALL_AIR_B);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 1)
				{
					m_pModelCom->Set_Animation(ANIM_DMG_SMALL_AIR_L);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 2)
				{
					m_pModelCom->Set_Animation(ANIM_DMG_SMALL_AIR_F);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 3)
				{
					m_pModelCom->Set_Animation(ANIM_DMG_SMALL_AIR_R);
					m_iSmallHit_Index = 0;
				}
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
					m_pModelCom->Set_Animation(ANIM_DMG_SMALL_U);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 3)
				{
					m_pModelCom->Set_Animation(ANIM_DMG_SMALL_R);
					m_iSmallHit_Index = 0;
				}
			}
		}
	}
	if (m_isConnectHitting == false)
	{
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_B, 1.5f * m_fDmg_Move_Ratio, 0.01f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_F, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_L, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_R, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL_U, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
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
	}
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_BIG, 2.0f , 0.035f,  AtkDir);
	
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
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_BLOW, 2.5f * m_fDmg_Move_Ratio, AtkDir);
	Go_Dir_Constant(dTimeDelta, 49, 2.5f * m_fDmg_Move_Ratio, AtkDir);
	Ground_Animation_Play(49, 50);

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
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_SPIN, 3.0f * m_fDmg_Move_Ratio, AtkDir);
	Go_Dir_Constant(dTimeDelta, 78, 3.0f * m_fDmg_Move_Ratio, AtkDir);
	Go_Dir_Deceleration(dTimeDelta, 79, 3.0f * m_fDmg_Move_Ratio, 0.1f, AtkDir);
	Ground_Animation_Play(78, 79);

#pragma endregion


#pragma region Dmg_Upper
	if (m_Moveset.m_Down_Dmg_Upper)
	{
		m_Moveset.m_Down_Dmg_Upper = false;

		m_isConnectHitting = false;

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isSkilling == false)
		{
			m_pModelCom->Set_Animation(ANIM_DMG_FALL);
			//m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
			m_pTransformCom->Set_Look(reverseAtkDir);

			Jumping(1.7f, 0.03f);
		}
	}

	if (m_isConnectHitting == false)
	{
		Go_Dir_Constant(dTimeDelta, ANIM_DMG_FALL, 0.2f * m_fDmg_Move_Ratio, AtkDir);
		Go_Dir_Constant(dTimeDelta, 71, 0.2f * m_fDmg_Move_Ratio, AtkDir);
	}
	Ground_Animation_Play(71, 72);
#pragma endregion



	if (m_Moveset.m_Down_GetUp)
	{
		m_Moveset.m_Down_GetUp = false;
		m_Moveset.m_Down_Dmg_Blow = false;

		m_pModelCom->Set_Animation(ANIM_GETUP);
	}

	
	if (m_Moveset.m_Down_GetUp_Move)
	{
		m_Moveset.m_Down_GetUp_Move = false;
		m_Moveset.m_Down_Dmg_Blow = false;

		m_pModelCom->Set_Animation(ANIM_GETUP_MOVE);
		//m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
	}
	//Go_Straight_Constant(dTimeDelta, 138, 2.0f);
	Go_Straight_Deceleration(dTimeDelta, 60, 3.0f * m_fDmg_Move_Ratio, 0.03f);
	m_pModelCom->Set_EarlyEnd(ANIM_GETUP_MOVE, true);


	//Go_Straight_Deceleration(dTimeDelta, 138, 1.0f, 0.01f);
	
}

void CPlayer_Rengoku::Moving_Restrict()
{
	_int iCurAnimIndex = m_pModelCom->Get_iCurrentAnimIndex();


	//히트시 제한 + 기상, 다운
	if (ANIM_DMG_BLOW == iCurAnimIndex || 49 == iCurAnimIndex || 50 == iCurAnimIndex
		|| ANIM_DMG_SPIN == iCurAnimIndex || 78 == iCurAnimIndex || 79 == iCurAnimIndex
		|| ANIM_DMG_FALL == iCurAnimIndex || 71 == iCurAnimIndex || 72 == iCurAnimIndex
		|| ANIM_DMG_SMALL_B == iCurAnimIndex || ANIM_DMG_SMALL_F == iCurAnimIndex || ANIM_DMG_SMALL_L == iCurAnimIndex 
		|| ANIM_DMG_SMALL_R == iCurAnimIndex || ANIM_DMG_SMALL_U == iCurAnimIndex
		|| ANIM_DMG_SMALL_RETURN == iCurAnimIndex || ANIM_DMG_BIG_RETURN == iCurAnimIndex || ANIM_DMG_BIG == iCurAnimIndex
		|| ANIM_DOWN_IDLE == iCurAnimIndex || ANIM_GETUP_MOVE == iCurAnimIndex || 60 == iCurAnimIndex
		|| ANIM_GETUP == iCurAnimIndex || 57 == iCurAnimIndex
		|| ANIM_DMG_SMALL_AIR_B == iCurAnimIndex || ANIM_DMG_SMALL_AIR_F == iCurAnimIndex || ANIM_DMG_SMALL_AIR_L == iCurAnimIndex
		|| ANIM_DMG_SMALL_AIR_R == iCurAnimIndex || ANIM_DMG_SMALL_AIR_U == iCurAnimIndex )
	{
		m_Moveset.m_isHitMotion = true;

		m_Moveset.m_State_Battle_Guard = false;
		m_isAirDashing = false;
		
		//다운상태
		if (ANIM_DOWN_IDLE == iCurAnimIndex)
		{
			m_Moveset.m_isDownMotion = true;
			m_Moveset.m_Down_Dmg_Blow = false;

			m_Moveset.m_isGetUpMotion = false;
		}

		//겟업 상태
		if (ANIM_GETUP == iCurAnimIndex || 57 == iCurAnimIndex || ANIM_DMG_BIG_RETURN == iCurAnimIndex
			|| ANIM_GETUP_MOVE == iCurAnimIndex || 60 == iCurAnimIndex || ANIM_DMG_SMALL_RETURN == iCurAnimIndex)
		{
			m_Moveset.m_isGetUpMotion = true;
		}
	}
	//콤보공격시 무빙제한
	else if (ANIM_ATK_COMBO == iCurAnimIndex
		|| 1 == iCurAnimIndex || 2 == iCurAnimIndex
		|| ANIM_ATK_COMBO_DOWN == iCurAnimIndex || ANIM_ATK_COMBO_NORMAL == iCurAnimIndex || ANIM_ATK_COMBO_UP == iCurAnimIndex
		|| ANIM_ATK_COMBO_SURGE == iCurAnimIndex || ANIM_ATK_COMBO_SURGE_CUTSCENE == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;

		if (ANIM_ATK_COMBO_SURGE == iCurAnimIndex || ANIM_ATK_COMBO_SURGE_CUTSCENE == iCurAnimIndex)
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
	else if (ANIM_ATK_AIR_COMBO == iCurAnimIndex || 9 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Jump = true;
		m_Moveset.m_isRestrict_JumpCombo = true;
	}
	//차지공격 시 무빙제한
	else if (ANIM_ATK_CHARGE == iCurAnimIndex || 11 == iCurAnimIndex || 12 == iCurAnimIndex )
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Charge = true;
	}
	//스킬공격 시 무빙제한
	else if (ANIM_ATK_SKILL_GUARD == iCurAnimIndex || 14 == iCurAnimIndex || 15 == iCurAnimIndex 
		|| ANIM_ATK_SKILL_MOVE == iCurAnimIndex || 17 == iCurAnimIndex || ANIM_ATK_SKILL_MOVE_AIR == iCurAnimIndex || 19 == iCurAnimIndex
		|| ANIM_ATK_SKILL_NORMAL == iCurAnimIndex || ANIM_ATK_SKILL_NORMAL_AIR == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;

		m_isSkilling = true;
	} 
	//잡기 공격 시 제한
	/*else if (ANIM_ATK_THROW == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Throw = true;
	}*/
	//점프 트랙 (이동키 + J키)공격 시 제한
	else if (ANIM_ATK_AIRTRACK == iCurAnimIndex || 25 == iCurAnimIndex || 26 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Jump = true;
	}
	//Awaken
	else if (ANIM_AWAKE == iCurAnimIndex || ANIM_AWAKE_COMPLETE == iCurAnimIndex )
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Jump = true;
		m_Moveset.m_isRestrict_Charge = true;
		m_Moveset.m_isRestrict_Step = true;
		m_Moveset.m_isRestrict_Dash = true;
	}
	//Special
	else if (ANIM_SPECIAL_CUTSCENE == iCurAnimIndex || 111 == iCurAnimIndex || 112 == iCurAnimIndex || 113 == iCurAnimIndex || 114 == iCurAnimIndex 
		|| 115 == iCurAnimIndex || 116 == iCurAnimIndex || 117 == iCurAnimIndex || 118 == iCurAnimIndex || 119 == iCurAnimIndex 
		|| ANIM_SPECIAL_READY == iCurAnimIndex || 108 == iCurAnimIndex || 109 == iCurAnimIndex || ANIM_SPECIAL_END == iCurAnimIndex)
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
	else if (ANIM_GUARD == iCurAnimIndex || 46 == iCurAnimIndex || 47 == iCurAnimIndex
		|| ANIM_GUARD_HIT_BIG == iCurAnimIndex || ANIM_GUARD_HIT_SMALL == iCurAnimIndex || ANIM_GUARD_PUSH == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isHitMotion = false;
	}
	//대시 시 제한
	else if (ANIM_DASH_RUN == iCurAnimIndex || 41 == iCurAnimIndex /* || 42 == iCurAnimIndex */ )
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Dash = true;
	}
	//점프 시 무빙제한
	else if (ANIM_JUMP == iCurAnimIndex
		|| 34 == iCurAnimIndex || 35 == iCurAnimIndex )
	{
		if (m_ePlayerState == PLAYER_BATTLE)
		{
			m_Moveset.m_isRestrict_Move = true;
			m_Moveset.m_isRestrict_Jump = true;
		}
	}
	//스텝 시 제한
	else if (ANIM_STEP_B == iCurAnimIndex || ANIM_STEP_F == iCurAnimIndex
		|| ANIM_STEP_L_0 == iCurAnimIndex || ANIM_STEP_R_0 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Step = true;
	}
	//더블스텝 시 제한
	else if (ANIM_STEP_L_1 == iCurAnimIndex || ANIM_STEP_R_1 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		
		m_Moveset.m_isRestrict_DoubleStep = true;
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
	}
}

HRESULT CPlayer_Rengoku::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rengoku"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CPlayer_Rengoku");
		return E_FAIL;
	}


	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CPlayer_Rengoku");
		return E_FAIL;
	}

	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CPlayer_Rengoku");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CPlayer_Rengoku");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CPlayer_Rengoku");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.f, 1.f, 1.f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CPlayer_Rengoku");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Rengoku::SetUp_ShaderResources()
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
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_bSuperArmor", &m_isSkilling, sizeof(_bool))))
		return E_FAIL;



	Safe_Release(pGameInstance);

	return S_OK;
}

CPlayer_Rengoku* CPlayer_Rengoku::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Rengoku* pInstance = new CPlayer_Rengoku(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Rengoku");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Rengoku::Clone(void* pArg)
{
	CPlayer_Rengoku* pInstance = new CPlayer_Rengoku(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Rengoku");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Rengoku::Free()
{
	Safe_Release(m_pSword);
	Safe_Release(m_pSwordHome);

	__super::Free();
}
