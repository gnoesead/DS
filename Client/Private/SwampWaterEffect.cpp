#include "pch.h"
#include "..\Public\SwampWaterEffect.h"

#include "GameInstance.h"

CSwampWaterEffect::CSwampWaterEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{

}

CSwampWaterEffect::CSwampWaterEffect(const CSwampWaterEffect& rhs)
	: CMasterEffect(rhs)
{

}

HRESULT CSwampWaterEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwampWaterEffect::Initialize(void* pArg)
{
	if(nullptr != pArg)
		memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vPos);

	m_pTransformCom->Scaling(m_EffectDesc.vScale);

	m_dFrameSpeed = (_double)Random::Generate_Float(0.019f, 0.023f);

	return S_OK;
}

void CSwampWaterEffect::Tick(_double TimeDelta) 
{
	m_dDelayAccTime += TimeDelta;

	if (m_EffectDesc.dDelay > m_dDelayAccTime)
		return;

	__super::Tick(TimeDelta);

	Update_Frame(TimeDelta);
}

void CSwampWaterEffect::LateTick(_double TimeDelta)
{
	if (m_EffectDesc.dDelay > m_dDelayAccTime)
		return;

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

HRESULT CSwampWaterEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(21);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CSwampWaterEffect::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_VILLAGE, TEXT("Prototype_Component_Texture_SwampWater"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwampWaterEffect::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_iFrame)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CSwampWaterEffect::Update_Frame(_double TimeDelta)
{
	m_FrameAccTime += TimeDelta;

	if (m_FrameAccTime >= m_dFrameSpeed)
	{
		++m_iFrame;
		m_FrameAccTime = 0.0;
		if (m_iFrame >= 20)
		{
			m_isDead = true;
		}
		//
	}
}

CSwampWaterEffect* CSwampWaterEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwampWaterEffect* pInstance = new CSwampWaterEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSwampWaterEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CSwampWaterEffect::Clone(void* pArg)
{
	CSwampWaterEffect* pInstance = new CSwampWaterEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSwampWaterEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwampWaterEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
