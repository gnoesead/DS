#include "pch.h"
#include "..\Public\NPC_Zenitsu.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "MonsterManager.h"

#include "Player.h"
#include "Fade_Manager.h"

CNPC_Zenitsu::CNPC_Zenitsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CNPC(pDevice, pContext)
{
}

CNPC_Zenitsu::CNPC_Zenitsu(const CNPC_Zenitsu& rhs)
	: CNPC(rhs)
{
}

HRESULT CNPC_Zenitsu::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC_Zenitsu::Initialize(void* pArg)
{

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pModelCom->Set_Animation(ANIM_ADV_STEALTH_IDLE);

	if (FAILED(Read_Animation_Control_File("Zenitsu.bin")))
	{
		MSG_BOX("Failed to AnimData Read : Zenitsu");
		return E_FAIL;
	}


	Get_PlayerComponent();

	_float4 Pos;
	XMStoreFloat4(&Pos, m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fLand_Y = Pos.y;

	m_pModelCom->Set_LinearDuration(ANIM_ADV_STEALTH_IDLE, 0.1f);
	m_pModelCom->Set_LinearDuration(ANIM_ADV_STEALTH_WALK, 0.1f);
	m_pModelCom->Set_LinearDuration(147, 0.0001f);
	m_pModelCom->Set_LinearDuration(148, 0.15f);

	m_pModelCom->Set_EarlyEnd(ANIM_ADV_STEALTH_IDLE, false, 0.9999f);
	m_pModelCom->Set_EarlyEnd(ANIM_ADV_STEALTH_WALK, false, 0.9999f);
	m_pModelCom->Set_EarlyEnd(147, false, 0.9999f);
	m_pModelCom->Set_EarlyEnd(148, false, 0.9999f);

	m_ResetPos[0] = { 4.61f, 0.05f, 7.37f, 1.f }; //첫지역
	m_ResetPos[1] = { 77.18f, 0.05f, 6.75f, 1.f }; // 처음 이동
	m_ResetPos[2] = { 74.1f, 0.05f, 66.63f, 1.f }; // 둘째 이동
	m_ResetPos[3] = { 197.1f, 0.05f, 31.95f, 1.f }; // 자코방 앞
	
	//m_ResetPos[3] = { 69.26f, 0.05f, 24.7f, 1.f }; // 자코방 앞

	CMonsterManager::GetInstance()->Set_ThreeCnt(0);
	m_iResetIndex = 0;
	CMonsterManager::GetInstance()->Set_Zenitsu_IndexPlus(false);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_ResetPos[0]));

	m_pTransformCom->Set_Look(m_CharacterDesc.NPCDesc.DirNPC);

	return S_OK;
}

void CNPC_Zenitsu::Tick(_double dTimeDelta)
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
	{
		Find_Section();

		if (m_iPlayer_Section == m_CharacterDesc.NPCDesc.iSection || m_iPlayer_Section_Sub == m_CharacterDesc.NPCDesc.iSection)
		{
			__super::Tick(dTimeDelta);

			if (true == m_isDead)
			{
				Safe_Release(pGameInstance);
				return;
			}
			Animation_Control(dTimeDelta);

			//애니메이션 처리
			m_pModelCom->Play_Animation(dTimeDelta);
			RootAnimation(dTimeDelta);

			//이벤트 콜
			EventCall_Control(dTimeDelta);

			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			{
				Safe_Release(pGameInstance);
				return;
			}
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this)))
			{
				Safe_Release(pGameInstance);
				return;
			}
		}
	}
	else if (pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
	{
		//젠이츠 다시 생김
		if (CMonsterManager::GetInstance()->Get_ThreeCnt() == 3)
		{
			if (pGameInstance->Get_DIKeyDown(DIK_9))
			{
				CMonsterManager::GetInstance()->Set_BattleOn(false);
				CMonsterManager::GetInstance()->Set_ThreeCnt(0);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 69.26f, 0.05f, 24.7f, 1.f });
			}
		}

		if (CMonsterManager::GetInstance()->Get_BattleOn() == false)
		{
			__super::Tick(dTimeDelta);

			if (true == m_isDead)
			{
				Safe_Release(pGameInstance);
				return;
			}
			Animation_Control(dTimeDelta);

			//애니메이션 처리
			m_pModelCom->Play_Animation(dTimeDelta);
			RootAnimation(dTimeDelta);

			//이벤트 콜
			EventCall_Control(dTimeDelta);

			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
			{
				Safe_Release(pGameInstance);
				return;
			}
			if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this)))
			{
				Safe_Release(pGameInstance);
				return;
			}
		}
	}
	
	Safe_Release(pGameInstance);
}

void CNPC_Zenitsu::LateTick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
	{
		Find_Section();

		if (m_iPlayer_Section == m_CharacterDesc.NPCDesc.iSection || m_iPlayer_Section_Sub == m_CharacterDesc.NPCDesc.iSection)
		{
			__super::LateTick(dTimeDelta);

			Gravity(dTimeDelta);
		}
	}
	else
	{
		if (CMonsterManager::GetInstance()->Get_BattleOn() == false)
		{
			__super::LateTick(dTimeDelta);

			Gravity(dTimeDelta);
		}
	}

	Safe_Release(pGameInstance);
#ifdef _DEBUG
	/*if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
		return;*/
#endif
}

HRESULT CNPC_Zenitsu::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
	{
		Find_Section();

		if (m_iPlayer_Section == m_CharacterDesc.NPCDesc.iSection || m_iPlayer_Section_Sub == m_CharacterDesc.NPCDesc.iSection)
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
	}
	else
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
	}
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CNPC_Zenitsu::Render_ShadowDepth()
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


void CNPC_Zenitsu::EventCall_Control(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_iEvent_Index = 0;
	}

	if (EventCallProcess())
	{
#pragma region Animation_0
		/*if (ANIM_ATK_COMBO == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				_tchar szTest[MAX_PATH] = TEXT("TestSound.wav");
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_SLASH);
				CSoundMgr::Get_Instance()->PlaySound(szTest, CSoundMgr::PLAYER_SLASH, 0.9f);
			}

			CEffectPlayer::Get_Instance()->Play("hjd", m_pTransformCom);
		}*/
		
#pragma endregion
		m_iEvent_Index++;
	}
}

void CNPC_Zenitsu::Animation_Control(_double dTimeDelta)
{
	//Set_Height();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	if(pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
		Animation_Control_Village(dTimeDelta);
	else if(pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE)
		Animation_Control_House(dTimeDelta);

	Safe_Release(pGameInstance);
}

void CNPC_Zenitsu::Animation_Control_Village(_double dTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 564.4f, 4.55f ,362.73f, 1.0f });

	if (m_isTalking)
	{
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.05f);
	}
}

void CNPC_Zenitsu::Animation_Control_House(_double dTimeDelta)
{
	m_dDelay_GoOn += dTimeDelta;
	if (m_dDelay_GoOn > 0.8f)
	{
		m_dDelay_GoOn = 0.0;

		if (Calculate_Distance() > 2.5f)
		{
			if (m_isGoOn == false)
			{
				m_isGoOn = true;
				m_isFirst_GoOn = true;
			}
		}
		else
		{
			m_isGoOn = false;
		}
	}
	m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.04f);


	if (m_isGoOn)
	{
		if (m_isFirst_GoOn)
		{
			m_isFirst_GoOn = false;

			m_pModelCom->Set_Animation(ANIM_ADV_STEALTH_WALK);
			m_isFirst_GoStop = true;
		}
	}
	else
	{
		if (m_isFirst_GoStop)
		{
			m_isFirst_GoStop = false;

			m_pModelCom->Set_Animation(148);
		}
	}

	Go_Straight_Constant(dTimeDelta, ANIM_ADV_STEALTH_WALK, 0.45f);
	Go_Straight_Constant(dTimeDelta, 147, 0.45f);
	Go_Straight_Deceleration(dTimeDelta, 148, 0.45f, 0.07f);


	//잠입모드 발각 시
	if (CMonsterManager::GetInstance()->Get_ZenitsuBack())
	{
		CMonsterManager::GetInstance()->Set_ZenitsuBack(false);

		m_isPlayerBack = true;
		m_dDelay_PlayerBack = 0.0;
	}
	if (m_isPlayerBack)
	{
		m_dDelay_PlayerBack += dTimeDelta;
		if (m_dDelay_PlayerBack > 2.5)
		{
			m_isPlayerBack = false;
			m_dDelay_PlayerBack = 0.0;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_ResetPos[m_iResetIndex]));
		}
	}


	//텔레포트 이동시
	if (CMonsterManager::GetInstance()->Get_Zenitsu_IndexPlus())
	{
		CMonsterManager::GetInstance()->Set_Zenitsu_IndexPlus(false);

		m_iResetIndex++;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_ResetPos[m_iResetIndex]));
	}

}


HRESULT CNPC_Zenitsu::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zenitsu"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CNPC_Zenitsu");
		return E_FAIL;
	}

	

	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CNPC_Zenitsu");
		return E_FAIL;
	}

	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CNPC_Zenitsu");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CNPC_Zenitsu");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CNPC_Zenitsu");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(0.7f, 0.7f, 0.7f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CNPC_Zenitsu");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNPC_Zenitsu::SetUp_ShaderResources()
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

CNPC_Zenitsu* CNPC_Zenitsu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC_Zenitsu* pInstance = new CNPC_Zenitsu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNPC_Zenitsu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Zenitsu::Clone(void* pArg)
{
	CNPC_Zenitsu* pInstance = new CNPC_Zenitsu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNPC_Zenitsu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Zenitsu::Free()
{
	
	__super::Free();
}
