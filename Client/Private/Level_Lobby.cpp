#include "pch.h"
#include "..\Public\Level_Lobby.h"

#include "GameInstance.h"
#include "Level_Loading.h"

#include "Camera.h"
#include "Player.h"
#include "MapObject.h"
#include "Player_Battle_Frame.h"
#include "Player_Battle_Hp.h"
#include "Player_Battle_Mp.h"
#include "Boss_Battle_Frame.h"
#include "Boss_Battle_Hp.h"
#include "Player_Battle_Ult_Frame.h"
#include "Player_Battle_Combo.h"
#include "Player_Battle_Ult_Effect.h"
#include "ColliderManager.h"
#include "Story_Board.h"

#include "Story_Manager.h"
#include "Fade.h"
#include "Fade_Manager.h"

#include "ParticleSystem.h"
#include "Effect_Texture.h"
#include "EffectPlayer.h"
#include "SoundMgr.h"


CLevel_Lobby::CLevel_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Lobby::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

   
    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
    {
        MSG_BOX("Failed to Ready_Layer_BackGround : CLevel_Lobby");
        return E_FAIL;
    }

	if (FAILED(Ready_Layer_Effect()))
	{
		MSG_BOX("Failed to Ready_Layer_Effect : Level_Static");
		return E_FAIL;
	}

	CFadeManager::GetInstance()->Set_Fade_In(true);
	
	CSoundMgr::Get_Instance()->StopAll();


    return S_OK;
}

void CLevel_Lobby::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	SetWindowText(g_hWnd, TEXT("Story_Board"));

	CStoryManager::GetInstance()->Tick();

	m_Select = CStoryManager::GetInstance()->Get_Select_Type();


	if (pGameInstance->Get_DIKeyDown(DIK_RETURN))
	{
		CFadeManager::GetInstance()->Set_Fade_Out(true);
	}

	if (m_Select == 0) {


		if (CFadeManager::GetInstance()->Get_Fade_Out_Done() == true) {

			CFadeManager::GetInstance()->Set_Fade_Out_Done(false);

			HRESULT hr = 0;


			if (nullptr == pGameInstance->Get_LoadedStage(LEVEL_GAMEPLAY))
			{
				pGameInstance->Clear_Light();
				hr = pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY), false, false);
			}
			else
			{
				pGameInstance->Clear_Light();
				hr = pGameInstance->Swap_Level(LEVEL_GAMEPLAY);
			}

			if (FAILED(hr))
			{
				Safe_Release(pGameInstance);
				return;
			}

		}

	}
	else if (m_Select == 1) {


		if (CFadeManager::GetInstance()->Get_Fade_Out_Done() == true) {

			CFadeManager::GetInstance()->Set_Fade_Out_Done(false);

			HRESULT hr = 0;

			if (nullptr == pGameInstance->Get_LoadedStage(LEVEL_VILLAGE))
			{
				pGameInstance->Clear_Light();
				hr = pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE), false, false);
			}
			else
			{
				pGameInstance->Clear_Light();
				hr = pGameInstance->Swap_Level(LEVEL_VILLAGE);
			}

			if (FAILED(hr)) {
				Safe_Release(pGameInstance);
				return;
			}
		}
	}
	else if (m_Select == 2) {


		if (CFadeManager::GetInstance()->Get_Fade_Out_Done() == true) {

			CFadeManager::GetInstance()->Set_Fade_Out_Done(false);

			HRESULT hr = 0;

			if (nullptr == pGameInstance->Get_LoadedStage(LEVEL_HOUSE))
			{
				pGameInstance->Clear_Light();
				hr = pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_HOUSE), false, false);
			}
			else
			{
				pGameInstance->Clear_Light();
				hr = pGameInstance->Swap_Level(LEVEL_HOUSE);
			}

			if (FAILED(hr)) {
				Safe_Release(pGameInstance);
				return;
			}
		}
	}
	else if (m_Select == 3) {


		if (CFadeManager::GetInstance()->Get_Fade_Out_Done() == true) {

			CFadeManager::GetInstance()->Set_Fade_Out_Done(false);

			HRESULT hr = 0;

			if (nullptr == pGameInstance->Get_LoadedStage(LEVEL_TRAIN))
			{
				pGameInstance->Clear_Light();
				hr = pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TRAIN), false, false);
			}
			else
			{
				pGameInstance->Clear_Light();
				hr = pGameInstance->Swap_Level(LEVEL_TRAIN);
			}

			if (FAILED(hr)) {
				Safe_Release(pGameInstance);
				return;
			}

		}

	}
	else if (m_Select == 4) {


		if (CFadeManager::GetInstance()->Get_Fade_Out_Done() == true) {

			CFadeManager::GetInstance()->Set_Fade_Out_Done(false);

			HRESULT hr = 0;

			if (nullptr == pGameInstance->Get_LoadedStage(LEVEL_FINALBOSS))
			{
				pGameInstance->Clear_Light();
				hr = pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_FINALBOSS), false, false);
			}
			else
			{
				pGameInstance->Clear_Light();
				hr = pGameInstance->Swap_Level(LEVEL_FINALBOSS);
			}

			if (FAILED(hr)) {
				Safe_Release(pGameInstance);
				return;
			}

		}

	}



	Safe_Release(pGameInstance);
}

HRESULT CLevel_Lobby::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Lobby::Ready_Lights()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    LIGHTDESC LightDesc;
    ZeroMemory(&LightDesc, sizeof LightDesc);

    LightDesc.eType = LIGHTDESC::TYPE_DIRECTION;
    LightDesc.vLightDir         = _float4(1.f, -1.f, 1.f, 0.f);
    LightDesc.vLightDiffuse     = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vLightAmbient     = _float4(1.f, 1.f, 1.f, 1.f);
    LightDesc.vLightSpecular    = _float4(1.f, 1.f, 1.f, 1.f);

    static _uint i = 0;

    if (i == 0)
    {
        if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
        {
            MSG_BOX("Failed to Add_GameObject : Direction_Light");
            return E_FAIL;
        }
    }

    ++i;

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

	CStory_Board::UIDESC UIDesc;

	// Bg
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 0;
	UIDesc.m_Back_Type = 0;
	UIDesc.m_Back_Layer = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Bg_Swap
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 16;
	UIDesc.m_Back_Type = 1;
	UIDesc.m_Back_Layer = -1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Bg_Deco_Top
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Bg_Deco_Bot
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Title
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 8;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Mini_Title
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 13;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Line
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 14;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Line
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 17;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Cloud_LT_Dark
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 10;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Cloud_LT
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 9;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Cloud_RT_Dark
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 12;
	UIDesc.m_Is_Y_Reverse = true;


	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Cloud_RT
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 11;
	UIDesc.m_Is_Y_Reverse = true;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Icon_0
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 3;
	
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Icon_1
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 4;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Icon_2
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 5;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Icon_3
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 6;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Icon_4
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 7;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	// Cursor
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Type = 15;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Story_Board"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


	// Fade
	CFade::UIDESC UIDesc2;
	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, TEXT("Prototype_GameObject_Fade"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Camera(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CCamera::CAMERADESC CameraDesc;
    ZeroMemory(&CameraDesc, sizeof CameraDesc);

    CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
    CameraDesc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
    CameraDesc.vAxisY = _float4(0.f, 1.f, 0.f, 0.f);

    CameraDesc.fFovY = XMConvertToRadians(60.f);
    CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
    CameraDesc.fNearZ = 0.3f;
    CameraDesc.fFarZ = 300.f;

    CameraDesc.TransformDesc.dSpeedPerSec = 10.0;
    CameraDesc.TransformDesc.dRadianRotationPerSec = XMConvertToRadians(90.f);
    CameraDesc.dSensitivity = 0.1;

    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, 
        TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : Camera_Free");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Player(const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CPlayer::CHARACTERDESC CharacterDesc;
    ZeroMemory(&CharacterDesc, sizeof CharacterDesc);

    
    CharacterDesc.WorldInfo.vPosition = _float4(130.f, 5.f, 140.f, 1.f);

    
    if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, pLayerTag, 
        TEXT("Prototype_GameObject_Player_Tanjiro"), &CharacterDesc)))
    {
        MSG_BOX("Failed to Add_GameObject : CLevel_Lobby");
        return E_FAIL;
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_MapObject(const _tchar* pLayerTag)
{
  //  Load_MapObject_Info(TEXT("../../Data/Object/Acaza_Battle/Acaza_Test.dat"), pLayerTag);
	
    return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Player_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#pragma region Player_Battle_Frame
	CPlayer_Battle_Frame::UIDESC UIDesc;
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 1;
	UIDesc.m_C_Num = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 2;
	UIDesc.m_C_Num = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


#pragma endregion

#pragma region Player_Battle_Hp
	CPlayer_Battle_Hp::UIDESC UIDesc2;
	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = true;
	UIDesc2.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = true;
	UIDesc2.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = true;
	UIDesc2.m_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
#pragma endregion

#pragma region Player_Battle_Mp

	CPlayer_Battle_Mp::UIDESC UIDesc3;
	ZeroMemory(&UIDesc3, sizeof UIDesc3);

	UIDesc3.m_Is_Reverse = false;
	UIDesc3.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc3, sizeof UIDesc3);

	UIDesc3.m_Is_Reverse = false;
	UIDesc3.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc3, sizeof UIDesc3);

	UIDesc3.m_Is_Reverse = false;
	UIDesc3.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	for (int i = 0; i < 4; i++) {

		ZeroMemory(&UIDesc3, sizeof UIDesc3);

		UIDesc3.m_Is_Reverse = false;
		UIDesc3.m_Type = 3;
		UIDesc3.m_Scale_Type = i;

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
			TEXT("Prototype_GameObject_Player_Battle_Mp"), &UIDesc3))) {
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

	}

#pragma endregion

#pragma region Player_Battle_Ult_Frame

	CPlayer_Battle_Ult_Frame::UIDESC UIDesc4;
	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 4;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 5;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 6;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 7;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 9;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 10;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 11;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 12;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc4, sizeof UIDesc4);

	UIDesc4.m_Is_Reverse = false;
	UIDesc4.m_Type = 13;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Frame"), &UIDesc4))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

#pragma endregion

#pragma region Player_Battle_Combo_Hit	
	CPlayer_Battle_Combo::UIDESC UIDesc5;
	ZeroMemory(&UIDesc5, sizeof UIDesc5);

	UIDesc5.m_Is_Reverse = false;
	UIDesc5.m_Type = 0;
	UIDesc5.m_Combo_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc5, sizeof UIDesc5);

	UIDesc5.m_Is_Reverse = false;
	UIDesc5.m_Type = 1;
	UIDesc5.m_Combo_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc5, sizeof UIDesc5);

	UIDesc5.m_Is_Reverse = false;
	UIDesc5.m_Type = 2;
	UIDesc5.m_Combo_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc5, sizeof UIDesc5);

	UIDesc5.m_Is_Reverse = false;
	UIDesc5.m_Type = 3;
	UIDesc5.m_Combo_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc5))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


#pragma endregion

#pragma region Player_Battle_Combo_Damage	
	CPlayer_Battle_Combo::UIDESC UIDesc6;
	ZeroMemory(&UIDesc6, sizeof UIDesc6);

	UIDesc6.m_Is_Reverse = false;
	UIDesc6.m_Type = 0;
	UIDesc6.m_Combo_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc6, sizeof UIDesc6);

	UIDesc6.m_Is_Reverse = false;
	UIDesc6.m_Type = 1;
	UIDesc6.m_Combo_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc6, sizeof UIDesc6);

	UIDesc6.m_Is_Reverse = false;
	UIDesc6.m_Type = 2;
	UIDesc6.m_Combo_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc6, sizeof UIDesc6);

	UIDesc6.m_Is_Reverse = false;
	UIDesc6.m_Type = 3;
	UIDesc6.m_Combo_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Combo"), &UIDesc6))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


#pragma endregion

#pragma region Player_Battle_Ult_Effect

	CPlayer_Battle_Ult_Effect::UIDESC UIDesc7;
	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 4;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 5;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc7, sizeof UIDesc7);

	UIDesc7.m_Is_Reverse = false;
	UIDesc7.m_Type = 6;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Player_UI"),
		TEXT("Prototype_GameObject_Player_Battle_Ult_Effect"), &UIDesc7))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

#pragma endregion

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Boss_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


#pragma region Boss_Battle_Frame
	CBoss_Battle_Frame::UIDESC UIDesc;
	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 1;
	UIDesc.m_C_Num = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc, sizeof UIDesc);

	UIDesc.m_Is_Reverse = false;
	UIDesc.m_Type = 2;
	UIDesc.m_C_Num = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Frame"), &UIDesc))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}


#pragma endregion

#pragma region Boss_Battle_Hp

	CBoss_Battle_Hp::UIDESC UIDesc2;
	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 0;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 1;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 2;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ZeroMemory(&UIDesc2, sizeof UIDesc2);

	UIDesc2.m_Is_Reverse = false;
	UIDesc2.m_Type = 3;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_Boss_UI"),
		TEXT("Prototype_GameObject_Boss_Battle_Hp"), &UIDesc2))) {
		Safe_Release(pGameInstance);
		return E_FAIL;
	}



#pragma endregion


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Lobby::Load_MapObject_Info(const _tchar* pPath, const _tchar* pLayerTag)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    HANDLE hFile = CreateFile(pPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    _ulong			dwByte = 0;
    _ulong			dwStrByte = 0;
    _uint			iSize = 0;

    CGameObject* pGameObject = { nullptr };

    ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iSize; ++i)
    {
        CMapObject::MAPOBJECT_INFO tMapObject_Info;
        ZeroMemory(&tMapObject_Info, sizeof tMapObject_Info);

        ReadFile(hFile, &tMapObject_Info.vPos, sizeof(_float4), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.vRotAngle, sizeof(_float3), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.vScale, sizeof(_float3), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.iMapObjectType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.iNumInstance, sizeof(_uint), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.fRange, sizeof(_float), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.fMinSize, sizeof(_float), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.fMaxSize, sizeof(_float), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.iInstanceType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.bRandomRatationY, sizeof(_bool), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.iArrangementType, sizeof(_uint), &dwByte, nullptr);

        ReadFile(hFile, &tMapObject_Info.iSceneType, sizeof(_uint), &dwByte, nullptr);


        ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
        ReadFile(hFile, &tMapObject_Info.szMeshName, dwStrByte, &dwByte, nullptr);

        const _tchar* pMapObjectTypeTag = TEXT("");

        switch (tMapObject_Info.iMapObjectType)
        {
        case CMapObject::MAPOBJECT_STATIC:
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_StaticMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        case CMapObject::MAPOBJECT_TERRAIN:
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_TerrainMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        case CMapObject::MAPOBJECT_ROTATION:
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_RotationMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        case CMapObject::MAPOBJECT_INSTANCE:
            if (FAILED(pGameInstance->Add_GameObject(LEVEL_LOBBY, TEXT("Layer_MapObject"),
                TEXT("Prototype_GameObject_InstanceMapObject"), &tMapObject_Info)))
                return E_FAIL;
            break;
        default:
            break;
        }
    }

    CloseHandle(hFile);

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Effect()
{
#pragma region TANJIRO_BASICCOMBO
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo1.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo2.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo2");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo3.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo3");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo4_Down.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo4_Down");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo4_Normal.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo4_Normal");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo4_Up.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo4_Up");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo_Air1.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo_Air1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo_Air2.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo_Air2");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_BasicCombo_Air3.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_BasicCombo_Air3");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Tilt.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Tilt");
		return E_FAIL;
	}
#pragma endregion

#pragma region TANJIRO_SURGECOMBO
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo1.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo2.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo2");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo3.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo3");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo4_Down.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo4_Down");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo4_Up.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo4_Up");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo4.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo4");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo_Air1.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo_Air1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo_Air2.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo_Air2");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeCombo_Air3.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeCombo_Air3");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_SurgeTilt.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_SurgeTilt");
		return E_FAIL;
	}
#pragma endregion

#pragma region TANJIRO_SUPERSKILL
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Super1.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Super1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Super2.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Super2");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Tanjiro/Tanjiro_Super3.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Tanjiro_Super3");
		return E_FAIL;
	}
#pragma endregion
	
#pragma region RENGOKU_BASICCOMBO
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Rengoku/Rengoku_BasicCombo1.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Rengoku_BasicCombo1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Rengoku/Rengoku_BasicCombo1_Decal.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Rengoku_BasicCombo1_Decal");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Rengoku/Rengoku_BasicCombo2.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Rengoku_BasicCombo2");
		return E_FAIL;
	}
#pragma endregion

#pragma region COMMON
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Hit_Shock.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Hit_Shock");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/SmellBundle.bin"))))
	{
		MSG_BOX("Failed to Load Effect : SmellBundle");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/SmellBundle_Disappear.bin"))))
	{
		MSG_BOX("Failed to Load Effect : SmellBundle_Disappear");
		return E_FAIL;
	}
#pragma endregion


#pragma region ZENITSU
	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Zenitsu/Zen_1.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Zen_1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Zenitsu/Zen_2_1.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Zen_2_1");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Zenitsu/Zen_2_2.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Zen_2_2");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Zenitsu/Zen_2_3.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Zen_2_3");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Zenitsu/Zen_3_Slash.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Zen_3_Slash");
		return E_FAIL;
	}

	if (FAILED(LoadEffects(TEXT("../Bin/DataFiles/Effect/Zenitsu/Zen_3_Wind.bin"))))
	{
		MSG_BOX("Failed to Load Effect : Zen_3_Slash");
		return E_FAIL;
	}

	
#pragma endregion

	return 	S_OK;
}

HRESULT CLevel_Lobby::LoadEffects(const _tchar* pPath, _bool isParticle, _int iCnt)
{
	char szFilePathChar[MAX_PATH] = "";
	WideCharToMultiByte(CP_ACP, 0, pPath, MAX_PATH, szFilePathChar, MAX_PATH, nullptr, nullptr);

	std::ifstream inputFile(szFilePathChar, std::ios::binary);

	if (!inputFile)
	{
		MSG_BOX("파일을 열 수 없습니다.");
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CEffectPlayer* pEffectPlayer = CEffectPlayer::Get_Instance();

	_ulong dwStrLen;
	int iEnum = 0;
	_float3 vFloat3 = { 0.f, 0.f, 0.f };

	_uint iNumParticleSystem = 0;

	inputFile.read(reinterpret_cast<char*>(&iNumParticleSystem), sizeof(_uint));

	CEffectPlayer::Get_Instance()->Set_NumParticleSystem(iNumParticleSystem);

	for (_uint i = 0; i < iNumParticleSystem; ++i)
	{
		inputFile.read(reinterpret_cast<char*>(&dwStrLen), sizeof(unsigned long));

		char* pTagName = new char[dwStrLen];

		inputFile.read(pTagName, dwStrLen);

		pEffectPlayer->Add_EffectTag(pTagName);

		CParticleSystem* pParentParticleSystem = nullptr;

		_uint iLevelIndex = LEVEL_STATIC;

		//if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("ParticleSystem"), TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex)))
		//	MSG_BOX("Failed to Add ParticleSystem");

		//size_t iSize = pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("ParticleSystem"));
		//pParentParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("ParticleSystem"), (_uint)iSize - 1));

		pParentParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex));

		pEffectPlayer->Add_Effect(pTagName, pParentParticleSystem);

		int iNumEffects = 0;

		inputFile.read(reinterpret_cast<char*>(&iNumEffects), sizeof(int));

		pParentParticleSystem->Set_NumEffects(iNumEffects);

		for (int j = 0; j < iNumEffects; ++j)
		{
			CParticleSystem* pParticleSystem = nullptr;
			_uint iLevelIndex = LEVEL_STATIC;

			//if (FAILED(pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("ChildParticleSystem"), TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex)))
			//	MSG_BOX("Failed to Add ParticleSystem");
			//size_t iSize = pGameInstance->Get_GameObject_ListSize(LEVEL_STATIC, TEXT("ChildParticleSystem"));
			//pParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("ChildParticleSystem"), (_uint)iSize - 1));

			pParticleSystem = dynamic_cast<CParticleSystem*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleSystem"), &iLevelIndex));

			pParentParticleSystem->Add_Parts(pParticleSystem);

			// ParticleSystem - Transform
			inputFile.read(reinterpret_cast<char*>(&vFloat3), sizeof(_float3));
			pParticleSystem->Set_Position(vFloat3);
			inputFile.read(reinterpret_cast<char*>(&vFloat3), sizeof(_float3));
			pParticleSystem->Set_Rotation(vFloat3);
			inputFile.read(reinterpret_cast<char*>(&vFloat3), sizeof(_float3));
			pParticleSystem->Set_Scale(vFloat3);

			inputFile.read(reinterpret_cast<char*>(&iEnum), sizeof(int));
			//pParticleSystem->Create_Effect(iEnum);
			pParticleSystem->Clone_Effect(iEnum);

			//Effect
			CEffect::EFFECTDESC EffectDesc;
			ZeroMemory(&EffectDesc, sizeof EffectDesc);

			EffectDesc.eEffectType = iEnum;

			// ParticleSystem
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDuration), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isLooping), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isPrewarm), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRandomStartDelay), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartDelayMin), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartDelayMax), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartLifeTimeOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartLifeTimeMin), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartLifeTimeMax), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartSpeedOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartSpeedMin), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fStartSpeedMax), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartSizeOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.is3DStartSize), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartSizeMin), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartSizeMax), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartRotationOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.is3DStartRotation), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartRotationMin), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vStartRotationMax), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartColorOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vColor), sizeof(_float4));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eGravityModifierOption), sizeof(int));	// 추가
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vGravityModifier), sizeof(_float2));	// 추가

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumGravityModiferOverLifetimes), sizeof(int));	// 추가

			pParticleSystem->Get_Effect()->Reserve_GravityModiferOverLifeTime(EffectDesc.iNumGravityModiferOverLifetimes);

			for (int k = 0; k < EffectDesc.iNumGravityModiferOverLifetimes; ++k)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
				inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

				pParticleSystem->Get_Effect()->Add_GravityModiferOverLifetime(LifetimeValue);
			}

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSimulationSpace), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSimulationSpeed), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eScalingMode), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iMaxParticles), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStopAction), sizeof(int));

			// Emission
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isEmission), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRateOverTimeOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverTimeMin), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverTimeMax), sizeof(float));

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumRateOverTime), sizeof(int));	// 추가

			pParticleSystem->Get_Effect()->Reserve_RateOverLifeTime(EffectDesc.iNumRateOverTime);

			for (int k = 0; k < EffectDesc.iNumRateOverTime; ++k)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
				inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

				pParticleSystem->Get_Effect()->Add_RateOverLifetime(LifetimeValue);
			}

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRateOverDistanceOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverDistanceMin), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRateOverDistanceMax), sizeof(float));

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumBursts), sizeof(int));

			pParticleSystem->Get_Effect()->Reserve_BurstList(EffectDesc.iNumBursts);

			for (int k = 0; k < EffectDesc.iNumBursts; ++k)
			{
				CEffect::BURSTDESC BurstDesc;
				ZeroMemory(&BurstDesc, sizeof BurstDesc);

				inputFile.read(reinterpret_cast<char*>(&BurstDesc.fTime), sizeof(float));
				inputFile.read(reinterpret_cast<char*>(&BurstDesc.eCountOption), sizeof(int));
				inputFile.read(reinterpret_cast<char*>(&BurstDesc.iCountMin), sizeof(int));
				inputFile.read(reinterpret_cast<char*>(&BurstDesc.iCountMax), sizeof(int));
				inputFile.read(reinterpret_cast<char*>(&BurstDesc.eCycle), sizeof(int));
				inputFile.read(reinterpret_cast<char*>(&BurstDesc.iNumCycles), sizeof(int));
				inputFile.read(reinterpret_cast<char*>(&BurstDesc.fInterval), sizeof(float));
				inputFile.read(reinterpret_cast<char*>(&BurstDesc.fProbability), sizeof(float));

				pParticleSystem->Get_Effect()->Add_BurstDesc(BurstDesc);
			}

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumParticlesPerFrame), sizeof(int));	// 추가

			// Shape
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isShape), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSpark), sizeof(bool));	// 추가
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fYExtendSpeed), sizeof(float));	// 추가
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fYExtendEndSize), sizeof(_float2));	// 추가
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eShapeType), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fShapeAngle), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fShapeRadius), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fShapeRadiusTickness), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fArc), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eArcMode), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fArcSpread), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eEmitFromOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fArcLength), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vShapePosition), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vShapeRotation), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vShapeScale), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isAlignToDirection), sizeof(bool));

			// Velocity over Lifetime
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isVelocity), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityLinearOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityLinearMin), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityLinearMax), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSpace), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityOrbitalOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOrbitalMin), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOrbitalMax), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityOffsetOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOffsetMin), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vVelocityOffsetMax), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocityRadialOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRadialMin), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRadialMax), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eVelocitySpeedModifierOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSpeedModifierMin), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSpeedModifierMax), sizeof(float));

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumSpeedOverLifeTimes), sizeof(int)); // 추가

			pParticleSystem->Get_Effect()->Reserve_SpeedOverLifeTime(EffectDesc.iNumSpeedOverLifeTimes);

			for (int k = 0; k < EffectDesc.iNumSpeedOverLifeTimes; ++k)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
				inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

				pParticleSystem->Get_Effect()->Add_SpeedOverLifetime(LifetimeValue);
			}

			// Limit Velocity over Lifetime
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesLimitVelocity), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eLimitVelocitySpeedOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vLimitVelocitySpeedMin), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vLimitVelocitySpeedMax), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eLimitVelocitySpace), sizeof(int));

			// Inherit Velocity
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isInheritVelocity), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eInheritMode), sizeof(int));

			// Force over Lifetime
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isForceOverLifetime), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vForce), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSpaceForce), sizeof(int));

			// Size over Lifetime
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSizeOverLifetime), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesSzOverLifeTime), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSizeOverLifetimeOption), sizeof(int));

			for (int k = 0; k < 3; ++k)
			{
				inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumSizes[k]), sizeof(int));

				pParticleSystem->Get_Effect()->Reserve_SizeOverLifeTime(k, EffectDesc.iNumSizes[k]);

				for (int l = 0; l < EffectDesc.iNumSizes[k]; ++l)
				{
					CEffect::LIFETIMEVALUE LifetimeValue;
					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

					inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
					inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

					pParticleSystem->Get_Effect()->Add_SizeOverLifeTime(k, LifetimeValue);
				}
			}

			// Rotation over Lifetime
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRotationOverLifetime), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesRotOverLifeTime), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRotOverLifetimeOption), sizeof(int));

			for (_uint k = 0; k < 3; ++k)
			{
				inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumRotations[k]), sizeof(int));

				pParticleSystem->Get_Effect()->Reserve_RotationOverLifeTime(k, EffectDesc.iNumRotations[k]);

				for (int l = 0; l < EffectDesc.iNumRotations[k]; ++l)
				{
					CEffect::LIFETIMEVALUE LifetimeValue;
					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

					inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
					inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

					pParticleSystem->Get_Effect()->Add_RotationOverLifeTime(k, LifetimeValue);
				}
			}

			// Position over Lifetime
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isPositionOverLifetime), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.ePosOverLifetimeOption), sizeof(int));

			for (_uint k = 0; k < 3; ++k)
			{
				inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumPositions[k]), sizeof(int));

				pParticleSystem->Get_Effect()->Reserve_PositionOverLifeTime(k, EffectDesc.iNumRotations[k]);

				for (int l = 0; l < EffectDesc.iNumPositions[k]; ++l)
				{
					CEffect::LIFETIMEVALUE LifetimeValue;
					ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

					inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
					inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

					pParticleSystem->Get_Effect()->Add_PositionOverLifeTime(k, LifetimeValue);
				}
			}

			// Rotation by Speed
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRotationBySpeed), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isSeparateAxesRotBySpeed), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eAngularVelocityOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vAngularVelocity), sizeof(_float3));

			// Collision
			// SubEmitters
			// Texture Sheet Animation
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isTextureSheetAnimation), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vTiles), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eTimeModeOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eFrameOverTimeOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eStartFrameOption), sizeof(int));

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumFrameOverTime), sizeof(int));

			pParticleSystem->Get_Effect()->Reserve_FrameOverLifeTime(EffectDesc.iNumFrameOverTime);

			for (int k = 0; k < EffectDesc.iNumFrameOverTime; ++k)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
				inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

				pParticleSystem->Get_Effect()->Add_FrameOverLifetime(LifetimeValue);
			}

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iStartFrame), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fFrameSpeedMin), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fFrameSpeedMax), sizeof(float));
			// Trail
			// Renderer
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isRenderer), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRenderMode), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eMaterialType), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eSortMode), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fSortingFudge), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fMinParticleSize), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fMaxParticleSize), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPivot), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fTextureOrder), sizeof(float));
			_float2 vCameraRightLookPos = _float2(0.f, 0.f);
			inputFile.read(reinterpret_cast<char*>(&vCameraRightLookPos), sizeof(_float2));
			if (CEffect::EFFECT_TEXTURE == EffectDesc.eEffectType)
			{
				CEffect_Texture* pTextureEffect = dynamic_cast<CEffect_Texture*>(pParticleSystem->Get_Effect());
				pTextureEffect->Set_Order(EffectDesc.fTextureOrder);
				pTextureEffect->Set_CameraRightLookPos(vCameraRightLookPos);
			}

			// Texture
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vFlip), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCutOff), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCutOffSoftness), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vBurnColor), sizeof(_float4));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fBurnSize), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDissolveDelay), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDissolveSpeed), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiffuseTilling), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiffuseOffset), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vGradientTilling), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vGradientOffset), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPaddingDelayStartEnd), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPaddingSpeedStart), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPaddingSpeedEnd), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDistortionStrength), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fDistortionSpeed), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.hasSecondTexture), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vNoisePanningSpeed), sizeof(_float4));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vSecondaryNoisePanningSpeed), sizeof(_float4));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPanningSpeed), sizeof(_float4));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vNormalPanningSpeed), sizeof(_float4));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.isPolarCoordinate), sizeof(bool));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCircleMaskOuterRadius), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCircleMaskInnerRadius), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fCircleMaskSmoothness), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRectangleMaskOuterRadius), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRectangleMaskInnerRadius), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fRectangleMaskSmoothness), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eCullMode), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiscardedPixelMin), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vDiscardedPixelMax), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPixelDiscardSpeedMin), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vPixelDiscardSpeedMax), sizeof(_float2));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fPixelDiscardDelay), sizeof(float));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.iNumAlpha), sizeof(int));

			pParticleSystem->Get_Effect()->Reserve_AlphaOverLifeTime(EffectDesc.iNumAlpha);

			for (int k = 0; k < EffectDesc.iNumAlpha; ++k)
			{
				CEffect::LIFETIMEVALUE LifetimeValue;
				ZeroMemory(&LifetimeValue, sizeof LifetimeValue);

				inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fLifetime), sizeof(float));
				inputFile.read(reinterpret_cast<char*>(&LifetimeValue.fValue), sizeof(float));

				pParticleSystem->Get_Effect()->Add_AlphaOverLifeTime(LifetimeValue);
			}

			//Material
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.eRenderGroupOption), sizeof(int));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vModelPivotScale), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vModelPivotRot), sizeof(_float3));
			inputFile.read(reinterpret_cast<char*>(&EffectDesc.vModelPivotPos), sizeof(_float3));

			for (_uint k = 0; k < CEffect::TEX_END; ++k)
			{
				inputFile.read(reinterpret_cast<char*>(&EffectDesc.eTextureShaderOption[k]), sizeof(int));
			}

			inputFile.read(reinterpret_cast<char*>(&EffectDesc.fTimeAcc), sizeof(float));

			pParticleSystem->Set_EffectDesc(&EffectDesc);

			// Keys
			inputFile.read(reinterpret_cast<char*>(&dwStrLen), sizeof(unsigned long));

			char	szTag[MAX_PATH];
			inputFile.read(szTag, dwStrLen);

			if (strlen(szTag) > 1)
			{
				_tchar* pTagName = new _tchar[dwStrLen];
				MultiByteToWideChar(CP_ACP, 0, szTag, (int)dwStrLen, pTagName, MAX_PATH);

				pParticleSystem->Add_Component_Model(LEVEL_STATIC, pTagName);

				pParticleSystem->Get_Effect()->Set_ModelKey(pTagName);

				if (!pEffectPlayer->Find_ModelKey(pTagName))
					pEffectPlayer->Add_ModelTag(pTagName);
				else
					Safe_Delete_Array(pTagName);
			}

			for (_uint k = 0; k < CEffect::TEX_END; ++k)
			{
				inputFile.read(reinterpret_cast<char*>(&dwStrLen), sizeof(unsigned long));
				inputFile.read(szTag, dwStrLen);

				if (strlen(szTag) > 1)
				{
					if (dwStrLen <= 1)
						continue;

					_tchar* pTagName = new _tchar[dwStrLen];
					MultiByteToWideChar(CP_ACP, 0, szTag, (int)dwStrLen, pTagName, MAX_PATH);

					switch (k)
					{
					case 0:		// TEX_DIFFUSE
						pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_DIFFUSE);
						pParticleSystem->Get_Effect()->Set_TextureKey(0, pTagName);
						if (!pEffectPlayer->Find_TextureKey(0, pTagName))
							pEffectPlayer->Add_TextureTag(CEffect::TEX_DIFFUSE, pTagName);
						else
							Safe_Delete_Array(pTagName);
						break;
					case 1:		//TEX_MASK
						pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_MASK);
						pParticleSystem->Get_Effect()->Set_TextureKey(1, pTagName);
						if (!pEffectPlayer->Find_TextureKey(1, pTagName))
							pEffectPlayer->Add_TextureTag(CEffect::TEX_MASK, pTagName);
						else
							Safe_Delete_Array(pTagName);
						break;
					case 2:		//TEX_RAMP
						pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_RAMP);
						pParticleSystem->Get_Effect()->Set_TextureKey(2, pTagName);
						if (!pEffectPlayer->Find_TextureKey(2, pTagName))
							pEffectPlayer->Add_TextureTag(CEffect::TEX_RAMP, pTagName);
						else
							Safe_Delete_Array(pTagName);
						break;
					case 3:		//TEX_NOISE
						pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_NOISE);
						pParticleSystem->Get_Effect()->Set_TextureKey(3, pTagName);
						if (!pEffectPlayer->Find_TextureKey(3, pTagName))
							pEffectPlayer->Add_TextureTag(CEffect::TEX_NOISE, pTagName);
						else
							Safe_Delete_Array(pTagName);
						break;
					case 4:		//TEX_NOISE2
						pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_NOISE2);
						pParticleSystem->Get_Effect()->Set_TextureKey(4, pTagName);
						if (!pEffectPlayer->Find_TextureKey(4, pTagName))
							pEffectPlayer->Add_TextureTag(CEffect::TEX_NOISE2, pTagName);
						else
							Safe_Delete_Array(pTagName);
						break;
					case 5:		//TEX_DISTORTION
						pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_DISTORTION);
						pParticleSystem->Get_Effect()->Set_TextureKey(5, pTagName);
						if (!pEffectPlayer->Find_TextureKey(5, pTagName))
							pEffectPlayer->Add_TextureTag(CEffect::TEX_DISTORTION, pTagName);
						else
							Safe_Delete_Array(pTagName);
						break;
					case 6:		//TEX_NORMAL
						pParticleSystem->Add_Component_Texture(LEVEL_STATIC, pTagName, CEffect::TEX_NORMAL);
						pParticleSystem->Get_Effect()->Set_TextureKey(6, pTagName);
						if (!pEffectPlayer->Find_TextureKey(6, pTagName))
							pEffectPlayer->Add_TextureTag(CEffect::TEX_NORMAL, pTagName);
						else
							Safe_Delete_Array(pTagName);
						break;
					}
				}
			}
		}
	
		if (isParticle)
		{
			pEffectPlayer->Add_ParticlePool(pTagName);
			pEffectPlayer->Add_Particles_In_Pool(pTagName, iCnt);
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Lobby* CLevel_Lobby::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_Lobby* pInstance = new CLevel_Lobby(pDevice, pContext);
    
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Lobby");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Lobby::Free()
{
    __super::Free();
}
