#include "pch.h"
#include "..\Public\AlertRect.h"

#include "GameInstance.h"

CAlertRect::CAlertRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CAlertRect::CAlertRect(const CAlertRect& rhs)
	: CGameObject(rhs)
{

}

HRESULT CAlertRect::Initialize_Prototype()
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

HRESULT CAlertRect::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_vector vPos = XMVectorSetY(m_EffectDesc.vPos, 0.01f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pTransformCom->Scaling(_float3(0.01f, 1.f, 0.1f));

	m_pTransformCom->Rotation(_float3(0.f, Get_Angle(m_EffectDesc.vLook), 0.f));
	
	return S_OK;
}

void CAlertRect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	m_pTransformCom->Rotation(_float3(0.f, -Get_Angle(m_EffectDesc.vLook), 0.f));

	m_pTransformCom->Scaling(_float3(1.5f, 1.f, 0.003f));
}

void CAlertRect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;

	Safe_Release(pGameInstance);
}

HRESULT CAlertRect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(10);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CAlertRect::Add_Components()
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

	/* For.Com_Model*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_AlertRect"),
		TEXT("Com_Texture"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAlertRect::SetUp_ShaderResources()
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

	_float4 vCamPos = pGameInstance->Get_CameraPosition();

	if (FAILED(m_pShaderCom->SetUp_Vector("g_vCamPosition", &vCamPos)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fDiffuseRatio", &m_fDiffuseRatio, sizeof _float)))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

CAlertRect* CAlertRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAlertRect* pInstance = new CAlertRect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAlertRect");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CAlertRect::Clone(void* pArg)
{
	CAlertRect* pInstance = new CAlertRect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAlertRect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAlertRect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
