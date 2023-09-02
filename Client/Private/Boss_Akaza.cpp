#include "pch.h"
#include "Boss_Akaza.h"
#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"
#include "Player.h"
#include "PlayerManager.h"

#include "AtkCollManager.h"
#include "Camera_Manager.h"
#include "Camera_Free.h"


CBoss_Akaza::CBoss_Akaza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CBoss_Akaza::CBoss_Akaza(const CBoss_Akaza& rhs)
	: CMonster(rhs)
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

	Get_PlayerComponent();

	m_StatusDesc.fHp = 100.f;
	m_StatusDesc.fHp_Max = 100.f;
	m_eCurAnimIndex = ANIM_IDLE;
	m_eCurstate = STATE_BEGIN;
	m_eCurPhase = BEGIN;
	m_eCurNavi = NAVI_ACAZA;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(120.f, 0.f, 130.f, 1.f));
	m_pModelCom->Set_LinearDuration(ANIM_AWAKE_START, 1.0);
	return S_OK;

}

void CBoss_Akaza::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);
	m_pModelCom->Set_LinearDuration(ANIM_AWAKE_START, 1.2);
	if (true == m_isDead)
		return;

#ifdef _DEBUG
	Debug_State(dTimeDelta);

#endif // _DEBUG	

	Update_Hit_Messenger(dTimeDelta);
	Update_Trigger(dTimeDelta);
	Update_State(dTimeDelta);

	m_pModelCom->Set_Animation(m_eCurAnimIndex);
	m_pModelCom->Play_Animation_For_Boss(dTimeDelta);

	EventCall_Control(dTimeDelta);

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
	//Get_PlayerComponent();

	//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector	vLightEye = XMVectorSet(130.f, 10.f, 140.f, 1.f);
	_vector	vLightAt = XMVectorSet(60.f, 0.f, 60.f, 1.f);

	/*if (m_pPlayerTransformCom != nullptr)
	{
		vLightEye = vPlayerPos + XMVectorSet(-5.f, 10.f, -5.f, 1.f);
		vLightAt = vPlayerPos;

	}*/

	_vector	vLightUp = XMVectorSet(0.f, 1.f, 0.f, 1.f);

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
#ifdef _DEBUG
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
			m_isDead = true;
		}

		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
			Trigger_DashPunch();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			Trigger_AirGun();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			Trigger_DashKick();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_4))
		{
			Trigger_UpperKick();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{
			Trigger_JumpAirGun();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_6))
		{
			Trigger_JumpStomp();
		}
	}

	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT))
	{
		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
			Trigger_Hit_CutScene();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			Trigger_ComboPunch();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			Trigger_Nachim();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_4))
		{
			Trigger_PushAway();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{
			Trigger_Heal();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_6))
		{
			Trigger_Awake();
		}
	}

	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL))
	{
		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
			Trigger_Escape();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			Trigger_Awake_ComboPunch();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			Trigger_NextPhase3();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_4))
		{
			Trigger_Dash_ComboPunch();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{
			Trigger_Nachim_ComboPunch();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_6))
		{
			m_bTrigger = true;
			m_eCurstate = STATE_CINEMATIC;

			m_bAnimFinish = false;
			m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC1);
			m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC2);
			m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC3);
			m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC4);
			m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC5);
			m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC6);
			m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC7);
			m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC8);
			m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC9);
			m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC10);

		}
	}

	Safe_Release(pGameInstance);
}

#endif //_DEBUG

void CBoss_Akaza::EventCall_Control(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_iEvent_Index = 0;
	}

	if (EventCallProcess())
	{
		_vector vMonsterDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vDir = Calculate_Dir();

		_vector vRandomDir = Random_Dir(vMonsterDir);

		_double dLifeTime = 0.20;
		_double dLongLifeTime = 1.0;
		_double dSpeed = 5.0;
#pragma region AWAKE_ComboPunch
 		if (ANIM_AWAKE_COMBOPUNCH_LOOP == m_pModelCom->Get_iCurrentAnimIndex())
		{
			vRandomDir = Random_Dir(vMonsterDir, -30.f, 5.f, -25.f, 25.f);

			if (0 <= m_iEvent_Index && 35 >= m_iEvent_Index)
			{
				if (0 == m_iEvent_Index || 5 == m_iEvent_Index || 12 == m_iEvent_Index ||
					19 == m_iEvent_Index || 26 == m_iEvent_Index || 33 == m_iEvent_Index)
				{//피격용 콜라이더
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
					Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.f, 1.f, 1.f), _float3(0.f, 1.0f, 1.5f), dLifeTime,
						CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
				}
				//이펙트용 콜라이더
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 0.75f;
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.0f, 0.f), 0.3,
					CAtkCollider::TYPE_EFFECT, vRandomDir, m_fSmallDmg, m_pTransformCom, 2.5, CAtkCollider::TYPE_BULLET, "Akaza_ATK_BulletPunch", &EffectWorldDesc);
			}
			if (32 == m_iEvent_Index)
			{
				int a = 0;
			}
		}
		if (ANIM_AWAKE_COMBOPUNCH_END == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.x = -1.f;
				EffectWorldDesc.fScale = 1.5f;
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.0f, 0.5f), 0.5,
					CAtkCollider::TYPE_EFFECT, vMonsterDir, m_fSmallDmg, m_pTransformCom, 3.0, CAtkCollider::TYPE_BULLET, "Akaza_ATK_BulletPunch_Long", &EffectWorldDesc);

				EffectWorldDesc.fScale = 1.f;
				CEffectPlayer::Get_Instance()->Play("Akaza_WindRing", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.z = -2.f;
				EffectWorldDesc.fScale = 1.5f;
				EffectWorldDesc.dSpeed = 0.8;

				CEffectPlayer::Get_Instance()->Play("Akaza_WindRing", m_pTransformCom, &EffectWorldDesc);

				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_BIGBLOW, vMonsterDir, m_fBigBlowDmg); // 빅블로우
			}


		}
#pragma endregion // AWAKE_ComboPunch

#pragma region 대시펀치
		if (ANIM_SUPERARMOR == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Battle_ATK_SuperArmor_0", m_pTransformCom);
			}

		}
		if (ANIM_SUPERARMOR2 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Battle_ATK_SuperArmor_1", m_pTransformCom);
			}
		}

		if (ANIM_SUPERARMOR3 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			dLifeTime = 0.70;
			if (0 == m_iEvent_Index)
			{//0
				CEffectPlayer::Get_Instance()->Play("Battle_ATK_SuperArmor_2", m_pTransformCom);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.5f;
				EffectWorldDesc.dSpeed = 0.75;
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Fist", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.fScale = 1.4f;
				EffectWorldDesc.dSpeed = 1.5;
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
			}
			if (1 == m_iEvent_Index)
			{//0.1
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.2f;
				EffectWorldDesc.dSpeed = 1.3;
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
			}
			if (2 == m_iEvent_Index)
			{//0.17
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 1.0f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_BLOW, vMonsterDir, m_fBlowDmg);
			}
			if (3 == m_iEvent_Index)
			{//0.2
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 1.1f;
				EffectWorldDesc.dSpeed = 1.2;
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom, &EffectWorldDesc);
			}
			if (4 == m_iEvent_Index)
			{//0.3
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_SuperArmor_2_Wind", m_pTransformCom);
			}
		}
#pragma endregion 대시펀치 끝
		if (ANIM_AWAKE_PUSHAWAY == m_pModelCom->Get_iCurrentAnimIndex())
		{

			if (0 == m_iEvent_Index) // 0.15
			{
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_Push", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(10.0f, 10.0f, 10.0f), _float3(0.f, 5.0f, 0.0f), dLifeTime,
					CAtkCollider::TYPE_BIG, vMonsterDir, 0.0f);
			}

		}
		if (ANIM_NACHIM == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.05
			{
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.fScale = 2.f;
				CEffectPlayer::Get_Instance()->Play("Akaza_Compass", m_pTransformCom, &EffectWorldDesc);
			}

		}
		if (ANIM_DASH == m_pModelCom->Get_iCurrentAnimIndex())
		{
			dLifeTime = 0.3;
			if (0 == m_iEvent_Index) // 0.01
			{
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 0.75f, 0.0f), dLifeTime,
					CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, 0.0f);
			}

		}
		if (ANIM_AIRGUN == m_pModelCom->Get_iCurrentAnimIndex())
		{
			// 이펙트 추가 난 없음


		}
		if (ANIM_AIRGUN2 == m_pModelCom->Get_iCurrentAnimIndex())
		{

			if (0 == m_iEvent_Index) // 0.2
			{
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_Shoot_Projectile", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 1.5f, 0.75f), dLongLifeTime,
					CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
			}
			if (1 == m_iEvent_Index) // 0.3
			{

				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_Shoot_Projectile", m_pTransformCom);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 1.5f, 0.75f), dLongLifeTime,
					CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");

			}


		}
		if (ANIM_AIRGUN3 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			// 이펙트 추가 난 없음


		}

		if (ANIM_COMBO_DOWN == m_pModelCom->Get_iCurrentAnimIndex())
		{

			if (0 == m_iEvent_Index) // 0.75
			{
				CEffectPlayer::Get_Instance()->Play("Akaza_Stomp_Small", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Akaza_Shockwave_XYZ_Small", m_pTransformCom);
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 0.75f, 0.75f), dLongLifeTime,
					CAtkCollider::TYPE_UPPER, vMonsterDir, m_fUpperDmg);

				Camera_Shake(0.5, 150);
			}

		}
		if (ANIM_COMBO_UP == m_pModelCom->Get_iCurrentAnimIndex())
		{

			if (0 == m_iEvent_Index) // 0.27
			{
				CEffectPlayer::Get_Instance()->Play("ATK_Combo_Up", m_pTransformCom);
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 0.75f, 0.75f), dLongLifeTime,
					CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
			}
			if (1 == m_iEvent_Index) // 0.65
			{
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_Combo_Upper", m_pTransformCom);
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 0.750f, 0.750f), dLongLifeTime,
					CAtkCollider::TYPE_UPPER, vMonsterDir, m_fUpperDmg);
			}


		}
#pragma region 평타콤보
		if (ANIM_COMBO1 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			//dLifeTime = 0.20;
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_Combo_0", m_pTransformCom);
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.5f, 1.5f, 1.5f), _float3(0.f, 1.5f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
			}

		}
		if (ANIM_COMBO2 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			//dLifeTime = 0.20;
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_Combo_1", m_pTransformCom);
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg);
			}

		}
		if (ANIM_COMBO3 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)
			{
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_Combo_2", m_pTransformCom);
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg);
			}
			if (1 == m_iEvent_Index)
			{//0.75
				CEffectPlayer::Get_Instance()->Play("Akaza_ATK_Combo_2_1", m_pTransformCom);
			}
			if (2 == m_iEvent_Index)
			{//0.8
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg);
			}

		}
		if (ANIM_COMBO_PIST == m_pModelCom->Get_iCurrentAnimIndex()) // 기본평타
		{
 			if (0 <= m_iEvent_Index && 10 >= m_iEvent_Index)
			{
				if (0 == m_iEvent_Index || 2 == m_iEvent_Index ||
					5 == m_iEvent_Index || 10 == m_iEvent_Index)
				{//피격 콜라이더
					//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
					Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), dLifeTime,
						CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
				}

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.x = Random::Generate_Float(-1.f, 1.f);
				EffectWorldDesc.vPosition.y = Random::Generate_Float(0.2f, 2.f);

				CEffectPlayer::Get_Instance()->Play("Akaza_Atk_Combo_3", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.x = Random::Generate_Float(-1.f, 1.f);
				EffectWorldDesc.vPosition.y = Random::Generate_Float(0.2f, 2.f);

				CEffectPlayer::Get_Instance()->Play("Akaza_Atk_Combo_3", m_pTransformCom, &EffectWorldDesc);
			}
			


			if (11 == m_iEvent_Index)
			{//0.7
				//tag, size3, Pos3(left, up, front), duration , vDIr, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg);
			 
				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.x = -1.f;
				EffectWorldDesc.fScale = 1.5f;
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 0.0f, 0.5f), 0.5,
					CAtkCollider::TYPE_EFFECT, vMonsterDir, m_fSmallDmg, m_pTransformCom, 4.0, CAtkCollider::TYPE_BULLET, "Akaza_ATK_BulletPunch_Long", &EffectWorldDesc);

				EffectWorldDesc.fScale = 1.f;
				CEffectPlayer::Get_Instance()->Play("Akaza_WindRing", m_pTransformCom, &EffectWorldDesc);

				EffectWorldDesc.vPosition.z = -2.f;
				EffectWorldDesc.fScale = 1.5f;
				EffectWorldDesc.dSpeed = 0.8;

				CEffectPlayer::Get_Instance()->Play("Akaza_WindRing", m_pTransformCom, &EffectWorldDesc);
			}
		}
#pragma endregion 평타콤보 끝
#pragma region 공중장풍
		if (ANIM_JUMPAIRGUN2 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (m_bAwake == true)
			{
				vRandomDir = Random_Dir(vMonsterDir, 15.f, 60.f, -20.f, 20.f); // 조절 하면 될듯~
				// -> Rotation_Dir() 이건 원하는 방향으로 

				if (0 == m_iEvent_Index)
				{

					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vRandomDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
				if (1 == m_iEvent_Index)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vRandomDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
				if (2 == m_iEvent_Index)
				{

					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vRandomDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
				if (3 == m_iEvent_Index)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vRandomDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
				if (4 == m_iEvent_Index)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vRandomDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
				if (5 == m_iEvent_Index)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vRandomDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
			}
			else
			{

				if (0 == m_iEvent_Index)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
				if (1 == m_iEvent_Index)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
				if (2 == m_iEvent_Index)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
				if (3 == m_iEvent_Index)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
				if (4 == m_iEvent_Index)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
				if (5 == m_iEvent_Index)
				{
					//tag, size3, Pos3(left, up, front), duration, atktype, vDir, vSetDir, Dmg, Transform, speed, BulletType, EffTag
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLongLifeTime,
						CAtkCollider::TYPE_SMALL, vDir, m_fSmallDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_BULLET, "Akaza_ATK_Projectile");
				}
			}

		}
#pragma endregion 공중장풍 끝
		m_iEvent_Index++;
	}
}
void CBoss_Akaza::Update_AnimIndex(_uint iAnimIndex)
{
	if (m_eCurAnimIndex != m_ePreAnimIndex)
	{
		m_ePreAnimIndex = m_eCurAnimIndex;
	}
}

void CBoss_Akaza::Update_Hit_Messenger(_double dTimeDelta)
{
	if (m_bNoDmg == false && m_bStart == true)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		_int PlayerIndex = CPlayerManager::GetInstance()->Get_PlayerIndex();
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), PlayerIndex));

		_float4 AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();

		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small() || m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
		{
			if (m_bSuperArmor == false)
			{
				if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small())
				{
					m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
					Trigger_Hit_Small();

				}
				if (m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
				{
					m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
					Trigger_Hit_ConnectSmall();
				}

				if (true == m_isJumpOn)
					Jumping(0.2f, 0.030f);
				/*CEffectPlayer::Get_Instance()->Play("Hit_Spark", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Hit_Shock", m_pTransformCom);*/
			}
			else
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
				m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
			}

			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow())
		{
			if (m_bSuperArmor == false)
			{
				m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
				Trigger_Hit_Blow();
			}
			else
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
			/*CEffectPlayer::Get_Instance()->Play("Hit_Spark", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Hit_Shock", m_pTransformCom);*/
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Upper())
		{
			if (m_bSuperArmor == false)
			{
				m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
				Trigger_Hit_Upper();
			}
			else
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);

			/*CEffectPlayer::Get_Instance()->Play("Hit_Spark", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Hit_Shock", m_pTransformCom);*/
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Big())
		{
			if (m_bSuperArmor == false)
			{
				m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
				if (m_isJumpOn == false)
					Trigger_Hit_Big();
				else
					Trigger_Hit_Blow();
			}
			else
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);

			/*CEffectPlayer::Get_Instance()->Play("Hit_Spark", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Hit_Shock", m_pTransformCom);*/
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Bound())
		{
			if (m_bSuperArmor == false)
			{
				m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
				Trigger_Hit_Bound();
				Set_FallingStatus(3.0f, 0.0f);
			}
			else
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Bound(false);
			
			/*CEffectPlayer::Get_Instance()->Play("Hit_Spark", m_pTransformCom);
				CEffectPlayer::Get_Instance()->Play("Hit_Shock", m_pTransformCom);*/
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_CutScene())
		{

			m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
			Trigger_Hit_CutScene();

			m_pColliderCom[COLL_SPHERE]->Set_Hit_CutScene(false);

			pPlayer->Set_Hit_SurgeCutScene(true);
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		}

		Safe_Release(pGameInstance);
	}
	else
	{
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Spin(false);
		m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
	}

}

void CBoss_Akaza::Update_Trigger(_double dTimeDelta)
{

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

void CBoss_Akaza::Update_State(_double dTimeDelta)
{

	switch (m_eCurstate)
	{
	case CBoss_Akaza::STATE_IDLE:
		Update_Interact(dTimeDelta);
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
		break;
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
	case CBoss_Akaza::STATE_CINEMATIC:
		Update_Awake_Cinematic(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_HIT_SMALL:
		Update_Hit_Small(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_HIT_UPPER:
		Update_Hit_Upper(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_HIT_BIG:
		Update_Hit_Big(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_HIT_BLOW:
		Update_Hit_Blow(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_HIT_BIGBLOW:
		Update_Hit_BigBlow(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_HIT_BOUND:
		Update_Hit_Bound(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_SPIN:
		Update_Hit_Spin(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_HIT_CUTSCENE:
		Update_Hit_CutScene(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_GETUP:
		Update_Hit_GetUp(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_ROLLGETUP:
		Update_Hit_RollGetUp(dTimeDelta);
		break;
	case CBoss_Akaza::STATE_BIGGETUP:
		Update_Hit_BigGetUp(dTimeDelta);
		break;


	}

}

void CBoss_Akaza::Update_Interact(_double dTimeDelta)
{
	if (m_eCurPhase == BEGIN)
	{
		Trigger_Begin();
	}

	if (m_eCurPhase == PHASE_1)
	{
		Trigger_Interact_Phase_1(dTimeDelta);
	}
	if (m_eCurPhase == PHASE_2)
	{
		Trigger_Interact_Phase_2(dTimeDelta);
	}
	if (m_eCurPhase == PHASE_3)
	{
		Trigger_Interact_Phase_3(dTimeDelta);
	}

}

void CBoss_Akaza::Update_Begin(_double dTimeDelta)
{
	// 조건 주면 시작
	m_dTriggerTime += dTimeDelta;

	if (m_dTriggerTime > 15.0)
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_BATTLE_START;
		}

		if (m_pModelCom->Get_AnimFinish(ANIM_BATTLE_START))
		{
			m_dTriggerTime = 0.0;
			m_eCurPhase = PHASE_1;
			m_eCurAnimIndex = ANIM_IDLE;
			m_bStart = true;
			m_iTriggerCnt = 1;
			Trigger_Interact();

		}
	}

}

void CBoss_Akaza::Update_Phase_1(_double dTimeDelta)
{
	if (m_StatusDesc.fHp <= 0.f && m_bNoDmg == false)
	{
		m_StatusDesc.fHp = 0.f;
		m_bTrigger = false;
		m_bPatternStart = false;
		m_bNoDmg = true;
		m_iTriggerCnt = 5;
		m_dTriggerTime = 0.0;
		m_iIdleCnt = 0;
	}

	if (m_bTrigger == false)
	{
		if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) > 0.5f)
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
		if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) <= 0.5f)
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

void CBoss_Akaza::Update_Phase_2(_double dTimeDelta)
{
	// 개방하고 25초 유지 m_bAwake가 활성되면 25초 후에 다시 false
	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) < 0.7f && m_bFirstAwake == false)
	{

		m_bFirstAwake = true;
		m_bAwake = true;
		m_bNoDmg = true;
		m_bTrigger = false;
		m_iTriggerCnt = 0;
		m_dAwakeTime = 0.0;

		m_bPatternStart = false;
		m_dTriggerTime = 0.0;
		m_iIdleCnt = 0;

	}
	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) <= 0.3f && m_bSecondAwake == false)
	{

		m_bSecondAwake = true;
		m_bAwake = true;
		m_bNoDmg = true;
		m_bTrigger = false;
		m_iTriggerCnt = 0;

		m_dAwakeTime = 0.0;

		m_bPatternStart = false;
		m_dTriggerTime = 0.0;
		m_iIdleCnt = 0;

	}
	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) <= 0.0f)
	{
		m_bNoDmg = true;
		m_StatusDesc.fHp = 0.f;
		m_eCurPhase = PHASE_3;

	}
	if (m_bAwake == true)
	{
		m_dAwakeTime += dTimeDelta;
		if (24.5 < m_dAwakeTime && m_dAwakeTime <= 24.5 + dTimeDelta)
			m_pRendererCom->Set_Invert();
		if (m_dAwakeTime > 25.0)
		{
			m_dAwakeTime = 0.0;
			m_bAwake = false;
			m_pRendererCom->Set_Invert();
			m_fOutlineThickness = 2.0f;

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

void CBoss_Akaza::Update_Phase_3(_double dTimeDelta)
{
}

void CBoss_Akaza::Trigger_Interact_Phase_1(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_IDLE;

	if (m_StatusDesc.fHp > 50.f)
	{
		if (m_bPatternStart == false)
		{
			if (m_pModelCom->Check_PickAnimRatio(ANIM_IDLE, 0.30, dTimeDelta))
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
			if (1.00 < m_dTriggerTime && m_dTriggerTime <= 1.00 + dTimeDelta)
				m_iIdleCnt++;

			if (m_iIdleCnt == 1)
			{
				m_dTriggerTime = 0.0;
				m_iIdleCnt = 0;
				m_iRandomPatternNum = Random::Generate_Int(1, 10);


				if (fDistance <= 5.f)
				{
					if (1 <= m_iRandomPatternNum && m_iRandomPatternNum < 7)
					{
						m_iTriggerCnt = 5;
						m_bTrigger = false;
					}
					if (7 <= m_iRandomPatternNum && m_iRandomPatternNum < 10)
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
			if (m_pModelCom->Check_PickAnimRatio(ANIM_IDLE, 0.30, dTimeDelta))
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
	
}

void CBoss_Akaza::Trigger_Interact_Phase_2(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_IDLE;

	if (m_bAwake == false)
	{
		if (m_bPatternStart == false)
		{
			m_dTriggerTime += dTimeDelta;
			if (0.60 < m_dTriggerTime && m_dTriggerTime <= 0.60 + dTimeDelta)
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
			if (0.60 < m_dTriggerTime && m_dTriggerTime <= 0.60 + dTimeDelta)
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
					if (1 <= m_iRandomPatternNum && m_iRandomPatternNum < 9)
					{
						m_iTriggerCnt = 8;
						m_bTrigger = false;
					}
					if (9 <= m_iRandomPatternNum && m_iRandomPatternNum <= 12)
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

}

void CBoss_Akaza::Trigger_Interact_Phase_3(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_IDLE;
}

void CBoss_Akaza::Trigger_Interact()
{
	m_bTrigger = true;
	m_bAir_Motion = false;
	m_bSuperArmor = false;
	m_eCurstate = STATE_IDLE;

}

void CBoss_Akaza::Trigger_Begin()
{
	m_eCurstate = STATE_BEGIN;
	m_bAnimFinish = false;
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
	m_pModelCom->Set_AnimisFinish(ANIM_SUPERARMOR);
	m_pModelCom->Set_AnimisFinish(ANIM_SUPERARMOR2);
	m_pModelCom->Set_AnimisFinish(ANIM_SUPERARMOR3);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);
	m_bRunPunch = false;
	m_bStep_B = false;
	m_bDashOn = false;
	m_bCharge = false;
	m_bMove = false;
	m_bSuperArmor = true;
}

void CBoss_Akaza::Trigger_Guard()
{
	m_bTrigger = true;
}

void CBoss_Akaza::Trigger_AirGun()
{
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_iLoopCount = 0;
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
	m_bNoDmg = true;
	m_bAnimFinish = false;
	m_eCurstate = STATE_PUSHAWAY;
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
}

void CBoss_Akaza::Trigger_Nachim()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_NACHIM;
	m_pModelCom->Set_AnimisFinish(ANIM_NACHIM);
	m_bSuperArmor = true;
}

void CBoss_Akaza::Trigger_ComboPunch()
{
	m_bTrigger = true;
}

void CBoss_Akaza::Trigger_JumpStomp()
{
	m_bTrigger = true;
	m_eCurstate = STATE_JUMPSTOMP;
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_SKILL_UP);
	m_pModelCom->Set_AnimisFinish(ANIM_SKILL_UP);
	m_pModelCom->Set_AnimisFinish(ANIM_SKILL_DOWN);
	m_pModelCom->Set_AnimisFinish(ANIM_SKILL_DOWNEND);
	m_bAnimFinish = false;
	m_dJumpStompTime = 0.0;

}

void CBoss_Akaza::Trigger_DashKick()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_DASHKICK;
	m_pModelCom->Set_AnimisFinish(ANIM_DASH);
	m_pModelCom->Set_AnimisFinish(ANIM_COMBO_DOWN);
	m_bMove = false;
}

void CBoss_Akaza::Trigger_JumpAirGun()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_JUMPAIRGUN;
	m_bJump = false;
	m_bAnimFinish = false;
	m_iRandomDirNum = Random::Generate_Int(1, 3);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_LEFT);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_RIGHT);
	m_pModelCom->Set_AnimisFinish(ANIM_JUMPAIRGUN);
	m_pModelCom->Set_AnimisFinish(ANIM_JUMPAIRGUN2);
	m_pModelCom->Set_AnimisFinish(ANIM_JUMPAIRGUN3);
	m_pModelCom->Set_AnimisFinish(ANIM_JUMPLOOP);
	m_pModelCom->Set_AnimisFinish(ANIM_JUMPLAND);
}

void CBoss_Akaza::Trigger_Dash_ComboPunch()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_DASH_COMBO_PUNCH;
	m_bAnimFinish = false;
	m_pModelCom->Set_AnimisFinish(ANIM_COMBO1);
	m_pModelCom->Set_AnimisFinish(ANIM_COMBO2);
	m_pModelCom->Set_AnimisFinish(ANIM_COMBO3);
	m_pModelCom->Set_AnimisFinish(ANIM_COMBO_PIST);
	m_dTimeAcc = 0.0;
}

void CBoss_Akaza::Trigger_UpperKick()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_UPPERKICK;
	m_pModelCom->Set_AnimisFinish(ANIM_DASH);
	m_pModelCom->Set_AnimisFinish(ANIM_COMBO_UP);
	m_bMove = false;
	m_bSuperArmor = true;
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

	m_iLoopCount = 0;
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
	m_pModelCom->Set_AnimisFinish(ANIM_NACHIM);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_Start);

	m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);
	m_pModelCom->Set_AnimisFinish(ANIM_HEAL);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_LOOP);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_END);

	Pos_FixY();

}

void CBoss_Akaza::Trigger_Heal()
{
	m_bTrigger = true;
	m_eCurstate = STATE_HEAL;
	m_bAnimFinish = false;
	m_pModelCom->Set_AnimisFinish(ANIM_HEAL);

}

void CBoss_Akaza::Trigger_Awake()
{
	m_bTrigger = true;
	m_eCurstate = STATE_AWAKE;
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_START);
	m_pModelCom->Set_AnimisFinish((ANIM_AWAKE_END));
	m_bAnimFinish = false;
	m_bAwake = true;
	Pos_FixY();
}

void CBoss_Akaza::Trigger_Awake_ComboPunch()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_AWAKE_COMBOPUNCH;
	m_bDashOn = false;
	m_bStep_B = false;
	m_bMove = false;
	m_iLoopCount = 0;
	m_pModelCom->Set_AnimisFinish(ANIM_DASH);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_Start);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_END);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);
}

void CBoss_Akaza::Trigger_Nachim_ComboPunch()
{
	m_bTrigger = true;
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_eCurstate = STATE_NACHIM_COMBOPUNCH;
	m_bAnimFinish = false;
	m_bStep_B = false;
	m_iLoopCount = 0;

	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_Start);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_LOOP);
	m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_END);
	m_pModelCom->Set_AnimisFinish(ANIM_STEP_BEHIND);

}

void CBoss_Akaza::Trigger_Nachim_AirGun()
{

}

void CBoss_Akaza::Trigger_Awake_Cinematic()
{
	m_bTrigger = true;
	m_eCurstate = STATE_CINEMATIC;

	m_bAnimFinish = false;
	m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC1);
	m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC2);
	m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC3);
	m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC4);
	m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC5);
	m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC6);
	m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC7);
	m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC8);
	m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC9);
	m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC10);
}

void CBoss_Akaza::Trigger_Hit_Small()
{

	m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
	m_isConnectHitting = false;
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_iSmallHit_Index++;
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_FRONT);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_LEFT);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_RIGHT);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN_SMALL);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_SMALL_AIR);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_AIR_UPPER);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_END);
	m_eCurstate = STATE_HIT_SMALL;
}

void CBoss_Akaza::Trigger_Hit_ConnectSmall()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(false);
	m_isConnectHitting = true;
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_iSmallHit_Index++;
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_FRONT);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_LEFT);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_RIGHT);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN_SMALL);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_SMALL_AIR);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_AIR_UPPER);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_END);
	m_eCurstate = STATE_HIT_SMALL;
}

void CBoss_Akaza::Trigger_Hit_Upper()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(false);
	m_bTrigger = true;
	m_bAnimFinish = false;

	m_pModelCom->Set_AnimisFinish(ANIM_HIT_AIR_UPPER);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_END);
	m_eCurstate = STATE_HIT_UPPER;
	Jumping(1.5f, 0.03f);
}

void CBoss_Akaza::Trigger_Hit_Big()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_POWER_GUARDBREAK);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN_SMALL);
	m_eCurstate = STATE_HIT_BIG;
}

void CBoss_Akaza::Trigger_Hit_Blow()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(false);
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(false);
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_LOOP);
	m_pModelCom->Set_AnimisFinish((ANIM_HIT_BLOW_END));
	m_eCurstate = STATE_HIT_BLOW;
}

void CBoss_Akaza::Trigger_Hit_BigBlow()
{

}

void CBoss_Akaza::Trigger_Hit_Bound()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Bound(false);
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_isBounding = true;
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_LOOP);
	m_pModelCom->Set_AnimisFinish((ANIM_HIT_BLOW_END));
	m_eCurstate = STATE_HIT_BOUND;
}

void CBoss_Akaza::Trigger_Hit_Spin()
{
}

void CBoss_Akaza::Trigger_Hit_CutScene()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_CutScene(false);
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_bNoDmg = true;

	/*m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_LOOP);
	m_pModelCom->Set_AnimisFinish((ANIM_HIT_BLOW_END));*/
	m_eCurstate = STATE_HIT_CUTSCENE;
}

void CBoss_Akaza::Trigger_Hit_GetUp()
{
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_bNoDmg = true;
	m_bAir_Motion = false;
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_GETUP_DIZZY);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN_BIG);

	m_eCurstate = STATE_GETUP;
}

void CBoss_Akaza::Trigger_Hit_RollGetUp()
{
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_bNoDmg = true;
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_DOWNLOOP);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_GETUP_SPIN);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_GETUP_SPIN2);
	m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN_SMALL);
	m_iRandomDirNum = Random::Generate_Int(1, 3);
	m_eCurstate = STATE_ROLLGETUP;
}

void CBoss_Akaza::Trigger_Hit_BigGetUp()
{
}

void CBoss_Akaza::Update_Escape(_double dTimeDelta)
{
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	if (m_eCurPhase == PHASE_1)
	{
		if (m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND) == false)
			m_eCurAnimIndex = ANIM_STEP_BEHIND;
	}

	if (m_iRandomDirNum == 1)
	{
		if (m_eCurPhase == PHASE_1)
		{
			if (m_bStep_B == false && m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND))
			{
				m_bStep_B = true;
				m_eCurAnimIndex = ANIM_STEP_LEFT;
			}
		}
		if (m_eCurPhase == PHASE_2)
		{
			if (m_bStep_B == false)
			{
				m_bStep_B = true;
				m_eCurAnimIndex = ANIM_STEP_LEFT;
			}
		}

		if (m_pModelCom->Check_PickAnimRatio(ANIM_STEP_LEFT, 0.950, dTimeDelta))
			m_eCurAnimIndex = ANIM_STEP_LEFT2;
	}
	if (m_iRandomDirNum == 2)
	{
		if (m_eCurPhase == PHASE_1)
		{
			if (m_bStep_B == false && m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND))
			{
				m_bStep_B = true;
				m_eCurAnimIndex = ANIM_STEP_RIGHT;
			}
		}
		if (m_eCurPhase == PHASE_2)
		{
			if (m_bStep_B == false)
			{
				m_bStep_B = true;
				m_eCurAnimIndex = ANIM_STEP_RIGHT;
			}
		}
		if (m_pModelCom->Check_PickAnimRatio(ANIM_STEP_RIGHT, 0.950, dTimeDelta))
			m_eCurAnimIndex = ANIM_STEP_RIGHT2;
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_STEP_LEFT2) || m_pModelCom->Get_AnimFinish(ANIM_STEP_RIGHT2))
	{
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}

	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_STEP_BEHIND, 3.0f, 0.0, 0.7);
	Go_Dir_Constant(dTimeDelta, DIR_LEFT, ANIM_STEP_LEFT, 2.0f, 0.0, 0.7);
	Go_Dir_Constant(dTimeDelta, DIR_LEFT, ANIM_STEP_LEFT2, 2.0f, 0.0, 0.7);
	Go_Dir_Constant(dTimeDelta, DIR_RIGHT, ANIM_STEP_RIGHT, 2.0f, 0.0, 0.7);
	Go_Dir_Constant(dTimeDelta, DIR_RIGHT, ANIM_STEP_RIGHT2, 2.0f, 0.0, 0.7);

}

void CBoss_Akaza::Update_DashPunch(_double dTimeDelta)
{
	_vector vDir = Calculate_Dir_FixY();
	_float fDistance = Calculate_Distance();

	if (fDistance > 10.f && m_bStep_B == false && m_bDashOn == false)
	{
		m_pTransformCom->LerpVector(vDir, 1.0f);
		m_pTransformCom->Go_Straight(dTimeDelta, m_pNavigationCom[m_eCurNavi]);
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
			m_bDashOn = true;
		}
	}
	if (m_bDashOn == true)
	{
		////애니메이션 제어

		// 수정
		if (m_bStep_B == false)
		{
			if (m_pModelCom->Get_AnimFinish(ANIM_SUPERARMOR) == false)
				m_eCurAnimIndex = ANIM_SUPERARMOR;
		}
		if ((m_pModelCom->Get_AnimFinish(ANIM_SUPERARMOR) == true && m_bCharge == false) || (m_bStep_B == true && m_bCharge == false))
		{
			m_bCharge = true;
			m_eCurAnimIndex = ANIM_SUPERARMOR2;
		}

		if (m_pModelCom->Get_AnimFinish(ANIM_SUPERARMOR2) == true)
		{
			m_pModelCom->Set_AnimisFinish(ANIM_SUPERARMOR2);
			m_eCurAnimIndex = ANIM_SUPERARMOR3;
		}

		if (m_pModelCom->Get_AnimFinish(ANIM_SUPERARMOR3) == true)
		{
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
		}

		//행동제어
		if (m_eCurAnimIndex != ANIM_SUPERARMOR3 && m_eCurAnimIndex != ANIM_IDLE)
		{
			m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		}

		if (Check_Distance(2.f) == true)
			m_bMove = true;

		if (Check_Distance(2.f) == false && m_bMove == false)
			Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_SUPERARMOR3, 10.f, 0.01, 0.4);

	}

}

void CBoss_Akaza::Update_Guard(_double dTimeDelta)
{

}

void CBoss_Akaza::Update_AirGun(_double dTimeDelta)
{

	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_AIRGUN;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_AIRGUN) == true)
	{
		if (m_iLoopCount < 3)
			m_eCurAnimIndex = ANIM_AIRGUN2;

		if (m_pModelCom->Check_PickAnimRatio(ANIM_AIRGUN2, 0.90, dTimeDelta))
			m_iLoopCount++;
		if (m_iLoopCount >= 3 && m_pModelCom->Get_AnimFinish(ANIM_AIRGUN2) == true)
			m_eCurAnimIndex = ANIM_AIRGUN3;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_AIRGUN3) == true)
	{
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}

	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));

}

void CBoss_Akaza::Update_PushAway(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_AWAKE_PUSHAWAY;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_PUSHAWAY))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);
		m_bNoDmg = false;
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
}

void CBoss_Akaza::Update_Nachim(_double dTimeDelta)
{
	_float fDistance = Calculate_Distance();

	m_eCurAnimIndex = ANIM_NACHIM;
	if (m_bAwake == false)
	{
		//수정 Distance -> 영역 거리 수정해야할듯
		if (m_pModelCom->Check_PickAnimRatio(ANIM_NACHIM, 0.95, dTimeDelta) && fDistance > 10.f)
		{
			Trigger_AirGun();
		}
		if (m_pModelCom->Check_PickAnimRatio(ANIM_NACHIM, 0.95, dTimeDelta) && fDistance <= 10.f)
		{
			Trigger_Nachim_ComboPunch();

		}
	}
	// 개방하면 범위 늘어남 
	if (m_bAwake == true)
	{
		if (m_pModelCom->Check_PickAnimRatio(ANIM_NACHIM, 0.95, dTimeDelta) && fDistance > 20.f)
		{
			Trigger_AirGun();
		}
		if (m_pModelCom->Check_PickAnimRatio(ANIM_NACHIM, 0.95, dTimeDelta) && fDistance <= 20.f)
		{
			Trigger_Nachim_ComboPunch();

		}
	}
}

void CBoss_Akaza::Update_ComboPunch(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_COMBO_PIST;

	if (m_pModelCom->Get_AnimFinish(ANIM_COMBO_PIST) == true)
		m_eCurAnimIndex = ANIM_COMBO_PIST2;


	if (m_pModelCom->Get_AnimFinish(ANIM_COMBO_PIST2) == true)
	{
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
}

void CBoss_Akaza::Update_JumpStomp(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_SKILL_UP;
	}

	if (m_pModelCom->Check_PickAnimRatio(ANIM_SKILL_UP, 0.65f, dTimeDelta))
	{
		Jumping(12.0f, 0.1f);

	}
	if (m_pModelCom->Get_AnimFinish(ANIM_SKILL_UP) == true)
	{

		m_dJumpStompTime += dTimeDelta;

		if (m_dJumpStompTime <= 3.10)
			m_eCurAnimIndex = ANIM_SKILL_DOWN;

		if (0.10 < m_dJumpStompTime && m_dJumpStompTime <= 0.10 + dTimeDelta)
			JumpStop(3.0);
		if (m_dJumpStompTime <= 3.0)
		{
			if (Check_Distance_FixY(5.f) == false)
				m_pTransformCom->Chase_Target_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION), dTimeDelta, 1.50);
		}

		if (m_dJumpStompTime > 3.10)
		{
			m_pTransformCom->Go_Down(dTimeDelta * 15.0);

			// 땅에 떨어졌을 때 임시코드
			_float4 Pos;
			XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			if (Pos.y <= m_fLand_Y)
			{
				m_eCurAnimIndex = ANIM_SKILL_DOWNEND;

				if (m_pModelCom->Check_PickAnimRatio(ANIM_SKILL_DOWNEND, 0.10, dTimeDelta))
				{
					if (m_bAwake == true)
					{
						CEffectPlayer::Get_Instance()->Play("Akaza_Stomp_Big", m_pTransformCom);
						CEffectPlayer::Get_Instance()->Play("Akaza_Shockwave_Big", m_pTransformCom);
						Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(15.0f, 15.0f, 15.0f), _float3(0.f, 0.0f, 0.0f), 0.2,
							CAtkCollider::TYPE_BLOW, m_pTransformCom->Get_State(CTransform::STATE_LOOK), 10.f);
					}
					else
					{
						CEffectPlayer::Get_Instance()->Play("Akaza_Stomp_Medium", m_pTransformCom);
						CEffectPlayer::Get_Instance()->Play("Akaza_Shockwave_Medium", m_pTransformCom);
						Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(10.0f, 10.0f, 10.0f), _float3(0.f, 0.0f, 0.0f), 0.2,
							CAtkCollider::TYPE_BLOW, m_pTransformCom->Get_State(CTransform::STATE_LOOK), 5.f);
					}
					Camera_Shake();

				}

				Pos.y = m_fLand_Y;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));

			}
			if (m_pModelCom->Get_AnimFinish(ANIM_SKILL_DOWNEND))
			{
				m_eCurAnimIndex = ANIM_IDLE;
				Trigger_Interact();
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
	{
		m_bSuperArmor = true;
		m_eCurAnimIndex = ANIM_COMBO_DOWN;
	}


	if (m_pModelCom->Get_AnimFinish(ANIM_COMBO_DOWN) == true)
	{
		m_eCurAnimIndex = ANIM_IDLE;

		if (Check_Player_Y() == true)
			Trigger_Awake_ComboPunch();
		else
			Trigger_Interact();
	}

	if (Check_Distance(2.f) == true)
		m_bMove = true;

	if (Check_Distance(2.f) == false && m_bMove == false)
		Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_DASH, 10.f, 0.01, 1.0);





}

void CBoss_Akaza::Update_JumpAirGun(_double dTimeDelta)
{
	if (m_iRandomDirNum == 1)
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_STEP_LEFT;
		}
	}
	if (m_iRandomDirNum == 2)
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_STEP_RIGHT;
		}
	}
	if (m_iRandomDirNum == 3)
	{
		if (m_bJump == false)
		{
			m_bJump = true;
			m_eCurAnimIndex = ANIM_JUMPAIRGUN;
			Jumping(5.0f, 0.2f);
		}
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_STEP_RIGHT) || m_pModelCom->Get_AnimFinish(ANIM_STEP_LEFT))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_STEP_RIGHT);
		m_pModelCom->Set_AnimisFinish(ANIM_STEP_LEFT);
		m_eCurAnimIndex = ANIM_JUMPAIRGUN;
		Jumping(5.0f, 0.2f);

	}

	if (m_pModelCom->Check_PickAnimRatio(ANIM_JUMPAIRGUN, 0.25, dTimeDelta))
		JumpStop(2.0);

	if (m_pModelCom->Check_PickAnimRatio(ANIM_JUMPAIRGUN, 0.95, dTimeDelta))
	{
		m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		m_eCurAnimIndex = ANIM_JUMPAIRGUN2;
	}

	if (m_pModelCom->Check_PickAnimRatio(ANIM_JUMPAIRGUN2, 0.95, dTimeDelta))
	{
		m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		m_eCurAnimIndex = ANIM_JUMPAIRGUN3;
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_JUMPAIRGUN3))
	{
		//수정 네비 나오면
		m_eCurAnimIndex = ANIM_JUMPLOOP;
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		if (Pos.y <= m_fLand_Y)
		{
			m_eCurAnimIndex = ANIM_JUMPLAND;
			Pos.y = m_fLand_Y;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_JUMPLAND))
		{
			m_eCurAnimIndex = ANIM_IDLE;
			m_eCurstate = STATE_IDLE;
		}
	}
	Go_Dir_Constant(dTimeDelta, DIR_LEFT, ANIM_STEP_LEFT, 3.0, 0.0, 0.70);
	Go_Dir_Constant(dTimeDelta, DIR_RIGHT, ANIM_STEP_RIGHT, 3.0, 0.0, 0.70);
	if (m_iRandomDirNum == 1)
		Go_Dir_Constant(dTimeDelta, DIR_LEFT, ANIM_JUMPAIRGUN, 1.0, 0.0, 0.7);
	if (m_iRandomDirNum == 2)
		Go_Dir_Constant(dTimeDelta, DIR_RIGHT, ANIM_JUMPAIRGUN, 1.0, 0.0, 0.7);
}

void CBoss_Akaza::Update_Dash_ComboPunch(_double dTimeDelta)
{
	_vector vDir = Calculate_Dir_FixY();
	_float fDistance = Calculate_Distance();

	if (fDistance > 5.f && m_bAnimFinish == false)
	{
		m_pTransformCom->Go_Straight(dTimeDelta, m_pNavigationCom[m_eCurNavi]);
		m_eCurAnimIndex = ANIM_RUN;
	}
	else
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_COMBO1;
			m_pTransformCom->LerpVector(vDir, 1.f);
		}

		if (m_pModelCom->Check_PickAnimRatio(ANIM_COMBO1, 0.80, dTimeDelta))
		{
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_COMBO1);
			m_eCurAnimIndex = ANIM_COMBO2;
			m_pTransformCom->LerpVector(vDir, 1.f);
		}

		if (m_pModelCom->Check_PickAnimRatio(ANIM_COMBO2, 0.80, dTimeDelta))
		{
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_COMBO2);
			m_eCurAnimIndex = ANIM_COMBO3;
			m_pTransformCom->LerpVector(vDir, 1.f);
		}

		if (m_pModelCom->Check_PickAnimRatio(ANIM_COMBO3, 0.80, dTimeDelta))
		{
			m_pModelCom->Set_AnimResetTimeAcc(ANIM_COMBO3);
			m_eCurAnimIndex = ANIM_COMBO_PIST;
			m_pTransformCom->LerpVector(vDir, 1.f);
			m_bTrigger = true;
		}

		if (m_pModelCom->Get_AnimFinish(ANIM_COMBO_PIST))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_COMBO_PIST);
			
			if (m_eCurPhase == PHASE_1)
			{
				m_iRandomPatternNum++;
				if (m_iRandomPatternNum >= 4)
					m_iRandomPatternNum = 1;

				if (m_iRandomPatternNum == 1)
				{
					m_eCurAnimIndex = ANIM_IDLE;
					Trigger_UpperKick();
				}

				if (m_iRandomPatternNum == 2)
				{
					m_eCurAnimIndex = ANIM_IDLE;
					Trigger_DashPunch();
				}

				if (m_iRandomPatternNum == 3)
				{
					m_eCurAnimIndex = ANIM_IDLE;
					Trigger_Nachim();
				}
			}
			else if (m_eCurPhase == PHASE_2)
			{
				m_iRandomPatternNum++;
				if (m_iRandomPatternNum >= 5)
					m_iRandomPatternNum = 1;

				if (m_iRandomPatternNum == 1)
				{
					m_eCurAnimIndex = ANIM_IDLE;
					Trigger_UpperKick();
				}

				if (m_iRandomPatternNum == 2)
				{
					m_eCurAnimIndex = ANIM_IDLE;
					Trigger_DashKick();
				}

				if (m_iRandomPatternNum == 3)
				{
					m_eCurAnimIndex = ANIM_IDLE;
					Trigger_JumpStomp();
				}

				if (m_iRandomPatternNum == 4)
				{
					m_eCurAnimIndex = ANIM_IDLE;
					Trigger_JumpAirGun();
				}
			}
		}
	}
	if (m_eCurAnimIndex == ANIM_RUN)
		m_pTransformCom->LerpVector(vDir, 0.3f);

	//==============================================================================

	//_vector vMonsterDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	//if (m_eCurAnimIndex == ANIM_COMBO_PIST)
	//{
		//m_dTimeAcc += dTimeDelta;
		///////////////////////////////////////////어택콜라이더 0.70 까지///////////////////////////////////////////////////////////
		// Event_Time(타임델타, 부를시간, 누적시작)
		//if (Event_Time(dTimeDelta,0.250, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.30, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.39, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.520, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.70, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		/////////////////////////////////////////////이펙트 여기 아래부터//////////////////////////////////////////////////////////////////
		//if (Event_Time(dTimeDelta,0.250, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.270, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.30, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//
		//if (Event_Time(dTimeDelta,0.320, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.340, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.360, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.390, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.410, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.430, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.450, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.470, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.490, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.520, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.540, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.560, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.580, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.60, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.620, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
		//if (Event_Time(dTimeDelta,0.640, m_dTimeAcc))
		//{
		//	Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.5f, 1.5f), 0.2,
		//		CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);
		//}
	//}

	//==============================================================================


	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_COMBO1, 2.f, 0.0, 0.3);
	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_COMBO2, 1.5f, 0.0, 0.5);
	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_COMBO3, 1.5f, 0.0, 0.5);
	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_COMBO_PIST, 2.5f, 0.0, 0.2);

}

void CBoss_Akaza::Update_UpperKick(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimFinish(ANIM_DASH) == false)
	{
		m_eCurAnimIndex = ANIM_DASH;

	}

	if (m_pModelCom->Get_AnimFinish(ANIM_DASH) == true)
		m_eCurAnimIndex = ANIM_COMBO_UP;

	if (Check_Player_Y() == false)
	{

		if (m_pModelCom->Get_AnimFinish(ANIM_COMBO_UP) == true)
		{
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
		}
	}
	else
	{
		if (m_pModelCom->Check_PickAnimRatio(ANIM_COMBO_UP, 0.75, dTimeDelta))
		{
			m_eCurAnimIndex = ANIM_IDLE;

			Trigger_Awake_ComboPunch();
		}
	}


	if (Check_Distance(2.f) == true)
		m_bMove = true;

	if (Check_Distance(2.f) == false && m_bMove == false)
		Go_Straight_Constant(dTimeDelta, ANIM_DASH, 10.f); // 이펙트수정


}

void CBoss_Akaza::Update_NextPhase2(_double dTimeDelta)
{
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_PUSHAWAY) == false)
		m_eCurAnimIndex = ANIM_AWAKE_PUSHAWAY;

	if (m_pModelCom->Check_PickAnimRatio(ANIM_AWAKE_PUSHAWAY, 0.95, dTimeDelta))
		Trigger_Heal();
}

void CBoss_Akaza::Update_NextPhase3(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_AWAKE_PUSHAWAY;
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_PUSHAWAY) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_PUSHAWAY);

		m_eCurAnimIndex = ANIM_NACHIM;
	}

	if (m_pModelCom->Check_PickAnimRatio(ANIM_NACHIM, 0.95, dTimeDelta) &&
		m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_Start) == false)
	{

		m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vDir = Calculate_Dir_FixY();
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

		if (m_iLoopCount < 3)
			m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_LOOP;

		if (m_pModelCom->Check_PickAnimRatio(ANIM_AWAKE_COMBOPUNCH_LOOP, 0.90, dTimeDelta))
			m_iLoopCount++;

		if (m_iLoopCount >= 3)
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
			if (m_pModelCom->Check_PickAnimRatio(ANIM_STEP_BEHIND, 0.95, dTimeDelta))
			{
				Trigger_Heal();
				//m_eCurAnimIndex = ANIM_HEAL;
			}


		}
	}

	if (m_pModelCom->Get_AnimRatio(ANIM_AWAKE_COMBOPUNCH_END, 0.30))
		Go_Straight_Constant(dTimeDelta, ANIM_AWAKE_COMBOPUNCH_END, 1.f);

	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_STEP_BEHIND, 3.0f, 0.0, 0.70);
	Pos_FixY();

}

void CBoss_Akaza::Update_Heal(_double dTimeDelta)
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

			m_StatusDesc.fHp = m_StatusDesc.fHp_Max * (_float)dRatio;
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
		m_bNoDmg = false;

		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();

	}
}

void CBoss_Akaza::Update_Awake(_double dTimeDelta)
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
		CCameraManager::GetInstance()->Set_Is_Cut_In_On(true);
		CCameraManager::GetInstance()->Set_Cut_In_Finish_Type(CCamera_Free::AKAZA_AWAKE);
	}
	if (m_pModelCom->Check_PickAnimRatio(ANIM_AWAKE_START, 0.990, dTimeDelta))
	{
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_AWAKE_START);
		m_eCurAnimIndex = ANIM_AWAKE_END;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_END) == true)
	{
		m_bNoDmg = false;
		m_pModelCom->Get_AnimFinish(ANIM_AWAKE_END);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
	Pos_FixY();
}

void CBoss_Akaza::Update_Awake_ComboPunch(_double dTimeDelta)
{
	if (m_bDashOn == false)
	{
		m_bDashOn = true;
		m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		m_eCurAnimIndex = ANIM_DASH;
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_DASH))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_DASH);

		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_READY;

	}

	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_READY) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_READY);
		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_Start;
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_Start) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_Start);
		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH;
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH) == true)
	{

		if (m_iLoopCount < 3)
			m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_LOOP;

		if (m_pModelCom->Check_PickAnimRatio(ANIM_AWAKE_COMBOPUNCH_LOOP, 0.90, dTimeDelta))
			m_iLoopCount++;

		if (m_iLoopCount >= 3)
		{
			if (m_bStep_B == false)
			{
				m_bStep_B = true;
				m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_END;
			}

			if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_END) == true)
			{
				m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_END);
				m_eCurAnimIndex = ANIM_STEP_BEHIND;
			}

			//if (m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND) == true)
			if (m_pModelCom->Check_PickAnimRatio(ANIM_STEP_BEHIND, 0.950, dTimeDelta))
			{
				m_eCurAnimIndex = ANIM_IDLE;
				Trigger_Interact();
			}

		}
	}


	if (m_pModelCom->Get_AnimRatio(ANIM_AWAKE_COMBOPUNCH_END, 0.30))
		Go_Straight_Constant(dTimeDelta, ANIM_AWAKE_COMBOPUNCH_END, 1.f);

	if (Check_Distance_FixY(2.f) == true)
		m_bMove = true;

	if (Check_Distance_FixY(2.f) == false && m_bMove == false)
		Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_DASH, 10.f, 0.01, 1.0);


	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_STEP_BEHIND, 3.0f, 0.2, 0.70);

}

void CBoss_Akaza::Update_Nachim_ComboPunch(_double dTimeDelta)
{
	//순간이동 // awake펀치 // 힐

	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vDir = Calculate_Dir_FixY();
		_float fDistance = Calculate_Distance() - 2.f;
		_vector vChangePos = vMonsterPos + (vDir * fDistance);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vChangePos);
		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_READY;

	}
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_READY) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_READY);
		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_Start;
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_Start) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_Start);
		m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH;
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH) == true)
	{

		if (m_iLoopCount < 3)
			m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_LOOP;

		if (m_pModelCom->Check_PickAnimRatio(ANIM_AWAKE_COMBOPUNCH_LOOP, 0.98, dTimeDelta))
			m_iLoopCount++;

		if (m_iLoopCount >= 3)
		{
			if (m_bStep_B == false)
			{
				m_bStep_B = true;
				m_eCurAnimIndex = ANIM_AWAKE_COMBOPUNCH_END;
			}

			if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE_COMBOPUNCH_END) == true)
			{
				m_pModelCom->Set_AnimisFinish(ANIM_AWAKE_COMBOPUNCH_END);
				m_eCurAnimIndex = ANIM_STEP_BEHIND;
			}

			//if (m_pModelCom->Get_AnimFinish(ANIM_STEP_BEHIND) == true)
			if (m_pModelCom->Check_PickAnimRatio(ANIM_STEP_BEHIND, 0.950, dTimeDelta))
			{
				m_eCurAnimIndex = ANIM_IDLE;
				Trigger_Interact();
			}

		}
	}

	if (m_pModelCom->Get_AnimRatio(ANIM_AWAKE_COMBOPUNCH_END, 0.30))
		Go_Straight_Constant(dTimeDelta, ANIM_AWAKE_COMBOPUNCH_END, 1.f);

	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_STEP_BEHIND, 3.0f, 0.2, 0.70);
}

void CBoss_Akaza::Update_Nachim_AirGun(_double dTimeDelta)
{

}

void CBoss_Akaza::Update_Awake_Cinematic(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_CINEMATIC1;

	}
	if (m_pModelCom->Get_AnimFinish(ANIM_CINEMATIC1) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC1);
		m_eCurAnimIndex = ANIM_CINEMATIC2;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_CINEMATIC2) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC2);
		m_eCurAnimIndex = ANIM_CINEMATIC3;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_CINEMATIC3) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC3);
		m_eCurAnimIndex = ANIM_CINEMATIC4;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_CINEMATIC4) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC4);
		m_eCurAnimIndex = ANIM_CINEMATIC5;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_CINEMATIC5) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC5);
		m_eCurAnimIndex = ANIM_CINEMATIC6;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_CINEMATIC6) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC6);
		m_eCurAnimIndex = ANIM_CINEMATIC7;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_CINEMATIC7) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC7);
		m_eCurAnimIndex = ANIM_CINEMATIC8;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_CINEMATIC8) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC8);
		m_eCurAnimIndex = ANIM_CINEMATIC9;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_CINEMATIC9) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC9);
		m_eCurAnimIndex = ANIM_CINEMATIC10;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_CINEMATIC10) == true)
	{
		m_pModelCom->Set_AnimisFinish(ANIM_CINEMATIC10);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}

}

void CBoss_Akaza::Update_Hit_Small(_double dTimeDelta)
{

	if (m_isJumpOn)
	{
		m_bAir_Motion = true;
		if (m_iSmallHit_Index == 1)
			m_eCurAnimIndex = ANIM_HIT_SMALL_AIR;

		if (m_iSmallHit_Index == 2)
			m_eCurAnimIndex = ANIM_HIT_AIR_UPPER;

		if (m_iSmallHit_Index >= 3)
			m_iSmallHit_Index = 1;

		if ((m_pModelCom->Get_AnimFinish(ANIM_HIT_SMALL_AIR) || m_pModelCom->Get_AnimFinish(ANIM_HIT_AIR_UPPER)))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_HIT_SMALL_AIR);
			m_pModelCom->Set_AnimisFinish(ANIM_HIT_AIR_UPPER);
			m_eCurAnimIndex = ANIM_HIT_BLOW;
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_HIT_BLOW))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW);
			m_eCurAnimIndex = ANIM_HIT_BLOW_LOOP;
		}
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_SMALL_AIR, 0.7f, 0.0, 0.4);
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_AIR_UPPER, 0.7f, 0.0, 0.4);
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_BLOW, 1.5f);

	}
	if (m_isJumpOn == false && m_bAir_Motion == false)
	{
		if (false == m_bAnimFinish)
		{
			if (m_iSmallHit_Index == 1)
			{
				m_eCurAnimIndex = ANIM_HIT_FRONT;
			}
			if (m_iSmallHit_Index == 2)
				m_eCurAnimIndex = ANIM_HIT_LEFT;

			if (m_iSmallHit_Index == 3)
			{
				m_eCurAnimIndex = ANIM_HIT_RIGHT;

			}
			if (m_iSmallHit_Index >= 4)
			{
				m_iSmallHit_Index = 1;
			}
		}
		if ((m_pModelCom->Get_AnimFinish(ANIM_HIT_FRONT) || m_pModelCom->Get_AnimFinish(ANIM_HIT_LEFT) || m_pModelCom->Get_AnimFinish(ANIM_HIT_RIGHT)))
		{
			m_bAnimFinish = true;
			m_pModelCom->Set_AnimisFinish(ANIM_HIT_FRONT);
			m_pModelCom->Set_AnimisFinish(ANIM_HIT_LEFT);
			m_pModelCom->Set_AnimisFinish(ANIM_HIT_RIGHT);

			m_eCurAnimIndex = ANIM_HIT_RETURN_SMALL;
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_HIT_RETURN_SMALL))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN_SMALL);
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
		}
	}
	if (m_isConnectHitting == false)
	{
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_FRONT, 0.7f, 0.0, 0.4);
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_LEFT, 0.7f, 0.0, 0.4);
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_RIGHT, 0.7f, 0.0, 0.4);
	}

	Land_Anim_Play(ANIM_HIT_BLOW_LOOP, ANIM_HIT_BLOW_END);
	Land_Anim_Play(ANIM_HIT_BLOW, ANIM_HIT_BLOW_END);
	Land_Anim_Play(ANIM_HIT_SMALL_AIR, ANIM_HIT_BLOW_END);
	Land_Anim_Play(ANIM_HIT_AIR_UPPER, ANIM_HIT_BLOW_END);

	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_BLOW_END))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_END);
		m_eCurAnimIndex = ANIM_HIT_GETUP_DIZZY;
		Trigger_Hit_GetUp();
	}

}

void CBoss_Akaza::Update_Hit_Upper(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_HIT_AIR_UPPER;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_AIR_UPPER))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_AIR_UPPER);
		m_eCurAnimIndex = ANIM_HIT_BLOW;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_BLOW))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW);
		m_eCurAnimIndex = ANIM_HIT_BLOW_LOOP;
	}

	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_BLOW_END))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_END);
		m_eCurAnimIndex = ANIM_HIT_GETUP_DIZZY;
		Trigger_Hit_GetUp();
	}
	Land_Anim_Play(ANIM_HIT_BLOW_LOOP, ANIM_HIT_BLOW_END);
}

void CBoss_Akaza::Update_Hit_Big(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_HIT_POWER_GUARDBREAK;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_POWER_GUARDBREAK))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_POWER_GUARDBREAK);
		m_eCurAnimIndex = ANIM_HIT_RETURN_SMALL;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_RETURN_SMALL))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN_SMALL);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
	//Go_Backward_Constant(dTimeDelta, ANIM_HIT_POWER_GUARDBREAK, 1.2f);
	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_POWER_GUARDBREAK, 1.2f, 0.01, 0.85);
}

void CBoss_Akaza::Update_Hit_Blow(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_HIT_BLOW;
		Jumping(1.0f, 0.05f);
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_BLOW))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW);
		m_eCurAnimIndex = ANIM_HIT_BLOW_LOOP;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_BLOW_END))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_END);
		m_eCurAnimIndex = ANIM_HIT_GETUP_DIZZY;
		Trigger_Hit_GetUp();
		//Trigger_Hit_RollGetUp();
	}

	Land_Anim_Play(ANIM_HIT_BLOW_LOOP, ANIM_HIT_BLOW_END);

	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_BLOW, 3.0f);
	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_BLOW_LOOP, 3.0f);
	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_BLOW_END, 1.0f, 0.0, 0.3);

}

void CBoss_Akaza::Update_Hit_BigBlow(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Hit_Bound(_double dTimeDelta)
{
	if (m_isJumpOn == true)
	{
		m_eCurAnimIndex = ANIM_HIT_DMGFALL_LOOP;
	}
	else
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_HIT_BOUND;
		}
	}

	if (m_pModelCom->Get_iCurrentAnimIndex() == ANIM_HIT_BOUND && m_isBounding == true)
	{
		m_isBounding = false;
		Jumping(2.0f, 0.05f);
	}
	else
	{
		Land_Anim_Play(ANIM_HIT_DMGFALL_LOOP, ANIM_HIT_DMGFALL_BOUND);
		Land_Anim_Play(ANIM_HIT_BOUND, ANIM_HIT_DMGFALL_BOUND);
		if (m_pModelCom->Get_AnimFinish(ANIM_HIT_DMGFALL_BOUND))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_HIT_DMGFALL_BOUND);
			m_eCurAnimIndex = ANIM_HIT_GETUP_DIZZY;
			Trigger_Hit_GetUp();
		}
	}

	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_BOUND, 0.5f);
	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_DMGFALL_LOOP, 0.5f);
}

void CBoss_Akaza::Update_Hit_Spin(_double dTimeDelta)
{
}

void CBoss_Akaza::Update_Hit_CutScene(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_DEATH;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_DEATH))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_DEATH);
		m_eCurAnimIndex = ANIM_HIT_GETUP;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_GETUP))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_GETUP);
		m_eCurAnimIndex = ANIM_HIT_GETUP2;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_GETUP2))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_GETUP2);
		m_eCurAnimIndex = ANIM_HIT_RETURN_BIG;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_RETURN_BIG))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN_BIG);
		m_bNoDmg = false;
		m_eCurAnimIndex = ANIM_AWAKE_PUSHAWAY;
		Trigger_PushAway();
	}
}

void CBoss_Akaza::Update_Hit_GetUp(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_HIT_GETUP_DIZZY;
	}
	if (m_pModelCom->Check_PickAnimRatio(ANIM_HIT_GETUP_DIZZY, 0.55, dTimeDelta))
	{
		//m_pModelCom->Set_AnimResetTimeAcc(ANIM_HIT_GETUP_DIZZY);
		m_eCurAnimIndex = ANIM_HIT_RETURN_BIG;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_RETURN_BIG))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN_BIG);
		m_eCurAnimIndex = ANIM_AWAKE_PUSHAWAY;
		Trigger_PushAway();
	}

}

void CBoss_Akaza::Update_Hit_RollGetUp(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_HIT_DOWNLOOP;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_DOWNLOOP))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_DOWNLOOP);
		m_eCurAnimIndex = ANIM_HIT_GETUP_SPIN;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_GETUP_SPIN))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_GETUP_SPIN);
		m_eCurAnimIndex = ANIM_HIT_GETUP_SPIN2;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_GETUP_SPIN2))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_GETUP_SPIN2);
		m_eCurAnimIndex = ANIM_HIT_RETURN_SMALL;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_HIT_RETURN_SMALL))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_RETURN_SMALL);
		m_bNoDmg = false;
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}

	if (m_iRandomDirNum == 1)
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_GETUP_SPIN2, 2.0f, 0.01, 0.75);
	if (m_iRandomDirNum == 2)
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_GETUP_SPIN2, 2.0f, 0.01, 0.75);
	if (m_iRandomDirNum == 3)
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_HIT_GETUP_SPIN2, 2.0f, 0.01, 0.75);

}

void CBoss_Akaza::Update_Hit_BigGetUp(_double dTimeDelta)
{
}

void CBoss_Akaza::Land_Anim_Play(ANIM CurAnim, ANIM LandAnim)
{
	if (m_pModelCom->Get_iCurrentAnimIndex() == CurAnim)
	{
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (Pos.y <= m_fLand_Y)
		{
			m_eCurAnimIndex = LandAnim;

			Pos.y = m_fLand_Y;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));
		}
	}
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

	m_CharacterDesc.NaviDesc.iCurrentIndex = 1;
	/* for.Com_Navigation_Acaza*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Acaza"),
		TEXT("Com_Navigation_Acaza"), (CComponent**)&m_pNavigationCom[NAVI_ACAZA], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Acaza: CPlayer");
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

	m_fOutlineThickness = 0.8f;
	m_fOutlineFaceThickness = 0.f;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineThickness", &m_fOutlineThickness, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_OutlineFaceThickness", &m_fOutlineFaceThickness, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_bSuperArmor", &m_bSuperArmor, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
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
