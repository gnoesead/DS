#include "pch.h"
#include "..\Public\Monster_Zako.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "MonsterManager.h"

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

	

	return S_OK;
}

void CMonster_Zako::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

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

void CMonster_Zako::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	Gravity(dTimeDelta);


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

HRESULT CMonster_Zako::Render_ShadowDepth()
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


void CMonster_Zako::EventCall_Control(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_iEvent_Index = 0;
	}

	if (EventCallProcess())
	{
#pragma region Attack
		/*if (ANIM_ATK_0 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 1.0f, 1.1f), 0.1,
					CAtkCollider::TYPE_SMALL, XMLoadFloat4(&m_Dir_To_Player), 10.0f);
			}
		}*/

#pragma endregion
		m_iEvent_Index++;
	}
}

void CMonster_Zako::Trigger()
{
	//Hit_Trigger
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small()
		|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Big()
		|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow()
		|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Spin())
	{
		m_eCurState = STATE_HIT;
	}
}

void CMonster_Zako::Animation_Control(_double dTimeDelta)
{
	if (m_isDeath_Motion)
	{
		/*m_dDelay_Die += dTimeDelta;
		if (m_dDelay_Die > 10.0f)
			m_isDead = true;

		m_pColliderCom[COLL_SPHERE]->Set_Death(true);*/
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

void CMonster_Zako::Animation_Control_Idle(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.05f);
	_float fDistance = Calculate_Distance();

	m_dDelay_Move += dTimeDelta;
	//앞 뒤 이동
	if (fDistance < 4.5f)
	{
		if (m_isFirst_Move_0)
		{
			m_isFirst_Move_0 = false;
			m_isFirst_Move_1 = true;

			m_pModelCom->Set_Animation(ANIM_WALK_BACK);
			m_dDelay_Move = 0.0;
		}
	}
	else if (4.5f <= fDistance && fDistance < 7.0f)
	{
		if (m_isFirst_Move_1)
		{
			m_isFirst_Move_0 = true;
			m_isFirst_Move_1 = false;

			m_dDelay_Move = 0.0;
			m_pModelCom->Set_Animation(ANIM_IDLE);
		}

		//옆 이동
		m_dCoolTime_SideMove += dTimeDelta;
		if (5.7f < m_dCoolTime_SideMove)
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
		if (m_isFirst_Move_0)
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
	

	// Attack CoolTime
	m_dCoolTime_AtkPattern += dTimeDelta;
	if (11.1f < m_dCoolTime_AtkPattern)
	{
		m_dCoolTime_AtkPattern = 0.0;

		m_eCurState = STATE_ATTACK;
		m_isFirst_AtkPattern = true;
	}
	
}

void CMonster_Zako::Animation_Control_Attack(_double dTimeDelta, _int AttackIndex)
{	
	switch (AttackIndex)
	{
	case 0: //ANIM_ATK_CLAWS
		Animation_Control_Attack_Claws(dTimeDelta);
		break;
	case 1:
		break;
	default:
		break;
	}


	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_IDLE)
	{
		m_eCurState = STATE_IDLE;
		m_isFirst_Move_0 = true;
		m_isFirst_Move_1 = true;
	}
}

void CMonster_Zako::Animation_Control_Attack_Claws(_double dTimeDelta)
{
	if (m_isFirst_AtkPattern)
	{
		m_isFirst_AtkPattern = false;

		if (2.0f < Calculate_Distance())
			m_pModelCom->Set_Animation(ANIM_RUN);
		else
			m_pModelCom->Set_Animation(ANIM_ATK_CLAWS);
		m_isFirst_ATK_0 = true;
	}

	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	//달리는 도중
	if (iCurAnim == ANIM_RUN)
	{
		m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.2f);
		if (1.5f < Calculate_Distance())
			Go_Straight_Constant(dTimeDelta, ANIM_RUN, 1.6f);
		else
		{
			if (m_isFirst_ATK_0)
			{
				m_isFirst_ATK_0 = false;

				m_pModelCom->Set_Animation(58);
			}
		}
	}
	if (iCurAnim == 58)
	{
		if (m_pModelCom->Get_AnimFinish(58))
			m_pModelCom->Set_Animation(ANIM_ATK_CLAWS);
	}

}

void CMonster_Zako::Animation_Control_Hit(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();


}

void CMonster_Zako::Animation_Control_Down(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

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



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.0f , 1.0f , 1.0f );
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CMonster_Zako");
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
