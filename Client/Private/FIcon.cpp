#include "pch.h"
#include "..\Public\FIcon.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"


CFIcon::CFIcon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CFIcon::CFIcon(const CFIcon & rhs)
	: CUI(rhs)
{
}

HRESULT CFIcon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFIcon::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// Main 0
	if (m_UI_Desc.m_Type == 0) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.5f;
		m_Origin_Y = 0.5f;
		m_Size_Param = 1.f;
		m_UI_Layer = 5;
		m_fZ = 0.12f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	// Main 1
	if (m_UI_Desc.m_Type == 1) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.5f;
		m_Origin_Y = 0.5f;
		m_Size_Param = 0.608333f;
		m_UI_Layer = 4;
		m_fZ = 0.11f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	// Main 2
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

	// Mini 0 
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
		
	// Mini 1
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

	// Talk 0
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

	// Talk 1
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

	// Lock_On_Target
	if (m_UI_Desc.m_Type == 7) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.45f;
		m_Origin_Y = 0.45f;
		m_Size_Param = 0.4f;
		m_UI_Layer = 2;
		m_fZ = 0.09f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	// Lock_On_Target_Glow
	if (m_UI_Desc.m_Type == 8) {
		m_fX = 919;
		m_fY = 65;
		m_Origin_X = 0.6f;
		m_Origin_Y = 0.6f;
		m_Size_Param = 1.f;
		m_UI_Layer = 1;
		m_fZ = 0.07f;

		m_pTransformCom->Scaling({ m_Origin_X, m_Origin_Y , 1.f });
	}

	return S_OK;
}

void CFIcon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	if (m_UI_Desc.m_Type == 8) {

		m_pTransformCom->Scaling({ m_Origin_X * m_Size_Param, m_Origin_Y * m_Size_Param, 1.f });

	}
	
}

void CFIcon::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);


	if (m_UI_Desc.m_Type == 8) {

		m_Size_Param += TimeDelta * m_Size_Param_Dir;

		if (m_Size_Param > 1.5f) {
			m_Size_Param = 1.5f;
			m_Size_Param_Dir *= -1.f;
		}

		if (m_Size_Param < 1.f) {
			m_Size_Param = 1.f;
			m_Size_Param_Dir *= -1.f;
		}
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	// Camera
	m_vTargetPos = Convert::ToVector(pGameInstance->Get_CameraPosition());
	
	m_vTargetPos = { XMVectorGetX(m_vTargetPos), XMVectorGetY(m_vTargetPos) ,XMVectorGetZ(m_vTargetPos), XMVectorGetW(m_vTargetPos) };

	m_pTransformCom->LookAt(m_vTargetPos);


	// Monster
	if (pGameInstance->Get_CurLevelIdx() == LEVEL_FINALBOSS) {

		CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera"), 0));

		m_vBattle_Targt = pCamera->Get_Battle_Target_Pos();

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vBattle_Targt);


		if (m_UI_Desc.m_Type == 7 || m_UI_Desc.m_Type == 8) {
			m_Is_Render = pCamera->Get_Is_Battle();
		}

	}


	Safe_Release(pGameInstance);


	Get_Boss_Info(TimeDelta);

	
	Set_Personal_Pos();
	

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_WORLD_UI, this)))
		return;
}

HRESULT CFIcon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_Is_Reverse == false) {
		m_pShaderCom->Begin(1);
	}
	else {
		m_pShaderCom->Begin(2);
	}

	
	if (m_Is_CutScene == false && m_Is_Render == true) {
		m_pVIBufferCom->Render();
	}
	
	
	
	return S_OK;
}

HRESULT CFIcon::Add_Components()
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


	if (m_UI_Desc.m_Type <= 2) {
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FIcon_Main"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 3 || m_UI_Desc.m_Type == 4) {
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FIcon_Mini"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 5 || m_UI_Desc.m_Type == 6) {
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FIcon_Talk"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 7 || m_UI_Desc.m_Type == 8) {
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FIcon_LockOn"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CFIcon::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;	
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Is_Side_Cut_R", &m_Is_Side_Cut_R, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Is_Side_Cut_L", &m_Is_Side_Cut_L, sizeof(_bool))))
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


	if (m_UI_Desc.m_Type <= 2) {
		
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 3 || m_UI_Desc.m_Type == 4) {
		
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type - 3)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 5 || m_UI_Desc.m_Type == 6) {
		
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type - 5)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 7 || m_UI_Desc.m_Type == 8) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type - 7)))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CFIcon::Get_Boss_Info(_double TimeDelta)
{
	
}

void CFIcon::Set_Personal_Pos()
{

	_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	Pos += {0.f,m_UI_Desc.m_Up_Mount, 0.f};

	Pos += vLook * m_fZ;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
}

CFIcon * CFIcon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFIcon*		pInstance = new CFIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CFIcon::Clone(void * pArg)
{
	CFIcon*		pInstance = new CFIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFIcon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);

}
