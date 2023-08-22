#include "pch.h"
#include "..\Public\TrainSmoke.h"

#include "GameInstance.h"

CTrainSmoke::CTrainSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CTrainSmoke::CTrainSmoke(const CTrainSmoke& rhs)
	: CGameObject(rhs)
{

}

HRESULT CTrainSmoke::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrainSmoke::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fPlusZ = Random::Generate_Float(-0.1f, 0.1f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(205.5f, 8.2f, 415.0f + m_fPlusZ, 1.f));

	m_dFrameSpeed = (_double)Random::Generate_Float(0.05f, 0.08f);

	m_dSpeedX = (_double)Random::Generate_Float(-2.f, 2.f);

	m_dSpeedY = (_double)Random::Generate_Float(8.f, 12.f);

	m_iFrame = Random::Generate_Int(0, 15);

	m_fSizeSpeed = Random::Generate_Float(4.f, 35.f);

	m_fSize = Random::Generate_Float(2.f, 4.f);

	return S_OK;
}

void CTrainSmoke::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	Update_Frame(TimeDelta);

	m_fSize += m_fSizeSpeed * (_float)TimeDelta;
	m_pTransformCom->Scaling(_float3(m_fSize, m_fSize, m_fSize));

	m_pTransformCom->Set_Speed(m_dSpeedY);
	m_pTransformCom->Go_Up(TimeDelta);
	m_pTransformCom->Set_Speed(m_dSpeedX);
	m_pTransformCom->Go_Left(TimeDelta);
}

void CTrainSmoke::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;

	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pGameInstance);
}

HRESULT CTrainSmoke::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(16);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTrainSmoke::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_TRAIN, TEXT("Prototype_Component_Texture_Train_Smoke"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrainSmoke::SetUp_ShaderResources()
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

	_float fUVRatioX = 0.25f * (m_iFrame % 4);
	_float fUVRatioY = 0.25f * (m_iFrame / 4);

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fUVRatioX", &fUVRatioX, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fUVRatioY", &fUVRatioY, sizeof _float)))
		return E_FAIL;

	_float fAlpha = 1.f - ((_float)m_iFrame / (m_iNumX * m_iNumY - 1));

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &fAlpha, sizeof _float)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CTrainSmoke::Update_Frame(_double TimeDelta)
{
	m_FrameAccTime += TimeDelta;

	if (m_FrameAccTime >= m_dFrameSpeed)
	{
		++m_iFrame;
		m_FrameAccTime = 0.0;
		if (m_iFrame >= m_iNumX * m_iNumY)
		{
			m_iFrame = 0;
			m_fSize = 3.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(205.5f, 9.8f, 415.0f + m_fPlusZ, 1.f));
		}

	}
}

CTrainSmoke* CTrainSmoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrainSmoke* pInstance = new CTrainSmoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrainSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CTrainSmoke::Clone(void* pArg)
{
	CTrainSmoke* pInstance = new CTrainSmoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrainSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrainSmoke::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
