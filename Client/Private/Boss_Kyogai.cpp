#include "pch.h"
#include "Boss_Kyogai.h"
#include "GameInstance.h"
#include "SoundMgr.h"
#include "RotationMapObject.h"
#include "EffectPlayer.h"
#include "Player.h"
#include "PlayerManager.h"
#include "AtkCollManager.h"
#include "AlertCircle.h"
#include "AlertRect.h"

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

	m_eCurAnimIndex = ANIM_IDLE;
	m_eCurstate = STATE_INTERACT;
	m_eCurPhase = PHASE_1;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(114.f, 0.f, 117.f, 1.f));
	m_eCurNavi = NAVI_HOUSE_4_0;

	//m_eCurNavi = NAVI_ACAZA;
	m_StatusDesc.fHp = 100.f;
	m_StatusDesc.fHp_Max = 100.f;

	return S_OK;
}

void CBoss_Kyogai::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (true == m_isDead)
		return;


#ifdef _DEBUG


#endif // _DEBUG
	Debug_State(dTimeDelta);
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
//#ifdef _DEBUG
void CBoss_Kyogai::Debug_State(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_F2))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(114.f, 0.f, 117.f, 1.f));
		m_iTriggerCnt = 0;
		m_iIdleCnt = 0;
		m_eCurstate = STATE_INTERACT;
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
			Trigger_AtkCmb();

		}
		if (pGameInstance->Get_DIKeyDown(DIK_2))
		{
			Trigger_AtkCmb2();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_3))
		{
			Trigger_StompKick();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_4))
		{
			Trigger_AtkStep();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_5))
		{
			Trigger_Awake();
		}
		if (pGameInstance->Get_DIKeyDown(DIK_6))
		{
			Trigger_AtkPunch();
		}

	}
	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT))
	{

		if (pGameInstance->Get_DIKeyDown(DIK_1))
		{
			Trigger_LinkerCmb();

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
	}
	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL))
	{

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
	}
	Safe_Release(pGameInstance);
}
//#endif //_DEBUG

void CBoss_Kyogai::EventCall_Control(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iCurLevel = pGameInstance->Get_CurLevelIdx();

	CAnimation* pAnim = m_pModelCom->Get_Animation();
	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_iEvent_Index = 0;
	}

	if (EventCallProcess())
	{
		_vector vMonsterDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vDir = Calculate_Dir_ZeroY();

		_vector vRandomDir = Random_Dir(vDir, 0.f, 0.f, 0.f, 360.f);

		_double dLifeTime = 0.20;
		_double dLongLifeTime = 1.0;
		_double dSpeed = 5.0;
#pragma region AWAKE_ComboPunch
		if (ANIM_ATKCMB2 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_1_1", m_pTransformCom);
			}
			if (1 == m_iEvent_Index)	// 0.25
			{
				dLongLifeTime = 6.0;
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET);

				CAtkCollider* pAtkCollider = dynamic_cast<CAtkCollider*> (pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_MonsterAtk"), (_uint)pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("Layer_MonsterAtk")) - 1));


				Create_AlertRect(BLADE_THREE_RANDOM);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.x = -1.f;

				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(-1.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET, "Kyogai_BladeAtk" , &EffectWorldDesc);

				EffectWorldDesc.vPosition.x = 0.f;
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET, "Kyogai_BladeAtk", &EffectWorldDesc);

				EffectWorldDesc.vPosition.x = 1.f;
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.f, 1.0f, 1.0f), _float3(1.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET, "Kyogai_BladeAtk", &EffectWorldDesc);

			}
			if (2 == m_iEvent_Index)	// 1.20
			{
				dLongLifeTime = 6.0;
				
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET);

				Create_AlertRect(BLADE_THREE_RANDOM);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.x = -1.f;

				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(-1.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET, "Kyogai_BladeAtk", &EffectWorldDesc);

				EffectWorldDesc.vPosition.x = 0.f;
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET, "Kyogai_BladeAtk", &EffectWorldDesc);

				EffectWorldDesc.vPosition.x = 1.f;
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.f, 1.0f, 1.0f), _float3(1.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET, "Kyogai_BladeAtk", &EffectWorldDesc);
			}
		}

		if (ANIM_ATKCMB3 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_1_2", m_pTransformCom);
			}
			if (1 == m_iEvent_Index)	// 0.25
			{
				dLongLifeTime = 3.0;
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_BIG, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET);
				
				Create_AlertRect(BLADE_THREE_RANDOM);

				CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
				EffectWorldDesc.vPosition.x = -1.f;

				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(-1.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET, "Kyogai_BladeAtk", &EffectWorldDesc);

				EffectWorldDesc.vPosition.x = 0.f;
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET, "Kyogai_BladeAtk", &EffectWorldDesc);

				EffectWorldDesc.vPosition.x = 1.f;
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.f, 1.0f, 1.0f), _float3(1.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET, "Kyogai_BladeAtk", &EffectWorldDesc);
			}
		}

		if (ANIM_ATKCMB_02LOOP == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.19
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_2", m_pTransformCom);
			}

			if (1 == m_iEvent_Index)	// 0.62
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_2", m_pTransformCom);
			}

		}
		if (ANIM_KICKDOWN == m_pModelCom->Get_iCurrentAnimIndex())
		{

			if (0 == m_iEvent_Index)	// 0.6
			{
				//CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_11", m_pTransformCom);

				CAlertCircle::EFFECTDESC EffectDesc;
				EffectDesc.pOwnerTransform = m_pTransformCom;
				EffectDesc.iType = CAlertCircle::TYPE_KICKDOWN;

				pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertCircle"), &EffectDesc, false);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg);

			}


		}

		if (ANIM_STOMPKICK == m_pModelCom->Get_iCurrentAnimIndex())
		{

			if (0 == m_iEvent_Index)	// 1.1
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_11", m_pTransformCom);

				CAlertCircle::EFFECTDESC EffectDesc;
				EffectDesc.pOwnerTransform = m_pTransformCom;
				EffectDesc.iType = CAlertCircle::TYPE_KICKDOWN;

				pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertCircle"), &EffectDesc, false);

				for (_uint i = 0; i < 70; ++i)
				{
					pGameInstance->Add_GameObject(LEVEL_HOUSE, TEXT("Layer_Effect"),
						TEXT("Prototype_GameObject_RoomSmoke"));
				}
			}

			if (1 == m_iEvent_Index) // 1.65
			{
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg);
			}


		}
		if (ANIM_ATKCMBW05 == m_pModelCom->Get_iCurrentAnimIndex()) // 앞발 내밀고 배꼽 팡팡 
		{

			if (0 == m_iEvent_Index)	// 0.95
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_11", m_pTransformCom);

				CAlertCircle::EFFECTDESC EffectDesc;
				EffectDesc.pOwnerTransform = m_pTransformCom;
				EffectDesc.iType = CAlertCircle::TYPE_KICKDOWN;

				pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertCircle"), &EffectDesc, false);

				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_BIG, vMonsterDir, m_fBigDmg);

			}

		}

		if (ANIM_ATKPUNCH == m_pModelCom->Get_iCurrentAnimIndex())
		{
			CAlertCircle::EFFECTDESC EffectDesc;
			EffectDesc.pOwnerTransform = m_pTransformCom;
			EffectDesc.iType = CAlertCircle::TYPE_ROOMCHANGE;

			if (0 == m_iEvent_Index) // 1.30
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_9_2", m_pTransformCom);

				pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertCircle"), &EffectDesc, false);

			}

			if (1 == m_iEvent_Index) // 1.50
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_9", m_pTransformCom);
				//tag, size3, Pos3(left, up, front), duration, atktype, vDir, fDmg
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fSmallDmg);

				pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertCircle"), &EffectDesc, false);
			}

		}
		if (ANIM_ATKPUNCH2 == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index) // 0.2
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_10_2", m_pTransformCom);
			}

			if (1 == m_iEvent_Index) // 0.4
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_10", m_pTransformCom);

			}

			if (2 == m_iEvent_Index) // 0.5
			{
				Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 1.5f), dLifeTime,
					CAtkCollider::TYPE_SMALL, vMonsterDir, m_fSmallDmg);
			}
		}

		if (ANIM_ATKSTEP_BACK == m_pModelCom->Get_iCurrentAnimIndex())
		{
			if (0 == m_iEvent_Index)	// 0.0
			{
				CEffectPlayer::Get_Instance()->Play("Kyogai_AtkStepB", m_pTransformCom);
			}
			if (1 == m_iEvent_Index)	// 1.0
			{
	
				dLongLifeTime = 5.f;

				if (m_iAtkStepTypeNum == 1) // 일직선
				{
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIDELAYBULLET);

					Create_AlertRect(BLADE_THREE_FRONT , vMonsterDir);
				}
				if (m_iAtkStepTypeNum == 2) // 삼각
				{
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIDELAYBULLET);

					Create_AlertRect(BLADE_THREE_FRONT , vMonsterDir);

					vMonsterDir = Rotation_Dir(vMonsterDir, 10);
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(1.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIDELAYBULLET);

					Create_AlertRect(BLADE_THREE_FRONT , vMonsterDir);
	
					vMonsterDir = Rotation_Dir(vMonsterDir, -20);
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(-1.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_CONNECTSMALL, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIDELAYBULLET);

					Create_AlertRect(BLADE_THREE_FRONT , vMonsterDir);
				
				}

				if (m_iAtkStepTypeNum == 3) // 랜덤 두곳
				{
					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_CONNECTSMALL, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET);

					Create_AlertRect(BLADE_THREE_RANDOM);

					vRandomDir = Random_Dir(vDir, 0.f, 0.f, 0.f, 360.f);

					Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
						CAtkCollider::TYPE_CONNECTSMALL, vRandomDir, m_fBigDmg, m_pPlayerTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIBULLET);

					Create_AlertRect(BLADE_THREE_RANDOM);
				}

				/*Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(-1.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIDELAYBULLET , "Kyogai_BladeAtk");
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(0.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIDELAYBULLET , "Kyogai_BladeAtk", &EffectWorldDesc);

				EffectWorldDesc.vPosition.x = 1.f;
				Make_AtkBulletColl(TEXT("Layer_MonsterAtk"), _float3(1.0f, 1.0f, 1.0f), _float3(1.f, 1.0f, 0.f), dLongLifeTime,
					CAtkCollider::TYPE_EFFECT, vMonsterDir, m_fBigDmg, m_pTransformCom, dSpeed, CAtkCollider::TYPE_KYOGAIDELAYBULLET , "Kyogai_BladeAtk");*/
			}
		}

		m_iEvent_Index++;
	}

	Safe_Release(pGameInstance);
}


void CBoss_Kyogai::Update_Hit_Messenger(_double dTimeDelta)
{
	if (m_bNoDmg == false /*&& m_bStart == true*/)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		_int PlayerIndex = CPlayerManager::GetInstance()->Get_PlayerIndex();
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), PlayerIndex));

		_float4 AtkDir = m_pColliderCom[COLL_SPHERE]->Get_AtkDir();

		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Small() || m_pColliderCom[COLL_SPHERE]->Get_Hit_ConnectSmall())
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

			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		}
		/*if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Blow())
		{
			m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
			Trigger_Hit_Blow();
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Upper())
		{
			m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
			Trigger_Hit_Upper();
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Big())
		{
			m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);

			Trigger_Hit_Big();

			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_Bound())
		{
			m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
			Trigger_Hit_Bound();
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();
			Set_FallingStatus(3.0f, 0.0f);
		}
		if (m_pColliderCom[COLL_SPHERE]->Get_Hit_CutScene())
		{
			m_pTransformCom->LerpVector(-XMLoadFloat4(&AtkDir), 0.9f);
			Trigger_Hit_CutScene();
			pPlayer->Set_Hit_SurgeCutScene(true);
			pPlayer->Set_Hit_Success(true);
			m_StatusDesc.fHp -= m_pColliderCom[COLL_SPHERE]->Get_fDamage();

		}*/

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

void CBoss_Kyogai::Update_AnimIndex(_uint iAnimIndex)
{
	if (m_eCurAnimIndex != m_ePreAnimIndex)
	{
		m_ePreAnimIndex = m_eCurAnimIndex;
	}
}

void CBoss_Kyogai::Update_Trigger(_double dTimeDelta)
{

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

void CBoss_Kyogai::Update_State(_double dTimeDelta)
{

	switch (m_eCurstate)
	{
	case CBoss_Kyogai::STATE_INTERACT:
		Update_Interact(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_BEGIN:
		Update_Begin(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_NEXTPHASE:
		Update_NextPhase(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_HEAL:
		Update_Heal(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_AWAKE:
		Update_Awake(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_GUARD:
		Update_Guard(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKCMB:
		Update_AtkCmb(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKCMB2:
		Update_AtkCmb2(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKSTEP:
		Update_AtkStep(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_STOMPKICK:
		Update_StompKick(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_ATKPUNCH:
		Update_AtkPunch(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_LINKERCMB:
		Update_LinkerCmb(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_HIT_SMALL:
		Update_Hit_Small(dTimeDelta);
		break;
	case CBoss_Kyogai::STATE_HIT_BIG:
		Update_Hit_Big(dTimeDelta);
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
	if (m_StatusDesc.fHp <= 0.f)
	{
		m_bTrigger = false;
		m_bPatternStart = false;
		m_iTriggerCnt = 5;
		m_dTriggerTime = 0.0;
		m_iIdleCnt = 0;
	}

	if (m_bTrigger == false)
	{
		if (m_StatusDesc.fHp > 50.f)
		{

			switch (m_iTriggerCnt)
			{
			case 1:
				Trigger_Begin();
				break;

			case 2:
				Trigger_Interact();
				break;

			}


		}
		if (m_StatusDesc.fHp <= 50.f)
		{
			switch (m_iTriggerCnt)
			{
			case 1:
				Trigger_Begin();
				break;

			case 5:
				Trigger_NextPhase();
				break;
			}
		}

	}

}

void CBoss_Kyogai::Update_Phase_2(_double dTimeDelta)
{
	// 개방하고 25초 유지 m_bAwake가 활성되면 25초 후에 다시 false
	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) < 0.7f && m_bFirstAwake == false)
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
	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) < 0.7f && m_bSecondAwake == false)
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
		if (24.5 < m_dAwakeTime && m_dAwakeTime <= 24.5 + dTimeDelta)
			m_pRendererCom->Set_Invert();
		if (m_dAwakeTime > 25.0)
		{
			m_dAwakeTime = 0.0;
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
				Trigger_Begin();
				break;


			}
		}
		if (m_bAwake == true)
		{

			switch (m_iTriggerCnt)
			{
			case 1:
				Trigger_Awake();
				break;


			}
		}

	}
}

void CBoss_Kyogai::Trigger_Interact_Phase_1(_double dTimeDelta)
{
	m_eCurAnimIndex = ANIM_IDLE;

	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) > 0.5f)
	{
		if (m_bPatternStart == false)
		{
			m_dTriggerTime += dTimeDelta;
			if (0.6 < m_dTriggerTime && m_dTriggerTime <= 0.6 + dTimeDelta)
				m_iIdleCnt++;

			if (m_iIdleCnt == 1)
			{
				m_iIdleCnt = 0;
				m_bTrigger = false;
				m_iTriggerCnt++;

				if (m_iTriggerCnt >= 6)
				{
					m_iTriggerCnt = 1;
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

				m_iRandomPatternNum = Random::Generate_Int(1, 10);


				if (fDistance <= 5.f) //가까울 때
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
				if (fDistance > 5.f) // 멀 때
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
	if ((m_StatusDesc.fHp / m_StatusDesc.fHp_Max) <= 0.5f && (m_StatusDesc.fHp / m_StatusDesc.fHp_Max) > 0.f)
	{
		if (m_bPatternStart == true)
		{
			if (0.30 < m_dTriggerTime && m_dTriggerTime <= 0.30 + dTimeDelta)
				m_iIdleCnt++;

			if (m_iIdleCnt == 1)
			{
				m_dTriggerTime = 0.0;
				m_iIdleCnt = 0;
				m_bTrigger = false;
				m_iTriggerCnt = 1;
				m_bPatternStart = false;

			}
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


				if (fDistance <= 5.f) // 거리가 가까울 때 
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
				if (fDistance > 5.f) // 거리가 멀 때
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

void CBoss_Kyogai::Trigger_Interact_Phase_2(_double dTimeDelta)
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

}


void CBoss_Kyogai::Trigger_Interact()
{
	m_bTrigger = true;
	m_eCurstate = STATE_INTERACT;
}

void CBoss_Kyogai::Trigger_Begin()
{
	m_eCurstate = STATE_BEGIN;
}


void CBoss_Kyogai::Trigger_Guard()
{
	m_bTrigger = true;
	m_eCurstate = STATE_INTERACT;
}

void CBoss_Kyogai::Trigger_PushAway()
{
	m_bTrigger = true;
	m_eCurstate = STATE_INTERACT;
}

void CBoss_Kyogai::Trigger_NextPhase()
{
	m_bTrigger = true;
	m_eCurstate = STATE_INTERACT;
}

void CBoss_Kyogai::Trigger_Heal()
{
	m_bTrigger = true;
	m_eCurstate = STATE_INTERACT;
}

void CBoss_Kyogai::Trigger_Awake()
{
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_eCurstate = STATE_AWAKE;
}

void CBoss_Kyogai::Trigger_JumpStep()
{
}

void CBoss_Kyogai::Trigger_AtkCmb()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKCMB;
	m_bAnimFinish = false;
}

void CBoss_Kyogai::Trigger_AtkCmb2()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKCMB2;
	m_bAnimFinish = false;
}

void CBoss_Kyogai::Trigger_AtkStep()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKSTEP;
	m_bAnimFinish = false;

	if (m_bAtkStepType == false)
		m_iAtkStepTypeNum++;

	if (m_iAtkStepTypeNum > 3)
		m_bAtkStepType = true;

	if (m_bAtkStepType == true)
		m_iAtkStepTypeNum = Random::Generate_Int(1, 3);

	if (Check_Distance(2.f) == true)
		m_iRandomDirNum = 0;
	else
		m_iRandomDirNum = 1;
}

void CBoss_Kyogai::Trigger_StompKick()
{
	m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
	m_bTrigger = true;
	m_eCurstate = STATE_STOMPKICK;
	m_bAnimFinish = false;
}

void CBoss_Kyogai::Trigger_AtkPunch()
{
	m_bTrigger = true;
	m_eCurstate = STATE_ATKPUNCH;
	m_bAnimFinish = false;
}

void CBoss_Kyogai::Trigger_LinkerCmb()
{
	m_bTrigger = true;
	m_eCurstate = STATE_LINKERCMB;
	m_bAnimFinish = false;
	m_iLinkerNum = Random::Generate_Int(1, 2);
}

void CBoss_Kyogai::Trigger_Hit_Small()
{
	m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(false);
	m_isConnectHitting = false;
	m_bTrigger = true;
	m_bAnimFinish = false;
	m_iSmallHit_Index++;
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_BASETURN_L);
	m_pModelCom->Set_AnimResetTimeAcc(ANIM_BASETURN_R);
	m_pModelCom->Get_AnimFinish(ANIM_BASETURN_L);
	m_pModelCom->Get_AnimFinish(ANIM_BASETURN_R);

	m_eCurstate = STATE_HIT_SMALL;
}

void CBoss_Kyogai::Trigger_Hit_ConnectSmall()
{
}

void CBoss_Kyogai::Trigger_Hit_Upper()
{
}

void CBoss_Kyogai::Trigger_Hit_Big()
{
}

void CBoss_Kyogai::Trigger_Hit_Blow()
{
}

void CBoss_Kyogai::Trigger_Hit_BigBlow()
{
}

void CBoss_Kyogai::Trigger_Hit_Bound()
{
}

void CBoss_Kyogai::Trigger_Hit_Spin()
{
}

void CBoss_Kyogai::Trigger_Hit_CutScene()
{
}

void CBoss_Kyogai::Trigger_Hit_GetUp()
{
}

void CBoss_Kyogai::Trigger_Hit_RollGetUp()
{
}

void CBoss_Kyogai::Trigger_Hit_BigGetUp()
{
}

void CBoss_Kyogai::Trigger_Awake_RoomChange(_double dTimeDelta)
{
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

		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();

	}
}

void CBoss_Kyogai::Update_Awake(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_AWAKE;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_AWAKE))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_AWAKE);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
}

void CBoss_Kyogai::Update_JumpStep(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_AtkCmb(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_ATKCMB1;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB1))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB1);
		m_eCurAnimIndex = ANIM_ATKCMB2;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB2))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB2);
		m_eCurAnimIndex = ANIM_ATKCMB3;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB3))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB3);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
}

void CBoss_Kyogai::Update_AtkCmb2(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_ATKCMB_01READY;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB_01READY))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB_01READY);
		m_eCurAnimIndex = ANIM_ATKCMB_02LOOP;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB_02LOOP))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB_02LOOP);
		m_eCurAnimIndex = ANIM_ATKCMB_03END;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKCMB_03END))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMB_03END);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
}

void CBoss_Kyogai::Update_AtkStep(_double dTimeDelta)
{
	if (m_iRandomDirNum == 1)
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_ATKSTEP_BACK;
			Jumping(1.f, 0.03f);
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_ATKSTEP_BACK))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKSTEP_BACK);
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
		}
		if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKSTEP_BACK, 0.32, dTimeDelta))
		{
			_vector vDir = Calculate_Dir_FixY();
			m_pTransformCom->LerpVector(vDir, 0.9f);
			JumpStop(0.4);
		}
	}
	else
	{
		if (m_bAnimFinish == false)
		{
			m_bAnimFinish = true;
			m_eCurAnimIndex = ANIM_ATKSTEP_FRONT;
			Jumping(1.f, 0.03f);
		}
		if (m_pModelCom->Get_AnimFinish(ANIM_ATKSTEP_FRONT))
		{
			m_pModelCom->Set_AnimisFinish(ANIM_ATKSTEP_FRONT);
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
		}
		if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKSTEP_FRONT, 0.32, dTimeDelta))
		{
			_vector vDir = Calculate_Dir_FixY();
			m_pTransformCom->LerpVector(vDir, 0.9f);
			JumpStop(0.4);
		}
	}
	Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_ATKSTEP_BACK, 3.f, 0.01, 0.16);
	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_ATKSTEP_FRONT, 3.f, 0.01, 0.16);
}

void CBoss_Kyogai::Update_StompKick(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_STOMPKICK;
	}
	if (m_pModelCom->Get_AnimFinish(ANIM_STOMPKICK))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_STOMPKICK);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
	if (m_pModelCom->Check_PickAnimRatio(ANIM_STOMPKICK, 0.380, dTimeDelta))
	{
		m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		Jumping(1.f, 0.06f);
	}
	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_STOMPKICK, 1.f, 0.38, 0.56);
}

void CBoss_Kyogai::Update_AtkPunch(_double dTimeDelta)
{
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_ATKPUNCH;
	}
	//if (m_pModelCom->Get_AnimFinish(ANIM_ATKPUNCH))
	if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKPUNCH, 0.70, dTimeDelta))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKPUNCH);
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_ATKPUNCH);
		m_eCurAnimIndex = ANIM_ATKPUNCH2;

	}
	if (m_pModelCom->Get_AnimFinish(ANIM_ATKPUNCH2))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKPUNCH2);
		m_eCurAnimIndex = ANIM_IDLE;
		Trigger_Interact();
	}
	_vector vDir = Calculate_Dir_FixY();
	m_pTransformCom->LerpVector(vDir, 0.7f);
	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_ATKPUNCH, 1.5f, 0.50, 0.60);

}

void CBoss_Kyogai::Update_LinkerCmb(_double dTimeDelta)
{
	//거리가 멀다면 스텝으로 다가가 주먹 발차기 스텝북
	if (m_bAnimFinish == false)
	{
		m_bAnimFinish = true;
		m_eCurAnimIndex = ANIM_ATKPUNCH;
	}

	if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKPUNCH, 0.70, dTimeDelta))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKPUNCH);
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_ATKPUNCH);
		m_eCurAnimIndex = ANIM_ATKPUNCH2;

	}
	//if (m_pModelCom->Get_AnimFinish(ANIM_ATKPUNCH2))
	if (m_pModelCom->Check_PickAnimRatio(ANIM_ATKPUNCH2, 0.540, dTimeDelta))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_ATKPUNCH2);
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_ATKPUNCH2);
		if (m_iLinkerNum == 1)
		{
			m_eCurAnimIndex = ANIM_KICKDOWN;
			CEffectPlayer::Get_Instance()->Play("Kyogai_AtkCmb_11", m_pTransformCom);
		}
		if (m_iLinkerNum == 2)
		{
			m_eCurAnimIndex = ANIM_ATKCMBW05;
		}
	}

	//if (m_pModelCom->Get_AnimFinish(ANIM_STOMPKICK))
	if (m_pModelCom->Check_PickAnimRatio(ANIM_KICKDOWN, 0.850, dTimeDelta) || m_pModelCom->Check_PickAnimRatio(ANIM_ATKCMBW05, 0.850, dTimeDelta))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_KICKDOWN);
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_KICKDOWN);
		m_pModelCom->Set_AnimisFinish(ANIM_ATKCMBW05);
		m_pModelCom->Set_AnimResetTimeAcc(ANIM_ATKCMBW05);
		Trigger_AtkStep();
	}

	if (m_pModelCom->Check_PickAnimRatio(ANIM_KICKDOWN, 0.130, dTimeDelta))
	{
		m_pTransformCom->LookAt_FixY(m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION));
		Jumping(0.9f, 0.06f);
	}

	_vector vDir = Calculate_Dir_FixY();
	m_pTransformCom->LerpVector(vDir, 0.7f);
	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_ATKPUNCH, 1.5f, 0.50, 0.60);
	Go_Dir_Constant(dTimeDelta, DIR_UP, ANIM_KICKDOWN, 1.2f, 0.13, 0.39);
}

void CBoss_Kyogai::Update_Hit_Small(_double dTimeDelta)
{
	if (m_isJumpOn == false && m_bAir_Motion == false)
	{
		if (false == m_bAnimFinish)
		{
			if (m_iSmallHit_Index == 1)
			{
				m_eCurAnimIndex = ANIM_BASETURN_L;
			}
			if (m_iSmallHit_Index == 2)
				m_eCurAnimIndex = ANIM_BASETURN_R;

			if (m_iSmallHit_Index >= 3)
			{
				m_iSmallHit_Index = 1;

			}


		}
		if ((m_pModelCom->Get_AnimFinish(ANIM_BASETURN_L) || m_pModelCom->Get_AnimFinish(ANIM_BASETURN_R)))
		{
			m_bAnimFinish = true;
			m_pModelCom->Set_AnimisFinish(ANIM_BASETURN_L);
			m_pModelCom->Set_AnimisFinish(ANIM_BASETURN_R);
			m_eCurAnimIndex = ANIM_IDLE;
			Trigger_Interact();
			//m_eCurAnimIndex = ANIM_BASETURN_L;
		}
	}
	if (m_isConnectHitting == false)
	{
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_BASETURN_L, 0.3f, 0.0, 0.7);
		Go_Dir_Constant(dTimeDelta, DIR_DOWN, ANIM_BASETURN_R, 0.3f, 0.0, 0.7);

	}


	/*if (m_pModelCom->Get_AnimFinish(ANIM_HIT_BLOW_END))
	{
		m_pModelCom->Set_AnimisFinish(ANIM_HIT_BLOW_END);
		m_eCurAnimIndex = ANIM_HIT_GETUP_DIZZY;
		Trigger_Hit_GetUp();
	}*/
}

void CBoss_Kyogai::Update_Hit_Upper(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_Big(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_Blow(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_BigBlow(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_Bound(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_Spin(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_CutScene(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_GetUp(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_RollGetUp(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Hit_BigGetUp(_double dTimeDelta)
{
}

void CBoss_Kyogai::Update_Awake_RoomChange(_double dTimeDelta)
{
}

void CBoss_Kyogai::Create_AlertRect(BLADETYPE eBladeType , _fvector vDir)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_uint iCurLevel = pGameInstance->Get_CurLevelIdx();

	CAtkCollider* pAtkCollider = dynamic_cast<CAtkCollider*> (pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_MonsterAtk"), (_uint)pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("Layer_MonsterAtk")) - 1));

	CAlertRect::EFFECTDESC EffectDesc;
	


	switch (eBladeType)
	{
	case BLADE_THREE_RANDOM:
	case BLADE_THREE_FRONT:
	{
		if(eBladeType == BLADE_THREE_RANDOM)
			EffectDesc.vLook = pAtkCollider->Get_Dir();
		else if (BLADE_THREE_FRONT)
			EffectDesc.vLook = vDir;

		_vector vNormalVector = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), EffectDesc.vLook));

		EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
		pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);

		EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + vNormalVector * 0.8f;
		pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);

		EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) - vNormalVector * 0.8f;
		pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);
		break;
	}
	
	case BLADE_FIVE_RANDOM:
	case BLADE_FIVE_FRONT:
	{
		if (eBladeType == BLADE_FIVE_RANDOM)
			EffectDesc.vLook = pAtkCollider->Get_Dir();
		else if (BLADE_FIVE_FRONT)
			EffectDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		_vector vNormalVector = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), EffectDesc.vLook));

		EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
		pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);

		EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + vNormalVector * 0.8f;
		pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);

		EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) + vNormalVector * 1.6f;
		pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);

		EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) - vNormalVector * 0.8f;
		pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false);

		EffectDesc.vPos = dynamic_cast<CTransform*>(pAtkCollider->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION) - vNormalVector * 1.6f;
		pGameInstance->Add_GameObject(iCurLevel, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertRect"), &EffectDesc, false); }
		break;
	default:
		break;
	}
	

	Safe_Release(pGameInstance);
}

HRESULT CBoss_Kyogai::Add_Components()
{
	/* for.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Kyogai"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		MSG_BOX("Failed to Add_Com_Model : CBoss_Kyogai");
		return E_FAIL;
	}

	/* for.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CBoss_Kyogai");
		return E_FAIL;
	}


	m_CharacterDesc.TransformDesc.dSpeedPerSec = 5.0;
	m_CharacterDesc.TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);
	// for.Com_Transform 
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &m_CharacterDesc.TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CBoss_Kyogai");
		return E_FAIL;
	}


	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize = _float3(0.8f, 0.8f, 0.8f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_AABB].vSize.y * 0.5f, 0.f);
	//for.Com_AABB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_AABB"), (CComponent**)&m_pColliderCom[COLL_AABB], &m_CharacterDesc.ColliderDesc[COLL_AABB])))
	{
		MSG_BOX("Failed to Add_Com_AABB : CBoss_Kyogai");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize = _float3(1.f, 2.f, 1.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vRotation = _float3(0.f, XMConvertToRadians(45.f), 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_OBB].vSize.y * 0.5f, 0.f);
	//for.Com_OBB 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_OBB"), (CComponent**)&m_pColliderCom[COLL_OBB], &m_CharacterDesc.ColliderDesc[COLL_OBB])))
	{
		MSG_BOX("Failed to Add_Com_OBB : CBoss_Kyogai");
		return E_FAIL;
	}



	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize = _float3(1.f, 1.f, 1.f);
	//m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, 0.0f, 0.f);
	m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vPosition = _float3(0.f, m_CharacterDesc.ColliderDesc[CCharacter::COLL_SPHERE].vSize.x, 0.f);
	// for.Com_Sphere 
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Sphere"), (CComponent**)&m_pColliderCom[COLL_SPHERE], &m_CharacterDesc.ColliderDesc[COLL_SPHERE])))
	{
		MSG_BOX("Failed to Add_Com_Sphere : CBoss_Kyogai");
		return E_FAIL;
	}

	m_CharacterDesc.NaviDesc.iCurrentIndex = 0;
	/* for.Com_Navigation_House_4_0*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_House_4_0"),
		TEXT("Com_Navigation_House_4_0"), (CComponent**)&m_pNavigationCom[NAVI_HOUSE_4_0], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_House_4_0: CBoss_Kyogai");
		return E_FAIL;
	}



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
