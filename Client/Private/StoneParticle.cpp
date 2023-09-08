#include "pch.h"
#include "..\Public\StoneParticle.h"

#include "GameInstance.h"

CStoneParticle::CStoneParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectW(pDevice, pContext)
{

}

CStoneParticle::CStoneParticle(const CStoneParticle& rhs)
	: CEffectW(rhs)
{

}

HRESULT CStoneParticle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStoneParticle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectWDesc.vPos + XMVectorSet(m_fPlusX, m_fPlusY, m_fPlusZ, 0.f));

	m_fAlpha = 1.f;

	m_fColor = Random::Generate_Float(0.4f, 0.6f);

	m_pTransformCom->Rotation(_float3((_float)Random::Generate_Int(0, 360), (_float)Random::Generate_Int(0, 360), 0.f));

	return S_OK;
}

void CStoneParticle::Tick(_double TimeDelta) 
{
	__super::Tick(TimeDelta);

	m_dSpeedY -= (_double)m_EffectWDesc.fGravity * (TimeDelta);

	m_pTransformCom->Set_Speed(m_dSpeedY);
	m_pTransformCom->Go_Up(TimeDelta);
	m_pTransformCom->Set_Speed(m_dSpeedX);
	m_pTransformCom->Go_Right(TimeDelta);
	m_pTransformCom->Set_Speed(m_dSpeedZ);
	m_pTransformCom->Go_Straight(TimeDelta);

	m_vSize.x += m_fSizeSpeedX * (_float)TimeDelta;
	m_vSize.y += m_fSizeSpeedX * (_float)TimeDelta;

	m_pTransformCom->Scaling(m_vSize);

	m_dLifeAccTime += TimeDelta;

	if (m_dLifeAccTime > 4.f)
	{
		CEffectW_Manager::Get_Instance()->Collect_Effect((CEffectW_Manager::EFFECTW_TYPE)m_EffectWDesc.eEffectWType, this);
		Set_Dead();
	}
}

void CStoneParticle::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Camera
	_vector m_vTargetPos = Convert::ToVector(pGameInstance->Get_CameraPosition());

	m_vTargetPos = { XMVectorGetX(m_vTargetPos), XMVectorGetY(m_vTargetPos) ,XMVectorGetZ(m_vTargetPos), XMVectorGetW(m_vTargetPos) };

	m_pTransformCom->LookAt(m_vTargetPos);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pGameInstance);
}

HRESULT CStoneParticle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(26);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CStoneParticle::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_T_e_Cmn_Stone003"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStoneParticle::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_float fUVRatioX = (1.f / 2.f) * (m_iFrame % 2);
	_float fUVRatioY = (1.f / 2.f) * (m_iFrame / 2);

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fUVRatioX", &fUVRatioX, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fUVRatioY", &fUVRatioY, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fColor", &m_fColor, sizeof _float)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CStoneParticle* CStoneParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStoneParticle* pInstance = new CStoneParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStoneParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CStoneParticle::Clone(void* pArg)
{
	CStoneParticle* pInstance = new CStoneParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStoneParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStoneParticle::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
