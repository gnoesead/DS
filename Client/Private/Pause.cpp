#include "pch.h"
#include "..\Public\Pause.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "DialogManager.h"
#include "MissionManager.h"
#include "Fade_Manager.h"
#include "Level_Loading.h"
#include "OptionManager.h"

CPause::CPause(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPause::CPause(const CPause & rhs)
	: CUI(rhs)
{
}

HRESULT CPause::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPause::Initialize(void * pArg)
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
		m_fY = 360;
		m_Origin_PosX = 640;
		m_Origin_PosY = 220;
		
		m_Origin_X = 280.f;
		m_Origin_Y = 160.f;
		m_Size_Param = 2.f;
		m_UI_Layer = 11;
	}

	// 재개하기
	if (m_UI_Desc.m_Type == 1) {
		m_fX = 650;
		m_fY = 323;
	
		m_Origin_X = 560.f;
		m_Origin_Y = 110.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 12;
	}

	// 로비로 돌아가기
	if (m_UI_Desc.m_Type == 2) {

		m_fX = 650;
		m_fY = 383;

		m_Origin_X = 560.f;
		m_Origin_Y = 110.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 12;
	}

	// 옵션
	if (m_UI_Desc.m_Type == 3) {

		m_fX = 650;
		m_fY = 443;

		m_Origin_X = 560.f;
		m_Origin_Y = 110.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 12;
	}

	// 백그라운드
	if (m_UI_Desc.m_Type == 6) {

		m_fX = 640;
		m_fY = 360;
		
		m_Origin_X = 1280;
		m_Origin_Y = 720;
		m_Size_Param = 1.f;
		m_UI_Layer = 10;
	}

	m_Is_TimeFree = true;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	Set_UI();

	m_szText_0.push_back(L"일시 정지 메뉴");

	m_szText_1.push_back(L"재개하기");
	
	m_szText_2.push_back(L"옵션");
	
	m_szText_3.push_back(L"로비로 돌아가기");
	

	return S_OK;
}

void CPause::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if(m_Is_In == true)
		Fade_In(TimeDelta);

	if (m_Is_Out == true)
		Fade_Out(TimeDelta);

	Set_UI();

}

void CPause::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	Get_Player_Info(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (pGameInstance->Get_DIKeyDown(DIK_8)) {

		pGameInstance->Time_Stop();

		if (m_Alpha == 0.f) {
			m_Is_In = true;
		}

	}


	if (pGameInstance->Get_DIKeyDown(DIK_DOWN)) {

		if (COptionManager::GetInstance()->Get_Is_Option_On() == false) {

			m_Cur_Num++;

			if (m_Cur_Num > 2) {
				m_Cur_Num = 2;
			}
		}
	}

	if (pGameInstance->Get_DIKeyDown(DIK_UP)) {

		if (COptionManager::GetInstance()->Get_Is_Option_On() == false) {

			m_Cur_Num -= 1;

			if (m_Cur_Num <= 0) {
				m_Cur_Num = 0;
			}
		}
	}


	if (m_Cur_Num == 0) {
		if (pGameInstance->Get_DIKeyDown(DIK_RETURN)) {

			if (COptionManager::GetInstance()->Get_Is_Option_On() == false) {

				if (m_Alpha == 1.f) {
					m_Is_Out = true;
					m_Is_Font_Render = false;
					pGameInstance->Time_Stop_Off();
				}

			}
		}
	}


	if (m_UI_Desc.m_Type == 1) {

		if (m_Cur_Num == 0) {
			m_Is_Selected = true;
		}
		else {
			m_Is_Selected = false;
		}
	}

	if (m_UI_Desc.m_Type == 2) {

		if (m_Cur_Num == 1) {
			m_Is_Selected = true;
		}
		else {
			m_Is_Selected = false;
		}

		if (m_Is_Selected) {
			if (pGameInstance->Get_DIKeyDown(DIK_RETURN)) {

				if (COptionManager::GetInstance()->Get_Is_Option_On() == false) {
					COptionManager::GetInstance()->Set_Is_Option_On(true);
					COptionManager::GetInstance()->Set_Is_Reset(false);
				}

			}
		}
	}

	if (m_UI_Desc.m_Type == 3) {

		if (m_Cur_Num == 2) {
			m_Is_Selected = true;
		}
		else {
			m_Is_Selected = false;
		}

		if (m_Is_Selected) {

			if (pGameInstance->Get_DIKeyDown(DIK_RETURN))
			{
				if (COptionManager::GetInstance()->Get_Is_Option_On() == false) {
					CFadeManager::GetInstance()->Set_Fade_Out(true);
					pGameInstance->Time_Stop_Off();
				}
			}

		}
	}

	Safe_Release(pGameInstance);

	if (m_Alpha == 0.f) {
		m_Is_Render = false;
	}
	else {
		m_Is_Render = true;
	}


	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;
}

HRESULT CPause::Render()
{

	if (m_Is_Render == true) {

		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		if (m_Is_Reverse == false)
			m_pShaderCom->Begin(1);
		else {
			m_pShaderCom->Begin(2);
		}

		if (m_Is_CutScene == false) {

			m_pVIBufferCom->Render();

		}
	}

	// 폰트 랜더
	if (m_Is_Font_Render) {

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (m_UI_Desc.m_Type == 0) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szText_0[0].c_str(), _float2((_float)m_fX - 75.f 
				, (_float)m_fY - 130.f), _float2(0.6f, 0.6f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
				return E_FAIL;
		}
		else if (m_UI_Desc.m_Type == 1) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szText_1[0].c_str(), _float2((_float)m_fX - 57.f 
				, (_float)m_fY - 15.f), _float2(0.54f, 0.54f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
				return E_FAIL;
		}
		else if (m_UI_Desc.m_Type == 2) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szText_2[0].c_str(), _float2((_float)m_fX - 38.f 
				, (_float)m_fY - 15.f), _float2(0.54f, 0.54f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
				return E_FAIL;
		}
		else if (m_UI_Desc.m_Type == 3) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szText_3[0].c_str(), _float2((_float)m_fX - 84.f 
				, (_float)m_fY - 15.f), _float2(0.54f, 0.54f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
				return E_FAIL;
		}

		Safe_Release(pGameInstance);

	}


	return S_OK;
}

HRESULT CPause::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pause"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPause::SetUp_ShaderResources()
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
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type >= 1 && m_UI_Desc.m_Type <= 3) {
		if (m_Is_Selected) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 2)))
				return E_FAIL;
		}
		else {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 1)))
				return E_FAIL;
		}
		
	}
	else if (m_UI_Desc.m_Type == 6 ) {
		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 3)))
			return E_FAIL;
	}


	return S_OK;
}

void CPause::Set_UI()
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

void CPause::Get_Player_Info(_double TimeDelta)
{

	
}

void CPause::Fade_In(_double TimeDelta)
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

void CPause::Fade_Out(_double TimeDelta)
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



CPause * CPause::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPause*		pInstance = new CPause(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPause");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject * CPause::Clone(void * pArg)
{
	CPause*		pInstance = new CPause(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPause");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPause::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}
