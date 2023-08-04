#include "pch.h"
#include "..\Public\Dialog.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"


CDialog::CDialog(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CDialog::CDialog(const CDialog & rhs)
	: CUI(rhs)
{
}

HRESULT CDialog::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDialog::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// Frame
	if (m_UI_Desc.m_Type == 0) {

		m_fX = 640;
		m_fY = 660;
		m_Origin_PosX = 640;
		m_Origin_PosY = 620;
		m_Start_PosX = 640;
		m_Start_PosY = 660;

		m_Origin_X = 1308.f;
		m_Origin_Y = 228.f;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 1;
	}

	// Name_Frame
	if (m_UI_Desc.m_Type == 1) {

		m_fX = 398;
		m_fY = 565;
		m_Origin_PosX = 398;
		m_Origin_PosY = 525;
		m_Start_PosX = 398;
		m_Start_PosY = 565;

		m_Origin_X = 470;
		m_Origin_Y = 69;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 1;
	}

	// Arrow
	if (m_UI_Desc.m_Type == 2) {

		m_fX = 640;
		m_fY = 740;
		m_Origin_PosX = 640;
		m_Origin_PosY = 700;
		m_Start_PosX = 640;
		m_Start_PosY = 740;

		m_Origin_X = 32;
		m_Origin_Y = 32;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 2;
	}


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	Set_UI();


	m_szText_1[0] = L"대화창 생성 : F , 대화창 소멸 : G";

	m_szName[TANJIRO] = L"카마도 탄지로";
	m_szName[MOTHER] = L"어머니";
	m_szName[GIRL] = L"소녀";




	return S_OK;
}

void CDialog::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if(m_Is_In == true)
		Fade_In(TimeDelta);

	if (m_Is_Out == true)
		Fade_Out(TimeDelta);

	Set_UI();

}

void CDialog::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Get_Player_Info(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_F) && m_Is_In == false && m_Is_Out == false) {
		m_Is_In = true;
		m_Is_Font_Render = true;
	}
	
	if (pGameInstance->Get_DIKeyDown(DIK_G) && m_Is_In == false && m_Is_Out == false) {
		m_Is_Out = true;
		m_Is_Font_Render = false;
	}
		

	Safe_Release(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CDialog::Render()
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

	// 폰트 랜더
	if (m_Is_Font_Render) {

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);


		if (m_UI_Desc.m_Type == 0) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_KR"), m_szText_1[0].c_str(), _float2((_float)m_fX - (_float)m_szText_1[0].size() * 5.4f , (_float)m_fY - 9.f), _float2(0.5f, 0.5f))))
				return E_FAIL;
		}
		else if (m_UI_Desc.m_Type == 1) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_KR"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - (_float)m_szText_1[0].size() * 4.f, (_float)m_fY - 7.f), _float2(0.5f, 0.5f))))
				return E_FAIL;
		}


		Safe_Release(pGameInstance);

	}


	return S_OK;
}

HRESULT CDialog::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dialog"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CDialog::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Alpha", &m_Alpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type)))
		return E_FAIL;



	return S_OK;
}

void CDialog::Set_UI()
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

void CDialog::Get_Player_Info(_double TimeDelta)
{

	
}

void CDialog::Fade_In(_double TimeDelta)
{
	m_Alpha += (_float)TimeDelta * 2.f;

	if (m_Alpha >= 1.f)
	{
		m_Alpha = 1.f;
	}

	m_fY -= TimeDelta * 120.0;

	if (m_fY <= m_Origin_PosY)
	{
		m_fY = m_Origin_PosY;
		
	}

	if (m_Alpha >= 1.f && m_fY <= m_Origin_PosY) {
		m_Is_In = false;
	}


}

void CDialog::Fade_Out(_double TimeDelta)
{

	m_Alpha -= (_float)TimeDelta * 2.5f;

	if (m_Alpha <= 0.f)
	{
		m_Alpha = 0.f;
	}

	m_fY += TimeDelta * 100.0;

	if (m_fY >= m_Start_PosY)
	{
		m_fY = m_Start_PosY;
		
	}

	if (m_Alpha <= 0.f && m_fY >= m_Start_PosY) {
		m_Is_Out = false;
	}

}



CDialog * CDialog::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDialog*		pInstance = new CDialog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDialog");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CDialog::Clone(void * pArg)
{
	CDialog*		pInstance = new CDialog(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDialog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDialog::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
