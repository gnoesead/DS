#include "pch.h"
#include "Boss_Akaza.h"
#include "GameInstance.h"
#include "SoundMgr.h"

#include "AtkCollManager.h"

CBoss_Akaza::CBoss_Akaza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CBoss_Akaza::CBoss_Akaza(const CBoss_Akaza& rhs)
	: CCharacter(rhs)
{
}

HRESULT CBoss_Akaza::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Akaza::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Read_Animation_Control_File("Akaza.bin")))
	{
		MSG_BOX("Failed to AnimData Read : Akaza");
		return E_FAIL;
	}
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_FINALBOSS, TEXT("Layer_Player"), TEXT("Com_Transform")));

	Safe_Release(pGameInstance);


	m_eCurAnimIndex = ANIM_IDEL;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(140.f, 0.f, 130.f, 1.f));

	return S_OK;

}

void CBoss_Akaza::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (true == m_isDead)
		return;

	Update_TriggerTime(dTimeDelta);
	Update_Trigger(dTimeDelta);
	Update_State(dTimeDelta);

	m_pModelCom->Set_Animation(m_eCurAnimIndex);

	m_pModelCom->Play_Animation(dTimeDelta);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_Z))
	{
		/*m_eCurstate = STATE_AIRGUN;
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN);
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN2);
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN3);*/
		/*m_eCurstate = STATE_DASHPUNCH;
		m_pModelCom->Set_AnimisFinish(ANIM_CHARGE);
		m_pModelCom->Set_AnimisFinish(ANIM_CHARGE_ROOP);
		m_pModelCom->Set_AnimisFinish(ANIM_DASH);
		m_pModelCom->Set_AnimisFinish(ANIM_DASH_PUNCH);
		m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);
		m_bRunPunch = false;
		m_bStep_B = false;
		m_bDashOn = false;*/
		m_eCurstate = STATE_AIRGUN;

		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN);
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN2);
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN3);

	}
	if (pGameInstance->Get_DIKeyDown(DIK_X))
	{
		m_eCurstate = STATE_NACHIM;
		m_pModelCom->Set_AnimisFinish(ANIM_NACHIM);

	}
	if (pGameInstance->Get_DIKeyDown(DIK_C))
	{
		m_eCurstate = STATE_PUSHAWAY;
		m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
	}
	if (pGameInstance->Get_DIKeyDown(DIK_F1))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(140.f, 0.f, 130.f, 1.f));
		m_eCurstate = STATE_IDLE;
	}

	Safe_Release(pGameInstance);


	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this)))
		return;

}

void CBoss_Akaza::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);


	Update_AnimIndex(m_eCurAnimIndex);
	Gravity(dTimeDelta);
}

HRESULT CBoss_Akaza::Render()
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

HRESULT CBoss_Akaza::Render_ShadowDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;


	/*CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);*/


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

void CBoss_Akaza::Update_AnimIndex(_uint iAnimIndex)
{
	if (m_eCurAnimIndex != m_ePreAnimIndex)
	{
		m_ePreAnimIndex = m_eCurAnimIndex;
	}
}

void CBoss_Akaza::Update_Trigger(_double dTimeDelta)
{

	/*CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	switch (m_eCurPhase)
	{
	case CBoss_Akaza::BEGIN:
		Update_Begin(dTimeDelta);
		break;
	case CBoss_Akaza::PHASE_1:
		Update_Phase_1(dTimeDelta);
		break;
	case CBoss_Akaza::PHASE_2:
		Update_Phase_2(dTimeDelta);
		break;
	case CBoss_Akaza::PHASE_3:
		Update_Phase_3(dTimeDelta);
		break;
	}

	if (false == m_bBegin)
	{
		m_eCurstate = STATE_BEGIN;

	}*/



}

void CBoss_Akaza::Update_TriggerTime(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Reset(_double dTimeDelta)
{
	/*if (true == m_bHit)
	{
		m_bHit = false;
		m_pModelCom->Set_AnimisFinish()
	}*/
}

void CBoss_Akaza::Update_State(_double dTimeDelta)
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);
	_float fDistance = Convert::GetLength(vPlayerPos - vMonsterPos);


	switch (m_eCurstate)
	{
	case CBoss_Akaza::STATE_IDLE:
		Update_Idle(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_BEGIN:
		Update_Begin(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_ESCAPE:
		Update_Escape(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_DASHPUNCH:
		Update_DashPunch(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_GUARD:
		Update_Guard(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_AIRGUN:
		Update_AirGun(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_PUSHAWAY:
		Update_PushAway(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_COMBO_PUNCH:
		Update_ComboPunch(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_JUMPSTOMP:
		Update_JumpStomp(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_DASHKICK:
		Update_DashKick(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_JUMPAIRGUN:
		Update_JumpAirGun(dTimeDelta);
	case CBoss_Akaza::STATE_DASH_COMBO_PUNCH:
		Update_Dash_ComboPunch(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_UPPERKICK:
		Update_UpperKick(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_NEXTPHASE2:
		Update_NextPhase2(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_NEXTPHASE3:
		Update_NextPhase3(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_HEAL:
		Update_Heal(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_AWAKE:
		Update_Awake(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_NACHIM:
		Update_Nachim(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_NACHIM_COMBOPUNCH:
		Update_Nachim_ComboPunch(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_NACHIM_AIRGUN:
		Update_Nachim_AirGun(dTimeDelta);
		break;


	}

}

void CBoss_Akaza::Update_Idle(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_IDEL;
}

void CBoss_Akaza::Update_Begin(_double dTimeDelta)
{
	/*if (m_pModelCom->Get_AnimFinish(ANIM_BATTLE_START))
	{
		m_eCurAnimIndex = ANIM_BATTLE_START;
		m_bSetAnim = true;
	}*/

	//m_bBegin = true;

}

void CBoss_Akaza::Update_Phase_1(_double dTimeDelta)
{
	m_eCurstate = STATE_DASHPUNCH;
}

void CBoss_Akaza::Update_Phase_2(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Phase_3(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Escape(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_DashPunch(_double dTimeDelta)
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);
	_float fDistance = Convert::GetLength(vPlayerPos - vMonsterPos);

	if (fDistance > 10.f && m_bStep_B == false && m_bDashOn == false)
	{
		m_pTransformCom->LerpVector(vDir, 1.0f);
		m_pTransformCom->Go_Straight(dTimeDelta);
		m_eCurAnimIndex = ANIM_RUN;
		m_bRunPunch = true;
	}
	if (fDistance <= 10.f && fDistance > 5.f && false == m_bStep_B && m_bDashOn == false)
		m_bDashOn = true;

	if (fDistance <= 5.f && m_bRunPunch == false && m_bStep_B == false && m_bDashOn == false)
		m_bStep_B = true;

	if (true == m_bStep_B && m_bDashOn == false)
	{
		Go_Straight_Deceleration(dTimeDelta, ANIM_STEP_BEHIND, 3.f, 0.2f);
		m_eCurAnimIndex = ANIM_STEP_BEHIND;
		if (m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND) == true)
		{
			m_pModelCom->Set_AnimisFinish(ANIM_CHARGE);

			//m_bStep_B = false;
			m_bDashOn = true;
		}
	}
	if (m_bDashOn == true)
	{
		////애니메이션 제어
		if (m_bStep_B == false)
		{
			if (m_pModelCom->Get_AnimFinish(ANIM_CHARGE) == false)
				m_eCurAnimIndex = ANIM_CHARGE;
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_CHARGE) == true || m_bStep_B == true)
			m_eCurAnimIndex = ANIM_CHARGE_ROOP;

		if (m_pModelCom->Get_AnimFinish(ANIM_CHARGE_ROOP) == true)
			m_eCurAnimIndex = ANIM_DASH;

		if (m_pModelCom->Get_AnimFinish(ANIM_DASH) == true)
			m_eCurAnimIndex = ANIM_DASH_PUNCH;

		if (m_pModelCom->Get_AnimFinish(ANIM_DASH_PUNCH) == true)
		{
			m_eCurAnimIndex = ANIM_IDEL;
			m_eCurstate = STATE_IDLE;
		}

		//행동제어
		if (m_pModelCom->Get_iCurrentAnimIndex() != ANIM_DASH
			&& m_pModelCom->Get_iCurrentAnimIndex() != ANIM_DASH_PUNCH)
		{
			m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		}

		Go_Straight_Constant(dTimeDelta, ANIM_DASH, 13.f);
		Go_Straight_Deceleration(dTimeDelta, ANIM_DASH_PUNCH, 3.f, 0.2f);
	}

}

void CBoss_Akaza::Update_Guard(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_AirGun(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimFinish(ANIM_AIRGUN) == false)
		m_eCurAnimIndex = ANIM_AIRGUN;

	if (m_pModelCom->Get_AnimFinish(ANIM_AIRGUN) == true &&
		m_pModelCom->Get_AnimFinish(ANIM_AIRGUN2) == false)
		m_eCurAnimIndex = ANIM_AIRGUN2;

	if (m_pModelCom->Get_AnimFinish(ANIM_AIRGUN2) == true &&
		m_pModelCom->Get_AnimFinish(ANIM_AIRGUN3) == false)
		m_eCurAnimIndex = ANIM_AIRGUN3;

	if (m_pModelCom->Get_AnimFinish(ANIM_AIRGUN3) == true)
	{
		//m_eCurstate = STATE_DASHPUNCH;
		m_eCurAnimIndex = ANIM_IDEL;
		m_eCurstate = STATE_IDLE;

	}

	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

}

void CBoss_Akaza::Update_PushAway(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_AWAKE_PUSHAWAY;
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_PUSHAWAY))
	{
		m_eCurAnimIndex = ANIM_IDEL;
		m_eCurstate = STATE_IDLE;
	}
}

void CBoss_Akaza::Update_Nachim(_double dTimeDelta)
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);
	_float fDistance = Convert::GetLength(vPlayerPos - vMonsterPos);

	m_eCurAnimIndex = ANIM_NACHIM;

	if (m_pModelCom->Check_PickAnimRatio(ANIM_NACHIM, 0.95, dTimeDelta) && fDistance > 10.f)
	{
		m_eCurstate = STATE_AIRGUN;
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN);
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN2);
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN3);
	}
	if (m_pModelCom->Check_PickAnimRatio(ANIM_NACHIM, 0.95, dTimeDelta) && fDistance <= 10.f)
	{
		m_eCurstate = STATE_COMBO_PUNCH;
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN);
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN2);
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN3);
	}

}

void CBoss_Akaza::Update_ComboPunch(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_COMBO_PIST;

	if (m_pModelCom->Get_AnimFinish(ANIM_COMBO_PIST) == true)
		m_eCurAnimIndex = ANIM_COMBO_PIST2;


	if (m_pModelCom->Get_AnimFinish(ANIM_COMBO_PIST2) == true)
	{
		m_eCurAnimIndex = ANIM_IDEL;
		m_eCurstate = STATE_IDLE;
	}
}

void CBoss_Akaza::Update_JumpStomp(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_DashKick(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_JumpAirGun(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Dash_ComboPunch(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_UpperKick(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_NextPhase2(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_NextPhase3(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Heal(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Awake(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Nachim_ComboPunch(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Nachim_AirGun(_double dTimeDelta)
{

}

HRESULT CBoss_Akaza::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Akaza"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CBoss_Akaza_Akaza");
		return E_FAIL;
	}

	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CBoss_Akaza_Akaza");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CBoss_Akaza::SetUp_ShaderResources()
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
}

CBoss_Akaza* CBoss_Akaza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Akaza* pInstance = new CBoss_Akaza(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_Akaza");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Akaza::Clone(void* pArg)
{
	CBoss_Akaza* pInstance = new CBoss_Akaza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Akaza");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Akaza::Free()
{
	__super::Free();
}
