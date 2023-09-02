#include "pch.h"
#include "..\Public\Swamp.h"

#include "GameInstance.h"
#include "SwampWaterEffect.h"

static _uint g_iNum = 0;

CSwamp::CSwamp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CSwamp::CSwamp(const CSwamp& rhs)
	: CGameObject(rhs)
{

}

HRESULT CSwamp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

static _float  Get_Angle(_fvector vLook)
{
	_vector vNormalizeLook = XMVector3Normalize(vLook);

	float fAngle = atan2f(XMVectorGetZ(vNormalizeLook), XMVectorGetX(vNormalizeLook));

	fAngle = XMConvertToDegrees(fAngle);

	if (fAngle >= 180.f)
		fAngle += 180.f;

	return fAngle ;
}

HRESULT CSwamp::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(_float3(90.f, 0.f, 0.f));

	m_iNum = g_iNum;
	++g_iNum;

	return S_OK;
}

void CSwamp::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Pattern_Setting(TimeDelta);



	State_Setting(TimeDelta);

	m_pTransformCom->Scaling(_float3(m_fSize, m_fSize, 1.f));

	Position_Setting();
}

void CSwamp::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

	Safe_Release(pGameInstance);
}

HRESULT CSwamp::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(20);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CSwamp::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_T_e_Skl_In_Slash_Par001"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwamp::SetUp_ShaderResources()
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

	Safe_Release(pGameInstance);

	return S_OK;
}

void CSwamp::Position_Setting()
{
	_vector vPos = XMVectorSetY(m_EffectDesc.pTransform->Get_State(CTransform::STATE_POSITION) ,3.4f + 0.001f * m_iNum);

	if(STATE_INCREASING != m_eCurState && STATE_DECREASING != m_eCurState)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

}

void CSwamp::Pattern_Setting(_double TimeDelta)
{
	switch (m_eCurPattern)
	{
	case PATTERN_NONE:
		m_eCurState = STATE_WAIT;
		m_bWaterEffect = false;
		break;
	case PATTERN_STEPIN:
		m_eCurState = STATE_INCREASING;
		m_fScaleSpeed = 8.f;
	
		if (!m_bWaterEffect && m_fSize > 2.5f)
		{
			Create_WatterEffect();
			m_bWaterEffect = true;
		}

		if (m_fSize > 3.f)
		{
			m_fSize = 3.f;
			m_eCurState = STATE_WAIT;
			m_eCurPattern = PATTERN_NONE;
			m_bWaterEffect = false;
		}

		break;
	case PATTERN_STEPOUT:
		m_eCurState = STATE_DECREASING;
		m_fScaleSpeed = 3.f;

		if (!m_bWaterEffect)
		{
			Create_WatterEffect();
			m_bWaterEffect = true;
		}

		if (m_fSize < 0.001f)
		{
			m_fSize = 0.001f;
			m_eCurState = STATE_WAIT;
			m_eCurPattern = PATTERN_NONE;
			m_bWaterEffect = false;
		}
		break;
	case PATTERN_DECREASE:
		m_eCurState = STATE_DECREASING;
		m_fScaleSpeed = 8.f;

		if (m_fSize < 0.001f)
		{
			m_fSize = 0.001f;
			m_eCurState = STATE_WAIT;
			m_eCurPattern = PATTERN_NONE;
		}
		break;
	
	case PATTERN_SHORYU:
		if (!m_bWaterEffect)
		{
			Create_WatterEffect();
			m_bWaterEffect = true;
		}

		m_eCurState = STATE_DECREASING;
		m_fScaleSpeed = 2.f;
		
		if (m_fSize < 0.001f)
		{
			m_fSize = 0.001f;
			m_bDown = false;
			m_bWaterEffect = false;
			m_eCurPattern = PATTERN_NONE;
		}
	
		break;
	case PATTERN_THROWAWAY:
		if (!m_bWaterEffect)
		{
			Create_WatterEffect();
			m_bWaterEffect = true;
		}

		m_eCurState = STATE_DECREASING;
		m_fScaleSpeed = 1.f;

		if (m_fSize < 0.001f)
		{
			m_isDead = true;
		}
		break;
	case PATTERN_BIG:
		m_eCurState = STATE_INCREASING;
		m_fScaleSpeed = 8.f;

		if (m_fSize > 12.f)
		{
			m_fSize = 12.f;

			m_dBigAccTime += TimeDelta;

			if (m_dBigAccTime > 1.5)
			{
				m_dBigAccTime = 0.0;
				m_eCurPattern = PATTERN_TOSMALL;
			}
		}
		break;
	case PATTERN_TOSMALL:
		m_eCurState = STATE_DECREASING;
		m_fScaleSpeed = 15.f;

		if (m_fSize < 3.f)
		{
			m_fSize = 3.f;
			m_eCurState = STATE_WAIT;
			m_eCurPattern = PATTERN_NONE;
		}
		break;
	default:
		break;
	}
}

void CSwamp::State_Setting(_double TimeDelta)
{
	switch (m_eCurState)
	{
	case STATE_WAIT:
		break;
	case STATE_INCREASING:
		m_fSize += m_fScaleSpeed * (_float)TimeDelta;
	
		break;
	case STATE_DECREASING:
		m_fSize -= m_fScaleSpeed * (_float)TimeDelta;
		
		break;
	default:
		break;
	}
}

void CSwamp::Create_WatterEffect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CSwampWaterEffect::EFFECTDESC EffectDesc;
	EffectDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.8f, 0.f, 0.f);
	EffectDesc.vScale = _float3(3.f, 7.f, 1.f);
	EffectDesc.dDelay = 0.0;

	pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Swamp"), TEXT("Prototype_GameObject_SwampWaterEffect"), &EffectDesc);

	EffectDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.1f, 0.8f, 0.00f, 0.f);
	EffectDesc.vScale = _float3(-1.5f, 4.f, 1.f);
	EffectDesc.dDelay = 0.12;

	pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Swamp"), TEXT("Prototype_GameObject_SwampWaterEffect"), &EffectDesc);

	EffectDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(-0.1f, 0.8f, -0.0f, 0.f);
	EffectDesc.vScale = _float3(-2.f, 3.f, 1.f);
	EffectDesc.dDelay = 0.17;

	pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Swamp"), TEXT("Prototype_GameObject_SwampWaterEffect"), &EffectDesc);

	Safe_Release(pGameInstance);
}

CSwamp* CSwamp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwamp* pInstance = new CSwamp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSwamp");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CSwamp::Clone(void* pArg)
{
	CSwamp* pInstance = new CSwamp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSwamp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwamp::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
