#include "pch.h"
#include "..\Public\Effect_Texture.h"

#include "GameInstance.h"

CEffect_Texture::CEffect_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CEffect_Texture::CEffect_Texture(const CEffect_Texture& rhs)
	: CEffect(rhs)
{

}

HRESULT CEffect_Texture::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Texture::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eEffectDesc.eEffectType = EFFECT_TEXTURE;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION),
		XMMatrixTranslation(m_eEffectDesc.vPivot.x, m_eEffectDesc.vPivot.y, m_eEffectDesc.vPivot.z)));

	if (OP_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
		m_fLifeTime = m_eEffectDesc.fStartLifeTimeMin;
	else if (OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
	{
		_float fRandNum = Random::Generate_Float(m_eEffectDesc.fStartLifeTimeMin, m_eEffectDesc.fStartLifeTimeMax);
		m_fLifeTime = fRandNum;
	}

	if (m_eEffectDesc.isRandomStartDelay)
	{
		_float fRandNum = Random::Generate_Float(m_eEffectDesc.fStartDelayMin, m_eEffectDesc.fStartDelayMax);
		m_fStartDelay = fRandNum;
	}
	else
		m_fStartDelay = m_eEffectDesc.fStartDelayMin;

	Check_PassIndex();

	return S_OK;
}

void CEffect_Texture::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

}

void CEffect_Texture::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);
}

HRESULT CEffect_Texture::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_isRendering)
	{
		if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
			return E_FAIL;

		if (BILLBOARD == m_eEffectDesc.eRenderMode)
		{
			if (FAILED(m_pVIBufferPointCom->Render()))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEffect_Texture::Add_Components(void)
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	/* For.Com_VIBufferPoint */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Point_Effect"),
		TEXT("Com_VIBufferPoint"), (CComponent**)&m_pVIBufferPointCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_VtxPoint_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Texture::SetUp_ShaderResources(void)
{
	if(FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	if (BILLBOARD == m_eEffectDesc.eRenderMode)
	{
		if (false == m_eEffectDesc.is3DStartSize)
		{
			if (FAILED(m_pShaderCom->SetUp_RawValue("g_vSize", (void*)&m_eEffectDesc.vStartSizeMin, sizeof(_float3))))
				return E_FAIL;
		}

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_fTextureOrder", (void*)&m_fTextureOrder, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_fCameraRightLookPos", (void*)&m_vCameraRightLookPos, sizeof(_float2))))
			return E_FAIL;
	}

	return S_OK;
}

void CEffect_Texture::Check_PassIndex(void)
{
	if (nullptr != m_pTextures[TEX_DIFFUSE])
	{
		if (nullptr != m_pTextures[TEX_NOISE])
			m_iPassIndex = 2;
		else
			m_iPassIndex = 1;

		if (nullptr != m_pTextures[TEX_DISTORTION])
			m_iPassIndex = 4;
	}
	else if (nullptr != m_pTextures[TEX_MASK])
	{
		if (nullptr != m_pTextures[TEX_RAMP])
			m_iPassIndex = 3;
		else
			m_iPassIndex = 6;

		if (nullptr != m_pTextures[TEX_DISTORTION])
			m_iPassIndex = 5;
		else
			m_iPassIndex = 6;
	}
	else
		m_iPassIndex = 0;
}

CEffect_Texture* CEffect_Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Texture* pInstance = new CEffect_Texture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Texture::Clone(void* pArg)
{
	CEffect_Texture* pInstance = new CEffect_Texture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Texture::Free(void)
{
	__super::Free();

	Safe_Release(m_pVIBufferPointCom);
	Safe_Release(m_pVIBufferRectCom);
}
