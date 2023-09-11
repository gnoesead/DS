#include "pch.h"
#include "..\Public\Dialog.h"

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Layer.h"
#include "Player.h"
#include "DialogManager.h"
#include "MissionManager.h"


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


	m_szName[TANJIRO] = L"카마도 탄지로";
	m_szName[GIRL] = L"소녀";
	m_szName[MOTHER] = L"여자";
	m_szName[BOY] = L"남자아이";
	m_szName[MAN] = L"남자";
	m_szName[ZENITSU] = L"아가츠마 젠이츠";



// Main
	// 0
	m_szMain_1.push_back(L"방해되니까 비켜");
	m_szMain_1.push_back(L"말씀이 심하시네요");
	m_szMain_1.push_back(L"좀 비켜주실래요?");
	
	// 1
	m_szMain_2.push_back(L"여긴 위험하니 집으로 들어가세요");
	m_szMain_2.push_back(L"그래 알겠어");
	
	// 2
	m_szMain_3.push_back(L"여긴 위험하니 집으로 들어가세요");
	m_szMain_3.push_back(L"그래 알겠어");
	
// Sub
	// 3
	m_szSub_1.push_back(L"혹시 수상한 혈귀를 보신 적 있나요?");
	m_szSub_1.push_back(L"혈귀..?? 음..잘 모르겠는걸");
	m_szSub_1.push_back(L"그럼 마을에 이상한 현상이라도 없었나요?");
	m_szSub_1.push_back(L"음..아! 요즘 젊은 소녀들이 갑자기 사라진다는 소문이 있어");
	m_szSub_1.push_back(L"(소녀들이 사라진다고,,?) 흠..그렇군요. 감사합니다!");
	// 4
	m_szSub_2.push_back(L"저기..말씀 좀 여쭤봐도 될까요?");
	m_szSub_2.push_back(L"내 딸...내 딸이 사려졌어..아무도 믿어주질 않아..너도 똑같겠지");

	// 5
	m_szSub_3.push_back(L"지붕에서 계속 이상한 기운이 느껴져..");
	m_szSub_3.push_back(L"(지붕..? 단서가 있을지도 몰라 가보자!)");

	// 6
	m_szSub_4.push_back(L"저희 아빠가 다쳤어요..제발 도와주세요!!");
	m_szSub_4.push_back(L"이런!! 사람들이..!");

	// 7
	m_szSub_5.push_back(L"저 집에 들어갈 생각이라면 그만둬요. 얼마나 음산한지..소름이 끼친다구요");
	


// Talk
	// 8
	m_szTalk_1.push_back(L"방해되니까 비켜");
	m_szTalk_1.push_back(L"말씀이 심하시네요");

	// 9
	m_szTalk_2.push_back(L"탄지로!! 왔구나! 이 집은 뭔가 무서워..들어가지말자");
	m_szTalk_2.push_back(L"이 집에서 악취가 뿜어져나와..들어가봐야겠어.");

	// 10
	m_szTalk_3.push_back(L"주변에 혈귀들이 있어. 들키지 않게 조용히 해야해");
	m_szTalk_3.push_back(L"난 분명 죽을거야!! 도와줘 탄지로!!!");


// Village Smell
	// 11
	m_szVillageSmell_1.push_back(L"(이 냄새는....!!!! 혈귀다)");
	m_szVillageSmell_1.push_back(L"(....여자들이 사라진 이유가..혹시?)");

	// 12
	m_szVillageSmell_2.push_back(L"(혈귀의 냄새가 아직 남아있어)");
	m_szVillageSmell_2.push_back(L"(....서두르자...!)");

	// 13
	m_szVillageSmell_3.push_back(L"(냄새가 더 진해졌어..아니 이건 뭔가 달라)");
	m_szVillageSmell_3.push_back(L"(어디서 나오는거지..벽 너머의 저 집인가?)");


// Village Talk
	// 14
	m_szHouseSmell_1.push_back(L"(사람들이 모두 쓰러져있어! 이 앞에 혈귀가..?!)");
	
	// 15
	m_szHouseSmell_2.push_back(L"(혈귀의 냄새가 남아있어2)");
	m_szHouseSmell_2.push_back(L"(....서두르자)");

	// 16
	m_szHouseSmell_3.push_back(L"(혈귀의 냄새가 남아있어3)");
	m_szHouseSmell_3.push_back(L"(....서두르자)");



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

	
	m_Dialog_Type = CDialogManager::GetInstance()->Get_Dialog_Type();

	if (pGameInstance->Get_DIKeyDown(DIK_F) && m_Is_Font_Render == true && m_Dialog_Type != 99) {

		m_Cur_Num++;
		m_Name_Type *= -1;

		// Main
		if (m_Dialog_Type == 0) {
			if (m_Cur_Num > m_szMain_1.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 1) {
			if (m_Cur_Num > m_szMain_2.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 2) {
			if (m_Cur_Num > m_szMain_3.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		// Sub
		else if (m_Dialog_Type == 3) {
			if (m_Cur_Num > m_szSub_1.size() - 1) {
				
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 4) {
			if (m_Cur_Num > m_szSub_2.size() - 1) {
				
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 5) {
			if (m_Cur_Num > m_szSub_3.size() - 1) {
				
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 6) {
			if (m_Cur_Num > m_szSub_4.size() - 1) {
				
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 7) {
			if (m_Cur_Num > m_szSub_5.size() - 1) {
				
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		// Talk
		else if (m_Dialog_Type == 8) {
			if (m_Cur_Num > m_szTalk_1.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 9) {
			if (m_Cur_Num > m_szTalk_2.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 10) {
			if (m_Cur_Num > m_szTalk_3.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		// Village Smell
		else if (m_Dialog_Type == 11) {
			if (m_Cur_Num > m_szVillageSmell_1.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 12) {
			if (m_Cur_Num > m_szVillageSmell_2.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 13) {
			if (m_Cur_Num > m_szVillageSmell_3.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		// House Smell
		else if (m_Dialog_Type == 14) {
			if (m_Cur_Num > m_szHouseSmell_1.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 15) {
			if (m_Cur_Num > m_szHouseSmell_2.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}
		else if (m_Dialog_Type == 16) {
			if (m_Cur_Num > m_szHouseSmell_3.size() - 1) {
				m_Cur_Num = 0;
				m_Is_Out = true;
				m_Is_Font_Render = false;
				m_Name_Type = 1;
			}
		}

	}

	if (pGameInstance->Get_DIKeyDown(DIK_F) && m_Is_In == false && m_Is_Out == false && m_Dialog_Type != 99) {
		m_Is_In = true;
		m_Is_Font_Render = true;
	}

	if (m_Is_Font_Render == true)
		CMissionManager::GetInstance()->Set_Is_Dialog_On(true);
	else {
		CMissionManager::GetInstance()->Set_Is_Dialog_On(false);
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

HRESULT CDialog::Render()
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
			if (m_Dialog_Type == 0) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szMain_1[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szMain_1[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 1) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szMain_2[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szMain_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 2) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szMain_3[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szMain_3[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 3) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSub_1[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szSub_1[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 4) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSub_2[m_Cur_Num].c_str(), _float2((_float)m_fX - 60.f - (_float)m_szSub_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 5) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSub_3[m_Cur_Num].c_str(), _float2((_float)m_fX - 50.f - (_float)m_szSub_3[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 6) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSub_4[m_Cur_Num].c_str(), _float2((_float)m_fX - 50.f - (_float)m_szSub_4[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 7) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szSub_5[m_Cur_Num].c_str(), _float2((_float)m_fX - 85.f - (_float)m_szSub_5[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 8) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szTalk_1[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szTalk_1[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 9) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szTalk_2[m_Cur_Num].c_str(), _float2((_float)m_fX - 60.f - (_float)m_szTalk_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 10) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szTalk_3[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szTalk_3[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 11) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szVillageSmell_1[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szVillageSmell_1[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 12) {
				if (m_Cur_Num == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szVillageSmell_2[m_Cur_Num].c_str(), _float2((_float)m_fX + 0.f - (_float)m_szVillageSmell_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szVillageSmell_2[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szVillageSmell_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				
			}
			else if (m_Dialog_Type == 13) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szVillageSmell_3[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szVillageSmell_3[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 14) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szHouseSmell_1[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szHouseSmell_1[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 15) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szHouseSmell_2[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szHouseSmell_2[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}
			else if (m_Dialog_Type == 16) {

				if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szHouseSmell_3[m_Cur_Num].c_str(), _float2((_float)m_fX - 40.f - (_float)m_szHouseSmell_3[m_Cur_Num].size() * 5.f, (_float)m_fY - 15.f), _float2(0.5f, 0.5f))))
					return E_FAIL;
			}

		}
		else if (m_UI_Desc.m_Type == 1) {
			if (m_Dialog_Type == 0) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[GIRL].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 1) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[MOTHER].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 2) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[GIRL].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 3) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[MAN].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 4) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[MAN].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 5) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[GIRL].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 6) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[BOY].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 7) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[GIRL].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 8) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[GIRL].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 9) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[ZENITSU].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type == 10) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[ZENITSU].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
			else if (m_Dialog_Type >= 11 && m_Dialog_Type <= 16) {

				if (m_Name_Type == 1) {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}
				else {
					if (FAILED(pGameInstance->Draw_Font(TEXT("Font_DM"), m_szName[TANJIRO].c_str(), _float2((_float)m_fX - 100.f + 4.f, (_float)m_fY - 12.f), _float2(0.5f, 0.5f))))
						return E_FAIL;
				}

			}
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
