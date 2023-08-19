#include "pch.h"
#include "..\Public\Player_Zenitsu.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

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

	m_pSword->Tick(dTimeDelta);
	m_pSwordHome->Tick(dTimeDelta);


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
	if (m_ePlayerType == PLAYER_ZENITSU)
	{

		Animation_Control(dTimeDelta);

		//애니메이션 처리
		m_pModelCom->Play_Animation(dTimeDelta);
		RootAnimation(dTimeDelta);

		//이벤트 콜
		EventCall_Control(dTimeDelta);
	}
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this)))
		return;
}

void CPlayer_Zenitsu::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	m_pSword->LateTick(dTimeDelta);
	m_pSwordHome->LateTick(dTimeDelta);

	//playerswap
	if (m_ePlayerType == PLAYER_ZENITSU)
	{
		if (m_isAir_Hekireki && m_pModelCom->Get_iCurrentAnimIndex() == ANIM_JUMP_IDLE)
		{
			m_isAir_Hekireki = false;
			m_isJumpOn = true;
		}


		if (m_isAir_Hekireki == false)
			Gravity(dTimeDelta);
	}


	//임시 코드
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_DIKeyDown(DIK_H))
	{
		if (m_bGround_Enemy)
			m_bGround_Enemy = false;
		else
			m_bGround_Enemy = true;
	}
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

	if (EventCallProcess())
	{
#pragma region Combo_Attack
		if (ANIM_ATK_COMBO == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				_tchar szTest[MAX_PATH] = TEXT("TestSound.wav");
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_SLASH);
				CSoundMgr::Get_Instance()->PlaySound(szTest, CSoundMgr::PLAYER_SLASH, 0.9f);
			}

			CEffectPlayer::Get_Instance()->Play("hjd", m_pTransformCom);
		}
		if (22 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				_tchar szTest[MAX_PATH] = TEXT("TestSound.wav");
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_SLASH);
				CSoundMgr::Get_Instance()->PlaySound(szTest, CSoundMgr::PLAYER_SLASH, 0.9f);
			}
			else if (1 == m_iEvent_Index)
			{
				_tchar szTest[MAX_PATH] = TEXT("TestSound.wav");
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_SLASH);
				CSoundMgr::Get_Instance()->PlaySound(szTest, CSoundMgr::PLAYER_SLASH, 0.9f);
			}

		}
		if (23 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				_tchar szTest[MAX_PATH] = TEXT("TestSound.wav");
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_SLASH);
				CSoundMgr::Get_Instance()->PlaySound(szTest, CSoundMgr::PLAYER_SLASH, 0.9f);
			}
		}
		if (25 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				_tchar szTest[MAX_PATH] = TEXT("TestSound.wav");
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_SLASH);
				CSoundMgr::Get_Instance()->PlaySound(szTest, CSoundMgr::PLAYER_SLASH, 0.9f);
			}
		}
		if (ANIM_ATK_SPECIAL_CUTSCENE == m_pModelCom->Get_iCurrentAnimIndex())
		{

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

		Animation_Control_Battle_Attack(dTimeDelta);

		Animation_Control_Battle_Charge(dTimeDelta);

		Animation_Control_Battle_Skill(dTimeDelta);

		Animation_Control_Battle_Guard(dTimeDelta);

		Animation_Control_Battle_Dash(dTimeDelta);

		Animation_Control_Battle_Awaken(dTimeDelta);

		Animation_Control_Battle_Special(dTimeDelta);
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
		m_fMove_Speed = 2.0f;
		m_pTransformCom->Go_Straight(dTimeDelta * m_fMove_Speed * m_fScaleChange, m_pNavigationCom[m_eCurNavi]);
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
		m_Moveset.m_Down_Battle_JumpMove = false;
		m_isJump_Move = true;

		m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP);
		Jumping(4.5f, 0.2f);

		m_pModelCom->Set_EarlyEnd(58, true);
	}
	if (m_isJump_Move)
	{
		Go_Straight_Constant(dTimeDelta, ANIM_BATTLE_JUMP, m_fMove_Speed * 1.2f * m_fScaleChange);
		Go_Straight_Constant(dTimeDelta, 57, m_fMove_Speed * 1.2f * m_fScaleChange);
		Go_Straight_Constant(dTimeDelta, 58, m_fMove_Speed * 1.2f * m_fScaleChange);
		Go_Straight_Deceleration(dTimeDelta, 59, m_fMove_Speed * 1.2f * m_fScaleChange, 0.36f * m_fScaleChange); // Down
	}
	Ground_Animation_Play(58, 59);


	if (m_Moveset.m_Down_Battle_Jump)
	{
		m_Moveset.m_Down_Battle_Jump = false;
		m_isJump_Move = false;

		m_pModelCom->Set_Animation(ANIM_BATTLE_JUMP);
		Jumping(4.5f * m_fScaleChange, 0.2f * m_fScaleChange);

		m_pModelCom->Set_EarlyEnd(58, true);
	}



	//공중 공격 콤보
	if (m_Moveset.m_Down_Battle_Jump_Attack)
	{
		if (m_isComboing_Down == false)
		{
			m_Moveset.m_Down_Battle_Jump_Attack = false;
			m_isJump_Move = false;

			//콤보 첫 애니메이션 설정
			if (m_pModelCom->Get_Combo_Doing() == false)
			{
				m_pModelCom->Set_Combo_Doing(true);
				m_pModelCom->Set_Animation(ANIM_ATK_AIRCOMBO);

				JumpStop(0.40);
			}
			//아닐경우, 다음 콤보로 진행
			else
			{
				m_pModelCom->Set_Combo_Trigger(true);

				m_pModelCom->Set_EarlyEnd(18, true);
				JumpStop(0.85);
				m_isFirst_JumpAtk = false;
			}
		}
	}


	//공중 트랙공격 (이동키 + 공격키)
	if (m_Moveset.m_Down_Battle_Jump_TrackAttack)
	{
		m_Moveset.m_Down_Battle_Jump_TrackAttack = false;

		if (m_isComboing_Down == false)
		{
			m_isFirst_JumpAtk = false;

			m_pModelCom->Set_Animation(ANIM_ATK_AIRTRACK);
			JumpStop(0.9);
			Set_FallingStatus(3.0f, 0.0f);
		}
	}
	Ground_Animation_Play(1, 2);
	Go_Straight_Constant(dTimeDelta, 1, 3.f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 2, 3.f * m_fScaleChange, 0.2f * m_fScaleChange); // Down
}

void CPlayer_Zenitsu::Animation_Control_Battle_Attack(_double dTimeDelta)
{
	_int iCurAnimIndex = m_pModelCom->Get_iCurrentAnimIndex();

	if (m_isComboing_Upper == false)
	{
		// 콤보공격
		if (m_Moveset.m_Down_Battle_Combo)
		{
			m_Moveset.m_Down_Battle_Combo = false;
			m_isComboing = true;

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
					}
				}
			}
		}
	}
	//m_pModelCom->Set_EarlyEnd(ANIM_ATK_COMBO, true);
	//m_pModelCom->Set_EarlyEnd(4, true);
	//m_pModelCom->Set_EarlyEnd(5, true);

	// 공격 모션별 전진이동 제어 (Timedelta, 애니메이션인덱스,  초기화속도,  감속도)
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_COMBO, 3.0f * m_fScaleChange, 0.15f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 4, 2.3f * m_fScaleChange, 0.12f * m_fScaleChange);
	Go_Straight_Deceleration(dTimeDelta, 5, 2.3f * m_fScaleChange, 0.10f * m_fScaleChange);
	//분기

	Go_Straight_Deceleration(dTimeDelta, 7, 2.5f * m_fScaleChange, 0.07f * m_fScaleChange); // Normal
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

		if (Get_LockOn_MonPos())
			m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));

		m_pModelCom->Set_Animation(ANIM_ATK_CHARGE);
	}


	if (m_Moveset.m_Up_Battle_Charge && m_dDelay_Charge > 1.0f)
	{
		m_Moveset.m_Up_Battle_Charge = false;
		m_dDelay_Charge = 0.0;

		if (Get_LockOn_MonPos())
			m_pTransformCom->LookAt_FixY(XMLoadFloat4(&m_LockOnPos));

		m_pModelCom->Set_Animation(21);
	}
	else if (m_Moveset.m_Up_Battle_Charge && m_dDelay_Charge <= 1.0f)
	{
		m_Moveset.m_Up_Battle_Charge = false;
		m_dDelay_Charge = 0.0;

		m_pModelCom->Set_Animation(ANIM_BATTLE_IDLE);
	}
	Go_Straight_Deceleration(dTimeDelta, 21, 4.5f * m_fScaleChange, 0.15f * m_fScaleChange);
}

void CPlayer_Zenitsu::Animation_Control_Battle_Skill(_double dTimeDelta)
{
	if (m_isTestHit)
	{
		m_isTestHit = false;
		m_isHekireki_Hit = true;
	}
	//벽력일섬 히트시 살짝 느려지는거 구현 위한것. 이게 켜질시 히트상태임.
	if (m_isHekireki_Hit)
	{
		m_dDelay_Hekireki_Hit += dTimeDelta;
		m_isHekireki_After = true;

		//히트상태는 이 딜레이 시간동안 유지된다.
		if (m_dDelay_Hekireki_Hit > 0.23f)
		{
			m_dDelay_Hekireki_Hit = 0.0;
			m_isHekireki_Hit = false;
		}
	}

	_int CurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (CurAnim != ANIM_ATK_SKILL_HEKIREKI && CurAnim != ANIM_ATK_SKILL_HEKIREKI_AIR && CurAnim != ANIM_ATK_SKILL_HEKIREKI_END && CurAnim != ANIM_ATK_SKILL_HEKIREKI_AIR_END)
	{
		//스킬_0
		if (m_Moveset.m_Down_Skill_Normal || m_Moveset.m_Down_Skill_Move)
		{
			m_Moveset.m_Down_Skill_Normal = false;
			m_Moveset.m_Down_Skill_Move = false;

			m_dTime_Hekireki = 0.0f;

			if (m_bGround_Enemy)
				m_isAir_Hekireki = false;
			else
				m_isAir_Hekireki = true;

			if (m_isAir_Hekireki)
				m_pModelCom->Set_Animation(ANIM_ATK_SKILL_HEKIREKI_AIR);
			else
				m_pModelCom->Set_Animation(ANIM_ATK_SKILL_HEKIREKI);

			m_pTransformCom->LookAt(m_vBattleTargetPos);

		}
	}
	else
	{
		m_Moveset.m_Down_Skill_Normal = false;
		m_Moveset.m_Down_Skill_Move = false;
	}


	if (m_isHekireki)
	{
		if (m_isHekireki_Hit == false)
			m_dTime_Hekireki += dTimeDelta;

		if (0.35f < m_dTime_Hekireki && m_dTime_Hekireki < 0.42f)
		{
			if (m_isHekireki_Hit)
				m_pTransformCom->Go_Straight(dTimeDelta * 0.6f * m_fScaleChange, m_pNavigationCom[m_eCurNavi]);
			else
				m_pTransformCom->Go_Straight(dTimeDelta * 20.f * m_fScaleChange, m_pNavigationCom[m_eCurNavi]);
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

		m_pModelCom->Set_Animation(ANIM_ATK_SKILL_GUARD);
	}
	Go_Straight_Deceleration(dTimeDelta, ANIM_ATK_SKILL_GUARD, 4.f * m_fScaleChange, 0.18f * m_fScaleChange);
}

void CPlayer_Zenitsu::Animation_Control_Battle_Guard(_double dTimeDelta)
{
	//탄지로 63 64 65 66hit 67hit 68push 
	//가드 시작
	if (m_Moveset.m_Down_Battle_Guard)
	{
		m_Moveset.m_Down_Battle_Guard = false;

		m_pTransformCom->Set_Look(m_Moveset.m_Input_Dir);
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

		m_pModelCom->Set_Animation(ANIM_BATTLE_DASH);
	}
	Go_Straight_Constant(dTimeDelta, 46, 3.0f * m_fScaleChange);


	if (m_Moveset.m_Down_Battle_Step)
	{
		m_Moveset.m_Down_Battle_Step = false;

		m_pTransformCom->Set_Look(m_vLook);
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
				}
				//아닐경우, 다음 콤보로 진행
				else
				{
					m_pModelCom->Set_Combo_Trigger(true);

				}
			}
			else if (m_isRight)
			{
				//콤보 첫 애니메이션 설정
				if (m_pModelCom->Get_Combo_Doing() == false)
				{
					m_pModelCom->Set_Combo_Doing(true);
					m_pModelCom->Set_Animation(ANIM_BATTLE_STEP_R);
				}
				//아닐경우, 다음 콤보로 진행
				else
				{
					m_pModelCom->Set_Combo_Trigger(true);

				}
			}
		}
	}
	_vector vDir = XMLoadFloat4(&m_Moveset.m_Input_Dir);
	_float4 fDir;
	XMStoreFloat4(&fDir, -vDir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_F, 4.5f * m_fScaleChange, 0.18f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_B, 4.5f * m_fScaleChange, 0.18f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_L, 4.5f * m_fScaleChange, 0.18f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, ANIM_BATTLE_STEP_R, 4.5f * m_fScaleChange, 0.18f * m_fScaleChange, m_Moveset.m_Input_Dir);

	//더블스텝
	Go_Dir_Deceleration(dTimeDelta, 71, 5.0f * m_fScaleChange, 0.2f * m_fScaleChange, m_Moveset.m_Input_Dir);
	Go_Dir_Deceleration(dTimeDelta, 73, 5.0f * m_fScaleChange, 0.2f * m_fScaleChange, m_Moveset.m_Input_Dir);
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

		m_pModelCom->Set_Animation(ANIM_ATK_SPECIAL_READY);
		m_dTime_Special_Ready = 0.0;
	}
	m_dTime_Special_Ready += dTimeDelta;
	if (m_dTime_Special_Ready > 1.5f)
		Go_Straight_Deceleration(dTimeDelta, 130, 4.0f * m_fScaleChange, 0.15f * m_fScaleChange);

	//Go_Straight_Constant(dTimeDelta, 108, 2.7f);
	if (m_pModelCom->Get_iCurrentAnimIndex() == 130 || m_pModelCom->Get_iCurrentAnimIndex() == 131)
	{
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
	if (m_Moveset.m_Down_Dmg_Small)
	{
		m_Moveset.m_Down_Dmg_Small = false;

		m_pModelCom->Set_Animation(ANIM_DMG_SMALL);
		m_Moveset.m_isHitMotion = true;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_DMG_SMALL))
	{
		m_Moveset.m_isHitMotion = false;
	}



}

void CPlayer_Zenitsu::Moving_Restrict()
{
	_int iCurAnimIndex = m_pModelCom->Get_iCurrentAnimIndex();

	//콤보공격시 무빙제한
	if (ANIM_ATK_COMBO == iCurAnimIndex || 4 == iCurAnimIndex || 5 == iCurAnimIndex
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
	}
	//대시 시 제한
	else if (ANIM_BATTLE_DASH == iCurAnimIndex || 46 == iCurAnimIndex || 47 == iCurAnimIndex)
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
	/*if (FAILED(m_pShaderCom->SetUp_RawValue("g_bSuperArmor", &m_bAwake, sizeof(_bool))))
		return E_FAIL;*/


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
