#include "pch.h"
#include "..\Public\Story_Board.h"

#include "GameInstance.h"
#include "Story_Manager.h"



CStory_Board::CStory_Board(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CStory_Board::CStory_Board(const CStory_Board& rhs)
	: CUI(rhs)
{
}

HRESULT CStory_Board::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX("Failed to Initialize_Prototype : BackGround");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStory_Board::Initialize(void* pArg)
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

	// Bg_Deco_1
	if (m_UI_Desc.m_Type == 1) {

		m_fX = 225;
		m_fY = 80;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1088.f;
		m_Origin_Y = 180.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 1;
	}

	// Bg_Deco_2
	if (m_UI_Desc.m_Type == 2) {

		m_fX = 1000;
		m_fY = 650;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1012.f;
		m_Origin_Y = 164.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 1;
	}

	// Icon_0
	if (m_UI_Desc.m_Type == 3) {

		m_fX = 110;
		m_fY = 535;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 96.f;
		m_Origin_Y = 96.f;
		m_Size_Param = 0.9f;
		m_UI_Layer = 6;
	}
	// Icon_1
	if (m_UI_Desc.m_Type == 4) {

		m_fX = 510;
		m_fY = 605;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 96.f;
		m_Origin_Y = 96.f;
		m_Size_Param = 0.8f;
		m_UI_Layer = 6;
	}
	// Icon_2
	if (m_UI_Desc.m_Type == 5) {

		m_fX = 860;
		m_fY = 550;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 96.f;
		m_Origin_Y = 96.f;
		m_Size_Param = 0.9f;
		m_UI_Layer = 6;
	}
	// Icon_3
	if (m_UI_Desc.m_Type == 6) {

		m_fX = 1050;
		m_fY = 580;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 96.f;
		m_Origin_Y = 96.f;
		m_Size_Param = 0.7f;
		m_UI_Layer = 6;
	}
	// Icon_4
	if (m_UI_Desc.m_Type == 7) {

		m_fX = 1107;
		m_fY = 445;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 96.f;
		m_Origin_Y = 96.f;
		m_Size_Param = 1.1f;
		m_UI_Layer = 6;
	}

	// Title
	if (m_UI_Desc.m_Type == 8) {
		m_fX = 120;
		m_fY = 90;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 596.f;
		m_Origin_Y = 290.f;
		m_Size_Param = 0.6f;
		m_UI_Layer = 4;
	}

	// Cloud_LT
	if (m_UI_Desc.m_Type == 9) {
		m_fX = 65;
		m_fY = 7;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1080.f;
		m_Origin_Y = 328.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 3;
		m_Alpha = 1.f;
		m_Is_Side_Cut_R = true;
	}
	// Cloud_LT_Dark
	if (m_UI_Desc.m_Type == 10) {

		m_fX = 140;
		m_fY = 70;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1032.f;
		m_Origin_Y = 560.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 2;
		m_Alpha = 1.f;
		m_Is_Side_Cut_R = true;
	}
	// Cloud_RD
	if (m_UI_Desc.m_Type == 11) {

		m_fX = 1200;
		m_fY = 720;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 848.f;
		m_Origin_Y = 308.f;
		m_Size_Param = 0.7f;
		m_UI_Layer = 3;
		m_Alpha = 1.f;
		m_Is_Side_Cut_L = true;
	}
	// Cloud_RD_Dark
	if (m_UI_Desc.m_Type == 12) {

		m_fX = 1150;
		m_fY = 690;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1000.f;
		m_Origin_Y = 328.f;
		m_Size_Param = 0.73f;
		m_UI_Layer = 2;
		m_Alpha = 1.f;
		m_Is_Side_Cut_L = true;
	}

	// Mini_Title
	if (m_UI_Desc.m_Type == 13) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 232.f;
		m_Origin_Y = 96.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 2;
	}

	// Line
	if (m_UI_Desc.m_Type == 14) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1280.f;
		m_Origin_Y = 720.f;
		m_Size_Param = 1.f;
		m_UI_Layer = 5;
	}

	// Cursor
	if (m_UI_Desc.m_Type == 15) {

		m_fX = 110;
		m_fY = 535;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 180.f;
		m_Origin_Y = 180.f;
		m_Size_Param = 0.7f;
		m_UI_Layer = 7;
	}

	// Mask_0
	if (m_UI_Desc.m_Type == 16) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 3080.f;
		m_Origin_Y = 1080.f;
		m_Size_Param = 0.7f;
		m_UI_Layer = 2;
	}

	// Mask_1
	if (m_UI_Desc.m_Type == 17) {

		m_fX = 640;
		m_fY = 360;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 3080.f;
		m_Origin_Y = 1080.f;
		m_Size_Param = 0.7f;
		m_UI_Layer = 2;
	}

	m_szTitle.push_back(L"시나리오");
	m_szContent.push_back(L"북서쪽 마을로");

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CStory_Board::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_Story_Index = CStoryManager::GetInstance()->Get_Select_Type();


	if (m_UI_Desc.m_Type == 15) {

		if (m_Story_Index == 0) {
			m_fX = 110;
			m_fY = 535;
		}
		if (m_Story_Index == 1) {

			m_fX = 510;
			m_fY = 605;
		}
		if (m_Story_Index == 2) {

			m_fX = 860;
			m_fY = 550;
		}
		if (m_Story_Index == 3) {

			m_fX = 1050;
			m_fY = 580;
		}
		if (m_Story_Index == 4) {

			m_fX = 1107;
			m_fY = 445;
		}
	}





	if (m_UI_Desc.m_Type == 8) {
		
		if (m_Story_Index == 0) {
			m_Origin_X = 432.f;
			m_Origin_Y = 212.f;
			m_Size_Param = 0.6f;
			m_fX = 120;
			m_fY = 90;
		}
		else {
			m_Origin_X = 596.f;
			m_Origin_Y = 290.f;
			m_Size_Param = 0.55f;
			m_fX = 184;
			m_fY = 100;
		}
		
	}


	// Cloud_LT
	if (m_UI_Desc.m_Type == 9) {

		m_fX += dTimeDelta * 5.f * m_PosX_Dir;

		if (m_fX - m_Origin_PosX >= 15.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX <= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}
	}
	// Cloud_LT_Dark
	if (m_UI_Desc.m_Type == 10) {

		m_fX -= dTimeDelta * 5.f * m_PosX_Dir;

		if (m_Origin_PosX - m_fX >= 15.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX >= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}


	}

	// Cloud_RD
	if (m_UI_Desc.m_Type == 11) {

		m_fX += dTimeDelta * 5.f * m_PosX_Dir;

		if (m_fX - m_Origin_PosX >= 15.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX <= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}

	}
	// Cloud_RD_Dark
	if (m_UI_Desc.m_Type == 12) {

		m_fX -= dTimeDelta * 5.f * m_PosX_Dir;

		if (m_Origin_PosX - m_fX >= 15.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX >= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}

	}


	Set_UI();


}

void CStory_Board::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;

}

HRESULT CStory_Board::Render()
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

	if (m_UI_Desc.m_Type == 13) {

		/*if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szTitle[m_Story_Index].c_str(), _float2((_float)m_fX - 43.f, (_float)m_fY - 30.f), _float2(0.5f, 0.5f), XMVectorSet(0.6f, 0.6f, 0.f, 1.f))))
			return E_FAIL;

		if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szContent[m_Story_Index].c_str(), _float2((_float)m_fX - 30.f, (_float)m_fY - 16.f), _float2(0.5f, 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
			return E_FAIL;*/
	}


	Safe_Release(pGameInstance);


	return S_OK;
}

void CStory_Board::Set_UI()
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

HRESULT CStory_Board::Add_Components()
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


	if (m_UI_Desc.m_Type == 0) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Bg"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 1 || m_UI_Desc.m_Type == 2) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Bg_Deco"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type >= 3 && m_UI_Desc.m_Type <= 7) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Icon"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 8) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Title"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type >= 9 && m_UI_Desc.m_Type <= 12) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Cloud"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 13) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Mini_Title"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 14) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Line"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 15) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Cursor"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type == 16 || m_UI_Desc.m_Type == 17) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Story_Bg_Mask"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT CStory_Board::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;


	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Is_Side_Cut_R", &m_Is_Side_Cut_R, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_Is_Side_Cut_L", &m_Is_Side_Cut_L, sizeof(_bool))))
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


	// 백그라운드
	if (m_UI_Desc.m_Type == 0) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_Story_Index)))
			return E_FAIL;
	}
	// 백그라운드 데코
	else if (m_UI_Desc.m_Type == 1 || m_UI_Desc.m_Type == 2) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type - 1)))
			return E_FAIL;
	}
	// 아이콘
	else if (m_UI_Desc.m_Type >= 3 && m_UI_Desc.m_Type <= 7) {

		if (m_UI_Desc.m_Type == 3) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))  
				return E_FAIL;
		}
		if (m_UI_Desc.m_Type == 4) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 1)))
				return E_FAIL;
		}
		if (m_UI_Desc.m_Type == 5) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 2)))
				return E_FAIL;
		}
		if (m_UI_Desc.m_Type == 6) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
				return E_FAIL;
		}
		if (m_UI_Desc.m_Type == 7) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 2)))
				return E_FAIL;
		}

	}
	// 타이틀
	else if (m_UI_Desc.m_Type == 8) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_Story_Index)))
			return E_FAIL;

	}
	// 구름
	else if (m_UI_Desc.m_Type >= 9 && m_UI_Desc.m_Type <= 12) {

		if (m_UI_Desc.m_Type == 9) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
				return E_FAIL;
		}
		if (m_UI_Desc.m_Type == 10) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 2)))
				return E_FAIL;
		}
		if (m_UI_Desc.m_Type == 11) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 1)))
				return E_FAIL;
		}
		if (m_UI_Desc.m_Type == 12) {
			if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 3)))
				return E_FAIL;
		}

	}
	// 소제목
	else if (m_UI_Desc.m_Type == 13) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

	}
	// 라인
	else if (m_UI_Desc.m_Type == 14) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

	}
	// 커서
	else if (m_UI_Desc.m_Type == 15) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;

	}
	// 마스크 텍스쳐
	else if (m_UI_Desc.m_Type == 16 || m_UI_Desc.m_Type == 17) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type - 16)))
			return E_FAIL;

	}



	return S_OK;
}

CStory_Board* CStory_Board::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStory_Board* pInstance = new CStory_Board(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStory_Board");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStory_Board::Clone(void* pArg)
{
	CStory_Board* pInstance = new CStory_Board(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CStory_Board");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStory_Board::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}