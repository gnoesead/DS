#include "pch.h"
#include "..\Public\Interaction.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"


CInteraction::CInteraction(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CInteraction::CInteraction(const CInteraction & rhs)
	: CUI(rhs)
{
}

HRESULT CInteraction::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteraction::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	// 배경
	if (m_UI_Desc.m_Type == 0) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.224f * 4.f;
		m_Origin_Y = 0.052f * 4.f;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 4;
		m_fZ = 0.f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	// 상호작용
	if (m_UI_Desc.m_Type == 1) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.5f;
		m_Origin_Y = 0.25f;
		m_Size_Param = 1.f;
		m_UI_Layer = 5;
		m_fZ = 0.01f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	// 
	if (m_UI_Desc.m_Type == 2) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.5f;
		m_Origin_Y = 0.5f;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 3;
		m_fZ = 0.1f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	// 
	if (m_UI_Desc.m_Type == 3) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.5f;
		m_Origin_Y = 0.5f;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 2;
		m_fZ = 0.09f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}
		
	// 
	if (m_UI_Desc.m_Type == 4) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.5f;
		m_Origin_Y = 0.5f;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 2;
		m_fZ = 0.09f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	// 
	if (m_UI_Desc.m_Type == 5) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.5f;
		m_Origin_Y = 0.5f;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 2;
		m_fZ = 0.09f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	// 
	if (m_UI_Desc.m_Type == 6) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.5f;
		m_Origin_Y = 0.5f;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 2;
		m_fZ = 0.09f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	// 
	if (m_UI_Desc.m_Type == 7) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.5f;
		m_Origin_Y = 0.5f;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 2;
		m_fZ = 0.09f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	return S_OK;
}

void CInteraction::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_float dist = Convert::GetLength(m_vPlayerPos - m_vBattle_Targt);

	if (dist < 3.f) {

		m_Alpha += (_float)TimeDelta * 1.5f;

		if (m_Alpha > 1.f)
			m_Alpha = 1.f;
	}
	else {
		m_Alpha -= (_float)TimeDelta * 1.5f;

		if (m_Alpha < 0.f)
			m_Alpha = 0.f;
	}
	


}

void CInteraction::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// Camera
	m_vTargetPos = Convert::ToVector(pGameInstance->Get_CameraPosition());
	
	m_vTargetPos = { XMVectorGetX(m_vTargetPos), XMVectorGetY(m_vTargetPos) ,XMVectorGetZ(m_vTargetPos), XMVectorGetW(m_vTargetPos) };

	m_pTransformCom->LookAt(m_vTargetPos);


	// Trigger
	if (pGameInstance->Get_CurLevelIdx() == 7) {

		CTransform* m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));

		m_vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

		m_vBattle_Targt = { 130.f, 0.f, 140.f, 1.f };

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vBattle_Targt);
	}


	Safe_Release(pGameInstance);


	Get_Boss_Info(TimeDelta);

	
	Set_Personal_Pos();
	

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_WORLD_UI, this)))
		return;
}

HRESULT CInteraction::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_UI_Desc.m_Type == 0)
		m_pShaderCom->Begin(1);
	else if(m_UI_Desc.m_Type == 1)
		m_pShaderCom->Begin(6);
	
	if (m_Is_CutScene == false) {
		m_pVIBufferCom->Render();
	}
	
	
	
	return S_OK;
}

HRESULT CInteraction::Add_Components()
{

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof TransformDesc);

	TransformDesc.dSpeedPerSec = 10.f;
	TransformDesc.dRadianRotationPerSec = (_double)XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Interaction"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CInteraction::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;	

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_Alpha, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type)))
		return E_FAIL;
	

	Safe_Release(pGameInstance);

	return S_OK;
}

void CInteraction::Get_Boss_Info(_double TimeDelta)
{
	
}

void CInteraction::Set_Personal_Pos()
{

	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	Pos += {0.f,m_UI_Desc.m_Up_Mount, 0.f};

	Pos += vLook * m_fZ;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
}

CInteraction * CInteraction::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInteraction*		pInstance = new CInteraction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteraction");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CInteraction::Clone(void * pArg)
{
	CInteraction*		pInstance = new CInteraction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteraction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);

}
