#include "pch.h"
#include "..\Public\AlertCircle.h"

#include "GameInstance.h"

static _uint iNum = 0;

CAlertCircle::CAlertCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{

}

CAlertCircle::CAlertCircle(const CAlertCircle& rhs)
	: CMasterEffect(rhs)
{

}

HRESULT CAlertCircle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlertCircle::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (m_EffectDesc.iType == TYPE_KICKDOWN)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_POSITION));
		m_fLandY = XMVectorGetY(m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Scaling(_float3(6.f, 6.f, 6.f));
	}
	else if (m_EffectDesc.iType == TYPE_ROOMCHANGE)
	{
		static _float fOffsetY = 0.f;
		
		m_dCreateTimeInterval = 0.4;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(126.536f , 0.01f + fOffsetY,123.840f , 1.f));

		fOffsetY += 0.001f;

		++iNum;

		m_iNum = iNum;
	}
	m_pTransformCom->Rotation(_float3(90.f, 0.f, 0.f));
	

	return S_OK;
}

void CAlertCircle::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_EffectDesc.iType == TYPE_KICKDOWN)
	{
		_vector vPos = m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_LOOK)) * 0.8f;
		vPos = XMVectorSetY(vPos, m_fLandY + 0.05f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (m_EffectDesc.iType == TYPE_ROOMCHANGE)
	{
		m_pTransformCom->Scaling(_float3(m_fScale, m_fScale, m_fScale));
		m_fScale += 55.f * (_float)TimeDelta;

		m_dAccTime += TimeDelta;

		if (!m_bMakeAlertCircle  && (m_dAccTime > m_dCreateTimeInterval) && m_iNum <3)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			CAlertCircle::EFFECTDESC EffectDesc;
			EffectDesc.pOwnerTransform = m_pTransformCom;
			EffectDesc.iType = CAlertCircle::TYPE_ROOMCHANGE;

			pGameInstance->Add_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_AlertCircle"), &EffectDesc, false);

			Safe_Release(pGameInstance);

			m_bMakeAlertCircle = true;
		}

		if (m_fScale > 50.f)
		{
			m_fScale = 50.f;

			if (m_dAccTime > 0.7 + m_dCreateTimeInterval * (3 - m_iNum))
				m_isDead = true;

			if (3 == m_iNum)
			{
				iNum = 0;
			}
		}
	}
	
}

void CAlertCircle::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EffectNoBloom, this)))
		return;

	if (m_EffectDesc.iType == TYPE_KICKDOWN)
	{
		if (STATE_SHOWON == m_eState)
		{
			m_fAlpha += 1.5f * (_float)TimeDelta;
			if (m_fAlpha > 1.f)
				m_eState = STATE_SHOWOFF;
		}
		else
			m_fAlpha -= 2.f * (_float)TimeDelta;

		if (m_fAlpha < 0.f)
			m_isDead = true;
	}
	else if (m_EffectDesc.iType == TYPE_ROOMCHANGE)
	{
		m_fAlpha = 0.8f;
	}
	
	Safe_Release(pGameInstance);
}

HRESULT CAlertCircle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(18);

	m_pVIBufferCom->Render();



	return S_OK;
}

HRESULT CAlertCircle::Add_Components()
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

HRESULT CAlertCircle::SetUp_ShaderResources()
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

CAlertCircle* CAlertCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAlertCircle* pInstance = new CAlertCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAlertCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CAlertCircle::Clone(void* pArg)
{
	CAlertCircle* pInstance = new CAlertCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAlertCircle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAlertCircle::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
