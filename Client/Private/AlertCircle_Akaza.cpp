#include "pch.h"
#include "..\Public\AlertCircle_Akaza.h"

#include "GameInstance.h"

static _uint iNum = 0;

CAlertCircle_Akaza::CAlertCircle_Akaza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{

}

CAlertCircle_Akaza::CAlertCircle_Akaza(const CAlertCircle_Akaza& rhs)
	: CMasterEffect(rhs)
{

}

HRESULT CAlertCircle_Akaza::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlertCircle_Akaza::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(m_EffectDesc.vScale);
	m_pTransformCom->Rotation(_float3(90.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_POSITION));

	return S_OK;
}

void CAlertCircle_Akaza::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	 
	 _vector vPos = m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	 vPos = XMVectorSetY(vPos, m_EffectDesc.fLandY);
	 m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CAlertCircle_Akaza::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EffectNoBloom, this)))
		return;

	if (STATE_SHOWON == m_eState)
	{
		m_fAlpha += m_EffectDesc.vTime.x * (_float)TimeDelta;
		if (m_fAlpha > 1.f)
			m_eState = STATE_SHOWOFF;
	}
	else
		m_fAlpha -= m_EffectDesc.vTime.y * (_float)TimeDelta;

	if (m_fAlpha < 0.f)
		Set_Dead();
	
	Safe_Release(pGameInstance);
}

HRESULT CAlertCircle_Akaza::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(18);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CAlertCircle_Akaza::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_e_Circle3_02"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlertCircle_Akaza::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CAlertCircle_Akaza* CAlertCircle_Akaza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAlertCircle_Akaza* pInstance = new CAlertCircle_Akaza(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAlertCircle_Akaza");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CAlertCircle_Akaza::Clone(void* pArg)
{
	CAlertCircle_Akaza* pInstance = new CAlertCircle_Akaza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAlertCircle_Akaza");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAlertCircle_Akaza::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
