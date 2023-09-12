#include "pch.h"
#include "..\Public\AlertMesh_Akaza.h"

#include "GameInstance.h"

CAlertMesh_Akaza::CAlertMesh_Akaza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{
}

CAlertMesh_Akaza::CAlertMesh_Akaza(const CAlertMesh_Akaza& rhs)
	: CMasterEffect(rhs)
{
}

HRESULT CAlertMesh_Akaza::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlertMesh_Akaza::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	switch (m_EffectDesc.eType)
	{
	case TYPE_INNER_0:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Akaza_Inner_00"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Scaling(m_EffectDesc.vScale);

		m_pTransformCom->Rotation(_float3(0.f, 0.f, -90.f));

		break;
	case TYPE_INNER_1:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Akaza_Inner_01"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Scaling(m_EffectDesc.vScale);
		 
		m_pTransformCom->Rotation(_float3(0.f, 0.f, -90.f));

		break;
	case TYPE_OUTWAVE:
		/* For.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Akaza_OuterWave"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Scaling(m_EffectDesc.vScale);

		m_pTransformCom->Rotation(_float3(0.f, 0.f, 0.f));

		break;
	}

	return S_OK;
}

void CAlertMesh_Akaza::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_vector vPos = m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_LOOK)) * 0.8f;
	vPos = XMVectorSetY(vPos, m_EffectDesc.fLandY);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_vCustomUV.x = m_EffectDesc.vCustomUV.x * m_dAccTime;
	m_vCustomUV.y = m_EffectDesc.vCustomUV.y * m_dAccTime;

	m_dAccTime += TimeDelta;
}

void CAlertMesh_Akaza::LateTick(_double TimeDelta)
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

HRESULT CAlertMesh_Akaza::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(12);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CAlertMesh_Akaza::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_RampTexture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ramp_efs_charge #1585379"),
		TEXT("Com_RampTexture"), (CComponent**)&m_pRampTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlertMesh_Akaza::SetUp_ShaderResources()
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

	if (FAILED(m_pRampTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pRampTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_RampTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vCustomUV", &m_vCustomUV, sizeof _float2)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

CAlertMesh_Akaza* CAlertMesh_Akaza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAlertMesh_Akaza* pInstance = new CAlertMesh_Akaza(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAlertMesh_Akaza");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CAlertMesh_Akaza::Clone(void* pArg)
{
	CAlertMesh_Akaza* pInstance = new CAlertMesh_Akaza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAlertMesh_Akaza");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAlertMesh_Akaza::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRampTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
