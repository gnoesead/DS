#include "pch.h"
#include "..\Public\Loading.h"

#include "GameInstance.h"
#include "Title_Manager.h"


CLoading::CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CLoading::CLoading(const CLoading& rhs)
	: CUI(rhs)
{
}

HRESULT CLoading::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX("Failed to Initialize_Prototype : BackGround");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading::Initialize(void* pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	

	if (FAILED(Add_Components()))
		return E_FAIL;
	

	// Bg
	if (m_UI_Desc.m_Type == 0) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1280.f;
		m_Origin_Y = 720.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 0;
	}

	// Circle
	if (m_UI_Desc.m_Type == 1) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1000.f;
		m_Origin_Y = 1000.f;
		m_Size_Param = 0.7f;
		m_UI_Layer = 2;
	}

	// Rect(Content)
	if (m_UI_Desc.m_Type == 2) {

		m_fX = 640;
		m_fY = 365;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 860.f;
		m_Origin_Y = 592.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 3;
		m_Alpha = 0.2f;
	}


	// Title
	if (m_UI_Desc.m_Type == 8) {

		m_fX = 640;
		m_fY = 146;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 520.f;
		m_Origin_Y = 68.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 4;
		m_Alpha = 1.f;
	}

	// Cloud_LT
	if (m_UI_Desc.m_Type == 9) {
		m_fX = 90;
		m_fY = 2;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1080.f;
		m_Origin_Y = 328.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 1;
		m_Alpha = 1.f;
	}
	// Cloud_RT
	if (m_UI_Desc.m_Type == 10) {

		m_fX = 1210;
		m_fY = 35;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 848.f;
		m_Origin_Y = 308.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 1;
		m_Alpha = 1.f;
	}
	// Cloud_LD
	if (m_UI_Desc.m_Type == 11) {

		m_fX = 290;
		m_fY = 715;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1080.f;
		m_Origin_Y = 328.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 1;
		m_Alpha = 1.f;
	}
	// Cloud_RD
	if (m_UI_Desc.m_Type == 12) {

		m_fX = 1150;
		m_fY = 625;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1080.f;
		m_Origin_Y = 328.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 1;
		m_Alpha = 1.f;
	}

	m_szTitle.push_back(L"시련돌파");
	m_szContent.push_back(L"로딩중");



	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CLoading::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);


	// Cloud_LT
	if (m_UI_Desc.m_Type == 9) {
		
		m_fX += dTimeDelta * 5.f * m_PosX_Dir;

		if (m_fX - m_Origin_PosX >=  20.f) {
			
			m_PosX_Dir *= -1.f;
		}
		
		if (m_fX <= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}
	}
	// Cloud_RT
	if (m_UI_Desc.m_Type == 10) {

		m_fX -= dTimeDelta * 5.f * m_PosX_Dir;

		if (m_Origin_PosX - m_fX >= 20.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX >= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}


	}
	// Cloud_LD
	if (m_UI_Desc.m_Type == 11) {

		m_fX += dTimeDelta * 5.f * m_PosX_Dir;

		if (m_fX - m_Origin_PosX >= 20.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX <= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}

	
	}
	// Cloud_RD
	if (m_UI_Desc.m_Type == 12) {

		m_fX -= dTimeDelta * 5.f * m_PosX_Dir;

		if (m_Origin_PosX - m_fX >= 20.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX >= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}


	}


	Set_UI();


}

void CLoading::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;

}

HRESULT CLoading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	if (m_UI_Desc.m_Is_X_Reverse == true && m_UI_Desc.m_Is_Y_Reverse != true) {
		m_pShaderCom->Begin(6);
	}
	else if (m_UI_Desc.m_Is_X_Reverse != true && m_UI_Desc.m_Is_Y_Reverse == true) {
		m_pShaderCom->Begin(9);
	}
	else if (m_UI_Desc.m_Is_X_Reverse == true && m_UI_Desc.m_Is_Y_Reverse == true) {
		m_pShaderCom->Begin(5);
	}
	else {
		m_pShaderCom->Begin(1);
	}




	m_pVIBufferCom->Render();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_UI_Desc.m_Type == 8) {

		if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szTitle[m_Loading_Index].c_str(), _float2((_float)m_fX - 43.f, (_float)m_fY - 16.f), _float2(0.5f, 0.5f), XMVectorSet(0.6f, 0.6f, 0.f, 1.f))))
			return E_FAIL;
	}

	if (m_UI_Desc.m_Type == 2) {

		if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szContent[m_Loading_Index].c_str(), _float2((_float)m_fX - 30.f, (_float)m_fY - 16.f), _float2(0.5f, 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);


	return S_OK;
}

void CLoading::Set_UI()
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

HRESULT CLoading::Add_Components()
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
	
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	
	

	return S_OK;
}

HRESULT CLoading::SetUp_ShaderResources()
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

	
	if (m_UI_Desc.m_Type == 9 || m_UI_Desc.m_Type == 11 || m_UI_Desc.m_Type == 12) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 9)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 10) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 10)))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type)))
			return E_FAIL;
	}

	
	
	
	return S_OK;
}

CLoading* CLoading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoading* pInstance = new CLoading(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading::Clone(void* pArg)
{
	CLoading* pInstance = new CLoading(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CLoading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}