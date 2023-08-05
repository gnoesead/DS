#include "pch.h"
#include "..\Public\Title.h"

#include "GameInstance.h"
#include "Title_Manager.h"


CTitle::CTitle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CTitle::CTitle(const CTitle& rhs)
	: CUI(rhs)
{
}

HRESULT CTitle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX("Failed to Initialize_Prototype : BackGround");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTitle::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	

	if (FAILED(Add_Components()))
		return E_FAIL;
	

	// Bg
	if (m_UI_Desc.m_Type == 0) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_X = 1280.f;
		m_Origin_Y = 720.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 0;
	}

	// 캐릭터
	if (m_UI_Desc.m_Type == 1) {

		m_fX = 410;
		m_fY = 360;
		m_Origin_X = 1232.f;
		m_Origin_Y = 1085.f;
		m_Size_Param = 0.665f;
		m_UI_Layer = 3;
	}

	// 로고
	if (m_UI_Desc.m_Type == 2) {

		m_fX = 952;
		m_fY = 240;
		m_Origin_X = 860.f;
		m_Origin_Y = 860.f;
		m_Size_Param = 0.48f;
		m_UI_Layer = 4;
	}

	// Select_Option
	if (m_UI_Desc.m_Type == 3) {

		m_fX = 960;
		m_fY = 555;
		m_Origin_X = 784.f;
		m_Origin_Y = 104.f;
		m_Size_Param = 0.64f;
		m_UI_Layer = 5;
	}

	// Select_GameStart
	if (m_UI_Desc.m_Type == 4) {

		m_fX = 960;
		m_fY = 497;
		m_Origin_X = 784.f;
		m_Origin_Y = 104.f;
		m_Size_Param = 0.64f;
		m_UI_Layer = 5;
	}

	// Eff_Fog
	if (m_UI_Desc.m_Type == 5) {

		m_fX = 640;
		m_fY = 600;
		m_Origin_X = 1000.f;
		m_Origin_Y = 388.f;
		m_Size_Param = 1.8f;
		m_UI_Layer = 1;
		m_UV_Speed_X = - 0.2f;
	}

	// Eff_Line
	if (m_UI_Desc.m_Type == 6) {
		m_fX = 1450;
		m_fY = 760;
		m_Origin_X = 1400.f;
		m_Origin_Y = 440.f;
		m_Size_Param = 1.2f;
		m_UI_Layer = 0.5;
		m_Alpha = 0.45f;
	}

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CTitle::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);


	Set_UI();

	if (m_UI_Desc.m_Type == 5) {
		m_Time_X += (_float)dTimeDelta * 0.1f;


		m_Alpha += (_float)dTimeDelta * 0.2f * m_Alpha_Dir;

		if (m_Alpha > 0.9f) {
			m_Alpha = 0.9f;
			m_Alpha_Dir *= -1.f;
		}

		if (m_Alpha < 0.4f) {
			m_Alpha = 0.4f;
			m_Alpha_Dir *= -1.f;
		}

	}

	


}

void CTitle::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;


	m_Select = CTitleManager::GetInstance()->Get_Select_Type();

	
	
}

HRESULT CTitle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_UI_Desc.m_Type == 1) {
		m_pShaderCom->Begin(7);
	}
	else if (m_UI_Desc.m_Type == 5) {
		m_pShaderCom->Begin(8);
	}
	else {
		m_pShaderCom->Begin(1);
	}
	

	m_pVIBufferCom->Render();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_UI_Desc.m_Type == 4) {
		if (m_Select == 0) {
			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGameStart, _float2((_float)m_fX - 49.f, (_float)m_fY - 16.f), _float2(0.6f, 0.6f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
				return E_FAIL;
		}
		else {
			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGameStart, _float2((_float)m_fX - 49.f, (_float)m_fY - 16.f), _float2(0.6f, 0.6f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
				return E_FAIL;
		}
	}
	else if (m_UI_Desc.m_Type == 3) {
		if (m_Select == 0) {
			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szOption, _float2((_float)m_fX - 23.f, (_float)m_fY - 16.f), _float2(0.6f, 0.6f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
				return E_FAIL;
		}
		else {
			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szOption, _float2((_float)m_fX - 23.f, (_float)m_fY - 16.f), _float2(0.6f, 0.6f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
				return E_FAIL;
		}
	}

	
	Safe_Release(pGameInstance);


	return S_OK;
}

void CTitle::Set_UI()
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

HRESULT CTitle::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	if (m_UI_Desc.m_Type >= 0 && m_UI_Desc.m_Type <= 2) {
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Title"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type >= 3 && m_UI_Desc.m_Type <= 4) {
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Title_Select"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type >= 5 && m_UI_Desc.m_Type <= 6) {
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Title_Eff"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	

	return S_OK;
}

HRESULT CTitle::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_Alpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Time_X", &m_Time_X, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_UV_Speed_X", &m_UV_Speed_X, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Time_Y", &m_Time_Y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_UV_Speed_Y", &m_UV_Speed_Y, sizeof(_float))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (m_UI_Desc.m_Type >= 0 && m_UI_Desc.m_Type <= 2) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 3) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_Select)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 4) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", !m_Select)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type >= 5 && m_UI_Desc.m_Type <= 6) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type - 5)))
			return E_FAIL;
	}
	
	return S_OK;
}

CTitle* CTitle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTitle* pInstance = new CTitle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTitle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTitle::Clone(void* pArg)
{
	CTitle* pInstance = new CTitle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CTitle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTitle::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}