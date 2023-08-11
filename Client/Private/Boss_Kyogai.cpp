#include "pch.h"
#include "Boss_Kyogai.h"
#include "GameInstance.h"
#include "SoundMgr.h"
#include "RotationMapObject.h"
#include "AtkCollManager.h"

CBoss_Kyogai::CBoss_Kyogai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CBoss_Kyogai::CBoss_Kyogai(const CBoss_Kyogai& rhs)
	: CMonster(rhs)
{
}

HRESULT CBoss_Kyogai::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Kyogai::Initialize(void* pArg)
{

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Read_Animation_Control_File("Kyogai.bin")))
	{
		MSG_BOX("Failed to AnimData Read : Kyogai");
		return E_FAIL;
	}

	Get_PlayerComponent();
	
	m_eCurAnimIndex = ANIM_IDEL;
	m_eCurstate = STATE_IDLE;
	m_eCurPhase = PHASE_1;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(140.f, 0.f, 130.f, 1.f));
	//m_eCurNavi = NAVI_ACAZA;

	return S_OK;

}

void CBoss_Kyogai::Tick(_double dTimeDelta)
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
	m_pModelCom->Play_Animation_For_Boss(dTimeDelta);

	Safe_Release(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this)))
		return;

}

void CBoss_Kyogai::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);
	Update_AnimIndex(m_eCurAnimIndex);
	Gravity(dTimeDelta);
}

HRESULT CBoss_Kyogai::Render()
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

#ifdef _DEBUG

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_tchar	m_szFPS[MAX_PATH] = TEXT("");
	_sntprintf_s(m_szFPS, MAX_PATH, TEXT("HP : %.2f"), m_StatusDesc.fHp);
	//wsprintf(m_szFPS, TEXT("HP : %.2f"), m_StatusDesc.fHp);
	if (FAILED(pGameInstance->Draw_Font(TEXT("Font_Default"), m_szFPS, _float2(640.f, 0.f), _float2(0.5f, 0.5f))))
		return E_FAIL;

	Safe_Release(pGameInstance);
#endif // DEBUG

	return S_OK;
}

HRESULT CBoss_Kyogai::Render_ShadowDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

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
#ifdef _DEBUG
void CBoss_Kyogai::Debug_State(_double dTimeDelta)
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
	if (pGameInstance->Get_DIKeyState(DIK_F2))
	{
		Camera_Shake();

	}
	if (pGameInstance->Get_DIKeyState(DIK_SPACE))
	{
		if (pGameInstance->Get_DIKeyDown(DIK_DELETE))
		{
			m_StatusDesc.fHp -= 20.f;
		}
		if (pGameInstance->Get_DIKeyDown(DIK_INSERT))
		{
			m_StatusDesc.fHp += 20.f;
		}

		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
		

		}
		if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			

		}
		if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			
		}
		if (pGameInstance->Get_DIKeyDown(DIK_4))
		{
			
		}
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{
			

		}
		if (pGameInstance->Get_DIKeyDown(DIK_6))
		{
			

		}
		if (pGameInstance->Get_DIKeyDown(DIK_7))
		{
			
		}
		if (pGameInstance->Get_DIKeyDown(DIK_8))
		{
			
		}
		if (pGameInstance->Get_DIKeyDown(DIK_0))
		{
			
		}
		if (pGameInstance->Get_DIKeyDown(DIK_Q))
		{
			
		}
		if (pGameInstance->Get_DIKeyDown(DIK_W))
		{
			

		}
		if (pGameInstance->Get_DIKeyDown(DIK_E))
		{
			

		}
		if (pGameInstance->Get_DIKeyDown(DIK_R))
		{
			

		}
		if (pGameInstance->Get_DIKeyDown(DIK_T))
		{
			
		}
		if (pGameInstance->Get_DIKeyDown(DIK_Y))
		{
			

		}
		if (pGameInstance->Get_DIKeyDown(DIK_U))
		{
			
		}
		if (pGameInstance->Get_DIKeyDown(DIK_I))
		{
			
		}
		if (pGameInstance->Get_DIKeyDown(DIK_O))
		{
			

		}


	}
	Safe_Release(pGameInstance);
}
#endif //_DEBUG

void CBoss_Kyogai::Update_AnimIndex(_uint iAnimIndex)
{
	if (m_eCurAnimIndex != m_ePreAnimIndex)
	{
		m_ePreAnimIndex = m_eCurAnimIndex;
	}
}

void CBoss_Kyogai::Update_Trigger(_double dTimeDelta)
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	switch (m_eCurPhase)
	{
	case CBoss_Kyogai::BEGIN:
		Update_Begin(dTimeDelta);
		break;
	case CBoss_Kyogai::PHASE_1:
		Update_Phase_1(dTimeDelta);
		break;
	case CBoss_Kyogai::PHASE_2:
		Update_Phase_2(dTimeDelta);
		break;
	
	}

}

void CBoss_Kyogai::Update_TriggerTime(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_State(_double dTimeDelta)
{

	switch (m_eCurstate)
	{
	case CBoss_Kyogai::STATE_IDLE:
		Update_Interact(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_BEGIN:
		Update_Begin(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_GUARD:
		Update_Guard(dTimeDelta);
		break;
	
	case CBoss_Kyogai::STATE_NEXTPHASE2:
		Update_NextPhase2(dTimeDelta);
		break;
	
	case CBoss_Kyogai::STATE_HEAL:
		Update_Heal(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_AWAKE:
		Update_Awake(dTimeDelta);
		break;
	

	}

}

void CBoss_Kyogai::Update_Interact(_double dTimeDelta)
{

	if (m_eCurPhase == PHASE_1)
	{
		Trigger_Interact_Phase_1(dTimeDelta);
	}
	if (m_eCurPhase == PHASE_2)
	{
		Trigger_Interact_Phase_2(dTimeDelta);
	}

}

void CBoss_Kyogai::Update_Begin(_double dTimeDelta)
{

}

void CBoss_Kyogai::Update_Phase_1(_double dTimeDelta)
{
	if (m_bTrigger == false)
	{
		if (m_StatusDesc.fHp > 50.f)
		{

			switch (m_iTriggerCnt)
			{
			case 1:
				Trigger_Interact();
				break;
			case 2:
				Trigger_DashPunch();
				break;
			case 3:
				Trigger_AirGun();
				break;
			case 4:
				Trigger_Dash_ComboPunch();
				break;
			case 5:
				Trigger_Escape();
				break;
			}


		}
		if (m_StatusDesc.fHp <= 50.f)
		{
			switch (m_iTriggerCnt)
			{
			case 0:
				Trigger_Nachim();
				break;
			case 1:
				Trigger_DashPunch();
				break;
			case 2:
				Trigger_AirGun();
				break;
			case 3:
				Trigger_Dash_ComboPunch();
				break;
			case 4:
				Trigger_Escape();
				break;
			case 5:
				Trigger_NextPhase3();
				break;
			}
		}

	}

}

void CBoss_Kyogai::Update_Phase_2(_double dTimeDelta)
{
	// 개방하고 25초 유지 m_bAwake가 활성되면 25초 후에 다시 false
	if (m_StatusDesc.fHp < 70.f && m_bFirstAwake == false)
	{
		m_bFirstAwake = true;
		m_bAwake = true;
		m_bTrigger = false;
		m_iTriggerCnt = 0;
		m_dAwakeTime = 0.0;

		m_bPatternStart = false;
		m_dTriggerTime = 0.0;
		m_iIdleCnt = 0;
	}
	if (m_StatusDesc.fHp < 30.f && m_bSecondAwake == false)
	{
		m_bSecondAwake = true;
		m_bAwake = true;
		m_bTrigger = false;
		m_iTriggerCnt = 0;

		m_dAwakeTime = 0.0;

		m_bPatternStart = false;
		m_dTriggerTime = 0.0;
		m_iIdleCnt = 0;
	}
	if (m_bAwake == true)
	{
		m_dAwakeTime += dTimeDelta;
		if(24.0 < m_dAwakeTime && m_dAwakeTime <= 24.0 + dTimeDelta)
			m_pRendererCom->Set_Invert();
		if (m_dAwakeTime > 25.0 )
		{
			m_pRendererCom->Set_Invert();
			m_bAwake = false;
			
		}
	}

	if (m_bTrigger == false)
	{
		if (m_bAwake == false)
		{
			switch (m_iTriggerCnt)
			{
			case 1:
				Trigger_JumpAirGun();
				break;
			case 2:
				Trigger_AirGun();
				break;
			case 3:
				Trigger_DashKick();
				break;
			case 4:
				Trigger_JumpStomp();
				break;
			case 5:
				Trigger_Dash_ComboPunch();
				break;
			case 6:
				Trigger_UpperKick();
				break;
			case 7:
				Trigger_Awake_ComboPunch();
				break;
			case 8:
				Trigger_Escape();
				break;

			}
		}
		if (m_bAwake == true)
		{

			switch (m_iTriggerCnt)
			{
			case 0:
				Trigger_Awake();
				break;
			case 1:
				Trigger_Nachim();
				break;
			case 2:
				Trigger_Awake_ComboPunch();
				break;
			case 3:
				Trigger_AirGun();
				break;
			case 4:
				Trigger_JumpStomp();
				break;
			case 5:
				Trigger_JumpAirGun();
				break;
			case 6:
				Trigger_NextPhase3();
				break;

			}
		}

	}
}

void CBoss_Kyogai::Update_Phase_3(_double dTimeDelta)
{
}

void CBoss_Kyogai::Trigger_Interact_Phase_1(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_IDEL;

	if (m_StatusDesc.fHp > 50.f)
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
				if (m_iTriggerCnt >= 6)
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
			if (1.0 < m_dTriggerTime && m_dTriggerTime <= 1.00 + dTimeDelta)
				m_iIdleCnt++;

			if (m_iIdleCnt == 1)
			{
				m_dTriggerTime = 0.0;
				m_iIdleCnt = 0;
				m_iRandomPatternNum = Random::Generate_Int(1, 10);


				if (fDistance <= 5.f)
				{
					if (1 <= m_iRandomPatternNum && m_iRandomPatternNum > 7)
					{
						m_iTriggerCnt = 5;
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
					if (1 <= m_iRandomPatternNum && m_iRandomPatternNum < 5)
					{
						m_iTriggerCnt = 2;
						m_bTrigger = false;
					}
					if (5 <= m_iRandomPatternNum && m_iRandomPatternNum < 8)
					{
						m_iTriggerCnt = 3;
						m_bTrigger = false;
					}
					if (8 <= m_iRandomPatternNum && m_iRandomPatternNum <= 10)
					{
						m_iTriggerCnt = 4;
						m_bTrigger = false;
					}
				}
			}
		}
	}
	if (m_StatusDesc.fHp <= 50.f && m_StatusDesc.fHp > 0.f)
	{
		if (m_bPatternStart == true)
		{
			if (m_pModelCom->Check_PickAnimRatio(ANIM_IDEL, 0.30, dTimeDelta))
				m_iIdleCnt++;

			if (m_iIdleCnt == 1)
			{
				m_dTriggerTime = 0.0;
				m_iIdleCnt = 0;
				m_bTrigger = false;
				m_iTriggerCnt = 0;
				m_bPatternStart = false;

			}
		}
		if (m_bPatternStart == false)
		{
			_float fDistance = Calculate_Distance();
			m_dTriggerTime += dTimeDelta;
			if (1.0 < m_dTriggerTime && m_dTriggerTime <= 1.00 + dTimeDelta)
				m_iIdleCnt++;

			if (m_iIdleCnt == 1)
			{
				m_dTriggerTime = 0.0;
				m_iIdleCnt = 0;
				m_iRandomPatternNum = Random::Generate_Int(1, 12);


				if (fDistance <= 5.f)
				{
					if (1 <= m_iRandomPatternNum && m_iRandomPatternNum < 7)
					{
						m_iTriggerCnt = 4;
						m_bTrigger = false;
					}
					if (7 <= m_iRandomPatternNum && m_iRandomPatternNum < 10)
					{
						m_iTriggerCnt = 1;
						m_bTrigger = false;
					}
					if (10 <= m_iRandomPatternNum && m_iRandomPatternNum < 12)
					{
						m_iTriggerCnt = 3;
						m_bTrigger = false;
					}
				}
				if (fDistance > 5.f)
				{
					if (1 <= m_iRandomPatternNum && m_iRandomPatternNum < 4)
					{
						m_iTriggerCnt = 1;
						m_bTrigger = false;
					}
					if (4 <= m_iRandomPatternNum && m_iRandomPatternNum < 7)
					{
						m_iTriggerCnt = 2;
						m_bTrigger = false;
					}
					if (7 <= m_iRandomPatternNum && m_iRandomPatternNum < 10)
					{
						m_iTriggerCnt = 3;
						m_bTrigger = false;
					}
					if (10 <= m_iRandomPatternNum && m_iRandomPatternNum <= 12)
					{
						m_iTriggerCnt = 4;
						m_bTrigger = false;
					}
				}
			}
		}
	}
	if (m_StatusDesc.fHp <= 0.f)
	{
		m_bTrigger = false;
		m_bPatternStart = false;
		m_iTriggerCnt = 5;
		m_dTriggerTime = 0.0;
		m_iIdleCnt = 0;
	}
}

void CBoss_Kyogai::Trigger_Interact_Phase_2(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_IDEL;

	if (m_bAwake == false)
	{
		if (m_bPatternStart == false)
		{
			m_dTriggerTime += dTimeDelta;
			if (1.0 < m_dTriggerTime && m_dTriggerTime <= 1.00 + dTimeDelta)
				m_iIdleCnt++;

			if (m_iIdleCnt == 1)
			{
				
				m_dTriggerTime = 0.0;
				m_iIdleCnt = 0;
				m_bTrigger = false;
				m_iTriggerCnt++;
				if (m_iTriggerCnt >= 9)
				{
					m_iTriggerCnt = 1;
					m_dTriggerTime = 0.0;
					m_iIdleCnt = 0;
					m_bPatternStart = true;
				}
			}
		}
		if (m_bPatternStart == true)
		{
			_float fDistance = Calculate_Distance();
			m_dTriggerTime += dTimeDelta;
			if (1.0 < m_dTriggerTime && m_dTriggerTime <= 1.00 + dTimeDelta)
				m_iIdleCnt++;

			if (m_iIdleCnt == 1)
			{
				m_dTriggerTime = 0.0;
				m_iIdleCnt = 0;
				m_iRandomPatternNum = Random::Generate_Int(1, 16);

				if (fDistance <= 5.f)
				{
					if (1 <= m_iRandomPatternNum && m_iRandomPatternNum < 12)
					{
						m_iTriggerCnt = 8;
						m_bTrigger = false;
					}
					if (12 <= m_iRandomPatternNum && m_iRandomPatternNum <= 16)
					{
						m_iTriggerCnt = 2;
						m_bTrigger = false;
					}
				}
				if (fDistance > 5.f)
				{
					if (1 <= m_iRandomPatternNum && m_iRandomPatternNum < 3)
					{
						m_iTriggerCnt = 1;
						m_bTrigger = false;
					}
					if (3 <= m_iRandomPatternNum && m_iRandomPatternNum < 5)
					{
						m_iTriggerCnt = 2;
						m_bTrigger = false;
					}
					if (5 <= m_iRandomPatternNum && m_iRandomPatternNum < 7)
					{
						m_iTriggerCnt = 3;
						m_bTrigger = false;
					}
					if (7 <= m_iRandomPatternNum && m_iRandomPatternNum < 9)
					{
						m_iTriggerCnt = 4;
						m_bTrigger = false;
					}
					if (9 <= m_iRandomPatternNum && m_iRandomPatternNum < 11)
					{
						m_iTriggerCnt = 5;
						m_bTrigger = false;
					}
					if (11 <= m_iRandomPatternNum && m_iRandomPatternNum < 13)
					{
						m_iTriggerCnt = 6;
						m_bTrigger = false;
					}
					if (13 <= m_iRandomPatternNum && m_iRandomPatternNum < 15)
					{
						m_iTriggerCnt = 7;
						m_bTrigger = false;
					}
					if (15 <= m_iRandomPatternNum && m_iRandomPatternNum <= 16)
					{
						m_iTriggerCnt = 8;
						m_bTrigger = false;
					}
				}
			}
		}
	}
	if (m_bAwake == true)
	{
		if (m_bPatternStart == true)
		{
			m_bPatternStart = false;
			m_dTriggerTime = 0.0;
			m_iIdleCnt = 0;
			//m_bTrigger = false;

		}
		if (m_bPatternStart == false)
		{
			_float fDistance = Calculate_Distance();
			m_dTriggerTime += dTimeDelta;
			if (0.30 < m_dTriggerTime && m_dTriggerTime <= 0.30 + dTimeDelta)
				m_iIdleCnt++;

			if (m_iIdleCnt == 1)
			{
				m_dTriggerTime = 0.0;
				m_iIdleCnt = 0;
				m_iRandomPatternNum = Random::Generate_Int(1, 12);

				if (fDistance <= 3.f)
				{
					if (1 <= m_iRandomPatternNum && m_iRandomPatternNum > 9)
					{
						m_iTriggerCnt = 8;
						m_bTrigger = false;
					}
					if (9 <= m_iRandomPatternNum && m_iRandomPatternNum >= 12)
					{
						m_iTriggerCnt = 4;
						m_bTrigger = false;
					}
				}
				if (fDistance > 3.f)
				{
					if (1 <= m_iRandomPatternNum && m_iRandomPatternNum < 3)
					{
						m_iTriggerCnt = 1;
						m_bTrigger = false;
					}
					if (3 <= m_iRandomPatternNum && m_iRandomPatternNum < 4)
					{
						m_iTriggerCnt = 2;
						m_bTrigger = false;
					}
					if (4 <= m_iRandomPatternNum && m_iRandomPatternNum < 7)
					{
						m_iTriggerCnt = 3;
						m_bTrigger = false;
					}
					if (7 <= m_iRandomPatternNum && m_iRandomPatternNum < 10)
					{
						m_iTriggerCnt = 4;
						m_bTrigger = false;
					}
					if (10 <= m_iRandomPatternNum && m_iRandomPatternNum <= 12)
					{
						m_iTriggerCnt = 5;
						m_bTrigger = false;
					}
				}
			}
		}
	}
	if (m_StatusDesc.fHp <= 0.f)
	{
		m_bTrigger = false;
		m_bPatternStart = false;
		m_iTriggerCnt = 5;
		m_dTriggerTime = 0.0;
		m_iIdleCnt = 0;
	}
}


void CBoss_Kyogai::Trigger_Interact()
{
	m_bTrigger = true;
	m_eCurstate = STATE_IDLE;
	//m_pModelCom->Set_AnimResetTimeAcc(STATE_IDLE);
}

void CBoss_Kyogai::Trigger_Begin()
{

	m_eCurstate = STATE_BEGIN;
}


void CBoss_Kyogai::Trigger_Guard()
{
	m_bTrigger = true;
}

void CBoss_Kyogai::Update_Guard(_double dTimeDelta)
{

}


void CBoss_Kyogai::Update_PushAway(_double dTimeDelta)
{
	
}

void CBoss_Kyogai::Update_NextPhase(_double dTimeDelta)
{
	
}


void CBoss_Kyogai::Update_Heal(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_StatusDesc.fHp = 0.f;
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_HEAL;
	}


	if (m_pModelCom->Get_AnimRatio(ANIM_HEAL, 0.0) && !m_pModelCom->Get_AnimRatio(ANIM_HEAL, 1.0))
	{

		_double dRatio = m_pModelCom->Get_dAnimRatio(ANIM_HEAL);
		if (dRatio > 0.0)
		{
			if (dRatio > 0.97)
				dRatio = 1.0;

			m_StatusDesc.fHp = 100.f * (_float)dRatio;
		}
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_HEAL))
	{
		if (m_eCurPhase == PHASE_1)
		{
			m_iTriggerCnt = 0;
			m_dAwakeTime = 0.0;
			m_eCurPhase = PHASE_2;
		}

		/*else if (m_eCurPhase == PHASE_2)
			m_eCurPhase = PHASE_3;*/

		m_eCurAnimIndex = ANIM_IDEL;
		Trigger_Interact();

	}
}

void CBoss_Kyogai::Update_Awake(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_AWAKE_PUSHAWAY;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_PUSHAWAY) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
		m_eCurAnimIndex = ANIM_AWAKE_START;
	}
	if (m_pModelCom->Check_PickAnimRatio(ANIM_AWAKE_START, 0.990, dTimeDelta))
	{
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_AWAKE_START);
		m_eCurAnimIndex = ANIM_AWAKE_END;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_END) == true)
	{
		m_pModelCom->Get_AnimFinish(ANIM_AWAKE_END);
		m_eCurAnimIndex = ANIM_IDEL;
		Trigger_Interact();
	}
}



HRESULT CBoss_Kyogai::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Akaza"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CBoss_Kyogai_Akaza");
		return E_FAIL;
	}

	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CBoss_Kyogai_Akaza");
		return E_FAIL;
	}


	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0 * 0.8;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CBoss_Kyogai_Akaza");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(0.8f, 0.8f, 0.8f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CBoss_Kyogai_Akaza");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CBoss_Kyogai_Akaza");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.f, 1.f, 1.f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CBoss_Kyogai_Akaza");
		return E_FAIL;
	}

	//m_CharacterDesc.NaviDesc.iCurrentIndex = 1;
	///* for.Com_Navigation_Acaza*/
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Acaza"),
	//	TEXT("Com_Navigation_Acaza"), (CComponent**)&m_pNavigationCom[NAVI_ACAZA], &m_CharacterDesc.NaviDesc)))
	//{
	//	MSG_BOX("Failed to Add_Com_Navigation_Acaza: CPlayer");
	//	return E_FAIL;
	//}



	return S_OK;
}

HRESULT CBoss_Kyogai::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_bSuperArmor", &m_bAwake, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CBoss_Kyogai* CBoss_Kyogai::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Kyogai* pInstance = new CBoss_Kyogai(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_Kyogai");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Kyogai::Clone(void* pArg)
{
	CBoss_Kyogai* pInstance = new CBoss_Kyogai(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Kyogai");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Kyogai::Free()
{
	__super::Free();
}
