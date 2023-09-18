#include "pch.h"
#include "..\Public\CustomParticle.h"

#include "GameInstance.h"

#include "ParticleManager.h"
#include "PlayerManager.h"

CCustomParticle::CCustomParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCustomParticle::CCustomParticle(const CCustomParticle& rhs)
	: CGameObject(rhs)
{
}

void CCustomParticle::Reset_Particle(CUSTOMPARTDESC* pCustomPartDesc)
{
	ZeroMemory(&m_CustomPartDesc, sizeof m_CustomPartDesc);

	m_CustomPartDesc = *pCustomPartDesc;

	m_pVIBufferCom->Reset_Particle(&m_CustomPartDesc.VIB_CustomPartDesc);

	Set_Dead(false);
	m_dTimeAcc = 0.0;
}

HRESULT CCustomParticle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCustomParticle::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_CustomPartDesc, pArg, sizeof m_CustomPartDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fAlpha = 1.f;

	return S_OK;
}

void CCustomParticle::Tick(_double dTimeDelta)
{
	if (true == m_isDead)
		return;
	if (CPlayerManager::GetInstance()->Get_Player_Death() == false)
	{
		__super::Tick(dTimeDelta);

		/*if (m_CustomPartDesc.fLifeTime * 0.7f > m_dTimeAcc)
		{
			m_fAlpha += (_float)dTimeDelta * 1.5f;

			if (1.f < m_fAlpha)
				m_fAlpha = 1.f;
		}
		else
		{
			m_fAlpha -= (_float)dTimeDelta * 0.3f;

			if (0.f > m_fAlpha)
				m_fAlpha = 0.f;
		}*/

		//m_fScale += (_float)dTimeDelta * 0.7f;

		/*if (1.f < m_fScale)
			m_fScale = 1.f;

		m_pTransformCom->Scaling(m_fScale);*/

		//m_pTransformCom->Turn(XMConvertToRadians(90.f * (_float)dTimeDelta), XMVectorSet(0.f, 1.f, 0.f, 0.f));

		m_pVIBufferCom->Tick(dTimeDelta);

		m_dTimeAcc += dTimeDelta;
	}
}

void CCustomParticle::LateTick(_double dTimeDelta)
{
	if (CPlayerManager::GetInstance()->Get_Player_Death() == false)
	{
		__super::LateTick(dTimeDelta);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, Convert::ToVector_W1(m_CustomPartDesc.vPosition));

		/*_int iCount = _int(m_dTimeAcc / (m_dCycle / m_CustomPartDesc.dSpriteSpeed));

		m_vTexCoord.x = _float(iCount % m_CustomPartDesc.vSpriteCount.x) * (1.f / (_float)(m_CustomPartDesc.vSpriteCount.x));

		m_vTexCoord.y = _float(iCount / m_CustomPartDesc.vSpriteCount.x) * (1.f / (_float)(m_CustomPartDesc.vSpriteCount.y));*/

		if (m_CustomPartDesc.VIB_CustomPartDesc.fLifeTime < m_dTimeAcc)
		{
			CParticleManager::GetInstance()->Collect_Particle(m_CustomPartDesc.szPoolTag, this);
			m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

			m_dTimeAcc = 0.0;

			Set_Dead();
		}
	}
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_Effect_Particle, this)))
		return;
}

HRESULT CCustomParticle::Render()	
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_CustomPartDesc.eShaderPass);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CCustomParticle::Add_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.dSpeedPerSec = 10.f;
	TransformDesc.dRadianRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
	{
		MSG_BOX("Failed to Add_Com_Transform : CCustomParticle");
		return E_FAIL;
	}

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed to Add_Com_Renderer : CCustomParticle");
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		MSG_BOX("Failed to Add_Com_Shader : CCustomParticle");
		return E_FAIL;
	}

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_CustomPartDesc.szBufferTag,
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_CustomPartDesc.VIB_CustomPartDesc)))
	{
		MSG_BOX("Failed to Add_Com_VIBuffer : CCustomParticle");
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_CustomPartDesc.szTextureTag,
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		MSG_BOX("Failed to Add_Com_Texture : CCustomParticle");
		return E_FAIL;
	}

	if (true == m_CustomPartDesc.bUseRamp)
	{
		/* for.Com_RampTexture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_CustomPartDesc.szRampTextureTag,
			TEXT("Com_RampTexture"), (CComponent**)&m_pRampTextureCom)))
		{
			MSG_BOX("Failed to Add_Com_RampTexture : CCustomParticle");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CCustomParticle::SetUp_ShaderResources()
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

	_float4	vCameraPosition = pGameInstance->Get_CameraPosition();
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vCamPosition", &vCameraPosition, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);
	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (true == m_CustomPartDesc.bUseRamp)
	{
		if (FAILED(m_pRampTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_RampTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_fRampMax", &m_CustomPartDesc.fRampMax, sizeof(_float))))
			return E_FAIL;
	}

	/*if (FAILED(m_pShaderCom->SetUp_RawValue("g_vTexCoord", &m_vTexCoord, sizeof(_float2))))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vSpriteCount", &m_CustomPartDesc.VIB_CustomPartDesc.vSpriteCount, sizeof(_int2))))
		return E_FAIL;

	
	return S_OK;
}

CCustomParticle* CCustomParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCustomParticle* pInstance = new CCustomParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCustomParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CCustomParticle::Clone(void* pArg)
{
	CCustomParticle* pInstance = new CCustomParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCustomParticle");
		Safe_Release(pInstance);
	}

	return pInstance;;
}

void CCustomParticle::Free()
{
	__super::Free();
	if (true == m_CustomPartDesc.bUseRamp)
		Safe_Release(m_pRampTextureCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}
