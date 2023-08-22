#include "pch.h"
#include "..\Public\Option.h"

#include "GameInstance.h"
#include "Title_Manager.h"
#include "OptionManager.h"


COption::COption(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

COption::COption(const COption& rhs)
	: CUI(rhs)
{
}

HRESULT COption::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		MSG_BOX("Failed to Initialize_Prototype : Option");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT COption::Initialize(void* pArg)
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
		m_UI_Layer = 30;
	}

	// Title
	if (m_UI_Desc.m_Type == 1) {

		m_fX = 100;
		m_fY = 80;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 160.f;
		m_Origin_Y = 96.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 32;
	}

	// Rect
	if (m_UI_Desc.m_Type == 2) {

		m_fX = 850;
		m_fY = 365;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 620.f;
		m_Origin_Y = 360.f;
		m_Size_Param = 1.3f;
		m_UI_Layer = 32;
	}

	// Line
	if (m_UI_Desc.m_Type == 3) {

		m_fX = 850;
		m_fY = 165;
		m_Origin_X = 820.f;
		m_Origin_Y = 18.f;
		m_Size_Param = 0.83f;
		m_UI_Layer = 33;
	}

	// Arrow
	if (m_UI_Desc.m_Type == 4) {

		m_Origin_X = 40.f;
		m_Origin_Y = 40.f;
		m_Size_Param = 0.6f;
		m_UI_Layer = 32;
	}

	// Bar_Back
	if (m_UI_Desc.m_Type == 5) {

		
		m_Origin_X = 460.f;
		m_Origin_Y = 28.f;
		m_Size_Param = 0.7f;
		m_UI_Layer = 32;
	}

	// Bar
	if (m_UI_Desc.m_Type == 6) {

		
		m_Origin_X = 450.f;
		m_Origin_Y = 28.f;
		m_Size_Param = 0.7f;
		m_UI_Layer = 33;
	}

	// Cursor
	if (m_UI_Desc.m_Type == 7) {

		m_UI_Layer = 32;
		m_Is_Side_Cut_R = true;
	}


	// Cloud_LT
	if (m_UI_Desc.m_Type == 9) {
		m_fX = 90;
		m_fY = 55;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1080.f;
		m_Origin_Y = 328.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 31;
		m_Is_Side_Cut_R = true;
	}
	// Cloud_RT
	if (m_UI_Desc.m_Type == 10) {

		m_fX = 1120;
		m_fY = 20;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 848.f;
		m_Origin_Y = 308.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 31;
		m_Is_Side_Cut_L = true;
	}
	// Cloud_LD
	if (m_UI_Desc.m_Type == 11) {

		m_fX = 290;
		m_fY = 715;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 848.f;
		m_Origin_Y = 308.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 31;
		m_Is_Side_Cut_L = true;
	}
	// Cloud_RD
	if (m_UI_Desc.m_Type == 12) {

		m_fX = 1120;
		m_fY = 710;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1080.f;
		m_Origin_Y = 328.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 31;
		m_Is_Side_Cut_L = true;

	}

	// Cloud_LT_D
	if (m_UI_Desc.m_Type == 13) {
		m_fX = 200;
		m_fY = 120;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1032.f;
		m_Origin_Y = 560.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 30.5;
		m_Is_Side_Cut_R = true;
	}
	// Cloud_RT_D
	if (m_UI_Desc.m_Type == 14) {

		m_fX = 1000;
		m_fY = 80;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1000.f;
		m_Origin_Y = 328.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 30.5;
		m_Is_Side_Cut_L = true;
	}
	// Cloud_LD_D
	if (m_UI_Desc.m_Type == 15) {

		m_fX = 290;
		m_fY = 685;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1080.f;
		m_Origin_Y = 328.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 30.5;
		m_Is_Side_Cut_L = true;
	}
	// Cloud_RD_D
	if (m_UI_Desc.m_Type == 16) {

		m_fX = 1150;
		m_fY = 640;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 1080.f;
		m_Origin_Y = 328.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 30.5;
		m_Is_Side_Cut_L = true;

	}
	// Anounce_Back
	if (m_UI_Desc.m_Type == 17) {

		m_fX = 950;
		m_fY = 670;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 128.f * 8.f;
		m_Origin_Y = 52.f;
		m_Size_Param = 0.69f;
		m_UI_Layer = 32;
		m_Is_Side_Cut_L = true;

	}
	// Button_Y
	if (m_UI_Desc.m_Type == 18) {

		m_fX = 1000;
		m_fY = 670;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 92.f;
		m_Origin_Y = 92.f;
		m_Size_Param = 0.35f;
		m_UI_Layer = 33;
	}
	// Button_B
	if (m_UI_Desc.m_Type == 19) {

		m_fX = 1120;
		m_fY = 670;
		m_Origin_PosX = (_float)m_fX;
		m_Origin_PosY = (_float)m_fY;
		m_Origin_X = 92.f;
		m_Origin_Y = 92.f;
		m_Size_Param = 0.35f;
		m_UI_Layer = 33;
	}
	
	m_szCameraMenu.push_back(L"카메라 설정");
	m_szCameraMenu.push_back(L"카메라 감도");
	m_szCameraMenu.push_back(L"카메라 흔들림");

	m_szCameraMenu_2.push_back(L"낮음");
	m_szCameraMenu_2.push_back(L"보통");
	m_szCameraMenu_2.push_back(L"높음");
	

	m_szGraphicMenu.push_back(L"그래픽 설정");
	m_szGraphicMenu.push_back(L"밝기");
	m_szGraphicMenu.push_back(L"SSAO");
	m_szGraphicMenu.push_back(L"모션블러");

	m_szGraphicMenu_2.push_back(L"ON");
	m_szGraphicMenu_2.push_back(L"OFF");

	m_szGraphicMenu_3.push_back(L"낮음");
	m_szGraphicMenu_3.push_back(L"보통");
	m_szGraphicMenu_3.push_back(L"높음");

	m_szSoundMenu.push_back(L"사운드 설정");
	m_szSoundMenu.push_back(L"마스터 볼륨");
	m_szSoundMenu.push_back(L"BGM 음량");
	m_szSoundMenu.push_back(L"SE 음량");
	m_szSoundMenu.push_back(L"음성 음량");



	m_szButtonMenu.push_back(L"결정");
	m_szButtonMenu.push_back(L"돌아가기");

	
	m_Is_TimeFree = true;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void COption::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 옵션 나가기
	if (pGameInstance->Get_DIKeyDown(DIK_B)) {

		COptionManager::GetInstance()->Set_Is_Option_On(false);
		m_Is_Font_Render = false;
		COptionManager::GetInstance()->Set_Is_Reset(false);
	}

	if (COptionManager::GetInstance()->Get_Is_Option_On() == true) {

		if (m_UI_Desc.m_Type == 3 || m_UI_Desc.m_Type == 4 || m_UI_Desc.m_Type == 5 || m_UI_Desc.m_Type == 6) {
			m_Alpha += (_float)dTimeDelta * 8.f;
		}
		else {
			m_Alpha += (_float)dTimeDelta * 2.f;
		}
		

		if (m_Alpha >= 0.5f) {
			m_Is_Font_Render = true;
		}


		if (m_UI_Desc.m_Type == 17) {
			if (m_Alpha >= 0.9f)
			{
				m_Alpha = 0.9f;
			}
		}
		else {
			if (m_Alpha >= 1.f)
			{
				m_Alpha = 1.f;
			}
		}
	
	}
	else {

		m_Alpha -= (_float)dTimeDelta * 2.f;

		if (m_Alpha <= 0.f)
		{
			m_Alpha = 0.f;
		}

	}

	if (m_UI_Desc.m_Type == 5 || m_UI_Desc.m_Type == 6) {

		m_fX = 992;

		if (m_UI_Desc.m_Line_Num == 0) {
			m_fY = 230;
		}
		else if (m_UI_Desc.m_Line_Num == 1) {
			m_fY = 305;
		}
		else if (m_UI_Desc.m_Line_Num == 2) {
			m_fY = 380;
		}
		else if (m_UI_Desc.m_Line_Num == 3) {
			m_fY = 455;
		}
		else if (m_UI_Desc.m_Line_Num == 4) {
			m_fY = 530;
		}

		m_fY -= 45.f;

		if (COptionManager::GetInstance()->Get_Select_Num() == 0) {
			if (m_UI_Desc.m_Line_Num == 1) {
				m_Alpha = 0.f;
			}
			if (m_UI_Desc.m_Line_Num == 2) {
				m_Alpha = 0.f;
			}
			if (m_UI_Desc.m_Line_Num == 3) {
				m_Alpha = 0.f;
			}
			if (m_UI_Desc.m_Line_Num == 4) {
				m_Alpha = 0.f;
			}
		}
		if (COptionManager::GetInstance()->Get_Select_Num() == 1) {

			if (m_UI_Desc.m_Line_Num == 1) {
				m_Alpha = 0.f;
			}
			if (m_UI_Desc.m_Line_Num == 2) {
				m_Alpha = 0.f;
			}
			if (m_UI_Desc.m_Line_Num == 3) {
				m_Alpha = 0.f;
			}
			if (m_UI_Desc.m_Line_Num == 4) {
				m_Alpha = 0.f;
			}
		}
		if (COptionManager::GetInstance()->Get_Select_Num() == 2) {

			if (m_UI_Desc.m_Line_Num == 4) {
				m_Alpha = 0.f;
			}
		}

	}

	if (m_UI_Desc.m_Type == 4) {

		if (m_UI_Desc.m_Arrow_Type == 0)
			m_fX = 810;
		else
			m_fX = 1174;

		if (m_UI_Desc.m_Line_Num == 0) {
			m_fY = 230;
		}
		else if (m_UI_Desc.m_Line_Num == 1) {
			m_fY = 305;
		}
		else if (m_UI_Desc.m_Line_Num == 2) {
			m_fY = 380;
		}
		else if (m_UI_Desc.m_Line_Num == 3) {
			m_fY = 455;
		}
		else if (m_UI_Desc.m_Line_Num == 4) {
			m_fY = 530;
		}

		m_fY -= 36;

		if (COptionManager::GetInstance()->Get_Select_Num() == 0) {
			if (m_UI_Desc.m_Line_Num == 2) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
			if (m_UI_Desc.m_Line_Num == 3) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
			if (m_UI_Desc.m_Line_Num == 4) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
		}
		if (COptionManager::GetInstance()->Get_Select_Num() == 1) {

			if (m_UI_Desc.m_Line_Num == 3) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
			if (m_UI_Desc.m_Line_Num == 4) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
		}
		if (COptionManager::GetInstance()->Get_Select_Num() == 2) {

			if (m_UI_Desc.m_Line_Num == 4) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
		}
	}


	if (m_UI_Desc.m_Type == 3) {

		if (m_UI_Desc.m_Line_Num == 0) {
			m_fX = 850;
			m_fY = 230;
		}
		else if (m_UI_Desc.m_Line_Num == 1) {
			m_fX = 850;
			m_fY = 305;
		}
		else if (m_UI_Desc.m_Line_Num == 2) {
			m_fX = 850;
			m_fY = 380;
		}
		else if (m_UI_Desc.m_Line_Num == 3) {
			m_fX = 850;
			m_fY = 455;
		}
		else if (m_UI_Desc.m_Line_Num == 4) {
			m_fX = 850;
			m_fY = 530;
		}


		if (COptionManager::GetInstance()->Get_Select_Num() == 0) {
			if (m_UI_Desc.m_Line_Num == 2) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
			if (m_UI_Desc.m_Line_Num == 3) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
			if (m_UI_Desc.m_Line_Num == 4) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
		}
		if (COptionManager::GetInstance()->Get_Select_Num() == 1) {

			if (m_UI_Desc.m_Line_Num == 3) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
			if (m_UI_Desc.m_Line_Num == 4) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
		}
		if (COptionManager::GetInstance()->Get_Select_Num() == 2) {
	
			if (m_UI_Desc.m_Line_Num == 4) {
				m_Alpha = 0.f;
				m_Is_Font_Render = false;
			}
		}

	}

	if ((m_UI_Desc.m_Type == 7) && COptionManager::GetInstance()->Get_Is_Option_On() == true) {

		
		if (pGameInstance->Get_DIKeyDown(DIK_UP)) {
			m_Select_Num--;

			if (m_Select_Num < 0) {
				m_Select_Num = 0;
			}
		}
		if (pGameInstance->Get_DIKeyDown(DIK_DOWN)) {
			m_Select_Num++;

			if (m_Select_Num > 2) {
				m_Select_Num = 2;
			}
		}

		if (m_Select_Num == m_UI_Desc.m_Menu_Type) {
			m_Is_Select = true;
		}
		else {
			m_Is_Select = false;
		}

		COptionManager::GetInstance()->Set_Select_Num(m_Select_Num);

		if (m_UI_Desc.m_Menu_Type == 0) {
			m_fX = 210;
			m_fY = 170;
			m_Origin_PosX = (_float)m_fX;
			m_Origin_PosY = (_float)m_fY;
		}
		else if (m_UI_Desc.m_Menu_Type == 1) {
			m_fX = 210;
			m_fY = 245;
			m_Origin_PosX = (_float)m_fX;
			m_Origin_PosY = (_float)m_fY;
		}
		else if (m_UI_Desc.m_Menu_Type == 2) {
			m_fX = 210;
			m_fY = 320;
			m_Origin_PosX = (_float)m_fX;
			m_Origin_PosY = (_float)m_fY;
		}

		if (m_Is_Select == true) {
			m_fX -= 20;
			m_Origin_X = 700.f;
			m_Origin_Y = 140.f;
			m_Size_Param = 0.65f;
		}
		else {
			m_fX -= 10;
			m_Origin_X = 600.f;
			m_Origin_Y = 120.f;
			m_Size_Param = 0.69f;
		}

	}


	if (m_UI_Desc.m_Type == 1 || m_UI_Desc.m_Type == 7 || m_UI_Desc.m_Type == 17 || m_UI_Desc.m_Type == 18 || m_UI_Desc.m_Type == 19) {

		if (m_UI_Desc.m_Type != 7)
			m_fX = m_Origin_PosX;

		m_fX -= COptionManager::GetInstance()->Get_Option_Move();
	}


	if (COptionManager::GetInstance()->Get_Is_Move_Done() == false) {

		if (m_UI_Desc.m_Type == 9 || m_UI_Desc.m_Type == 11 || m_UI_Desc.m_Type == 13 || m_UI_Desc.m_Type == 15) {

			m_fX = (_double)m_Origin_PosX - (_double)COptionManager::GetInstance()->Get_Option_Move();
		}

		if (m_UI_Desc.m_Type == 10 || m_UI_Desc.m_Type == 12 || m_UI_Desc.m_Type == 14 || m_UI_Desc.m_Type == 16 
			|| m_UI_Desc.m_Type == 17 || m_UI_Desc.m_Type == 18 || m_UI_Desc.m_Type == 19) {

			m_fX = (_double)m_Origin_PosX + (_double)COptionManager::GetInstance()->Get_Option_Move();
		}
	}

	if (m_UI_Desc.m_Type != 1 && m_UI_Desc.m_Type != 3 && m_UI_Desc.m_Type != 4 && m_UI_Desc.m_Type != 5 && m_UI_Desc.m_Type != 6 && m_UI_Desc.m_Type != 7) {

		if (COptionManager::GetInstance()->Get_Is_Move_Done() == true) {

			if (m_Is_Reset == false) {
				COptionManager::GetInstance()->Set_Is_Reset(true);
				m_fX = (_double)m_Origin_PosX;
			}


			Cloud_Control(dTimeDelta);
		}
	}


	Set_UI();



	Safe_Release(pGameInstance);

}

void COption::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);


	m_Is_Reset = COptionManager::GetInstance()->Get_Is_Reset();


	if (m_Alpha == 0.f) {
		m_Is_Render = false;
	}
	else {
		m_Is_Render = true;
	}



	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this)))
		return;

}

HRESULT COption::Render()
{
	if (m_Is_Render == true) {

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
			m_pShaderCom->Begin(17);
		}
		else {
			m_pShaderCom->Begin(1);
		}


		m_pVIBufferCom->Render();
	}

	if (m_Is_Font_Render) {

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (m_UI_Desc.m_Type == 7) {

			if (m_UI_Desc.m_Menu_Type == 0) {
				if (m_Is_Select == true) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szCameraMenu[0].c_str(), _float2((_float)m_fX - 120.f, (_float)m_fY - 17.f), _float2(0.55f, 0.55f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szCameraMenu[0].c_str(), _float2((_float)m_fX - 130.f, (_float)m_fY - 17.f), _float2(0.5f, 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
						return E_FAIL;
				}
			}

			if (m_UI_Desc.m_Menu_Type == 1) {
				if (m_Is_Select == true) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGraphicMenu[0].c_str(), _float2((_float)m_fX - 120.f, (_float)m_fY - 17.f), _float2(0.55f, 0.55f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGraphicMenu[0].c_str(), _float2((_float)m_fX - 130.f, (_float)m_fY - 17.f), _float2(0.5f, 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
						return E_FAIL;
				}
			}

			if (m_UI_Desc.m_Menu_Type == 2) {
				if (m_Is_Select == true) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu[0].c_str(), _float2((_float)m_fX - 120.f, (_float)m_fY - 17.f), _float2(0.55f, 0.55f), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu[0].c_str(), _float2((_float)m_fX - 130.f, (_float)m_fY - 17.f), _float2(0.5f, 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
						return E_FAIL;
				}
			}


		}

		if (m_UI_Desc.m_Type == 17) {

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szButtonMenu[0].c_str(), _float2((_float)m_fX + 70.f, (_float)m_fY - 13.f), _float2(0.5f, 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
				return E_FAIL;

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szButtonMenu[1].c_str(), _float2((_float)m_fX + 190.f, (_float)m_fY - 13.f), _float2(0.5f, 0.5f), XMVectorSet(1.f, 1.f, 1.f, 1.f))))
				return E_FAIL;


		}

		if (m_UI_Desc.m_Type == 3) {

			if (COptionManager::GetInstance()->Get_Select_Num() == 0) {
				if (m_UI_Desc.m_Line_Num == 0) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szCameraMenu[1].c_str(), _float2((_float)m_fX - 338.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;

					wsprintf(m_szCameraMenu_1, TEXT("%d"), m_szCameraMenu_1_Int);
					
					if (m_szCameraMenu_1_Int < 10) {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szCameraMenu_1, _float2(984.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
					else {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szCameraMenu_1, _float2(977.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}

				}
				else if (m_UI_Desc.m_Line_Num == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szCameraMenu[2].c_str(), _float2((_float)m_fX - 338.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;

					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szCameraMenu_2[m_szCameraMenu_2_Num].c_str(), _float2(970.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;

				}
			}
			else if (COptionManager::GetInstance()->Get_Select_Num() == 1) {
				if (m_UI_Desc.m_Line_Num == 0) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGraphicMenu[1].c_str(), _float2((_float)m_fX - 338.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;

					wsprintf(m_szGraphicMenu_1, TEXT("%d"), m_szGraphicMenu_1_Int);

					if (m_szGraphicMenu_1_Int < 10) {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGraphicMenu_1, _float2(984.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
					else {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGraphicMenu_1, _float2(977.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
				}
				else if (m_UI_Desc.m_Line_Num == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGraphicMenu[2].c_str(), _float2((_float)m_fX - 338.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;
					
					if (m_szGraphicMenu_2_Num == 0) {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGraphicMenu_2[m_szGraphicMenu_2_Num].c_str(), _float2(973.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
					else {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGraphicMenu_2[m_szGraphicMenu_2_Num].c_str(), _float2(969.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
					
				}
				else if (m_UI_Desc.m_Line_Num == 2) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGraphicMenu[3].c_str(), _float2((_float)m_fX - 338.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;

					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szGraphicMenu_3[m_szGraphicMenu_3_Num].c_str(), _float2(971.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;
				}
			}
			else if (COptionManager::GetInstance()->Get_Select_Num() == 2) {
				if (m_UI_Desc.m_Line_Num == 0) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu[1].c_str(), _float2((_float)m_fX - 338.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;

					wsprintf(m_szSoundMenu_1, TEXT("%d"), m_szSoundMenu_Int[0]);

					if (m_szSoundMenu_Int[0] < 10) {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu_1, _float2(984.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
					else {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu_1, _float2(977.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
				}
				else if (m_UI_Desc.m_Line_Num == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu[2].c_str(), _float2((_float)m_fX - 338.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;

					wsprintf(m_szSoundMenu_2, TEXT("%d"), m_szSoundMenu_Int[1]);

					if (m_szSoundMenu_Int[1] < 10) {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu_2, _float2(984.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
					else {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu_2, _float2(977.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
				}
				else if (m_UI_Desc.m_Line_Num == 2) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu[3].c_str(), _float2((_float)m_fX - 338.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;

					wsprintf(m_szSoundMenu_3, TEXT("%d"), m_szSoundMenu_Int[2]);

					if (m_szSoundMenu_Int[2] < 10) {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu_3, _float2(984.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
					else {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu_3, _float2(977.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
				}
				else if (m_UI_Desc.m_Line_Num == 3) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu[4].c_str(), _float2((_float)m_fX - 338.f, (_float)m_fY - 51.f), _float2(0.5f, 0.5f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
						return E_FAIL;

					wsprintf(m_szSoundMenu_4, TEXT("%d"), m_szSoundMenu_Int[3]);

					if (m_szSoundMenu_Int[3] < 10) {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu_4, _float2(984.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
					else {
						if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSoundMenu_4, _float2(977.f, (_float)m_fY - 35.f), _float2(0.45f, 0.45f), XMVectorSet(46.f / 255.f, 28.f / 255.f, 3.f / 255.f, 1.f))))
							return E_FAIL;
					}
				}
			}
		
		}

		Safe_Release(pGameInstance);

	}



	return S_OK;
}

void COption::Set_UI()
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

void COption::Cloud_Control(_double dTimeDelta)
{
	// Cloud_LT
	if (m_UI_Desc.m_Type == 9) {

		m_fX += dTimeDelta * 5.f * m_PosX_Dir;

		if (m_fX - m_Origin_PosX >= 20.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX <= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}
	}
	// Cloud_RT
	if (m_UI_Desc.m_Type == 10) {

		m_fX += dTimeDelta * 5.f * m_PosX_Dir;

		if (m_fX - m_Origin_PosX >= 20.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX <= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}


	}
	// Cloud_LD
	if (m_UI_Desc.m_Type == 11) {

		m_fX -= dTimeDelta * 5.f * m_PosX_Dir;

		if (m_Origin_PosX - m_fX >= 20.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX >= m_Origin_PosX) {
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


	// Cloud_LT_D
	if (m_UI_Desc.m_Type == 13) {

		m_fX -= dTimeDelta * 5.f * m_PosX_Dir;

		if (m_Origin_PosX - m_fX >= 20.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX >= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}


	}
	// Cloud_RT_D
	if (m_UI_Desc.m_Type == 14) {

		m_fX -= dTimeDelta * 5.f * m_PosX_Dir;

		if (m_Origin_PosX - m_fX >= 20.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX >= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}


	}
	// Cloud_LD_D
	if (m_UI_Desc.m_Type == 15) {

		m_fX += dTimeDelta * 5.f * m_PosX_Dir;

		if (m_fX - m_Origin_PosX >= 20.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX <= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}


	}
	// Cloud_RD_D
	if (m_UI_Desc.m_Type == 16) {

		m_fX += dTimeDelta * 5.f * m_PosX_Dir;

		if (m_fX - m_Origin_PosX >= 20.f) {

			m_PosX_Dir *= -1.f;
		}

		if (m_fX <= m_Origin_PosX) {
			m_PosX_Dir *= -1.f;
		}


	}
}

HRESULT COption::Add_Components()
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
	
	if (m_UI_Desc.m_Type <= 6) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Option"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 7) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Option_Cursor"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type >= 9 && m_UI_Desc.m_Type <= 16) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Option_Cloud"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type >= 17 && m_UI_Desc.m_Type <= 19) {

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Option"),
			TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;

	}

	return S_OK;
}

HRESULT COption::SetUp_ShaderResources()
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

	if (m_UI_Desc.m_Type <= 6) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type)))
			return E_FAIL;
	}
	else if (m_UI_Desc.m_Type == 7) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", (_uint)m_Is_Select)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type >= 9 && m_UI_Desc.m_Type <= 16) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Cloud_Type)))
			return E_FAIL;

	}
	else if (m_UI_Desc.m_Type >= 17 && m_UI_Desc.m_Type <= 19) {

		if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", m_UI_Desc.m_Type - 10)))
			return E_FAIL;
	}

	
	return S_OK;
}

COption* COption::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COption* pInstance = new COption(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : COption");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COption::Clone(void* pArg)
{
	COption* pInstance = new COption(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : COption");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COption::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}