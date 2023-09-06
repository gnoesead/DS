#include "pch.h"
#include "..\Public\Player_Zenitsu.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"
#include "PlayerManager.h"

#include "Camera_Manager.h"
#include "Camera_Free.h"
#include "Battle_UI_Manager.h"


CPlayer_Zenitsu::CPlayer_Zenitsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPlayer(pDevice, pContext)
{
}

CPlayer_Zenitsu::CPlayer_Zenitsu(const CPlayer_Zenitsu& rhs)
	: CPlayer(rhs)
{
}

HRESULT CPlayer_Zenitsu::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Zenitsu::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModelCom->Set_Animation(ANIM_BATTLE_IDLE);

	if (FAILED(Read_Animation_Control_File("Zenitsu.bin")))
	{
		MSG_BOX("Failed to AnimData Read : Zenitsu");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSword::SWORDDESC SwordDesc;
	ZeroMemory(&SwordDesc, sizeof SwordDesc);
	SwordDesc.m_PlayerName = CSword::PLAYER_ZENITSU;
	SwordDesc.pParentTransform = m_pTransformCom;
	SwordDesc.pBone = m_pModelCom->Get_Bone("R_HandCommon_1_Lct");
	m_pSword = dynamic_cast<CSword*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Sword"), &SwordDesc));

	CSwordHome::SWORDDESC SwordHomeDesc;
	ZeroMemory(&SwordHomeDesc, sizeof SwordHomeDesc);
	SwordHomeDesc.m_PlayerName = CSwordHome::PLAYER_ZENITSU;
	SwordHomeDesc.pParentTransform = m_pTransformCom;
	SwordHomeDesc.pBone = m_pModelCom->Get_Bone("L_Weapon_1");
	m_pSwordHome = dynamic_cast<CSwordHome*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_SwordHome"), &SwordHomeDesc));

	Safe_Release(pGameInstance);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 136.f,0.f,136.f,1.f });

	return S_OK;
}

void CPlayer_Zenitsu::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (m_isSwap_OnSky == false)
	{
		m_pSword->Tick(dTimeDelta);
		m_pSwordHome->Tick(dTimeDelta);
	}

	if (true == m_isDead)
		return;
#ifdef _DEBUG
	// 슬라이딩 벡터 타는 순간 조건문
	//if (m_pNavigationCom[m_eCurNavi]->is_MoveOnNavigation(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) == false)
	//{
	//	//여기에 원하는 조건 넣어주면 됨	
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 136.f,0.f,136.f,1.f });
	//}
#endif // _DEBUG

	
	//playerswap
	if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 1) // 젠이츠
	{
		Player_Change_Setting_Status(dTimeDelta);

		//if (CPlayerManager::GetInstance()->Get_Slow() == false)
		//{
			if (m_isSwapping_State == false)
				Animation_Control(dTimeDelta);
		//}
	}
	else
	{
		Player_Change(dTimeDelta);
	}


	if (m_isSwap_OnSky == false)
	{
		//if (CPlayerManager::GetInstance()->Get_Slow() == false)
		//{
			//애니메이션 처리
			m_pModelCom->Play_Animation(dTimeDelta);
		//}
		RootAnimation(dTimeDelta);

		//이벤트 콜
		EventCall_Control(dTimeDelta);

		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			return;
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this)))
			return;
	}

}

void CPlayer_Zenitsu::LateTick(_double dTimeDelta)
{
	if (m_isSwap_OnSky == false)
	{
		m_pSword->LateTick(dTimeDelta);
		m_pSwordHome->LateTick(dTimeDelta);
	}

	//playerswap
	if (CPlayerManager::GetInstance()->Get_PlayerIndex() == 1) // 젠이츠
	{
		__super::LateTick(dTimeDelta);


		if (m_isAir_Hekireki && m_pModelCom->Get_iCurrentAnimIndex() == ANIM_JUMP_IDLE)
		{
			m_isAir_Hekireki = false;
			m_isJumpOn = true;
		}

		//if (CPlayerManager::GetInstance()->Get_Slow() == false)
		//{
			if (m_isAir_Hekireki == false && m_isAirDashing == false)
				Gravity(dTimeDelta);
		//}

		//추가
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
	


	//임시 코드
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if (pGameInstance->Get_DIKeyDown(DIK_DELETE))
	{
		if (m_bMotionBlur == true)
			m_bMotionBlur = false;
		else if (m_bMotionBlur == false)
			m_bMotionBlur = true;
	}
	Safe_Release(pGameInstance);


#ifdef _DEBUG
	/*if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
		return;*/


#endif
}

HRESULT CPlayer_Zenitsu::Render()
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

#ifdef _DEBUG
		/*CNavigation* pNavi = m_pNavigationCom[m_eCurNavi];
		pNavi->Render();*/
#endif

	}
	return S_OK;
}

HRESULT CPlayer_Zenitsu::Render_ShadowDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();


	_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector	vLightEye = XMVectorSet(-5.f, 10.f, -5.f, 1.f);
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


void CPlayer_Zenitsu::EventCall_Control(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
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
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				//EffectWorldDesc.fScale = 1.8f;
				EffectWorldDesc.vPosition.y = 0.42f;

				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_0", m_pTransformCom , &EffectWorldDesc);
				else
					CEffectPlayer::Get_Instance()->Play("Zen_0", m_pTransformCom , &EffectWorldDesc);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 1.0f, 0.f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
			}
		}
		if (4 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{

				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_1", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Zen_Power_1", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.0f);
			}
		}
		if (5 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_2_1", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Zen_Power_2_1", m_pTransformCom);


				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (1 == m_iEvent_Index)
			{
				
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_2_2", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Zen_Power_2_2", m_pTransformCom);


				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.f), 0.1,
					CAtkCollider::TYPE_CONNECTSMALL, vPlayerDir, 1.0f);
			}
			else if (2 == m_iEvent_Index)
			{
				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_2_3", m_pTransformCom);
				else
					CEffectPlayer::Get_Instance()->Play("Zen_Power_2_3", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
			}
		}
		if (7 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.5f;

				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_3_Slash", m_pTransformCom , &EffectWorldDesc);
				else
					CEffectPlayer::Get_Instance()->Play("Zen_Power_3_Slash", m_pTransformCom , &EffectWorldDesc);


				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc2;
				EffectWorldDesc2.fScale = 1.8f;
				EffectWorldDesc2.vPosition.x = 1.3f;
				EffectWorldDesc2.vPosition.y = 0.33f;

				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_3_Wind", m_pTransformCom, &EffectWorldDesc2);
				else
					CEffectPlayer::Get_Instance()->Play("Zen_3_Wind", m_pTransformCom, &EffectWorldDesc2);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.2f), 0.1,
					CAtkCollider::TYPE_BIG, vPlayerDir, 5.0f);
			}
			else if (1 == m_iEvent_Index) {

				

			}



		}

		if (ANIM_ATK_COMBO_DOWN == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 1.5f), 0.5,
					CAtkCollider::TYPE_UPPER, vPlayerDir, 2.0f);
			}
			if (1 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.f), 0.5,
					CAtkCollider::TYPE_UPPER, vPlayerDir, 2.0f);
			}
			if (2 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.0f, 3.0f, 3.0f), _float3(0.f, 1.0f, 0.f), 0.5,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.0f);
			}
			if (3 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 0.f), 0.5,
					CAtkCollider::TYPE_BLOW, vPlayerDir, 4.0f);
			}
		}

		if (ANIM_ATK_COMBO_UP == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_UPPER, vPlayerDir, 6.0f);
			}
		}

		
		if (ANIM_ATK_SPECIAL_CUTSCENE == m_pModelCom->Get_iCurrentAnimIndex())
		{

		}
#pragma endregion


#pragma region Air_Attack, charge_attack
		if (ANIM_ATK_AIRTRACK == m_pModelCom->Get_iCurrentAnimIndex())
		{
			
		}
		if (2 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.5,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.0f);
			}
			if (1 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.0f, 2.0f, 2.0f), _float3(0.f, 1.0f, 1.5f), 0.5,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 2.0f);
			}
		}


		if (ANIM_ATK_AIRCOMBO == m_pModelCom->Get_iCurrentAnimIndex())
		{
			// Small_1
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.y += -0.3f;
				EffectWorldDesc.vPosition.z += -0.3f;

				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_Air_0", m_pTransformCom , &EffectWorldDesc);
				else
					CEffectPlayer::Get_Instance()->Play("Zen_Power_Air_0", m_pTransformCom , &EffectWorldDesc);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.5f, 3.5f, 3.5f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 3.0f);
			}

			

		}
		if (18 == m_pModelCom->Get_iCurrentAnimIndex())
		{

			// Small_2
			if (0 == m_iEvent_Index)
			{
				
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.z += -0.3f;

				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_Air_1", m_pTransformCom, &EffectWorldDesc);
				else
					CEffectPlayer::Get_Instance()->Play("Zen_Power_Air_1", m_pTransformCom, &EffectWorldDesc);

			}
			// Big
			if (1 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.2f;

				if (m_Moveset.m_iAwaken == 0)
					CEffectPlayer::Get_Instance()->Play("Zen_Air_2", m_pTransformCom, &EffectWorldDesc);
				else
					CEffectPlayer::Get_Instance()->Play("Zen_Power_Air_2", m_pTransformCom, &EffectWorldDesc);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.5f, 3.5f, 3.5f), _float3(0.f, 1.0f, 2.0f), 0.1,
					CAtkCollider::TYPE_BOUND, vPlayerDir, 4.0f);
			}
		}


		if (ANIM_ATK_CHARGE == m_pModelCom->Get_iCurrentAnimIndex())
		{
			
		}
		if (21 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(3.5f, 3.5f, 3.5f), _float3(0.f, 1.0f, 2.0f), 0.5,
					CAtkCollider::TYPE_BLOW, vPlayerDir, 10.0f);
			}
		}
#pragma endregion


#pragma region Dash_Tackle
		if (ANIM_BATTLE_DASH == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.5f, 1.7f), 5.0,
					CAtkCollider::TYPE_SMALL, vPlayerDir, 1.0f);
			}
		}

#pragma endregion


#pragma region SuperSkill
		if (ANIM_ATK_SKILL_HEKIREKI == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CCameraManager::GetInstance()->Set_Is_Dist_Update(false, 1.2f);

				CBattle_UI_Manager::GetInstance()->Set_Player_Type(1);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(0);

				CEffectPlayer::Get_Instance()->Play("Zen_Heki_Light", m_pTransformCom);

				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.8f, 1.8f, 1.8f), _float3(0.f, 0.5f, 0.0f), 1.0,
					CAtkCollider::TYPE_HEKIREKI, vPlayerDir, 8.6f);
			}
			else if (1 == m_iEvent_Index) {

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.2f;
				EffectWorldDesc.vPosition.y += 0.3f;

				CEffectPlayer::Get_Instance()->Play("Zen_Heki_Head", m_pTransformCom , &EffectWorldDesc);

			}
			else if (2 == m_iEvent_Index) {

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.5f;
				EffectWorldDesc.vPosition.y += 0.4f;
				EffectWorldDesc.vPosition.z += -0.3f;


				CEffectPlayer::Get_Instance()->Play("Zen_Heki_Elc", m_pTransformCom, &EffectWorldDesc);

			}
		}
		if (ANIM_ATK_SKILL_HEKIREKI_END == m_pModelCom->Get_iCurrentAnimIndex())
		{
			
		}


		if (ANIM_ATK_SKILL_HEKIREKI_AIR == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CCameraManager::GetInstance()->Set_Is_Dist_Update(false, 1.2f);

				CBattle_UI_Manager::GetInstance()->Set_Player_Type(1);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(0);

				CEffectPlayer::Get_Instance()->Play("Zen_Heki_Light", m_pTransformCom);

				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(1.8f, 1.8f, 1.8f), _float3(0.f, 0.5f, 0.0f), 1.0,
					CAtkCollider::TYPE_HEKIREKI, vPlayerDir, 8.6f);
			}
			else if (1 == m_iEvent_Index) {

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.2f;
				EffectWorldDesc.vPosition.y += 0.3f;


				CEffectPlayer::Get_Instance()->Play("Zen_Heki_Head", m_pTransformCom, &EffectWorldDesc);

			}
			else if (2 == m_iEvent_Index) {

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.5f;
				EffectWorldDesc.vPosition.y += 0.4f;
				EffectWorldDesc.vPosition.z += -0.3f;

				CEffectPlayer::Get_Instance()->Play("Zen_Heki_Elc", m_pTransformCom, &EffectWorldDesc);

			}
		}
		if (ANIM_ATK_SKILL_HEKIREKI_AIR_END == m_pModelCom->Get_iCurrentAnimIndex())
		{

		}


		if (ANIM_ATK_SKILL_GUARD == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CBattle_UI_Manager::GetInstance()->Set_Player_Type(1);
				CBattle_UI_Manager::GetInstance()->Set_Player_Skill_Type(1);

				Make_AttackColl(TEXT("Layer_PlayerAtk"), _float3(2.5f, 2.5f, 2.5f), _float3(0.f, 0.5f, 1.7f), 0.2,
					CAtkCollider::TYPE_UPPER, vPlayerDir, 7.0f);
			}
		}
#pragma endregion


		m_iEvent_Index++;
	}
}

void CPlayer_Zenitsu::Animation_Control(_double dTimeDelta)
{
	//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta);

	Moving_Restrict();

	Animation_Control_Battle_Dmg(dTimeDelta);

	if (m_Moveset.m_isHitMotion == false)
	{

		Animation_Control_Battle_Jump(dTimeDelta);

		Animation_Control_Battle_Move(dTimeDelta);

		if (m_isSwapping_State == false)
		{
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

void CPlayer_Zenitsu::Animation_Control_Battle_Move(_double dTimeDelta)
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
		//m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
		m_fMove_Speed = 2.0f;
		
		if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_BATTLE_RUN )
		{
			if (m_isCanNavi)
				m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange, m_pNavigationCom[m_eCurNavi]);
			else
				m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange);
		}
	}

	if (m_Moveset.m_Up_Battle_Run)
	{
		m_Moveset.m_Up_Battle_Run = false;
		m_pModelCom->Set_Animation(ANIM_BATTLE_RUN_END);
	}
	Go_Straight_Deceleration(dTimeDelta, ANIM_BATTLE_RUN_END, m_fMove_Speed * m_fScaleChange, 0.15f);
}

void CPlayer_Zenitsu::Animation_Control_Battle_Jump(_double dTimeDelta)
{
	if (m_Moveset.m_Down_Battle_JumpMove)
	{
		m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		//m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
		m_Moveset.m_Down_Battle_JumpMove = false;
		m_isJump_Move = true;

		m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP);
		Jumping(4.0f, 0.2f);

		m_pModelCom->Set_EarlyEnd(58, true);
	}
	if (m_isJump_Move)
	{
		Go_Straight_Constant(dTimeDelta, ANIM_BATTLE_JUMP, m_fMove_Speed * 1.2f * m_fScaleChange);
		Go_Straight_Constant(dTimeDelta, 57, m_fMove_Speed * 1.2f * m_fScaleChange);
		Go_Straight_Constant(dTimeDelta, 58, m_fMove_Speed * 1.2f * m_fScaleChange);
		//Go_Straight_Deceleration(dTimeDelta, 59, m_fMove_Speed * 1.2f * m_fScaleChange, 0.36f * m_fScaleChange); // Down
	}
	Ground_Animation_Play(58, 59);
	m_pModelCom->Set_LinearDuration(ANIM_BATTLE_JUMP, 0.001f);
	m_pModelCom->Set_LinearDuration(57, 0.001f);
	m_pModelCom->Set_LinearDuration(58, 0.001f);
	m_pModelCom->Set_LinearDuration(59, 0.001f);


	if (m_Moveset.m_Down_Battle_Jump)
	{
		m_Moveset.m_Down_Battle_Jump = false;
		m_isJump_Move = false;

		m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP);
		Jumping(4.0f * m_fScaleChange, 0.2f * m_fScaleChange);

		m_pModelCom->Set_EarlyEnd(58, true);
	}



	//공중 공격 콤보
	if (m_Moveset.m_Down_Battle_Jump_Attack)
	{
		m_Moveset.m_Down_Battle_Jump_Attack = false;
		m_isJump_Move = false;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		//콤보 첫 애니메이션 설정
		if (m_pModelCom->Get_Combo_Doing() == false)
		{
			m_pModelCom->Set_EarlyEnd(ANIM_ATK_AIRCOMBO, true, 0.5f);
			m_pModelCom->Set_Combo_Doing(true);
			m_pModelCom->Set_Animation(ANIM_ATK_AIRCOMBO);
			

			JumpStop(0.40);
		}
		//아닐경우, 다음 콤보로 진행
		else
		{
			m_pModelCom->Set_Combo_Trigger(true);

			m_pModelCom->Set_EarlyEnd(18, true, 0.8f);
			JumpStop(0.65);
			m_isFirst_JumpAtk = false;
		}
	}
	m_pModelCom->Set_EarlyEnd(ANIM_ATK_AIRCOMBO, false, 0.5f);


	//공중 트랙공격 (이동키 + 공격키)
	if (m_Moveset.m_Down_Battle_Jump_TrackAttack)
	{
		m_Moveset.m_Down_Battle_Jump_TrackAttack = false;

		if (m_isComboing_Down == false)
		{
			m_isFirst_JumpAtk = false;

			if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
			{
				if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
					m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
			}
			m_pModelCom->Set_Animation(ANIM_ATK_AIRTRACK);
			JumpStop(0.9);
			Set_FallingStatus(2.8f, 0.01f);
		}
	}
	m_pModelCom->Set_EarlyEnd(2, true, 0.55f);
	Ground_Animation_Play(1, 2);
	Go_Straight_Constant(dTimeDelta, 1, 3.f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 2, 3.f * m_fScaleChange, 0.2f * m_fScaleChange); // Down
}

void CPlayer_Zenitsu::Animation_Control_Battle_Attack(_double dTimeDelta)
{
	_int iCurAnimIndex = m_pModelCom->Get_iCurrentAnimIndex();

	_float fDistance = Get_Distance_To_LockOnPos();

	if (m_isComboing_Upper == false)
	{
		// 콤보공격
		if (m_Moveset.m_Down_Battle_Combo)
		{
			m_Moveset.m_Down_Battle_Combo = false;
			m_isComboing = true;

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
				if(3 == iCurAnimIndex)
					m_pModelCom->Set_EarlyEnd(ANIM_ATK_COMBO, true, 0.3f);
				else if( 4 == iCurAnimIndex)
					m_pModelCom->Set_EarlyEnd(4, true, 0.3f);
				else if(5 == iCurAnimIndex)
					m_pModelCom->Set_EarlyEnd(5, true, 0.4f);

				m_pModelCom->Set_Combo_Trigger(true);
				//콤보 분기 설정
				if (5 == iCurAnimIndex)
				{
					m_isFirst_Combo_Down = true;
					m_dTime_Comboing_Down = 0.0;

					m_isFirst_Combo_Upper = true;
					m_dTime_Comboing_Upper = 0.0;

					// 아래콤보 s콤보
					if (m_Moveset.m_Down_Battle_Combo_Down)
					{
						m_pModelCom->Set_Combo_Another(6);
					}
					// 위콤보 w콤보
					else if (m_Moveset.m_Down_Battle_Combo_Up)
					{
						m_pModelCom->Set_Combo_Another(8);

						m_isCan_AirDash = true;
					}
				}
			}
		}
	}
	m_pModelCom->Set_EarlyEnd(ANIM_ATK_COMBO, false, 0.3f);
	m_pModelCom->Set_EarlyEnd(4, false, 0.3f);
	m_pModelCom->Set_EarlyEnd(5, false, 0.4f);

	if (fDistance > 0.7f)
	{
		// 공격 모션별 전진이동 제어 (Timedelta, 애니메이션인덱스,  초기화속도,  감속도)
		Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO, 5.0f * m_fScaleChange * m_fAtk_Move_Ratio, 0.15f * m_fScaleChange);
		Go_Straight_Deceleration(dTimeDelta, 4, 2.7f * m_fScaleChange * m_fAtk_Move_Ratio, 0.10f * m_fScaleChange);
		Go_Straight_Deceleration(dTimeDelta, 5, 2.4f * m_fScaleChange * m_fAtk_Move_Ratio, 0.10f * m_fScaleChange);

		//분기
		Go_Straight_Deceleration(dTimeDelta, 7, 2.7f * m_fScaleChange * m_fAtk_Move_Ratio, 0.07f * m_fScaleChange); // Normal
	}
	Ground_Animation_Play(9, 10);

	//Down
	if (ANIM_ATK_COMBO_DOWN == iCurAnimIndex)
	{
		if (m_isFirst_Combo_Down)
		{
			m_isFirst_Combo_Down = false;
			Jumping(5.5f, 0.18f);
		}

		m_dTime_Comboing_Down += dTimeDelta;

		if (0.70f < m_dTime_Comboing_Down)
			Set_FallingStatus(6.6f, 0.0f);
		else if (0.3f < m_dTime_Comboing_Down)
			JumpStop(0.45f);
		else if (0.2f < m_dTime_Comboing_Down && m_dTime_Comboing_Down < 2.05f)
			Go_Straight_Constant(dTimeDelta, ANIM_ATK_COMBO_DOWN, 12.0f * m_fScaleChange);
		else if (0.1f < m_dTime_Comboing_Down && m_dTime_Comboing_Down < 1.05f)
			Go_Backward_Constant(dTimeDelta, ANIM_ATK_COMBO_DOWN, 12.0f * m_fScaleChange);
		else if (0.0f < m_dTime_Comboing_Down && m_dTime_Comboing_Down < 0.05f)
			Go_Straight_Constant(dTimeDelta, ANIM_ATK_COMBO_DOWN, 12.0f * m_fScaleChange);
	}

	//Up
	if (ANIM_ATK_COMBO_UP == iCurAnimIndex)
	{
		m_dTime_Comboing_Upper += dTimeDelta;

		if (m_isFirst_Combo_Upper && 0.5f < m_dTime_Comboing_Upper)
		{
			m_isFirst_Combo_Upper = false;
			Jumping(4.0f * m_fScaleChange, 0.3f * m_fScaleChange);

			if (0.5f < m_dTime_Comboing_Upper && m_dTime_Comboing_Upper < 0.505f)
				Go_Straight_Constant(dTimeDelta, ANIM_ATK_COMBO_UP, 15.0f * m_fScaleChange);
		}

		if (0.77f < m_dTime_Comboing_Upper)
		{
			JumpStop(0.08f);
		}
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_BATTLE_IDLE)
	{
		m_isComboing = false;
	}
}

void CPlayer_Zenitsu::Animation_Control_Battle_Charge(_double dTimeDelta)
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


	if (m_Moveset.m_Up_Battle_Charge && m_pModelCom->Get_iCurrentAnimIndex() == 20)
	{
		m_Moveset.m_Up_Battle_Charge = false;
	
		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pModelCom->Set_Animation(21);
	}
	else if (m_Moveset.m_Up_Battle_Charge )
	{
		m_Moveset.m_Up_Battle_Charge = false;
		
		m_pModelCom->Set_Animation(ANIM_BATTLE_IDLE);
	}
	Go_Straight_Deceleration(dTimeDelta, 21, 4.5f * m_fScaleChange * m_fAtk_Move_Ratio, 0.15f * m_fScaleChange);
}

void CPlayer_Zenitsu::Animation_Control_Battle_Skill(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_int CurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	
	
	//벽력일섬 콤보용
	m_dDelay_Hekireki_AnotherCan += dTimeDelta;
	if (CurAnim == ANIM_ATK_SKILL_HEKIREKI_END || CurAnim == ANIM_ATK_SKILL_HEKIREKI_AIR_END)
	{
		if (1.5 < m_dDelay_Hekireki_AnotherCan )
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			if (pGameInstance->Get_DIKeyDown(DIK_I))
				m_Moveset.m_Down_Skill_Normal = true;
			Safe_Release(pGameInstance);
		}
	}
	//마나제한
	if (m_Moveset.m_iAwaken < 2)
	{
		if (m_isCan_Mp_Skill == false)
		{
			m_Moveset.m_Down_Skill_Normal = false;
			m_Moveset.m_Down_Skill_Move = false;
			m_Moveset.m_Down_Skill_Guard = false;
		}
	}

	
	if (m_isHit_Hekireki)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera"), 0));
	
		pCamera->Change_Hekireki_Dir();

		Safe_Release(pGameInstance);

		m_isHit_Hekireki = false;
		//m_isHekireki_Hitting = true;

		CPlayerManager::GetInstance()->Set_Slow(0.2, 4);
	}
	
	//벽력일섬 히트시 살짝 느려지는거 구현 위한것. 이게 켜질시 히트상태임.
	/*if (m_isHekireki_Hitting)
	{
		m_dDelay_Hekireki_Hit += dTimeDelta;
		m_isHekireki_After = true;

		//히트상태는 이 딜레이 시간동안 유지된다.
		if (m_dDelay_Hekireki_Hit > 0.15f)
		{
			m_dDelay_Hekireki_Hit = 0.0;
			m_isHekireki_Hitting = false;
		}
	}*/
	


	if (CurAnim == ANIM_ATK_SKILL_HEKIREKI_AIR || CurAnim == ANIM_ATK_SKILL_HEKIREKI_AIR_END)
	{
		_float4 PlayerPos;
		XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (PlayerPos.y > m_fLand_Y + 0.1f)
		{
			m_isJumpOn = true;
		}

		JumpStop(0.1f);
		Set_FallingStatus(0.0f, 0.0f);
		m_isFirst_Hekireki_AirEnd = true;
	}
	else
	{
		if (m_isFirst_Hekireki_AirEnd)
		{
			m_isFirst_Hekireki_AirEnd = false;
			Set_FallingStatus(0.0f, 0.07f);
		}
	}

	
	
	m_dDelay_Hekireki_AnotherCan += dTimeDelta;
	if (m_dDelay_Hekireki_AnotherCan < 0.8f)
	{
		m_Moveset.m_Down_Skill_Normal = false;
		m_Moveset.m_Down_Skill_Move = false;
	}

	if (CurAnim != ANIM_ATK_SKILL_HEKIREKI && CurAnim != ANIM_ATK_SKILL_HEKIREKI_AIR /* && CurAnim != ANIM_ATK_SKILL_HEKIREKI_END && CurAnim != ANIM_ATK_SKILL_HEKIREKI_AIR_END*/)
	{
		//스킬_0
		if (m_Moveset.m_Down_Skill_Normal || m_Moveset.m_Down_Skill_Move)
		{
			m_Moveset.m_Down_Skill_Normal = false;
			m_Moveset.m_Down_Skill_Move = false;

			m_dDelay_Hekireki_AnotherCan = 0.0;

			m_dTime_Hekireki = 0.0f;


			_float4 PlayerPos;
			XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			Get_LockOn_MonPos();
			if ((m_LockOnPos.y <= PlayerPos.y + 0.01f) && m_isJumpOn == false)
				m_isAir_Hekireki = false;
			else
				m_isAir_Hekireki = true;


			if (m_isAir_Hekireki)
			{
				m_pModelCom->Set_Animation(ANIM_ATK_SKILL_HEKIREKI_AIR);
				//m_pTransformCom->LookAt(m_vBattleTargetPos);

				if (pGameInstance->Get_CurLevelIdx() != LEVEL_TRAIN)
				{
					if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
					{
						m_LockOnPos.y -= 0.7f;
						m_pTransformCom->LookAt(XMLoadFloat4(&m_LockOnPos));
					}
				}
				m_isHekireki_End_ForDir = true;
			}
			else
			{
				m_pModelCom->Set_Animation(ANIM_ATK_SKILL_HEKIREKI);
				//m_pTransformCom->LookAt(m_vBattleTargetPos);

				if (pGameInstance->Get_CurLevelIdx() != LEVEL_TRAIN)
				{
					if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
						m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
				}
				m_isHekireki_End_ForDir = true;
			}
			
			//Use_Mp_Skill();
			if (m_isCan_Mp_Skill)
			{
				if (m_StatusDesc.iAwaken < 2)
					m_StatusDesc.fMp -= 10.0f;
				m_dDelay_Mp_Used = 0.0;
			}
		}
	}
	else
	{
		m_Moveset.m_Down_Skill_Normal = false;
		m_Moveset.m_Down_Skill_Move = false;
	}

	if (CurAnim == ANIM_JUMP_IDLE)
	{
		if (m_isHekireki_End_ForDir)
		{
			m_isHekireki_End_ForDir = false;

			_float4	Dir;
			XMStoreFloat4(&Dir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
			Dir.y = 0.0f;
			XMStoreFloat4(&Dir, XMVector4Normalize(XMLoadFloat4(&Dir)));
			m_pTransformCom->Set_Look(Dir);
		}
	}

	if (m_isHekireki)
	{
		if (m_isHekireki_Hitting == false)
			m_dTime_Hekireki += dTimeDelta;
		// 35 42
		if (0.35f < m_dTime_Hekireki && m_dTime_Hekireki < 0.45f)
		{
			if (m_isHekireki_Hitting)
				m_pTransformCom->Go_Straight(dTimeDelta * 0.3f * m_fScaleChange, m_pNavigationCom[m_eCurNavi]);
			else
			{
				if(m_isAir_Hekireki)
					m_pTransformCom->Go_Straight(dTimeDelta * 18.f * m_fScaleChange, m_pNavigationCom[m_eCurNavi]);
				else
					m_pTransformCom->Go_Straight(dTimeDelta * 18.f * m_fScaleChange, m_pNavigationCom[m_eCurNavi]);
			}
		}

		_float4 PlayerPos;
		XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (PlayerPos.y <= m_fLand_Y)
		{
			PlayerPos.y = m_fLand_Y;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&PlayerPos));
		}
	}
	else
	{
		m_dTime_Hekireki = 0.0f;
	}



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

		Use_Mp_Skill();
	}
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_SKILL_GUARD, 0.f * m_fScaleChange, 0.18f * m_fScaleChange);

	Safe_Release(pGameInstance);
}

void CPlayer_Zenitsu::Animation_Control_Battle_Guard(_double dTimeDelta)
{
	//탄지로 63 64 65 66hit 67hit 68push 
	//가드 시작
	if (m_Moveset.m_Down_Battle_Guard)
	{
		m_Moveset.m_Down_Battle_Guard = false;

		if (CCameraManager::GetInstance()->Get_Is_Battle_LockFree() == false)
		{
			if (Get_LockOn_MonPos() && m_iLevelCur != LEVEL_TRAIN)
				m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));
		}
		m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		//m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
		m_pModelCom->Set_Animation(ANIM_BATTLE_GUARD);
	}

	//가드 도중


	//가드 내리기
	if (m_Moveset.m_Up_Battle_Guard)
	{
		m_Moveset.m_Up_Battle_Guard = false;

		m_pModelCom->Set_Animation(52);
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


	//푸시 ( O키 가드키 + 이동키)
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

void CPlayer_Zenitsu::Animation_Control_Battle_Dash(_double dTimeDelta)
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
	}
	Go_Straight_Constant(dTimeDelta, 46, 3.0f * m_fScaleChange);


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
	Ground_Animation_Play(58, 59);


	if (m_pModelCom->Get_iCurrentAnimIndex() == 46)
	{
		if (Get_Distance_To_LockOnPos() < 1.0f)
		{
			m_pModelCom->Set_Animation(47);
		}

		m_dDelay_Dash += dTimeDelta;
		if (m_dDelay_Dash > 2.0f)
		{
			m_pModelCom->Set_Animation(47);
		}
	}
	Go_Straight_Deceleration(dTimeDelta, 47, 3.0f * m_fScaleChange, 0.03f);


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

					m_pModelCom->Set_EarlyEnd(ANIM_BATTLE_STEP_L, false, 0.4f);
				}
				//아닐경우, 다음 콤보로 진행
				else
				{
					m_pModelCom->Set_Combo_Trigger(true);

					m_pModelCom->Set_EarlyEnd(ANIM_BATTLE_STEP_L, true, 0.4f);
				}
			}
			else if (m_isRight)
			{
				//콤보 첫 애니메이션 설정
				if (m_pModelCom->Get_Combo_Doing() == false)
				{
					m_pModelCom->Set_Combo_Doing(true);
					m_pModelCom->Set_Animation(ANIM_BATTLE_STEP_R);

					m_pModelCom->Set_EarlyEnd(ANIM_BATTLE_STEP_R, false, 0.4f);
				}
				//아닐경우, 다음 콤보로 진행
				else
				{
					m_pModelCom->Set_Combo_Trigger(true);

					m_pModelCom->Set_EarlyEnd(ANIM_BATTLE_STEP_R, true, 0.4f);
				}
			}
		}
	}
	_vector vDir = XMLoadFloat4(&m_Moveset.m_Input_Dir);
	_float4 fDir;
	XMStoreFloat4(&fDir, -vDir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_F, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.18f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_B, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.18f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_L, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.18f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_R, 4.5f * m_fScaleChange * m_fMoving_Ratio, 0.18f * m_fScaleChange, m_Moveset.m_Input_Dir);

	//더블스텝
	Go_Dir_Deceleration(dTimeDelta, 71, 5.5f * m_fScaleChange * m_fMoving_Ratio, 0.2f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, 73, 5.5f * m_fScaleChange * m_fMoving_Ratio, 0.2f * m_fScaleChange, m_Moveset.m_Input_Dir);
}

void CPlayer_Zenitsu::Animation_Control_Battle_Awaken(_double dTimeDelta)
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

void CPlayer_Zenitsu::Animation_Control_Battle_Special(_double dTimeDelta)
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
		Go_Straight_Deceleration(dTimeDelta, 130, 4.0f * m_fScaleChange, 0.15f * m_fScaleChange);

	//Go_Straight_Constant(dTimeDelta, 108, 2.7f);
	if (m_pModelCom->Get_iCurrentAnimIndex() == 130 || m_pModelCom->Get_iCurrentAnimIndex() == 131)
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

	if (m_pModelCom->Get_iCurrentAnimIndex() == 121 && m_isFirst_Special_Jump)
	{
		m_isFirst_Special_Jump = false;
		Jumping(10.65f, 0.10f);
	}



	if (m_pModelCom->Get_iCurrentAnimIndex() == 124)
	{
		JumpStop(0.1f);
		Set_FallingStatus(10.0f, 0.0f);
	}
	if (m_pModelCom->Get_iCurrentAnimIndex() == 125)
	{
		Go_Straight_Deceleration(dTimeDelta, 125, 6.0f * m_fScaleChange, 0.10f * m_fScaleChange);
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == 126)
	{
		Go_Backward_Constant(dTimeDelta, 26, 5.3f * m_fScaleChange);
	}

}

void CPlayer_Zenitsu::Animation_Control_Battle_Dmg(_double dTimeDelta)
{
	_float4 AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
	_vector vAtkDir = XMLoadFloat4(&AtkDir);
	_float4 reverseAtkDir;
	XMStoreFloat4(&reverseAtkDir, -vAtkDir);

	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

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
					m_pModelCom->Set_Animation(ANIM_DMG_AIR_CONNECT_0);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 2)
				{
					m_pModelCom->Set_Animation(ANIM_FALL);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 3)
				{
					m_pModelCom->Set_Animation(ANIM_DMG_AIR_CONNECT_1);
					m_iSmallHit_Index = 0;
				}
			}
			else
			{
				if (m_iSmallHit_Index == 0)
				{
					m_pModelCom->Set_Animation(138);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 1)
				{
					m_pModelCom->Set_Animation(139);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 2)
				{
					m_pModelCom->Set_Animation(140);
					m_iSmallHit_Index++;
				}
				else if (m_iSmallHit_Index == 3)
				{
					m_pModelCom->Set_Animation(141);
					m_iSmallHit_Index = 0;
				}
			}
		}
	}
	if (m_isConnectHitting == false)
	{
		Go_Dir_Deceleration(dTimeDelta, 138, 1.5f * m_fDmg_Move_Ratio, 0.01f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 139, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 140, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
		Go_Dir_Deceleration(dTimeDelta, 141, 1.0f * m_fDmg_Move_Ratio, 0.015f, AtkDir);
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
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_BIG, 2.0f , 0.035f, AtkDir);

#pragma endregion



#pragma region Dmg_Blow
	if (m_Moveset.m_Down_Dmg_Blow)
	{
		m_Moveset.m_Down_Dmg_Blow = false;

		//m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isSkilling == false)
		{
			m_pTransformCom->Set_Look(reverseAtkDir);
			Jumping(1.2f, 0.05f);
			m_pModelCom->Set_Animation(ANIM_DMG_BLOW);
		}
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_BLOW, 2.5f * m_fDmg_Move_Ratio, AtkDir);
	Go_Dir_Constant(dTimeDelta, 85, 2.5f * m_fDmg_Move_Ratio, AtkDir);
	Ground_Animation_Play(85, 86);

#pragma endregion



#pragma region Dmg_BigBlow
	if (m_Moveset.m_Down_Dmg_BigBlow)
	{
		m_Moveset.m_Down_Dmg_BigBlow = false;

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isSkilling == false)
		{
			Jumping(1.2f, 0.05f);
			m_pModelCom->Set_Animation(ANIM_DMG_SPIN);
			//m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
			m_pTransformCom->Set_Look(reverseAtkDir);
		}
	}
	Go_Dir_Constant(dTimeDelta, ANIM_DMG_SPIN, 3.0f * m_fDmg_Move_Ratio, AtkDir);
	Go_Dir_Constant(dTimeDelta, 110, 3.0f * m_fDmg_Move_Ratio, AtkDir);
	Go_Dir_Deceleration(dTimeDelta, 111, 3.0f * m_fDmg_Move_Ratio, 0.1f, AtkDir);
	Ground_Animation_Play(110, 111);

#pragma endregion


#pragma region Dmg_Upper
	if (m_Moveset.m_Down_Dmg_Upper)
	{
		m_Moveset.m_Down_Dmg_Upper = false;
		m_isConnectHitting = false;

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		if (m_isSkilling == false)
		{
			Jumping(1.85f, 0.03f);
			m_pModelCom->Set_Animation(ANIM_FALL);
			m_pTransformCom->LerpVector(XMLoadFloat4(&reverseAtkDir), 0.8f);
		}
	}
	if (m_isConnectHitting == false)
	{
		Go_Dir_Constant(dTimeDelta, ANIM_FALL, 0.2f * m_fDmg_Move_Ratio, AtkDir);
		Go_Dir_Constant(dTimeDelta, 100, 0.2f * m_fDmg_Move_Ratio, AtkDir);
	}
	Ground_Animation_Play(100, 101);
#pragma endregion


#pragma region Dmg_Web
	if (m_Moveset.m_Down_Dmg_Web)
	{
		m_Moveset.m_Down_Dmg_Web = false;

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		m_isConnectHitting = true;
		
		if (m_isSkilling == false)
		{
			if (m_iSmallHit_Index == 0)
			{
				m_pModelCom->Set_Animation(138);
				m_iSmallHit_Index++;
			}
			else if (m_iSmallHit_Index == 1)
			{
				m_pModelCom->Set_Animation(139);
				m_iSmallHit_Index++;
			}
			else if (m_iSmallHit_Index == 2)
			{
				m_pModelCom->Set_Animation(140);
				m_iSmallHit_Index++;
			}
			else if (m_iSmallHit_Index == 3)
			{
				m_pModelCom->Set_Animation(141);
				m_iSmallHit_Index = 0;
			}
		}
	}
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

		m_pModelCom->Set_Animation(ANIM_DOWN_GETUP_MOVE);
		//m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
		m_pTransformCom->LerpVector(XMLoadFloat4(&m_Moveset.m_Input_Dir), 0.8f);
	}
	//Go_Straight_Constant(dTimeDelta, 138, 2.0f);
	Go_Straight_Deceleration(dTimeDelta, ANIM_DOWN_GETUP_MOVE, 3.0f * m_fMoving_Ratio, 0.03f);
	m_pModelCom->Set_EarlyEnd(ANIM_DOWN_GETUP_MOVE, true);



}

void CPlayer_Zenitsu::Player_Change(_double dTimeDelta)
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

	if (iCurAnim == ANIM_BATTLE_JUMP || iCurAnim == 57 || iCurAnim == 58 || iCurAnim == 59)
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

void CPlayer_Zenitsu::Moving_Restrict()
{
	_int iCurAnimIndex = m_pModelCom->Get_iCurrentAnimIndex();


	//히트시 제한 + 기상, 다운
	if (ANIM_DMG_BLOW == iCurAnimIndex || 85 == iCurAnimIndex || 86 == iCurAnimIndex
		|| ANIM_DMG_SPIN == iCurAnimIndex || 110 == iCurAnimIndex || 111 == iCurAnimIndex
		|| ANIM_FALL == iCurAnimIndex || 100 == iCurAnimIndex || 101 == iCurAnimIndex
		|| ANIM_DMG_SMALL == iCurAnimIndex || ANIM_DMG_BIG == iCurAnimIndex
		|| ANIM_DOWN == iCurAnimIndex || ANIM_DOWN_GETUP_MOVE == iCurAnimIndex || 103 == iCurAnimIndex || 104 == iCurAnimIndex
		|| ANIM_GETUP == iCurAnimIndex || 96 == iCurAnimIndex 
		|| 138 == iCurAnimIndex || 139 == iCurAnimIndex || 140 == iCurAnimIndex || 141 == iCurAnimIndex || 142 == iCurAnimIndex
		|| ANIM_DMG_AIR_CONNECT_0 == iCurAnimIndex || ANIM_DMG_AIR_CONNECT_1 == iCurAnimIndex)
	{
		m_Moveset.m_isHitMotion = true;

		m_isAir_Hekireki = false;
		m_isAirDashing = false;

		//다운상태
		if (ANIM_DOWN == iCurAnimIndex)
		{
			m_Moveset.m_isDownMotion = true;
			m_Moveset.m_Down_Dmg_Blow = false;

			m_Moveset.m_isGetUpMotion = false;
		}

		//겟업 상태
		if (ANIM_GETUP == iCurAnimIndex || 96 == iCurAnimIndex 
			|| ANIM_DOWN_GETUP_MOVE == iCurAnimIndex || 103 == iCurAnimIndex || 104 == iCurAnimIndex)
		{
			m_Moveset.m_isGetUpMotion = true;
		}
	}
	//콤보공격시 무빙제한
	else if (ANIM_ATK_COMBO == iCurAnimIndex || 4 == iCurAnimIndex || 5 == iCurAnimIndex
		|| ANIM_ATK_COMBO_DOWN == iCurAnimIndex
		|| 7 == iCurAnimIndex
		|| ANIM_ATK_COMBO_UP == iCurAnimIndex || 9 == iCurAnimIndex || 10 == iCurAnimIndex || 11 == iCurAnimIndex
		|| 12 == iCurAnimIndex || 13 == iCurAnimIndex || 14 == iCurAnimIndex || 15 == iCurAnimIndex || 16 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;

		if (ANIM_ATK_COMBO_DOWN)
		{
			m_isComboing_Down = true;
		}

		if (ANIM_ATK_COMBO != iCurAnimIndex)
		{
			m_pSword->Set_SwordIn(false);
			m_pSwordHome->Set_SwordIn(false);
		}

		if (ANIM_ATK_COMBO_UP == iCurAnimIndex || 9 == iCurAnimIndex || 10 == iCurAnimIndex)
		{
			m_isComboing_Upper = true;
		}
	}
	//점프 콤보공격시 제한
	else if (ANIM_ATK_AIRCOMBO == iCurAnimIndex || 18 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Jump = true;
		m_Moveset.m_isRestrict_JumpCombo = true;

		m_pSword->Set_SwordIn(false);
		m_pSwordHome->Set_SwordIn(false);
	}
	//차지공격 시 무빙제한
	else if (ANIM_ATK_CHARGE == iCurAnimIndex || 20 == iCurAnimIndex || 21 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Charge = true;

		if (21 == iCurAnimIndex)
		{
			m_pSword->Set_SwordIn(false);
			m_pSwordHome->Set_SwordIn(false);
		}
	}
	//스킬공격 시 무빙제한
	else if (ANIM_ATK_SKILL_GUARD == iCurAnimIndex
		|| ANIM_ATK_SKILL_HEKIREKI == iCurAnimIndex || ANIM_ATK_SKILL_HEKIREKI_END == iCurAnimIndex
		|| ANIM_ATK_SKILL_HEKIREKI_AIR == iCurAnimIndex || ANIM_ATK_SKILL_HEKIREKI_AIR_END == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;

		m_isSkilling = true;

		if (ANIM_ATK_SKILL_GUARD != iCurAnimIndex)
		{
			m_pSword->Set_SwordIn(false);
			m_pSwordHome->Set_SwordIn(false);
		}

		if (ANIM_ATK_SKILL_HEKIREKI == iCurAnimIndex || ANIM_ATK_SKILL_HEKIREKI_END == iCurAnimIndex
			|| ANIM_ATK_SKILL_HEKIREKI_AIR == iCurAnimIndex || ANIM_ATK_SKILL_HEKIREKI_AIR_END == iCurAnimIndex)
		{
			m_pSword->Set_SwordIn(true);
			m_pSwordHome->Set_SwordIn(true);

			m_isHekireki = true;
		}
	}
	//잡기 공격 시 제한
	else if (ANIM_ATK_THROW == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Throw = true;
	}
	//점프 트랙 (이동키 + J키)공격 시 제한
	else if (ANIM_ATK_AIRTRACK == iCurAnimIndex || 1 == iCurAnimIndex || 2 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Jump = true;

		if (2 == iCurAnimIndex)
		{
			m_pSword->Set_SwordIn(false);
			m_pSwordHome->Set_SwordIn(false);
		}
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
	}
	//Special
	else if (ANIM_ATK_SPECIAL_CUTSCENE == iCurAnimIndex || 118 == iCurAnimIndex || 119 == iCurAnimIndex || 120 == iCurAnimIndex
		|| 121 == iCurAnimIndex || 122 == iCurAnimIndex || 123 == iCurAnimIndex || 124 == iCurAnimIndex || 125 == iCurAnimIndex || 126 == iCurAnimIndex
		|| ANIM_ATK_SPECIAL_READY == iCurAnimIndex || 128 == iCurAnimIndex || 129 == iCurAnimIndex || 130 == iCurAnimIndex || 131 == iCurAnimIndex)
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
	else if (ANIM_BATTLE_GUARD == iCurAnimIndex || 51 == iCurAnimIndex || 52 == iCurAnimIndex
		|| ANIM_BATTLE_GUARD_HIT_BIG == iCurAnimIndex || ANIM_BATTLE_GUARD_HIT_SMALL == iCurAnimIndex || ANIM_BATTLE_GUARD_PUSH == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isHitMotion = false;
	}
	//대시 시 제한
	else if (ANIM_BATTLE_DASH == iCurAnimIndex || 46 == iCurAnimIndex /* || 47 == iCurAnimIndex*/)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_KeyInput = true;
		m_Moveset.m_isRestrict_Dash = true;
	}
	//점프 시 무빙제한
	else if (ANIM_BATTLE_JUMP == iCurAnimIndex || 57 == iCurAnimIndex || 58 == iCurAnimIndex)
	{
		m_Moveset.m_isRestrict_Move = true;
		m_Moveset.m_isRestrict_Jump = true;

		if (58 == iCurAnimIndex)
		{
			m_isCan_Air_Hekireki = true;
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
	else if (71 == iCurAnimIndex || 73 == iCurAnimIndex)
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

		m_isComboing_Down = false;
		m_isComboing_Upper = false;

		m_isHekireki = false;
		m_isCan_Air_Hekireki = false;

		m_pSword->Set_SwordIn(true);
		m_pSwordHome->Set_SwordIn(true);

		m_isSkilling = false;
	}
}

HRESULT CPlayer_Zenitsu::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zenitsu"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CPlayer_Zenitsu");
		return E_FAIL;
	}


	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CPlayer_Zenitsu");
		return E_FAIL;
	}


	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CPlayer_Zenitsu");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CPlayer_Zenitsu");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CPlayer_Zenitsu");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.f, 1.f, 1.f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CPlayer_Zenitsu");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Zenitsu::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_bMotionBlur", &m_bMotionBlur, sizeof(_bool))))
		return E_FAIL;

	// 슈퍼아머 상태 넣어주셈
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_bSuperArmor", &m_isSkilling, sizeof(_bool))))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

CPlayer_Zenitsu* CPlayer_Zenitsu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Zenitsu* pInstance = new CPlayer_Zenitsu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Zenitsu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Zenitsu::Clone(void* pArg)
{
	CPlayer_Zenitsu* pInstance = new CPlayer_Zenitsu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Zenitsu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Zenitsu::Free()
{
	Safe_Release(m_pSword);
	Safe_Release(m_pSwordHome);

	__super::Free();
}
