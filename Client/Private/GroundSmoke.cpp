#include "pch.h"
#include "..\Public\GroundSmoke.h"

#include "GameInstance.h"

CGroundSmoke::CGroundSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{

}

CGroundSmoke::CGroundSmoke(const CGroundSmoke& rhs)
	: CMasterEffect(rhs)
{

}

HRESULT CGroundSmoke::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGroundSmoke::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fPlusX = Random::Generate_Float(m_EffectDesc.vStartPosX.x, m_EffectDesc.vStartPosX.y);
	m_fPlusY = Random::Generate_Float(m_EffectDesc.vStartPosY.x, m_EffectDesc.vStartPosY.y);
	m_fPlusZ = Random::Generate_Float(m_EffectDesc.vStartPosZ.x, m_EffectDesc.vStartPosZ.y);


	m_dFrameSpeed = (_double)Random::Generate_Float(m_EffectDesc.vFrameSpeed.x, m_EffectDesc.vFrameSpeed.y);

	m_dSpeedX = (_double)Random::Generate_Float(m_EffectDesc.vSpeedX.x, m_EffectDesc.vSpeedX.y);
	m_dSpeedY = (_double)Random::Generate_Float(m_EffectDesc.vSpeedY.x, m_EffectDesc.vSpeedY.y);
	m_dSpeedZ = (_double)Random::Generate_Float(m_EffectDesc.vSpeedZ.x, m_EffectDesc.vSpeedZ.y);

	m_vSize = { Random::Generate_Float(m_EffectDesc.vSizeX.x,m_EffectDesc.vSizeX.y) , Random::Generate_Float(m_EffectDesc.vSizeY.x, m_EffectDesc.vSizeY.y),1.f };
	
	m_fSizeSpeedX = Random::Generate_Float(m_EffectDesc.vSizeSpeedX.x, m_EffectDesc.vSizeSpeedX.y);
	m_fSizeSpeedY = Random::Generate_Float(m_EffectDesc.vSizeSpeedY.x, m_EffectDesc.vSizeSpeedY.y);

	m_iFrame = Random::Generate_Int(0, 12);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vPos + XMVectorSet(m_fPlusX, m_fPlusY, m_fPlusZ, 0.f));

	m_fAlpha = 0.5f;

	m_fColor = Random::Generate_Float(0.4f, 0.6f);

	return S_OK;
}

void CGroundSmoke::Tick(_double TimeDelta) 
{
	__super::Tick(TimeDelta);

	m_dSpeedY -= m_EffectDesc.fGravity * (TimeDelta);

	m_pTransformCom->Set_Speed(m_dSpeedY);
	m_pTransformCom->Go_Up(TimeDelta);
	m_pTransformCom->Set_Speed(m_dSpeedX);
	m_pTransformCom->Go_Right(TimeDelta);
	m_pTransformCom->Set_Speed(m_dSpeedZ);
	m_pTransformCom->Go_Straight(TimeDelta);

	Update_Frame(TimeDelta);

	m_vSize.x += m_fSizeSpeedX * (_float)TimeDelta;
	m_vSize.y += m_fSizeSpeedX * (_float)TimeDelta;

	m_pTransformCom->Scaling(m_vSize);
}

void CGroundSmoke::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Camera
	_vector m_vTargetPos = Convert::ToVector(pGameInstance->Get_CameraPosition());

	m_vTargetPos = { XMVectorGetX(m_vTargetPos), XMVectorGetY(m_vTargetPos) ,XMVectorGetZ(m_vTargetPos), XMVectorGetW(m_vTargetPos) };

	m_pTransformCom->LookAt(m_vTargetPos);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EffectNoBloom, this)))
		return;

	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pGameInstance);
}

HRESULT CGroundSmoke::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(25);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CGroundSmoke::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_T_e_cmn_Smoke023"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGroundSmoke::SetUp_ShaderResources()
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

	_float fUVRatioX = (1.f / 6.f) * (m_iFrame % 6);
	_float fUVRatioY = (1.f / 6.f) * (m_iFrame / 6);

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fUVRatioX", &fUVRatioX, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fUVRatioY", &fUVRatioY, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_fAlpha, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fColor", &m_fColor, sizeof _float)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGroundSmoke::Update_Frame(_double TimeDelta)
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

CGroundSmoke* CGroundSmoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGroundSmoke* pInstance = new CGroundSmoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGroundSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CGroundSmoke::Clone(void* pArg)
{
	CGroundSmoke* pInstance = new CGroundSmoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGroundSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGroundSmoke::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
