#include "pch.h"
#include "..\Public\Mission.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "MissionManager.h"


CMission::CMission(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMission::CMission(const CMission & rhs)
	: CUI(rhs)
{
}

HRESULT CMission::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMission::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_UI_Desc = *(UIDESC*)pArg;

	m_Is_Reverse = m_UI_Desc.m_Is_Reverse;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// Main
	if (m_UI_Desc.m_Type == 0) {

		m_fX = 50;
		m_fY = 70;
		m_Origin_PosX = 70;
		m_Origin_PosY = 70;
		m_Start_PosX = 50;
		m_Start_PosY = 70;

		m_Origin_X = 200.f;
		m_Origin_Y = 76.f;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 1;
	}

	// Sub
	if (m_UI_Desc.m_Type == 1) {

		m_fX = 210;
		m_fY = 160;
		m_Origin_PosX = 220;
		m_Origin_PosY = 160;
		m_Start_PosX = 210;
		m_Start_PosY = 160;

		m_Origin_X = 356 * 1.9f;
		m_Origin_Y = 64;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 1;
	}

	// Main_Icon
	if (m_UI_Desc.m_Type == 2) {

		m_fX = 30;
		m_fY = 67;
		m_Origin_PosX = 50;
		m_Origin_PosY = 67;
		m_Start_PosX = 30;
		m_Start_PosY = 67;

		m_Origin_X = 50;
		m_Origin_Y = 50;
		m_Size_Param = 0.67f;
		m_UI_Layer = 2;
	}
	
	// Sub_Icon
	if (m_UI_Desc.m_Type == 3) {

		m_fX = 50;
		m_fY = 160;
		m_Origin_PosX = 80;
		m_Origin_PosY = 160;
		m_Start_PosX = 50;
		m_Start_PosY = 160;

		m_Origin_X = 64;
		m_Origin_Y = 64;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 2;
	}

	// Sub_Check
	if (m_UI_Desc.m_Type == 4) {

		m_fX = 363;
		m_fY = 157;
		m_Origin_PosX = 370;
		m_Origin_PosY = 157;
		m_Start_PosX = 363;
		m_Start_PosY = 157;

		m_Origin_X = 64;
		m_Origin_Y = 64;
		m_Size_Param = 0.666678f;
		m_UI_Layer = 2;
	}

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	Set_UI();

	m_szMain.push_back(L"메인임무1");
	m_szMain.push_back(L"메인임무2");
	m_szMain.push_back(L"메인임무3");

	m_szMain_Sub.push_back(L"달성상황1");
	m_szMain_Sub.push_back(L"달성상황2");
	m_szMain_Sub.push_back(L"달성상황3");

	m_szSub.push_back(L"10시가 되면 집에 가세요");
	m_szSub.push_back(L"12시가 되면 집에 가세요");
	

	return S_OK;
}

void CMission::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if(m_Is_In == true)
		Fade_In(TimeDelta);

	if (m_Is_Out == true)
		Fade_Out(TimeDelta);

	Set_UI();

}

void CMission::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Get_Player_Info(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*if (pGameInstance->Get_DIKeyDown(DIK_F) && m_Is_In == false && m_Is_Out == false) {
		m_Is_In = true;
		m_Is_Font_Render = true;
	}
	
	if (pGameInstance->Get_DIKeyDown(DIK_G) && m_Is_In == false && m_Is_Out == false) {
		m_Is_Out = true;
		m_Is_Font_Render = false;
	}*/


	m_Main_Type = CMissionManager::GetInstance()->Get_Main_Mission_Type();
	m_Sub_Type = CMissionManager::GetInstance()->Get_Sub_Mission_Type();

	m_Is_Dialog_On = CMissionManager::GetInstance()->Get_Is_Dialog_On();

	Safe_Release(pGameInstance);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CMission::Render()
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


		if (m_UI_Desc.m_Type == 1) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_KR"), m_szSub[m_Sub_Type].c_str(), _float2((_float)m_fX - 150.f , (_float)m_fY - 9.f), _float2(0.5f, 0.5f))))
				return E_FAIL;
		}
		else if (m_UI_Desc.m_Type == 0) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_KR"), m_szMain[m_Main_Type].c_str(), _float2((_float)m_fX  - 10.f, (_float)m_fY - 9.f), _float2(0.5f, 0.5f))))
				return E_FAIL;

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_KR"), m_szMain_Sub[m_Main_Type].c_str(), _float2((_float)m_fX  - 10.f, (_float)m_fY + 20.f), _float2(0.5f, 0.5f))))
				return E_FAIL;
		}


		Safe_Release(pGameInstance);

	}


	return S_OK;
}

HRESULT CMission::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mission"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CMission::SetUp_ShaderResources()
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

void CMission::Set_UI()
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

void CMission::Get_Player_Info(_double TimeDelta)
{

	
}

void CMission::Fade_In(_double TimeDelta)
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

void CMission::Fade_Out(_double TimeDelta)
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



CMission * CMission::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMission*		pInstance = new CMission(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMission");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CMission::Clone(void * pArg)
{
	CMission*		pInstance = new CMission(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMission");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMission::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
