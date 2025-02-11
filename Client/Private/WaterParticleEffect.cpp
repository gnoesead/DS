#include "pch.h"
#include "..\Public\WaterParticleEffect.h"

#include "GameInstance.h"

CWaterParticleEffect::CWaterParticleEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{

}

CWaterParticleEffect::CWaterParticleEffect(const CWaterParticleEffect& rhs)
	: CMasterEffect(rhs)
{

}

HRESULT CWaterParticleEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWaterParticleEffect::Initialize(void* pArg)
{
	if(nullptr != pArg)
		memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fPlusX = Random::Generate_Float(-0.4f, 0.4f);
	m_fPlusZ = Random::Generate_Float(-0.4f, 0.4f);



	m_dFrameSpeed = (_double)Random::Generate_Float(0.03f, 0.05f);

	m_dSpeedX = (_double)Random::Generate_Float(-2.f, 2.f);

	m_dSpeedY = (_double)Random::Generate_Float(3.5f, 6.5f);

	m_iFrame = Random::Generate_Int(0, 5);

	if(m_EffectDesc.eType == TYPE_NORMAL)
		m_vSize = { Random::Generate_Float(0.7f, 1.1f) , Random::Generate_Float(1.1f, 1.5f),1.f };
	else if (m_EffectDesc.eType == TYPE_SCREW)
		m_vSize = { Random::Generate_Float(0.7f, 1.0f) , Random::Generate_Float(0.7f, 1.0f),1.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vPos + XMVectorSet(m_fPlusX, 0.f, m_fPlusZ, 0.f));

	return S_OK;
}

void CWaterParticleEffect::Tick(_double TimeDelta) 
{
	__super::Tick(TimeDelta);

	m_dSpeedY -= 10.f * TimeDelta;

	m_pTransformCom->Set_Speed(m_dSpeedY);
	m_pTransformCom->Go_Up(TimeDelta);
	m_pTransformCom->Set_Speed(m_dSpeedX);
	m_pTransformCom->Go_Right(TimeDelta);

	Update_Frame(TimeDelta);

	m_pTransformCom->Scaling(m_vSize);
}

void CWaterParticleEffect::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Camera
	_vector m_vTargetPos = Convert::ToVector(pGameInstance->Get_CameraPosition());

	m_vTargetPos = { XMVectorGetX(m_vTargetPos), XMVectorGetY(m_vTargetPos) ,XMVectorGetZ(m_vTargetPos), XMVectorGetW(m_vTargetPos) };

	m_pTransformCom->LookAt(m_vTargetPos);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;

	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pGameInstance);
}

HRESULT CWaterParticleEffect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(22);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CWaterParticleEffect::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_T_e_cmn_Water_Splash008_A"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWaterParticleEffect::SetUp_ShaderResources()
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

	_float fUVRatioX = (1.f / 4.f) * (m_iFrame % 4);
	_float fUVRatioY = (1.f / 4.f) * (m_iFrame / 4);

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fUVRatioX", &fUVRatioX, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fUVRatioY", &fUVRatioY, sizeof _float)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CWaterParticleEffect::Update_Frame(_double TimeDelta)
{
	m_FrameAccTime += TimeDelta;

	if (m_FrameAccTime >= m_dFrameSpeed)
	{
		++m_iFrame;
		m_FrameAccTime = 0.0;
		if (m_iFrame >= m_iNumX * m_iNumY)
		{
			m_iFrame = m_iNumX * m_iNumY - 1;

			m_isDead = true;
		}

	}
}

CWaterParticleEffect* CWaterParticleEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWaterParticleEffect* pInstance = new CWaterParticleEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWaterParticleEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CWaterParticleEffect::Clone(void* pArg)
{
	CWaterParticleEffect* pInstance = new CWaterParticleEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWaterParticleEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWaterParticleEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
