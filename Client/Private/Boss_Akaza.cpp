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

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
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
	m_eCurstate = STATE_IDLE;
	m_eCurPhase = PHASE_1;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(140.f, 0.f, 130.f, 1.f));

	

	return S_OK;

}

void CBoss_Akaza::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (true == m_isDead)
		return;

#ifdef _DEBUG
	Debug_State(dTimeDelta);
#endif // _DEBUG

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Update_TriggerTime(dTimeDelta);
	Update_Trigger(dTimeDelta);
	Update_State(dTimeDelta);

	m_pModelCom->Set_Animation(m_eCurAnimIndex);
	m_pModelCom->Play_Animation(dTimeDelta);

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

void CBoss_Akaza::Debug_State(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_F1))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(140.f, 0.f, 130.f, 1.f));
		m_iTriggerCnt = 0;
		m_iIdleCnt = 0;
		m_eCurstate = STATE_IDLE;
	}
	if (pGameInstance->Get_DIKeyState(DIK_SPACE))
	{
		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
			m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
			m_eCurstate = STATE_DASHPUNCH;
			m_pModelCom->Set_AnimisFinish(ANIM_CHARGE);
			m_pModelCom->Set_AnimisFinish(ANIM_CHARGE_ROOP);
			m_pModelCom->Set_AnimisFinish(ANIM_DASH);
			m_pModelCom->Set_AnimisFinish(ANIM_DASH_PUNCH);
			m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);
			m_bRunPunch = false;
			m_bStep_B = false;
			m_bDashOn = false;
			m_bCharge = false;

		}
		if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			m_eCurstate = STATE_AIRGUN;
			m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN);
			m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN2);
			m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN3);
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_AIRGUN);
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_AIRGUN2);
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_AIRGUN3);

		}
		if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
			m_eCurstate = STATE_DASHKICK;
			m_pModelCom->Set_AnimisFinish(ANIM_DASH);
			m_pModelCom->Set_AnimisFinish(ANIM_COMBO_DOWN);
		}
		if (pGameInstance->Get_DIKeyDown(DIK_4))
		{
			m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
			m_eCurstate = STATE_DASHKICK;
			m_pModelCom->Set_AnimisFinish(ANIM_DASH);
			m_pModelCom->Set_AnimisFinish(ANIM_COMBO_DOWN);
		}
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{
			m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
			m_eCurstate = STATE_JUMPAIRGUN;
			m_bJump = false;
			m_pModelCom->Set_AnimisFinish(ANIM_JUMPAIRGUN);
			m_pModelCom->Set_AnimisFinish(ANIM_JUMPAIRGUN2);
			m_pModelCom->Set_AnimisFinish(ANIM_JUMPAIRGUN3);
			m_pModelCom->Set_AnimisFinish(ANIM_JUMPROOP);
			m_pModelCom->Set_AnimisFinish(ANIM_JUMPLAND);

		}
		if (pGameInstance->Get_DIKeyDown(DIK_6))
		{
			m_eCurstate = STATE_JUMPSTOMP;
			m_pModelCom->Set_AnimisFinish(ANIM_SKILL_UP);
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_SKILL_UP);
			m_dJumpStompTime = 0.0;
			m_dLandStompTime = 0.0;
		}
		if (pGameInstance->Get_DIKeyDown(DIK_7))
		{
			m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
			m_eCurstate = STATE_UPPERKICK;
			m_pModelCom->Set_AnimisFinish(ANIM_DASH);
			m_pModelCom->Set_AnimisFinish(ANIM_COMBO_UP);
		}
		if (pGameInstance->Get_DIKeyDown(DIK_8))
		{
			m_eCurstate = STATE_AWAKE_COMBOPUNCH;

			m_bDashOn = false;
			m_pModelCom->Set_AnimisFinish(ANIM_DASH);
			m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN2);
			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH);
			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_ROOP);
			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_END);
		}
		if (pGameInstance->Get_DIKeyDown(DIK_0))
		{
			m_eCurstate = STATE_NACHIM;
			m_pModelCom->Set_AnimisFinish(ANIM_NACHIM);
		}
		if (pGameInstance->Get_DIKeyDown(DIK_Q))
		{
			m_eCurstate = STATE_PUSHAWAY;
			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
		}
		if (pGameInstance->Get_DIKeyDown(DIK_W))
		{
			m_eCurstate = STATE_HEAL;
			m_pModelCom->Set_AnimisFinish(ANIM_HEAL);

		}
		if (pGameInstance->Get_DIKeyDown(DIK_E))
		{
			m_eCurstate = STATE_AWAKE;
			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_START);

		}
		if (pGameInstance->Get_DIKeyDown(DIK_R))
		{
			m_eCurstate = STATE_ESCAPE;
			m_bStep_B = false;
			m_iRandomDirNum = Random::Generate_Int(1, 2);
			m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);
			m_pModelCom->Set_AnimisFinish(ANIM_STEP_LEFT);
			m_pModelCom->Set_AnimisFinish(ANIM_STEP_LEFT2);
			m_pModelCom->Set_AnimisFinish(ANIM_STEP_RIGHT);
			m_pModelCom->Set_AnimisFinish(ANIM_STEP_RIGHT2);

		}
		if (pGameInstance->Get_DIKeyDown(DIK_T))
		{
			m_eCurstate = STATE_NEXTPHASE2;

			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
			m_pModelCom->Set_AnimisFinish(ANIM_HEAL);

		}
		if (pGameInstance->Get_DIKeyDown(DIK_Y))
		{
			m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
			m_eCurstate = STATE_NEXTPHASE3;
			m_bAnimFinish = false;
			m_bStep_B = false;
			m_bHeal = false;

			m_iPunchCount = 0;
			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
			m_pModelCom->Set_AnimisFinish(ANIM_NACHIM);
			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH);
			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_Start);

			m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);
			m_pModelCom->Set_AnimisFinish(ANIM_HEAL);
			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_ROOP);
			m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_END);


		}
	}
	Safe_Release(pGameInstance);
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

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
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
	case CBoss_Akaza::STATE_AWAKE_COMBOPUNCH:
		Update_Awake_ComboPunch(dTimeDelta);
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

	if (m_eCurPhase == PHASE_1)
	{

		m_eCurAnimIndex = ANIM_IDEL;

		if (m_iHp > 50)
		{
			if (m_bPatternStart == false)
			{
				if (m_pModelCom->Check_PickAnimRatio(ANIM_IDEL, 0.30, dTimeDelta))
					m_iIdleCnt++;

				if (m_iIdleCnt == 1)
				{
					m_iIdleCnt = 0;
					m_bTrigger = false;
					m_iTriggerCnt++;
					if (m_iTriggerCnt >= 5)
					{
						m_iTriggerCnt = 0;
						m_bPatternStart = true;
					}
				}
			}
			if (m_bPatternStart == true)
			{
				_float fDistance = Calculate_Distance();
				m_dTriggerTime += dTimeDelta;
				if(1.0 < m_dTriggerTime && m_dTriggerTime <= 1.00 + dTimeDelta)
					m_iIdleCnt++;

				/*if (m_pModelCom->Check_PickAnimRatio(ANIM_IDEL, 0.90, dTimeDelta))
					m_iIdleCnt++;*/

				if (m_iIdleCnt == 1)
				{
					m_dTriggerTime = 0.0;
					m_iIdleCnt = 0;
					m_iRandomPatternNum = Random::Generate_Int(1, 10);


					if (fDistance <= 5.f)
					{
						if (1 <= m_iRandomPatternNum && m_iRandomPatternNum > 7)
						{
							m_iTriggerCnt = 4;
							m_bTrigger = false;
						}
						if (7 <= m_iRandomPatternNum && m_iRandomPatternNum > 10)
						{
							m_iTriggerCnt = 2;
							m_bTrigger = false;
						}
					}
					if (fDistance > 5.f)
					{
						if (1 <= m_iRandomPatternNum && m_iRandomPatternNum < 7)
						{
							m_iTriggerCnt = 2;
							m_bTrigger = false;
						}
						if (7 <= m_iRandomPatternNum && m_iRandomPatternNum <= 10)
						{
							m_iTriggerCnt = 3;
							m_bTrigger = false;
						}
					}
				}
			}
		}
		/*if (m_iHp <= 50)
		{

		}*/

	}


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
	if (m_bTrigger == false)
	{
		if (m_iHp > 50)
		{

			switch (m_iTriggerCnt)
			{
			case 1:
				Trigger_Idle();
				break;
			case 2:
				Trigger_DashPunch();
				break;
			case 3:
				Trigger_AirGun();
				break;

			case 4:
				Trigger_Escape();
				break;
			}


		}
		if (m_iHp <= 50)
		{

		}
		if (m_iHp <= 0)
		{
			Trigger_NextPhase2();
		}
	}

}

void CBoss_Akaza::Update_Phase_2(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Phase_3(_double dTimeDelta)
{
}

void CBoss_Akaza::Trigger_Idle()
{
	m_bTrigger = true;
	m_eCurstate = STATE_IDLE;
}

void CBoss_Akaza::Trigger_Begin()
{

	m_eCurstate = STATE_BEGIN;
}

void CBoss_Akaza::Trigger_Escape()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ESCAPE;
	m_bStep_B = false;
	m_iRandomDirNum = Random::Generate_Int(1, 2);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_LEFT);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_LEFT2);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_RIGHT);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_RIGHT2);
}

void CBoss_Akaza::Trigger_DashPunch()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_DASHPUNCH;
	m_pModelCom->Set_AnimisFinish(ANIM_CHARGE);
	m_pModelCom->Set_AnimisFinish(ANIM_CHARGE_ROOP);
	m_pModelCom->Set_AnimisFinish(ANIM_DASH);
	m_pModelCom->Set_AnimisFinish(ANIM_DASH_PUNCH);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);
	m_bRunPunch = false;
	m_bStep_B = false;
	m_bDashOn = false;
	m_bCharge = false;
}

void CBoss_Akaza::Trigger_Guard()
{
	m_bTrigger = true;
}

void CBoss_Akaza::Trigger_AirGun()
{
	m_bTrigger = true;
	m_eCurstate = STATE_AIRGUN;
	m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN);
	m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN2);
	m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN3);
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_AIRGUN);
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_AIRGUN2);
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_AIRGUN3);

}

void CBoss_Akaza::Trigger_PushAway()
{
	m_bTrigger = true;
	m_eCurstate = STATE_PUSHAWAY;
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
}

void CBoss_Akaza::Trigger_Nachim()
{
	m_bTrigger = true;
	m_eCurstate = STATE_NACHIM;
	m_pModelCom->Set_AnimisFinish(ANIM_NACHIM);
}

void CBoss_Akaza::Trigger_ComboPunch()
{
	m_bTrigger = true;
}

void CBoss_Akaza::Trigger_JumpStomp()
{
	m_bTrigger = true;
	m_eCurstate = STATE_JUMPSTOMP;
	m_pModelCom->Set_AnimisFinish(ANIM_SKILL_UP);
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_SKILL_UP);
	m_dJumpStompTime = 0.0;
	m_dLandStompTime = 0.0;
}

void CBoss_Akaza::Trigger_DashKick()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_DASHKICK;
	m_pModelCom->Set_AnimisFinish(ANIM_DASH);
	m_pModelCom->Set_AnimisFinish(ANIM_COMBO_DOWN);
}

void CBoss_Akaza::Trigger_JumpAirGun()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_JUMPAIRGUN;
	m_bJump = false;
	m_pModelCom->Set_AnimisFinish(ANIM_JUMPAIRGUN);
	m_pModelCom->Set_AnimisFinish(ANIM_JUMPAIRGUN2);
	m_pModelCom->Set_AnimisFinish(ANIM_JUMPAIRGUN3);
	m_pModelCom->Set_AnimisFinish(ANIM_JUMPROOP);
	m_pModelCom->Set_AnimisFinish(ANIM_JUMPLAND);
}

void CBoss_Akaza::Trigger_Dash_ComboPunch()
{
	m_bTrigger = true;
}

void CBoss_Akaza::Trigger_UpperKick()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_UPPERKICK;
	m_pModelCom->Set_AnimisFinish(ANIM_DASH);
	m_pModelCom->Set_AnimisFinish(ANIM_COMBO_UP);
}

void CBoss_Akaza::Trigger_NextPhase2()
{
	m_bTrigger = true;
	m_eCurstate = STATE_NEXTPHASE2;

	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
	m_pModelCom->Set_AnimisFinish(ANIM_HEAL);
}

void CBoss_Akaza::Trigger_NextPhase3()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_NEXTPHASE3;
	m_bAnimFinish = false;
	m_bStep_B = false;
	m_bHeal = false;

	m_iPunchCount = 0;
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
	m_pModelCom->Set_AnimisFinish(ANIM_NACHIM);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_Start);

	m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);
	m_pModelCom->Set_AnimisFinish(ANIM_HEAL);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_ROOP);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_END);
}

void CBoss_Akaza::Trigger_Heal()
{
	m_bTrigger = true;
	m_eCurstate = STATE_HEAL;
	m_pModelCom->Set_AnimisFinish(ANIM_HEAL);
}

void CBoss_Akaza::Trigger_Awake()
{
	m_bTrigger = true;
	m_eCurstate = STATE_AWAKE;
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_START);
}

void CBoss_Akaza::Trigger_Awake_ComboPunch()
{
	m_bTrigger = true;
	m_eCurstate = STATE_AWAKE_COMBOPUNCH;

	m_bDashOn = false;
	m_pModelCom->Set_AnimisFinish(ANIM_DASH);
	m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN2);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_ROOP);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_END);
}

void CBoss_Akaza::Trigger_Nachim_ComboPunch()
{
}

void CBoss_Akaza::Trigger_Nachim_AirGun()
{
}

void CBoss_Akaza::Update_Escape(_double dTimeDelta)
{

	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

	if (m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND) == false)
		m_eCurAnimIndex = ANIM_STEP_BEHIND;

	if (m_iRandomDirNum == 1)
	{

		if (m_bStep_B == false && m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND))
		{
			m_bStep_B = true;
			m_eCurAnimIndex = ANIM_STEP_LEFT;
		}

		if (m_pModelCom->Check_PickAnimRatio(ANIM_STEP_LEFT, 0.950, dTimeDelta))
			m_eCurAnimIndex = ANIM_STEP_LEFT2;
	}
	if (m_iRandomDirNum == 2)
	{

		if (m_bStep_B == false && m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND))
		{
			m_bStep_B = true;
			m_eCurAnimIndex = ANIM_STEP_RIGHT;
		}

		if (m_pModelCom->Check_PickAnimRatio(ANIM_STEP_RIGHT, 0.950, dTimeDelta))
			m_eCurAnimIndex = ANIM_STEP_RIGHT2;
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_STEP_LEFT2) || m_pModelCom->Get_AnimFinish(ANIM_STEP_RIGHT2))
	{
		m_eCurAnimIndex = ANIM_IDEL;
		m_eCurstate = STATE_IDLE;
	}

	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_STEP_BEHIND, 3.0f, 0.0, 0.7);
	Go_Dir_Constant(dTimeDelta, DIR_LEFT, ANIM_STEP_LEFT, 2.0f, 0.0, 0.7);
	Go_Dir_Constant(dTimeDelta, DIR_LEFT, ANIM_STEP_LEFT2, 2.0f, 0.0, 0.7);
	Go_Dir_Constant(dTimeDelta, DIR_RIGHT, ANIM_STEP_RIGHT, 2.0f, 0.0, 0.7);
	Go_Dir_Constant(dTimeDelta, DIR_RIGHT, ANIM_STEP_RIGHT2, 2.0f, 0.0, 0.7);

}

void CBoss_Akaza::Update_DashPunch(_double dTimeDelta)
{


	_vector vDir = Calculate_Dir();
	_float fDistance = Calculate_Distance();

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
		m_eCurAnimIndex = ANIM_STEP_BEHIND;

		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_STEP_BEHIND, 3.0f, 0.0, 0.7);
		if (m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND) == true)
		{
			//m_pModelCom->Set_AnimisFinish(ANIM_CHARGE);
			//m_eCurAnimIndex = ANIM_CHARGE;
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
		if (m_pModelCom->Get_AnimFinish(ANIM_CHARGE) == true && m_bCharge == false || m_bStep_B == true && m_bCharge == false)
		{
			m_bCharge = true;
			m_eCurAnimIndex = ANIM_CHARGE_ROOP;
		}

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

	//if (m_pModelCom->Get_AnimFinish(ANIM_AIRGUN) == true)
	if (m_pModelCom->Check_PickAnimRatio(ANIM_AIRGUN,0.950,dTimeDelta))
	{
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

	_float fDistance = Calculate_Distance();

	m_eCurAnimIndex = ANIM_NACHIM;

	if (m_pModelCom->Check_PickAnimRatio(ANIM_NACHIM, 0.95, dTimeDelta) && fDistance > 10.f)
	{
		m_eCurstate = STATE_AIRGUN;
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN);
		m_pModelCom->Set_AnimisFinish(ANIM_AIRGUN3);
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_AIRGUN);
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_AIRGUN3);
	}
	if (m_pModelCom->Check_PickAnimRatio(ANIM_NACHIM, 0.95, dTimeDelta) && fDistance <= 10.f)
	{
		m_eCurstate = STATE_COMBO_PUNCH;
		m_pModelCom->Set_AnimisFinish(ANIM_COMBO_PIST);
		m_pModelCom->Set_AnimisFinish(ANIM_COMBO_PIST2);

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

	m_eCurAnimIndex = ANIM_SKILL_UP;

	if (m_pModelCom->Check_PickAnimRatio(ANIM_SKILL_UP, 0.65f, dTimeDelta))
	{
		Jumping(12.0f, 0.1f);

	}
	if (m_pModelCom->Get_AnimFinish(ANIM_SKILL_UP) == true)
	{
		m_eCurAnimIndex = ANIM_SKILL_DOWN;
		m_dJumpStompTime += dTimeDelta;

		if (0.10 < m_dJumpStompTime && m_dJumpStompTime <= 0.10 + dTimeDelta)
			JumpStop(3.0);
		if (m_dJumpStompTime <= 3.0)
		{
			m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pTransformCom->Go_Straight(dTimeDelta * 1.0);
		}

		if (m_dJumpStompTime > 3.10)
			m_pTransformCom->Go_Down(dTimeDelta * 15.0);

		// 땅에 떨어졌을 때 임시코드
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (Pos.y <= 0.f)
		{
			m_dLandStompTime += dTimeDelta;
			Pos.y = 0.0f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));
			if (0.70 < m_dLandStompTime && m_dLandStompTime <= 0.70 + dTimeDelta)
			{
				m_dJumpStompTime = 0.0;
				m_eCurAnimIndex = ANIM_IDEL;
				m_eCurstate = STATE_IDLE;
			}
		}
	}




}

void CBoss_Akaza::Update_DashKick(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimFinish(ANIM_DASH) == false)
	{
		m_eCurAnimIndex = ANIM_DASH;

	}

	if (m_pModelCom->Get_AnimFinish(ANIM_DASH) == true)
		m_eCurAnimIndex = ANIM_COMBO_DOWN;

	if (m_pModelCom->Get_AnimFinish(ANIM_COMBO_DOWN) == true)
	{
		m_eCurAnimIndex = ANIM_IDEL;
		m_eCurstate = STATE_IDLE;
	}
	Go_Straight_Constant(dTimeDelta, ANIM_DASH, 10.f);
	Go_Straight_Deceleration(dTimeDelta, ANIM_COMBO_DOWN, 1.f, 0.2f);


}

void CBoss_Akaza::Update_JumpAirGun(_double dTimeDelta)
{
	if (m_bJump == false)
	{
		m_bJump = true;
		m_eCurAnimIndex = ANIM_JUMPAIRGUN;
		Jumping(5.0f, 0.2f);
	}
	if (m_pModelCom->Check_PickAnimRatio(ANIM_JUMPAIRGUN, 0.25, dTimeDelta))
		JumpStop(2.0);

	if (m_pModelCom->Check_PickAnimRatio(ANIM_JUMPAIRGUN, 0.95, dTimeDelta))
		m_eCurAnimIndex = ANIM_JUMPAIRGUN2;


	if (m_pModelCom->Check_PickAnimRatio(ANIM_JUMPAIRGUN2, 0.95, dTimeDelta))
		m_eCurAnimIndex = ANIM_JUMPAIRGUN3;

	if (m_pModelCom->Get_AnimFinish(ANIM_JUMPAIRGUN3))
	{
		m_eCurAnimIndex = ANIM_JUMPROOP;
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (Pos.y <= 0.f)
		{
			m_eCurAnimIndex = ANIM_JUMPLAND;
			Pos.y = 0.0f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));
		}

		if (m_pModelCom->Get_AnimFinish(ANIM_JUMPLAND))
		{
			m_eCurAnimIndex = ANIM_IDEL;
			m_eCurstate = STATE_IDLE;
		}
	}
}

void CBoss_Akaza::Update_Dash_ComboPunch(_double dTimeDelta)
{

}

void CBoss_Akaza::Update_UpperKick(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimFinish(ANIM_DASH) == false)
	{
		m_eCurAnimIndex = ANIM_DASH;

	}

	if (m_pModelCom->Get_AnimFinish(ANIM_DASH) == true)
		m_eCurAnimIndex = ANIM_COMBO_UP;

	if (m_pModelCom->Get_AnimFinish(ANIM_COMBO_UP) == true)
	{
		m_eCurAnimIndex = ANIM_IDEL;
		m_eCurstate = STATE_IDLE;
	}

	Go_Straight_Constant(dTimeDelta, ANIM_DASH, 10.f);
	Go_Straight_Deceleration(dTimeDelta, ANIM_COMBO_DOWN, 1.f, 0.2f);

}

void CBoss_Akaza::Update_NextPhase2(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_PUSHAWAY) == false)
		m_eCurAnimIndex = ANIM_AWAKE_PUSHAWAY;

	if (m_pModelCom->Check_PickAnimRatio(ANIM_AWAKE_PUSHAWAY, 0.95, dTimeDelta))
		m_eCurstate = STATE_HEAL;

	if (m_pModelCom->Get_AnimFinish(STATE_HEAL) == true)
		m_eCurPhase = PHASE_2;
}

void CBoss_Akaza::Update_NextPhase3(_double dTimeDelta)
{
	//회천 // 나침 // 순간이동 // awake펀치 // 힐
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_PUSHAWAY) == false)
		m_eCurAnimIndex = ANIM_AWAKE_PUSHAWAY;

	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_PUSHAWAY) == true &&
		m_bAnimFinish == false)
	{
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_NACHIM);
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_NACHIM;
	}

	if (m_pModelCom->Check_PickAnimRatio(ANIM_NACHIM, 0.95, dTimeDelta) &&
		m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_Start) == false)
	{

		m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vDir = Calculate_Dir();
		_float fDistance = Calculate_Distance() - 2.f;
		_vector vChangePos = vMonsterPos + (vDir * fDistance);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vChangePos);
		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_Start;

	}

	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_Start) == true &&
		m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH) == false)
		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH;

	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH) == true)
	{

		if (m_iPunchCount < 3)
			m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_ROOP;

		if (m_pModelCom->Check_PickAnimRatio(ANIM_AWAKE_COMBOPUNCH_ROOP, 0.90, dTimeDelta))
			m_iPunchCount++;

		if (m_iPunchCount >= 3)
		{
			if (m_bStep_B == false)
			{
				m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_END;
			}
			if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_END) == true &&
				m_bStep_B == false)
			{
				m_bStep_B = true;

			}

			if (m_bStep_B == true && m_bHeal == false)
			{
				m_bHeal = true;

				if (m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND) == false)
					m_eCurAnimIndex = ANIM_STEP_BEHIND;

			}
			if (m_pModelCom->Check_PickAnimRatio(ANIM_STEP_BEHIND, 0.90, dTimeDelta))
			{
				m_eCurAnimIndex = ANIM_HEAL;
			}

			if (m_pModelCom->Get_AnimFinish(ANIM_HEAL) == true)
			{
				m_eCurPhase = PHASE_3;
				m_eCurAnimIndex = ANIM_IDEL;
				m_eCurstate = STATE_IDLE;
			}

		}
	}

	if (m_pModelCom->Get_AnimRatio(ANIM_AWAKE_COMBOPUNCH_END, 0.30))
		Go_Straight_Constant(dTimeDelta, ANIM_AWAKE_COMBOPUNCH_END, 1.f);

	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_STEP_BEHIND, 3.0f, 0.0, 0.70);


}

void CBoss_Akaza::Update_Heal(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_HEAL;
	if (m_pModelCom->Get_AnimFinish(ANIM_HEAL))
	{
		m_eCurAnimIndex = ANIM_IDEL;
		m_eCurstate = STATE_IDLE;
	}
}

void CBoss_Akaza::Update_Awake(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_AWAKE_START;
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_START))
	{
		m_eCurAnimIndex = ANIM_IDEL;
		m_eCurstate = STATE_IDLE;
	}
}

void CBoss_Akaza::Update_Awake_ComboPunch(_double dTimeDelta)
{
	if (m_bDashOn == false)
	{
		m_bDashOn = true;
		m_eCurAnimIndex = ANIM_DASH;
	}


	if (m_pModelCom->Check_PickAnimRatio(ANIM_DASH, 0.950, dTimeDelta))
		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH;



	if (m_pModelCom->Check_PickAnimRatio(ANIM_AWAKE_COMBOPUNCH, 0.950, dTimeDelta))
		m_eCurAnimIndex = ANIM_AIRGUN2;


	if (m_pModelCom->Check_PickAnimRatio(ANIM_AIRGUN2, 0.950, dTimeDelta))
		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_ROOP;

	if (m_pModelCom->Check_PickAnimRatio(ANIM_AWAKE_COMBOPUNCH_ROOP, 0.950, dTimeDelta))
		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_END;

	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_END) == true)
	{
		m_eCurAnimIndex = ANIM_IDEL;
		m_eCurstate = STATE_IDLE;
	}

	Go_Straight_Constant(dTimeDelta, ANIM_DASH, 10.f);

	if (m_pModelCom->Get_AnimRatio(ANIM_AWAKE_COMBOPUNCH_END, 0.30))
		Go_Straight_Constant(dTimeDelta, ANIM_AWAKE_COMBOPUNCH_END, 1.f);

}

void CBoss_Akaza::Update_Nachim_ComboPunch(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Nachim_AirGun(_double dTimeDelta)
{

}

_float CBoss_Akaza::Calculate_Distance()
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);
	_float fDistance = Convert::GetLength(vPlayerPos - vMonsterPos);

	return fDistance;
}

_vector CBoss_Akaza::Calculate_Dir()
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);

	return vDir;
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


	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CBoss_Akaza_Akaza");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(1.f, 1.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CBoss_Akaza_Akaza");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CBoss_Akaza_Akaza");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.f, 1.f, 1.f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CBoss_Akaza_Akaza");
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
