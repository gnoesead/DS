#include "pch.h"
#include "..\Public\Monster_Spider.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "MonsterManager.h"

#include "Player.h"
#include "PlayerManager.h"

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

	First_Initiate();
	
	return S_OK;
}

void CMonster_Spider::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	MonsterManaging();

	if (true == m_isDead)
		return;

	if (m_isSpiderDead)
	{
		m_isSpiderDead = false;

		m_StatusDesc.fHp = m_StatusDesc.fHp_Max;

		RePos();
		First_Initiate();
	}

	Trigger();
	Animation_Control(dTimeDelta);

	//애니메이션 처리
 	m_pModelCom->Play_Animation(dTimeDelta);
	RootAnimation(dTimeDelta);

	//이벤트 콜
	EventCall_Control(dTimeDelta);	


	if (m_fLand_Y <= 1.1f)
	{
		Go_Dir_Constant(dTimeDelta, 18, 2.5f, _float4{ 0.0f, 0.0f, -1.0f, 0.0f });
		Go_Dir_Constant(dTimeDelta, ANIM_DOWN, 2.5f, _float4{ 0.0f, 0.0f, -1.0f, 0.0f });
	}
	
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
	if (FAILED(__super::Render_ShadowDepth()))
		return E_FAIL;

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
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.5f, 0.5f, 0.5f), _float3(0.f, 1.0f, 1.1f), 0.25,
					CAtkCollider::TYPE_SMALL, Calculate_Dir(), 10.0f);
			}
		}

		if (8 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.6f, 0.6f, 0.6f), _float3(0.f, 1.0f, 1.1f), 0.25,
					CAtkCollider::TYPE_BIG, Calculate_Dir(), 10.0f);
			}
		}
		
#pragma endregion
		m_iEvent_Index++;
	}
}

void CMonster_Spider::Trigger()
{
	//Hit_Trigger
	if (m_eCurState != STATE_DOWN)
	{
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Big()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Spin()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Upper()
			|| m_pColliderCom[COLL_SPHERE]->Get_Hit_Hekireki())
		{
			m_eCurState = STATE_HIT;
		}
	}
	else
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Spin(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);
	}
}

void CMonster_Spider::Animation_Control(_double dTimeDelta)
{
	if (m_isDeath_Motion)
	{
		m_dDelay_Die += dTimeDelta;
		if (m_dDelay_Die > m_fDeadTime)
		{
			m_isSpiderDead = true;
			m_dDelay_Die = 0.0;
			m_isDeath_Motion = false;
		}
		m_pColliderCom[COLL_SPHERE]->Set_Death(true);
	}
	else
	{
		if (m_isCrawlingOn)
		{
			Animation_Control_Crawling(dTimeDelta);
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
}

void CMonster_Spider::Animation_Control_Crawling(_double dTimeDelta)
{
	m_pModelCom->Set_Animation(ANIM_RUN);

	if (Calculate_Distance() < 20.0f)
		m_isSpider_Start = true;

	if (m_isSpider_Start)
	{
		if (m_fCrawling_Y <= 6.74f)
		{
			m_fLand_Y = m_fCrawling_Y;
			m_fCrawling_Y += (_float)dTimeDelta * m_fCrawlingSpeed;
			//m_fLand_Y = 8.0f;
		}
		else if (6.74f < m_fCrawling_Y)
		{
			//m_isNavi_Y_Off = false;

			if (m_isLeft)
				m_pTransformCom->LerpVector(_vector{ 1.0f, 0.0f, 0.0f, 0.0f }, 0.035f);
			else
				m_pTransformCom->LerpVector(_vector{ -1.0f, 0.0f, 0.0f, 0.0f }, 0.035f);

			m_pTransformCom->Go_Straight(dTimeDelta * 0.1f);

			m_dDelay_Crawling += dTimeDelta;
			if (m_dDelay_Crawling > 1.5)
			{
				m_isNavi_Y_Off = false;
				m_isCrawlingOn = false;
			}
		}
	}
}

void CMonster_Spider::Animation_Control_Idle(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();
	
	m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.1f);

	//거미의 이동
	m_dDelay_Spider += dTimeDelta;
	if (m_dDelay_Spider > 0.3)
	{
		if (Calculate_Distance() < 3.5f)
		{
			if (m_isFirst_Spider_0)
			{
				m_isFirst_Spider_0 = false;
				m_isFirst_Spider_1 = true;

				m_dDelay_Spider = 0.0;

				m_pModelCom->Set_Animation(ANIM_ADV_WALK);
			}
			Go_Backward_Constant(dTimeDelta, 2, 0.3f);
		}
		else if (3.5f <= Calculate_Distance() && Calculate_Distance() < 7.0f)
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
		else if (7.0f <= Calculate_Distance())
		{
			if (m_isFirst_Spider_0)
			{
				m_isFirst_Spider_0 = false;
				m_isFirst_Spider_1 = true;

				m_dDelay_Spider = 0.0;

				m_pModelCom->Set_Animation(ANIM_RUN);
			}
			Go_Straight_Constant(dTimeDelta, ANIM_RUN, 0.55f);
		}
	}


	//플레이어 뒤일때,
	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, Calculate_PlayerPos());
	_float4 MyPos;
	XMStoreFloat4(&MyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (PlayerPos.z > MyPos.z + 10.0f)
	{
		m_isDeath_Motion = true;
		m_eCurState = STATE_DOWN;
		m_pModelCom->Set_Animation(ANIM_DOWN);
	}

	//공격
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
			m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.1f);
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
			m_pTransformCom->LerpVector(Calculate_Dir_FixY(), 0.1f);
			
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
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_int PlayerIndex = CPlayerManager::GetInstance()->Get_PlayerIndex();
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), PlayerIndex));

	_float4 AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();

	AtkDir.y = 0.0f;
	XMStoreFloat4(&AtkDir, XMVector4Normalize(XMLoadFloat4(&AtkDir)));

	m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.05f);

	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();


	
	


	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small() || m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
	{
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
			m_isConnectHitting = false;
		}
		else if (m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
		{
			m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
			m_isConnectHitting = true;
		}

		m_dDelay_ComboChain = 1.0;
		pPlayer->Set_Hit_Success(true); // 플레이어가 맞았따
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage() * 3.0f;
		
		if (m_StatusDesc.fHp <= 0.0f)
		{
			m_pModelCom->Set_Animation(16);
			m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();

			if (m_isSpiderFirst_Dead)
			{
				m_isSpiderFirst_Dead = false;
				Jumping(0.45f, 0.04f);
			}
		}
		else
		{
			if (m_iSmallHit_Index == 0)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_SMALL);
				m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
				m_iSmallHit_Index++;
			}
			else if (m_iSmallHit_Index == 1)
			{
				m_pModelCom->Set_Animation(ANIM_DMG_BIG);
				m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
				m_iSmallHit_Index = 0;
			}
			
			
		}
	}
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_SMALL, 2.3f, 0.17f, m_Hit_AtkDir);


	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Big())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);

		m_dDelay_ComboChain = 1.0;
		pPlayer->Set_Hit_Success(true);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage() * 5.0f;

		if (m_StatusDesc.fHp <= 0.0f)
		{
			m_pModelCom->Set_Animation(16);
			Jumping(0.45f, 0.04f);
			
			m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
		}
		else
		{
			m_pModelCom->Set_Animation(16);
			Jumping(0.45f, 0.04f);
			
			m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();
		}

		//pGameInstance->Time_Slow(0.3, 0.4);
	}
	Go_Dir_Deceleration(dTimeDelta, ANIM_DMG_BIG, 2.0f, 0.10f, m_Hit_AtkDir);




#pragma region Hit_Upper
	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Upper() || m_pColliderCom[COLL_SPHERE]->Get_Hit_Bound())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Bound(false);

		m_dDelay_ComboChain = 6.0;
		pPlayer->Set_Hit_Success(true);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		m_isUpperHit = true;

		m_pModelCom->Set_Animation(16);
		Jumping(1.7f, 0.04f); // 1.65

		m_isSpiderBlow = true;

		//pGameInstance->Time_Slow(0.3, 0.4);
	}
#pragma endregion



	if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow() || m_pColliderCom[COLL_SPHERE]->Get_Hit_Spin() || m_pColliderCom[COLL_SPHERE]->Get_Hit_Hekireki())
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Spin(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Hekireki(false);

		m_dDelay_ComboChain = 2.5;
		pPlayer->Set_Hit_Success(true);
		m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage() * 2.0f;

		m_pModelCom->Set_Animation(16);
		Jumping(0.45f, 0.04f);

		m_Hit_AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();

		m_isSpiderBlow = true;

		//pGameInstance->Time_Slow(0.3, 0.4);
	}
	
	_float4 Dir_To_Monster;
	XMStoreFloat4(&Dir_To_Monster, -Calculate_Dir());


	Ground_Animation_Play(17, 18);
	if (m_isUpperHit)
	{
		Go_Dir_Constant(dTimeDelta, ANIM_BLOW, 0.5f, Dir_To_Monster, true);
		//m_pModelCom->Set_EarlyEnd(ANIM_BLOW, true);
		Go_Dir_Constant(dTimeDelta, 17, 0.5f, Dir_To_Monster, true);
		Go_Dir_Deceleration(dTimeDelta, 18, 0.5f, 0.05f, Dir_To_Monster, true);
	}
	else
	{
		Go_Dir_Constant(dTimeDelta, ANIM_BLOW, 1.5f, Dir_To_Monster, true);
		//m_pModelCom->Set_EarlyEnd(ANIM_BLOW, true);
		Go_Dir_Constant(dTimeDelta, 17, 1.5f, Dir_To_Monster, true);
		Go_Dir_Deceleration(dTimeDelta, 18, 1.5f, 0.15f, Dir_To_Monster, true);
	}

	//공중에 떠있을때,
	if (iCurAnim == 17)
	{
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (Pos.x < 201.0f || 209.5f < Pos.x)
		{
			m_isNavi_Y_Off = true;

			m_fLand_Y = 1.0f;

			m_isSpiderBlow_Outer = true;
		}
	}
	if (m_isSpiderBlow_Outer && iCurAnim == 18)
	{
		m_isDeath_Motion = true;
		m_isSpiderBlow_Outer = false;

	}

	
	if (iCurAnim == ANIM_IDLE)
	{
		m_eCurState = STATE_IDLE;
	}

	if (iCurAnim != 16 && iCurAnim != 17 && iCurAnim != 18)
	{
		m_dDelay_ComboChain -= dTimeDelta;
		if (m_dDelay_ComboChain <= 0.0f)
		{
			m_dDelay_ComboChain = 0.0;

			m_isFirst_Anim = true;

			m_eCurState = STATE_IDLE;
		}
	}

	if (iCurAnim == ANIM_DOWN  ) 
	{
		m_isUpperHit = false;

		m_dDelay_ComboChain = 0.0;

		m_isFirst_Anim = true;

		m_eCurState = STATE_DOWN;
	}


	Safe_Release(pGameInstance);
}

void CMonster_Spider::Animation_Control_Down(_double dTimeDelta)
{
	_int iCurAnim = m_pModelCom->Get_iCurrentAnimIndex();

	if (iCurAnim == ANIM_DOWN)
	{
		if (m_StatusDesc.fHp > 0.0f)
		{
			m_dDelay_Down += dTimeDelta;
			if (m_dDelay_Down > 5.0f)
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

void CMonster_Spider::First_Initiate()
{
#pragma region Byunsu
	m_eCurState =  STATE_IDLE ;

	m_isSpider_Start =  false ;

	m_isSpiderFirst_Dead = true;

	m_fCrawlingSpeed =  1.0f ;
	m_isCrawlingOn =  true ;
	m_fCrawling_Y =  0.0f ;
	m_dDelay_Crawling =  0.0 ;
	m_isLeft = false ;

	m_isUpperHit =  false ;

	m_dDelay_Spider =  0.0 ;
	m_isFirst_Spider_1 =  true ;
	m_isFirst_Spider_0 =  true ;

	m_vPos = { 140.f, 0.f,120.f,1.f };

	m_dDelay_ComboChain =  0.0 ;

	m_dCoolTime_Attack =  0.0 ;
	m_isFirst_Attack =  true ;
	m_dDelay_Attack =  0.0;

	m_Hit_AtkDir = { 0.0f, 0.0f, 0.0f ,0.0f };

	m_isFirst_AttackOn =  true;
	m_isFirst_AttackOff = false;

#pragma endregion


	m_fScale = Random::Generate_Float(1.0f, 2.2f);
	m_pTransformCom->Scaling(_float3{ m_fScale, m_fScale, m_fScale });

	m_fAttack = (rand() % 100 + 50) * 0.1f;

	m_fDeadTime = Random::Generate_Float(3.0f, 7.0f);

	m_eCurNavi = CLandObject::NAVI_TRAIN;

	m_isNavi_Y_Off = true;

	_float4 Pos;
	XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (Pos.x < 204.0f) // Left
	{
		_float4 UpDir;
		XMStoreFloat4(&UpDir, XMVector4Normalize(_vector{ 0.001f, 0.999f, 0.0f, 0.0f }));
		m_pTransformCom->Set_Look(UpDir);
		m_isLeft = true;
	}
	else //Right
	{
		_float4 UpDir;
		XMStoreFloat4(&UpDir, XMVector4Normalize(_vector{ -0.001f, 0.999f, 0.0f, 0.0f }));
		m_pTransformCom->Set_Look(UpDir);
		m_isLeft = false;
	}

	m_fCrawlingSpeed = ((rand() % 100) + 50) * 0.01f;

	m_StatusDesc.fHp = 25.0f;
	m_StatusDesc.fHp_Max = 25.0f;

	m_pColliderCom[COLL_SPHERE]->Set_Death(false);
}

void CMonster_Spider::RePos()
{
	_float4	PlayerPos;
	XMStoreFloat4(&PlayerPos, Calculate_PlayerPos());
	
	//left
	if (0 == (rand() % 2))
	{
		PlayerPos.z += Random::Generate_Float(5.0f, 20.0f);

		_float4 MyNewPos = { 201.3f, 6.9f, PlayerPos.z, 1.f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&MyNewPos));
	}
	//right
	else
	{
		PlayerPos.z += Random::Generate_Float(5.0f, 20.0f);

		_float4 MyNewPos = { 209.2f, 6.9f, PlayerPos.z, 1.f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&MyNewPos));
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

	m_CharacterDesc.NaviDesc.iCurrentIndex = 0;

	/* for.Com_Navigation_Train*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Train"),
		TEXT("Com_Navigation_Train"), (CComponent**)&m_pNavigationCom[NAVI_TRAIN], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Train: CPlayer");
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
