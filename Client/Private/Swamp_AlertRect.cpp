#include "pch.h"
#include "..\Public\Swamp_AlertRect.h"

#include "GameInstance.h"

CSwamp_AlertRect::CSwamp_AlertRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CSwamp_AlertRect::CSwamp_AlertRect(const CSwamp_AlertRect& rhs)
	: CGameObject(rhs)
{

}

HRESULT CSwamp_AlertRect::Initialize_Prototype()
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

HRESULT CSwamp_AlertRect::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	static _uint i = 0;
	m_iNum = i;
	++i;

	_vector vPos = XMVectorSetY(m_EffectDesc.pOwnerTransform->Get_State(CTransform::STATE_POSITION), 3.31f + m_iNum * 0.001f);
	m_pTransformCom->Scaling(_float3(0.03f, 1.f, 0.10f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

void CSwamp_AlertRect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	switch (m_eSTATE)
	{
	case STATE_SHOWON:
		m_fAlpha += 1.0f * (_float)TimeDelta;

		m_dSetDirAccTime += TimeDelta;

		if (m_dSetDirAccTime < 0.8)
		{
			_vector vLook = Calculate_Dir();

			m_pTransformCom->Rotation(_float3(0.f, -Get_Angle(vLook) + 90.f, 0.f));
		}

		if (m_fAlpha > 1.f)
			m_eSTATE = STATE_WAIT;
		break;
	case STATE_WAIT:
		m_dWaitAccTime += TimeDelta;

		if (m_dWaitAccTime > 0.1)
			m_eSTATE = STATE_SHOWOFF;
		break;
	case STATE_SHOWOFF:
		m_fAlpha -= 1.0f * (_float)TimeDelta;

		if (m_fAlpha < 0.f)
			m_isDead = true;
		break;

	}

}

void CSwamp_AlertRect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;

	Safe_Release(pGameInstance);
}

HRESULT CSwamp_AlertRect::Render()
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

		m_pShaderCom->Begin(11);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CSwamp_AlertRect::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Swamp_AlertRect"),
		TEXT("Com_Texture"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwamp_AlertRect::SetUp_ShaderResources()
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

	_bool b = false;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_bLiarColor", &b, sizeof _bool)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

_vector CSwamp_AlertRect::Calculate_Dir()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	_vector vPlayerPos = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 0, TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	Safe_Release(pGameInstance);
	_vector vDir = XMVector3Normalize(vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return vDir;
}

CSwamp_AlertRect* CSwamp_AlertRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSwamp_AlertRect* pInstance = new CSwamp_AlertRect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSwamp_AlertRect");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CSwamp_AlertRect::Clone(void* pArg)
{
	CSwamp_AlertRect* pInstance = new CSwamp_AlertRect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSwamp_AlertRect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwamp_AlertRect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
