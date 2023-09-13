#include "pch.h"
#include "..\Public\HandAura_Akaza.h"

#include "GameInstance.h"

CHandAura_Akaza::CHandAura_Akaza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{
}

CHandAura_Akaza::CHandAura_Akaza(const CHandAura_Akaza& rhs)
	: CMasterEffect(rhs)
{
}

HRESULT CHandAura_Akaza::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHandAura_Akaza::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);
	Safe_AddRef(m_EffectDesc.pBone);
	Safe_AddRef(m_EffectDesc.pOwnerTransform);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(m_EffectDesc.vScale);
	m_pTransformCom->Rotation(_float3(0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Convert::ToVector_W1(m_EffectDesc.vPos));

	return S_OK;
}

void CHandAura_Akaza::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_dAccTime += TimeDelta;
}

void CHandAura_Akaza::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EffectBloom, this)))
		return;

	if (m_EffectDesc.vTime.y > m_dAccTime)
	{
		m_fAlpha += m_EffectDesc.vTime.x * (_float)TimeDelta;
		if (m_fAlpha > 1.f)
			m_fAlpha = 1.f;
	}
	else if(m_EffectDesc.vTime.y < m_dAccTime)
		m_fAlpha -= 5.f * (_float)TimeDelta;

	_matrix matWorld = (m_pTransformCom->Get_WorldMatrix() * Remove::Scale(m_EffectDesc.pBone->Get_OffsetMatrix() *
		m_EffectDesc.pBone->Get_CombinedTransformationMatrix() *
		m_EffectDesc.pBone->Get_PivotMatrix() * m_EffectDesc.pOwnerTransform->Get_WorldMatrix()));
	

	m_WorldMatrix = Convert::ToFloat4x4(matWorld);


	if (m_fAlpha < 0.f)
		Set_Dead();

	Safe_Release(pGameInstance);
}

HRESULT CHandAura_Akaza::Render()
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

		m_pShaderCom->Begin(13);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CHandAura_Akaza::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Akaza_HandAura"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

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

	return S_OK;
}

HRESULT CHandAura_Akaza::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;*/

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;
	
	Safe_Release(pGameInstance);

	return S_OK;
}

CHandAura_Akaza* CHandAura_Akaza::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHandAura_Akaza* pInstance = new CHandAura_Akaza(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHandAura_Akaza");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CHandAura_Akaza::Clone(void* pArg)
{
	CHandAura_Akaza* pInstance = new CHandAura_Akaza(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHandAura_Akaza");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHandAura_Akaza::Free()
{
	__super::Free();

	Safe_Release(m_EffectDesc.pOwnerTransform);
	Safe_Release(m_EffectDesc.pBone);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
