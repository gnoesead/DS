#include "pch.h"
#include "..\Public\Web_UI.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "DialogManager.h"
#include "MissionManager.h"
#include "Fade_Manager.h"
#include "Level_Loading.h"
#include "OptionManager.h"
#include "Battle_UI_Manager.h"


Web_UI::Web_UI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

Web_UI::Web_UI(const Web_UI & rhs)
	: CUI(rhs)
{
}

HRESULT Web_UI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Web_UI::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	
	

	m_Is_TimeFree = true;
	
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


	m_fX = 640;
	m_fY = 360;
	m_Origin_X = 1280.f;
	m_Origin_Y = 720.f;
	m_Size_Param = 0.2f;
	m_UI_Layer = 11;

	m_Is_Render = false;


	Set_UI();


	
	
	return S_OK;
}

void Web_UI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	

	if (CBattle_UI_Manager::GetInstance()->Get_Web_UI_On() == true) {
		m_Alpha += (_float)TimeDelta * 2.f;
		m_Size_Param += TimeDelta * 6.f;

		if (m_Size_Param > 1.4f) {
			m_Size_Param = 1.4f;
		}

		if (m_Alpha > 1.f) {
			m_Alpha = 1.f;
			m_Timer_On = true;
		}

		if (m_Timer_On) {
			m_TimeAcc += (_float)TimeDelta * 1.f;

			if (m_TimeAcc > 0.8f) {
				m_TimeAcc = 0.f;
				m_Timer_On = false;
				CBattle_UI_Manager::GetInstance()->Set_Web_UI_Off();
			}

		}
	}
	else {
		m_Alpha -= (_float)TimeDelta * 0.7f;

		if (m_Alpha < 0.f) {
			m_Alpha = 0.f;
			m_Size_Param = 0.2f;
		}

	}


	

	Set_UI();

}

void Web_UI::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	
	m_Cur_Num = CBattle_UI_Manager::GetInstance()->Get_Web_UI_Num();

	m_Web_On = CBattle_UI_Manager::GetInstance()->Get_Web_UI_On();


	if (m_Alpha == 0) {
		m_Is_Render = false;
	}
	else {
		m_Is_Render = true;
	}

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT Web_UI::Render()
{

	if (m_Is_Render == true) {

		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		if (m_UI_Desc.m_Type == 0) {
			if (m_Is_Mask == true) {
				if (m_Is_Black_Mask)
					m_pShaderCom->Begin(27);
				else
					m_pShaderCom->Begin(14);
			}
			else {
				m_pShaderCom->Begin(1);
			}
		}
		
		
		
		m_pVIBufferCom->Render();

	}

	

	return S_OK;
}

HRESULT Web_UI::Add_Components()
{
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

	if (m_UI_Desc.m_Type == 0) {
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Web_UI"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}

	


	return S_OK;
}

HRESULT Web_UI::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Is_Side_Cut_R", &m_Is_Side_Cut_R, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Is_Side_Cut_L", &m_Is_Side_Cut_L, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_Alpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	if (m_UI_Desc.m_Type == 0) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", (_uint)m_Cur_Num)))
			return E_FAIL;
	}



	return S_OK;
}

void Web_UI::Set_UI()
{
	m_fSizeX = m_Origin_X * (_double)m_Size_Param;
	m_fSizeY = m_Origin_Y * (_double)m_Size_Param;

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		Viewport;
	m_pContext->RSGetViewports(&iNumViewports, &Viewport);

	_matrix		TransformMatrix = XMMatrixIdentity();

	TransformMatrix.r[0] = TransformMatrix.r[0] * (_float)m_fSizeX;
	TransformMatrix.r[1] = TransformMatrix.r[1] * (_float)m_fSizeY;

	TransformMatrix.r[3] = XMVectorSetX(TransformMatrix.r[3], (_float)m_fX - Viewport.Width * 0.5f);
	TransformMatrix.r[3] = XMVectorSetY(TransformMatrix.r[3], -(_float)m_fY + Viewport.Height * 0.5f);

	XMStoreFloat4x4(&m_WorldMatrix, TransformMatrix);

}

void Web_UI::Get_Player_Info(_double TimeDelta)
{

	
}

void Web_UI::Fade_In(_double TimeDelta)
{
	m_Alpha += (_float)TimeDelta * 2.7f;

	if (m_Alpha >= 1.f)
	{
		m_Alpha = 1.f;
	}

	if (m_Alpha >= 0.5f) {
		m_Is_Font_Render = true;
	}
	
	if (m_Alpha >= 1.f) {
		m_Is_In = false;
		
	}

}

void Web_UI::Fade_Out(_double TimeDelta)
{

	m_Alpha -= (_float)TimeDelta * 3.2f;

	if (m_Alpha <= 0.f)
	{
		m_Alpha = 0.f;
	}


	if (m_Alpha <= 0.f) {
		m_Is_Out = false;

	}

}



Web_UI * Web_UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	Web_UI*		pInstance = new Web_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Web_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * Web_UI::Clone(void * pArg)
{
	Web_UI*		pInstance = new Web_UI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Web_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Web_UI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
