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



	Initialize_Create_Coll();

	return S_OK;
}

void CWebShot::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (true == m_isDead)
		return;

	Tick_For_Index(dTimeDelta);

	//m_pTransformCom->Go_Dir(dTimeDelta * m_WebDesc.fSpeed, XMLoadFloat4(&m_WebDesc.vDir));
	//m_pTransformCom->Go_Straight(dTimeDelta);
}

void CWebShot::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EffectNoBloom, this)))
		return;
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
	if (m_WebDesc.iWebTexIndex == 12)
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
	//jik
	else if (m_WebDesc.iWebTexIndex == 32 )
	{
		_vector AtkDir = { 0.0f, 0.0f, -1.0f, 0.0f };
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.03f, 0.05f, 0.05f), _float3(-0.3f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.03f, 0.05f, 0.05f), _float3(-0.2f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.03f, 0.05f, 0.05f), _float3(-0.1f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.03f, 0.05f, 0.05f), _float3(0.0f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.03f, 0.05f, 0.05f), _float3(0.1f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.03f, 0.05f, 0.05f), _float3(0.2f, 0.0f, 0.0f), 13.0,
			CAtkCollider::TYPE_WEB, AtkDir, 0.0f);
		Make_AttackColl(TEXT("Layer_MonsterAtk"), _float3(0.03f, 0.05f, 0.05f), _float3(0.3f, 0.0f, 0.0f), 13.0,
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
	m_pTransformCom->Go_Straight(dTimeDelta);


	//애로우
	if (m_WebDesc.iWebTexIndex == 7)
	{
		
	}
	//직선
	else if (m_WebDesc.iWebTexIndex == 32 )
	{
		m_pTransformCom->Rotation(_float3{ 0.0f, 0.0f, 90.0f });
	}

}

HRESULT CWebShot::Render_For_Index()
{
	//한 번 렌더
	if (m_WebDesc.iWebTexIndex == 12 || m_WebDesc.iWebTexIndex == 32)
	{
		if (FAILED(SetUp_ShaderResources(0)))
			return E_FAIL;

		m_pShaderCom->Begin(27); //pass

		m_pVIBufferCom->Render();
	}
	//애로우 렌더
	else if (m_WebDesc.iWebTexIndex == 7)
	{
		if (FAILED(SetUp_ShaderResources(3)))
			return E_FAIL;
		m_pShaderCom->Begin(27); //pass
		m_pVIBufferCom->Render();

		if (FAILED(SetUp_ShaderResources(4)))
			return E_FAIL;
		m_pShaderCom->Begin(27); //pass
		m_pVIBufferCom->Render();
	}
	
	//두 번 렌더(크로스)
	else
	{
		for (_int i = 0; i < 2; i++)
		{
			if (FAILED(SetUp_ShaderResources(i)))
				return E_FAIL;

			m_pShaderCom->Begin(27); //pass

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
