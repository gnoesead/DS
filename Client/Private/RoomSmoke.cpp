#include "pch.h"
#include "..\Public\RoomSmoke.h"

#include "GameInstance.h"

CRoomSmoke::CRoomSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{

}

CRoomSmoke::CRoomSmoke(const CRoomSmoke& rhs)
	: CMasterEffect(rhs)
{

}

HRESULT CRoomSmoke::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRoomSmoke::Initialize(void* pArg)
{
	if(nullptr != pArg)
		memcpy(&m_EffectDesc, pArg, sizeof m_EffectDesc);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (TYPE_ALLROOM == m_EffectDesc.eType)
	{
		m_fPlusX = Random::Generate_Float(-16.f, 16.f);
		m_fPlusZ = Random::Generate_Float(-16.f, 16.f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(126.536f + m_fPlusX, 0.2f, 123.840f + m_fPlusZ, 1.f));

		m_dFrameSpeed = (_double)Random::Generate_Float(0.01f, 0.03f);

		m_dSpeedX = (_double)Random::Generate_Float(-2.f, 2.f);

		m_dSpeedY = (_double)Random::Generate_Float(8.f, 12.f);

		m_iFrame = Random::Generate_Int(0, 10);

		m_vSize = { Random::Generate_Float(1.f, 2.f) , Random::Generate_Float(1.f, 5.f),Random::Generate_Float(1.f, 2.f) };
	}
	else if (TYPE_PART == m_EffectDesc.eType)
	{
		m_fPlusX = Random::Generate_Float(-1.f, 1.f);
		m_fPlusY = Random::Generate_Float(0.f, 0.8f);
		m_fPlusZ = Random::Generate_Float(-1.f, 1.f);

		m_dFrameSpeed = (_double)Random::Generate_Float(0.02f, 0.02f);

		m_vSize = { Random::Generate_Float(1.f, 3.f) , Random::Generate_Float(2.f, 4.f),Random::Generate_Float(1.f, 3.f) };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vPos + XMVectorSet(m_fPlusX, m_fPlusY + 0.5f, m_fPlusZ, 0.f));
		m_iFrame = 0;
	}
	
	m_fTextureOrder = 3.f;
	m_fAlpha = 0.4f;

	return S_OK;
}

void CRoomSmoke::Tick(_double TimeDelta) 
{
	__super::Tick(TimeDelta);

	Update_Frame(TimeDelta);

	if (m_iFrame > 16)
	{
		//m_vSize.y -= 16.f * (_float)TimeDelta;
		/*m_pTransformCom->Set_Speed(10.0);
		m_pTransformCom->Go_Down(TimeDelta);*/

		m_fAlpha -= 0.5f * (_float)TimeDelta;
	}
	else
	{
		m_vSize.x += 2.f * (_float)TimeDelta;
		m_vSize.y += 3.f * (_float)TimeDelta;
		m_vSize.z += 2.f * (_float)TimeDelta;
	}

	m_pTransformCom->Scaling(m_vSize);

	if (m_fAlpha < 0.f)
		m_isDead = true;
}

void CRoomSmoke::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Camera
	_vector m_vTargetPos = Convert::ToVector(pGameInstance->Get_CameraPosition());

	m_vTargetPos = { XMVectorGetX(m_vTargetPos), XMVectorGetY(m_vTargetPos) ,XMVectorGetZ(m_vTargetPos), XMVectorGetW(m_vTargetPos) };

	m_pTransformCom->LookAt(m_vTargetPos);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this)))
		return;

	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	Safe_Release(pGameInstance);
}

HRESULT CRoomSmoke::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(19);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CRoomSmoke::Add_Components()
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

HRESULT CRoomSmoke::SetUp_ShaderResources()
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

	Safe_Release(pGameInstance);

	return S_OK;
}

void CRoomSmoke::Update_Frame(_double TimeDelta)
{
	m_FrameAccTime += TimeDelta;

	if (m_FrameAccTime >= m_dFrameSpeed)
	{
		++m_iFrame;
		m_FrameAccTime = 0.0;
		if (m_iFrame >= m_iNumX * m_iNumY)
		{
			m_iFrame = m_iNumX * m_iNumY - 1;

			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(126.536f + m_fPlusZ, 0.05f, 123.840f + m_fPlusZ, 1.f));
		}

	}
}

CRoomSmoke* CRoomSmoke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRoomSmoke* pInstance = new CRoomSmoke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRoomSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CRoomSmoke::Clone(void* pArg)
{
	CRoomSmoke* pInstance = new CRoomSmoke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRoomSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoomSmoke::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
