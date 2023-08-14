#include "pch.h"
#include "..\Public\Fade.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "Fade_Manager.h"


CFade::CFade(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CFade::CFade(const CFade & rhs)
	: CUI(rhs)
{
}

HRESULT CFade::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFade::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fX = 640;
	m_fY = 360;
	m_Origin_X = 1280.f;
	m_Origin_Y = 720.f;
	m_Size_Param = 1.f;
	m_UI_Layer = 99;

	// Fade_Out
	if (m_UI_Desc.m_Type == 0) {
		m_Alpha = 0.f;
	}
	// Fade_In
	else if (m_UI_Desc.m_Type == 1) {
		m_Alpha = 1.f;
	}
	// Out/In
	else if (m_UI_Desc.m_Type == 2) {
		m_Alpha = 0.f;
	}
	


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	Set_UI();


	

	return S_OK;
}

void CFade::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	

	// Fade_Out
	if (m_UI_Desc.m_Type == 0) {
		if (m_Is_Out == true)
			Fade_Out(TimeDelta);
	}
	// Fade_In
	else if (m_UI_Desc.m_Type == 1) {
		if (m_Is_In == true)
			Fade_In(TimeDelta);
	}
	// Out/In
	else if (m_UI_Desc.m_Type == 2) {
		if (m_Is_OutIn == true)
			Fade_OutIn(TimeDelta);
	}



	Set_UI();

}

void CFade::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Get_Info(TimeDelta);

	if (m_Alpha == 0.f) {
		m_Is_Render = false;
	}
	else {
		m_Is_Render = true;
	}

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CFade::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_Is_Reverse == false)
		m_pShaderCom->Begin(1);
	else {
		m_pShaderCom->Begin(2);
	}

	
	if (m_Is_CutScene == false && m_Is_Render == true) {

		m_pVIBufferCom->Render();

	}

	

	return S_OK;
}

HRESULT CFade::Add_Components()
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

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Black"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CFade::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;



	return S_OK;
}

void CFade::Set_UI()
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

void CFade::Get_Info(_double TimeDelta)
{
	// Fade_Out
	if (m_UI_Desc.m_Type == 0) {
		m_Is_Out = CFadeManager::GetInstance()->Get_Fade_Out();
		m_Is_Out_Done = CFadeManager::GetInstance()->Get_Fade_Out_Done();
	}
	// Fade_In
	else if (m_UI_Desc.m_Type == 1) {
		m_Is_In = CFadeManager::GetInstance()->Get_Fade_In();
		m_Is_In_Done = CFadeManager::GetInstance()->Get_Fade_In_Done();
	}
	// Out/In
	else if (m_UI_Desc.m_Type == 2) {
		m_Is_OutIn = CFadeManager::GetInstance()->Get_Fade_OutIn();
		m_Is_OutIn_Done = CFadeManager::GetInstance()->Get_Fade_OutIn_Done();
	}
	
}

void CFade::Fade_In(_double TimeDelta)
{
	
	m_Alpha -= (_float)TimeDelta * 1.5f;

	if (m_Alpha <= 0.f)
	{
		m_Alpha = 0.f;
		CFadeManager::GetInstance()->Set_Fade_In(false);
		CFadeManager::GetInstance()->Set_Fade_In_Done(true);
	}

	
}

void CFade::Fade_Out(_double TimeDelta)
{
	m_Alpha += (_float)TimeDelta * 1.5f;

	if (m_Alpha >= 1.f)
	{
		m_Alpha = 1.f;
		CFadeManager::GetInstance()->Set_Fade_Out(false);
		CFadeManager::GetInstance()->Set_Fade_Out_Done(true);
	}

}

void CFade::Fade_OutIn(_double TimeDelta)
{
	m_Alpha += (_float)TimeDelta * 1.5f * m_Alpha_Dir * m_InOut_Speed;

	if (m_Alpha >= 1.f)
	{
		m_Alpha = 1.f;
		m_Alpha_Dir *= -1.f;
		m_InOut_Speed = 1.f;
		m_Delay_On = true;
		_bool Is_Battle = CFadeManager::GetInstance()->Get_Is_Battle();
		CFadeManager::GetInstance()->Set_Is_Battle(!Is_Battle);
	}


	if (m_Alpha <= 0.f)
	{
		m_Alpha = 0.f;
		m_Alpha_Dir *= -1.f;
		m_InOut_Speed = 1.f;
		CFadeManager::GetInstance()->Set_Fade_OutIn(false);
		CFadeManager::GetInstance()->Set_Fade_OutIn_Done(true);
	}


	if (m_Delay_On) {
		m_Delay_TimeAcc += (_float)TimeDelta;

		m_Alpha = 0.999f;

		if (m_Delay_TimeAcc > CFadeManager::GetInstance()->Get_Delay_Time()) {
			m_Delay_TimeAcc = 0.f;
			m_Delay_On = false;
		}
	}



}





CFade * CFade::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFade*		pInstance = new CFade(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFade");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CFade::Clone(void * pArg)
{
	CFade*		pInstance = new CFade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFade::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
