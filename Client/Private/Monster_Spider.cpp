#include "pch.h"
#include "..\Public\Monster_Spider.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "MonsterManager.h"

CMonster_Spider::CMonster_Spider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CMonster_Spider::CMonster_Spider(const CMonster_Spider& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonster_Spider::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Spider::Initialize(void* pArg)
{
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	m_pModelCom->Set_Animation(ANIM_IDLE);

	if (FAILED(Read_Animation_Control_File("Spider.bin")))
	{
		MSG_BOX("Failed to AnimData Read : Spider");
		return E_FAIL;
	}

	m_fScale = (rand() % 14 + 8) * 0.1f;
	m_pTransformCom->Scaling(_float3{ m_fScale, m_fScale, m_fScale });

	m_fAttack = (rand() % 100 + 50) * 0.1f;


	return S_OK;
}

void CMonster_Spider::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	MonsterManaging();

	Calculate_To_Player();
	
	if (true == m_isDead)
		return;

	Trigger();
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

void CMonster_Spider::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	Gravity(dTimeDelta);


#ifdef _DEBUG
	/*if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
		return;*/
#endif
}

HRESULT CMonster_Spider::Render()
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


	return S_OK;
}

HRESULT CMonster_Spider::Render_ShadowDepth()
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


void CMonster_Spider::MonsterManaging()
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	//공격중일때,
	if (iCurAnim == ANIM_ATK_0 || iCurAnim == 5 || iCurAnim == ANIM_ATK_1 || iCurAnim == 7 || iCurAnim == 8)
	{
		if (m_isFirst_AttackOn)
		{
			m_isFirst_AttackOn = false;
			m_isFirst_AttackOff = true;
			CMonsterManager::GetInstance()->Cnt_AttackOn_Up();
		}
	}
	else
	{
		if (m_isFirst_AttackOff)
		{
			m_isFirst_AttackOn = true;
			m_isFirst_AttackOff = false;
			CMonsterManager::GetInstance()->Cnt_AttackOn_Down();
		}
	}
	
}

void CMonster_Spider::EventCall_Control(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_iEvent_Index = 0;
	}

	if (EventCallProcess())
	{
#pragma region Attack
		if (ANIM_ATK_0 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 1.0f, 1.1f), 0.1,
					CAtkCollider::TYPE_SMALL, XMLoadFloat4(&m_Dir_To_Player), 10.0f);
			}
		}

		if (8 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.6f, 0.6f, 0.6f), _float3(0.f, 1.0f, 1.1f), 0.1,
					CAtkCollider::TYPE_BLOW, XMLoadFloat4(&m_Dir_To_Player), 10.0f);
			}
		}
		
#pragma endregion
		m_iEvent_Index++;
	}
}

void CMonster_Spider::Trigger()
{
	//Hit_Trigger
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small()
		|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Big()
		|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow()
		|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Spin()
		|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Upper())
	{
		m_eCurState = STATE_HIT;
	}
}

void CMonster_Spider::Animation_Control(_double dTimeDelta)
{
	if (m_isDeath_Motion)
	{
		m_dDelay_Die += dTimeDelta;
		if (m_dDelay_Die > 10.0f)
			m_isDead = true;

		m_pColliderCom[COLL_SPHERE]->Set_Death(true);
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
			Animation_Control_Attack(dTimeDelta);
	}
}

void CMonster_Spider::Animation_Control_Idle(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	_float4 PlayerPos_FixY = m_PlayerPos;
	PlayerPos_FixY.y = 0.0f;


	m_pTransformCom->LerpVector(XMLoadFloat4(&m_Dir_To_Player_FixY), 0.1f);

	//거미의 이동
	m_dDelay_Spider += dTimeDelta;
	if (m_dDelay_Spider > 0.3)
	{
		if (m_fDistance_To_Player < 3.5f)
		{
			m_pTransformCom->Go_Backward(dTimeDelta * 0.3f);

			if (m_isFirst_Spider_0)
			{
				m_isFirst_Spider_0 = false;
				m_isFirst_Spider_1 = true;

				m_dDelay_Spider = 0.0;

				m_pModelCom->Set_Animation(ANIM_ADV_WALK);
			}
		}
		else if (3.5f <= m_fDistance_To_Player && m_fDistance_To_Player < 7.0f)
		{
			if (m_isFirst_Spider_1)
			{
				m_isFirst_Spider_0 = true;
				m_isFirst_Spider_1 = false;

				m_pModelCom->Set_Animation(ANIM_ADV_WALK_END);
			}

			//attack
			m_dCoolTime_Attack += dTimeDelta;
		}
		else if (7.0f <= m_fDistance_To_Player)
		{
			m_pTransformCom->Go_Straight(dTimeDelta * 0.55f);

			if (m_isFirst_Spider_0)
			{
				m_isFirst_Spider_0 = false;
				m_isFirst_Spider_1 = true;

				m_dDelay_Spider = 0.0;

				m_pModelCom->Set_Animation(ANIM_RUN);
			}
		}
	}

	
	if (m_dCoolTime_Attack > m_fAttack)
	{
		m_dCoolTime_Attack = 0.0;

		if (CMonsterManager::GetInstance()->Can_Attack())
		{
			m_eCurState = STATE_ATTACK;
			m_isFirst_Attack = true;
		}
		
	}

}

void CMonster_Spider::Animation_Control_Attack(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();
	if (iCurAnim == 0)
		m_pModelCom->Set_Animation(ANIM_IDLE);
	
	if (0 == (rand() % 2))
	{
		if (m_isFirst_Attack )
		{
			m_isFirst_Attack = false;
			m_pModelCom->Set_Animation(ANIM_ATK_1);
			m_dDelay_Attack = 0.0;
			m_pModelCom->Set_AnimResetTimeAcc(8);

			m_pColliderCom[COLL_SPHERE]->Set_SphereOff(true);
			
			
		}
		if (iCurAnim == 6 || iCurAnim == 7)
		{
			//_float4 PlayerPos_FixY = m_PlayerPos;
			//PlayerPos_FixY.y = 0.0f;
			//m_pTransformCom->LookAt(XMLoadFloat4(&PlayerPos_FixY));
			m_pTransformCom->LerpVector(XMLoadFloat4(&m_Dir_To_Player_FixY), 0.1f);
		}
		if (iCurAnim == 8)
		{
			m_dDelay_Attack += dTimeDelta;
			if (0.41f < m_dDelay_Attack && m_dDelay_Attack < 0.8f)
				Go_Straight_Constant(dTimeDelta, 8, 4.0f);

		}
		
	}
	else
	{
		if (m_isFirst_Attack)
		{
			m_isFirst_Attack = false;
			m_pModelCom->Set_Animation(ANIM_ATK_0);
			m_dDelay_Attack = 0.0;
			m_pModelCom->Set_AnimResetTimeAcc(5);

			m_pColliderCom[COLL_SPHERE]->Set_SphereOff(true);

			//_float4 PlayerPos_FixY = m_PlayerPos;
			//PlayerPos_FixY.y = 0.0f;
			//m_pTransformCom->LookAt(XMLoadFloat4(&PlayerPos_FixY));
			m_pTransformCom->LerpVector(XMLoadFloat4(&m_Dir_To_Player_FixY), 0.1f);
			
		}
		if (iCurAnim == ANIM_ATK_0)
			Go_Straight_Constant(dTimeDelta, ANIM_ATK_0, 2.8f);
		if (iCurAnim == 5)
		{
			m_dDelay_Attack += dTimeDelta;
			if (m_dDelay_Attack < 0.25f)
				Go_Straight_Constant(dTimeDelta, 5, 2.8f);
		}
	}


	if (iCurAnim == ANIM_IDLE)
	{
		m_eCurState = STATE_IDLE;
		m_dDelay_Attack = 0.0;

		m_pColliderCom[COLL_SPHERE]->Set_SphereOff(false);
		m_isFirst_Spider_0 = true;
		m_isFirst_Spider_1 = true;
	}
	
}

void CMonster_Spider::Animation_Control_Hit(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();


	m_dCoolTime_Collider += dTimeDelta;
	
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small() && m_dCoolTime_Collider > 0.2f)
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		m_dCoolTime_Collider = 0.0;

		if (m_StatusDesc.fHp <= 0.0f)
		{
			m_pModelCom->Set_Animation(17);
			m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_DMG_SMALL);
			m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
		}
	}
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL, 2.3f, 0.14f, m_Hit_AtkDir);


	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Big() && m_dCoolTime_Collider > 0.2f)
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		m_dCoolTime_Collider = 0.0;

		if (m_StatusDesc.fHp <= 0.0f)
		{
			m_pModelCom->Set_Animation(17);
			m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
		}
		else
		{
			m_pModelCom->Set_Animation(ANIM_DMG_BIG);
			m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
		}

	}
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_BIG, 2.0f, 0.10f, m_Hit_AtkDir);


	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow() && m_dCoolTime_Collider > 0.2f)
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		m_dCoolTime_Collider = 0.0;

		m_pModelCom->Set_Animation(17);
		m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();

	}
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Spin() && m_dCoolTime_Collider > 0.2f)
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Spin(false);

		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		m_dCoolTime_Collider = 0.0;

		
		m_pModelCom->Set_Animation(17);
		m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
		
	}

	Go_Dir_Constant(dTimeDelta, ANIM_BLOW, 1.5f, m_Dir_To_Monster);
	//m_pModelCom->Set_EarlyEnd(ANIM_BLOW, true);
	Go_Dir_Constant(dTimeDelta, 17, 1.5f, m_Dir_To_Monster);
	Go_Dir_Deceleration(dTimeDelta, 18, 1.5f, 0.15f, m_Dir_To_Monster);


	if (iCurAnim == ANIM_DOWN)
	{
		m_eCurState = STATE_DOWN;
	}

	if (iCurAnim == ANIM_IDLE)
	{
		m_eCurState = STATE_IDLE;
	}

}

void CMonster_Spider::Animation_Control_Down(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_DOWN)
	{
		if (m_StatusDesc.fHp > 0.0f)
		{
			m_dDelay_Down += dTimeDelta;
			if (m_dDelay_Down > 4.0f)
			{
				m_dDelay_Down = 0.0f;
				m_pModelCom->Set_Animation(ANIM_RETURN);
				m_isFirst_Spider_0 = true;
				m_isFirst_Spider_1 = true;
			}
		}
		else if (m_StatusDesc.fHp <= 0.0f)
		{
			m_isDeath_Motion = true;
		}
	}


	if (iCurAnim == ANIM_IDLE)
	{
		m_eCurState = STATE_IDLE;
	}


}

HRESULT CMonster_Spider::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Spider"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CMonster_Spider");
		return E_FAIL;
	}

	

	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CMonster_Spider");
		return E_FAIL;
	}


	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CMonster_Spider");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CMonster_Spider");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CMonster_Spider");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(0.5f * m_fScale, 0.5f * m_fScale, 0.5f * m_fScale);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CMonster_Spider");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CMonster_Spider::SetUp_ShaderResources()
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

CMonster_Spider* CMonster_Spider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Spider* pInstance = new CMonster_Spider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Spider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Spider::Clone(void* pArg)
{
	CMonster_Spider* pInstance = new CMonster_Spider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Spider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Spider::Free()
{
	
	__super::Free();
}
