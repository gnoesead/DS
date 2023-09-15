#include "pch.h"
#include "..\Public\WebShot.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "EffectPlayer.h"

#include "AtkCollManager.h"

#include "Player.h"
#include "PlayerManager.h"

#include "SwampManager.h"
#include "Swamp.h"
#include "WaterParticleEffect.h"

#include "WebManager.h"

#include "ParticleManager.h"
#include "CustomParticle.h"

CWebShot::CWebShot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{
}

CWebShot::CWebShot(const CWebShot& rhs)
	: CMasterEffect(rhs)
{
}

HRESULT CWebShot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWebShot::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		memcpy(&m_WebDesc, pArg, sizeof m_WebDesc);
	}

	if (FAILED(__super::Initialize(&m_WebDesc.WorldInfo)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	if (nullptr != pArg)
	{
		//초기 위치 설정
		m_pTransformCom->Scaling(m_WebDesc.vScale);
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_WorldInfo.vPosition));

		m_pTransformCom->Set_Look(m_WebDesc.vDir);
	}

	m_fTextureOrder = 999.f;

	//Jik
	if (m_WebDesc.iWebTexIndex == 32)
	{

	}
	else
	{
		Initialize_Create_Coll();
	}
	

	return S_OK;
}

void CWebShot::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (true == m_isDead)
		return;
	m_fTextureOrder = 999.f;
	Tick_For_Index(dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_LBRACKET))
	{
		_float3 vPos = Convert::ToFloat3(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		_float3 vMinRange = { -1.f, 0.f, -1.f };
		_float3 vMaxRange = { 1.f, 1.f, 1.f };
		_float3 vTPerD = { 0.2f, 0.5f, 0.2f };
		_int3	vDirOption = { 1, 0, 1 };
		// PoolTag, BufferTag, TextureTag, 
		// Pos, LifeTime, MinScale, MaxScale, MinSpeed, MaxSpeed, 
		// Range, TickPerSize, TickPerDir, ShaderPass, SpriteSpeed, SpriteXY
		CParticleManager::GetInstance()->PlayParticle("WebSprite",
			TEXT("Prototype_Component_VIBuffer_20_Particle"), TEXT("Prototype_Component_Texture_T_e_Plc_P1007_Atk_Spiderweb010")
			, vPos, 1.f, 0.5f, 1.f, 0.5f, 1.5f, vMinRange, vMaxRange, 0.5f, vTPerD, vDirOption, CCustomParticle::PASS_SPRITE, 0.f, _int2(6, 6), true);
	}

	Safe_Release(pGameInstance);

	//m_pTransformCom->Go_Dir(dTimeDelta * m_WebDesc.fSpeed, XMLoadFloat4(&m_WebDesc.vDir));
	//m_pTransformCom->Go_Straight(dTimeDelta);
}

void CWebShot::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EffectNoBloom, this)))
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), CPlayerManager::GetInstance()->Get_PlayerIndex()));
	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

	_float4 WebPos;
	XMStoreFloat4(&WebPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pGameInstance);

	if (WebPos.z < PlayerPos.z - 10.0f)
	{
		m_isDead = true;
	}

	
	if (m_isHit)
	{
		_float3 vPos = Convert::ToFloat3(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		_float3 vMinRange = { -0.5f, -0.5f, -0.5f };
		_float3 vMaxRange = { 0.5f, 1.f, 0.5f };
		_float3 vTPerD = { 3.f, 1.5f, 3.f };
		_int3	vDirOption = { 1, 0, 1 };
		// PoolTag, BufferTag, TextureTag, 
		// Pos, LifeTime, MinScale, MaxScale, MinSpeed, MaxSpeed, 
		// MinRange, MaxRange, TickPerSize, TickPerDir, ShaderPass, SpriteSpeed, SpriteXY
		CParticleManager::GetInstance()->PlayParticle("WebSprite",
			TEXT("Prototype_Component_VIBuffer_5_Particle"), TEXT("Prototype_Component_Texture_T_e_Plc_P1007_Atk_Spiderweb010")
			, vPos, 1.f, 0.1f, 0.2f, 1.f, 1.5f, vMinRange, vMaxRange, 1.f, vTPerD, vDirOption, CCustomParticle::PASS_SPRITE, 0.f, _int2(2, 2), true);

		m_fAlphaCut += 0.05f;
		if (1.0f <= m_fAlphaCut )
		{
			m_isDead = true;
		}
	}

	
	m_fUv_Web -= 0.03f;

#ifdef _DEBUG
	/*if (FAILED(m_pRendererCom->Add_DebugGroup(m_pNavigationCom)))
		return;*/
#endif
}

HRESULT CWebShot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Render_For_Index();

	return S_OK;
}

// 12:가로로긴 , 25:웹볼
void CWebShot::Initialize_Create_Coll()
{
	//가로로 긴
	if (m_WebDesc.iWebTexIndex == 12 )
	{
		_vector AtkDir = { 0.0f, 0.0f, -1.0f, 0.0f };
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(-0.3f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(-0.2f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(-0.1f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(0.0f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(0.1f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(0.2f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(0.3f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
	}
	//슬라이드
	else if (m_WebDesc.iWebTexIndex == 13)
	{
		_vector AtkDir = { 0.0f, 0.0f, -1.0f, 0.0f };
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(0.0f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(0.1f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(0.2f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.05f, 0.05f, 0.05f), _float3(0.3f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
	}
	//jik
	else if (m_WebDesc.iWebTexIndex == 32 )
	{
		_vector AtkDir = { 0.0f, 0.0f, -1.0f, 0.0f };
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.025f, 0.25f, 0.05f), _float3(-0.3f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.025f, 0.05f, 0.05f), _float3(-0.2f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.025f, 0.05f, 0.05f), _float3(-0.1f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.025f, 0.05f, 0.05f), _float3(0.0f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.025f, 0.05f, 0.05f), _float3(0.1f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.025f, 0.05f, 0.05f), _float3(0.2f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.025f, 0.05f, 0.05f), _float3(0.3f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
	}
	//콜라이더 한개 제작
	else
	{
		if (m_WebDesc.iWebTexIndex == 7)
		{
			_vector AtkDir = { 0.0f, 0.0f, -1.0f, 0.0f };
			Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.15f, 0.15f, 0.15f), _float3(0.f, 0.0f, 0.0f), 13.0,
				CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		}
		else
		{
			_vector AtkDir = { 0.0f, 0.0f, -1.0f, 0.0f };
			Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.35f, 0.35f, 0.35f), _float3(0.f, 0.0f, 0.0f), 13.0,
				CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		}
	}

}

void CWebShot::Tick_For_Index(_double dTimeDelta)
{
	m_dDelay_All += dTimeDelta;

	//m_pTransformCom->Go_Dir(dTimeDelta * m_WebDesc.fSpeed, XMLoadFloat4(&m_WebDesc.vDir));
	//m_pTransformCom->Go_Straight(dTimeDelta);

	m_pTransformCom->Set_Look(m_WebDesc.vDir);


	//애로우
	if (m_WebDesc.iWebTexIndex == 7)
	{
		m_pTransformCom->Go_Straight(dTimeDelta * m_WebDesc.fSpeed);
	}
	//슬라이드웹
	else if (m_WebDesc.iWebTexIndex == 13)
	{
		//m_WebDesc.fTurn
		m_pTransformCom->Rotation(_float3{ 0.0f, m_WebDesc.fTurn, 0.0f });


		m_pTransformCom->Go_Straight(dTimeDelta * m_WebDesc.fSpeed);
	}
	//직
	else if (m_WebDesc.iWebTexIndex == 32 )
	{
		m_pTransformCom->Rotation(_float3{ 30.0f , m_WebDesc.fTurn, 90.0f });

		m_pTransformCom->Go_Dir(dTimeDelta * m_WebDesc.fSpeed, XMLoadFloat4(& m_WebDesc.vDir));


		if (m_isHit == false)
		{
			//충돌
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), CPlayerManager::GetInstance()->Get_PlayerIndex()));
			CTransform* pTransform = pPlayer->Get_TransformCom();
			_float4 PlayerPos;
			XMStoreFloat4(&PlayerPos, pTransform->Get_State(CTransform::STATE_POSITION));

			if (Calculate_Distance_From_Pos(PlayerPos) < 0.7f)
			{
				m_isHit = true;
				if (m_isFirst_Hit)
				{
					m_isFirst_Hit = false;
					CPlayerManager::GetInstance()->Set_Hit_WebShot(true);
				}
			}
			Safe_Release(pGameInstance);
		}
	}
	else
	{
		m_pTransformCom->Go_Straight(dTimeDelta * m_WebDesc.fSpeed);
	}

}

HRESULT CWebShot::Render_For_Index()
{
	//한 번 렌더
	if (m_WebDesc.iWebTexIndex == 12 || m_WebDesc.iWebTexIndex == 32 || m_WebDesc.iWebTexIndex == 13)
	{
		if (FAILED(SetUp_ShaderResources(0)))
			return E_FAIL;

		m_pShaderCom->Begin(28); //pass

		m_pVIBufferCom->Render();
	}
	//애로우 렌더
	else if (m_WebDesc.iWebTexIndex == 7)
	{
		if (FAILED(SetUp_ShaderResources(3)))
			return E_FAIL;
		m_pShaderCom->Begin(28); //pass
		m_pVIBufferCom->Render();

		if (FAILED(SetUp_ShaderResources(4)))
			return E_FAIL;
		m_pShaderCom->Begin(28); //pass
		m_pVIBufferCom->Render();
	}
	
	//두 번 렌더(크로스)
	else
	{
		for (_int i = 0; i < 2; i++)
		{
			if (FAILED(SetUp_ShaderResources(i)))
				return E_FAIL;

			m_pShaderCom->Begin(28); //pass

			m_pVIBufferCom->Render();
		}
	}

	return S_OK;
}

void CWebShot::Make_AttackColl(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime, 
	CAtkCollider::ATK_TYPE AtkType, _vector vDir, _float fDmg, 
	CAtkCollider::BULLET_TYPE eBulletType)
{
	CAtkCollider::ATKCOLLDESC AtkCollDesc;
	ZeroMemory(&AtkCollDesc, sizeof AtkCollDesc);

	AtkCollDesc.ColliderDesc.vSize = Size;
	AtkCollDesc.ColliderDesc.vPosition = Pos;

	AtkCollDesc.dLifeTime = DurationTime;

	AtkCollDesc.pParentTransform = m_pTransformCom;

	AtkCollDesc.eAtkType = AtkType;
	AtkCollDesc.eBulletType = eBulletType;

	AtkCollDesc.fDamage = fDmg;

	AtkCollDesc.bBullet = false;

	XMStoreFloat4(&AtkCollDesc.AtkDir, XMVector4Normalize(vDir));
	CAtkCollManager::GetInstance()->Reuse_Collider(pLayerTag, &AtkCollDesc);
}

_vector CWebShot::Calculate_Dir_From_Pos(_float4 Pos)
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float4 MyPos;
	XMStoreFloat4(&MyPos, vMyPos);

	Pos.y = MyPos.y;
	_vector vTarget = XMLoadFloat4(&Pos);

	return XMVector3Normalize(vTarget - vMyPos);
}

_float CWebShot::Calculate_Distance_From_Pos(_float4 Pos)
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float4 MyPos;
	XMStoreFloat4(&MyPos, vMyPos);

	Pos.y = MyPos.y;
	_vector vTarget = XMLoadFloat4(&Pos);

	_float fDistance = Convert::GetLength(vTarget - vMyPos);

	return fDistance;
}

HRESULT CWebShot::Add_Components()
{
	// for.Com_Transform 	
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.dSpeedPerSec = 5.0;
	TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CWebShot");
		return E_FAIL;
	}


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed to Add_Com_Renderer : CWebShot");
		return E_FAIL;
	}


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CWebShot");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		MSG_BOX("Failed to Add_Com_VIBuffer : CWebShot");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_TRAIN, TEXT("Prototype_Component_Texture_Web"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSG_BOX("Failed to Add_Com_Texture : CWebShot");
		return E_FAIL;
	}




	return S_OK;
}

HRESULT CWebShot::SetUp_ShaderResources(_int icross)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	//평면
	if (icross == 0)
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
	}
	//평면 크로스
	else if (icross == 1)
	{
		//m_pTransformCom->Turn(90.0f, _vector{ 0.0f, 1.0f, 0.0f, 0.0f });

		m_pTransformCom->Rotation(_float3{ 0.0f, 90.0f, 0.0f });

		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		m_pTransformCom->Rotation(_float3{ 0.0f, 0.0f, 0.0f });
	}

	//애로우(눕힌거)
	else if (icross == 3)
	{
		//m_pTransformCom->Turn(-90.0f, _vector{ 1.0f, 0.0f, 0.0f, 0.0f });
		m_pTransformCom->Rotation(_float3{ -90.0f, 0.0f, 0.0f });

		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
		//m_pTransformCom->Rotation(_float3{ 0.0f, 0.0f, 0.0f });
	}
	// 눕힌후 돌린거
	else if (icross == 4)
	{
		//m_pTransformCom->Turn(-90.0f, _vector{ 1.0f, 0.0f, 0.0f, 0.0f });
		//m_pTransformCom->Turn(90.0f, _vector{ 0.0f, 0.0f, 1.0f, 0.0f });
		m_pTransformCom->Rotation(_float3{ 0.0f, -90.0f, 90.0f });

		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		m_pTransformCom->Rotation(_float3{ 0.0f, 0.0f, 0.0f });
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_WebDesc.iWebTexIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Web", &m_fUv_Web, sizeof _float)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fWebAlpha", &m_fAlphaCut, sizeof _float)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CWebShot* CWebShot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWebShot* pInstance = new CWebShot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWebShot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWebShot::Clone(void* pArg)
{
	CWebShot* pInstance = new CWebShot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWebShot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWebShot::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
